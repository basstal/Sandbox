#include "TransformGizmo.hpp"

#include <vector>

#include "Grid.hpp"
#include "ImGuiExamples.hpp"
#include "ImGuiRenderer.hpp"
#include "Engine/Camera.hpp"
#include "Engine/EntityComponent/GameObject.hpp"
#include "Engine/EntityComponent/Components/Transform.hpp"
#include "FileSystem/Logger.hpp"
#include "GLFW/glfw3.h"
#include "glm/fwd.hpp"
#include "glm/detail/type_quat.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Platform/GlfwCallbackBridge.hpp"
#include "Misc/BoundingBox.hpp"
#include "Misc/CollisionDetection.hpp"
#include "Misc/GlmExtensions.hpp"
#include "Misc/Math.hpp"
#include "Misc/Ray.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Core/Buffer.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/Surface.hpp"
#include "VulkanRHI/Core/Swapchain.hpp"
#include "VulkanRHI/Rendering/RenderAttachments.hpp"
#include "VulkanRHI/Rendering/RenderTarget.hpp"

static bool isGizmoActive = false;
static GLFWcursorposfun lastCallback = nullptr;
static std::weak_ptr<Sandbox::Editor> editorWeakPtr;

constexpr glm::vec3 AXIS_X = glm::vec3(1.0f, 0.0f, 0.0f);
constexpr glm::vec3 AXIS_Y = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 AXIS_Z = glm::vec3(0.0f, 0.0f, 1.0f);
static Sandbox::DelegateHandle cursorBindHandler;

static void GlfwCursorPosition(GLFWwindow* window, double xPos, double yPos)
{
    // auto editor = editorWeakPtr.lock();
    // if (editor)
    // {
    //     editor->transformGizmo->UpdateGizmoAndObjectPosition(window, editor->camera);
    // }
}

static void ApplyGizmoMovement(GLFWwindow* window, bool active, const std::shared_ptr<Sandbox::GlfwCallbackBridge>& glfwInputBridge)
{
    if (isGizmoActive == active)
    {
        return;
    }
    if (!isGizmoActive)
    {
        cursorBindHandler = glfwInputBridge->onCursorPosition.Bind(GlfwCursorPosition);
    }
    else
    {
        glfwInputBridge->onCursorPosition.Unbind(cursorBindHandler);
        cursorBindHandler = Sandbox::DelegateHandle::Null;
    }
    isGizmoActive = active;
}

std::vector<Sandbox::SimpleVertex> GenerateArrowData(glm::vec3 color)
{
    constexpr float PI = 3.1415926f;
    constexpr int SEGMENTS = 32;
    constexpr float RADIUS_CYLINDER = 0.1f;
    constexpr float HEIGHT_CYLINDER = 1.0f;
    constexpr float RADIUS_CONE = 0.2f;
    constexpr float HEIGHT_CONE = 0.5f;
    std::vector<Sandbox::SimpleVertex> vertices;
    float deltaAngle = 2.0f * PI / SEGMENTS;
    for (int i = 0; i < SEGMENTS; ++i)
    {
        float angle = static_cast<float>(i) * deltaAngle;
        float nextAngle = static_cast<float>(i + 1) * deltaAngle;

        // 底部圆的顶点
        glm::vec3 bottom1(RADIUS_CYLINDER * cos(angle), RADIUS_CYLINDER * sin(angle), 0);
        glm::vec3 bottom2(RADIUS_CYLINDER * cos(nextAngle), RADIUS_CYLINDER * sin(nextAngle), 0);

        // 顶部圆的顶点
        glm::vec3 top1(RADIUS_CYLINDER * cos(angle), RADIUS_CYLINDER * sin(angle), HEIGHT_CYLINDER);
        glm::vec3 top2(RADIUS_CYLINDER * cos(nextAngle), RADIUS_CYLINDER * sin(nextAngle), HEIGHT_CYLINDER);

        // 添加侧面的顶点
        vertices.push_back({bottom1, color});
        vertices.push_back({bottom2, color});
        vertices.push_back({top1, color});

        vertices.push_back({bottom2, color});
        vertices.push_back({top2, color});
        vertices.push_back({top1, color});
    }

    glm::vec3 coneTip(0, 0, HEIGHT_CYLINDER + HEIGHT_CONE); // 锥体顶点位于圆柱体顶部之上
    for (int i = 0; i < SEGMENTS; ++i)
    {
        float angle = static_cast<float>(i) * deltaAngle;
        float nextAngle = static_cast<float>(i + 1) * deltaAngle;

        // 锥体基底圆的顶点
        glm::vec3 base1(RADIUS_CONE * cos(angle), RADIUS_CONE * sin(angle), HEIGHT_CYLINDER);
        glm::vec3 base2(RADIUS_CONE * cos(nextAngle), RADIUS_CONE * sin(nextAngle), HEIGHT_CYLINDER);

        // 添加锥体侧面的顶点
        vertices.push_back({coneTip, color});
        vertices.push_back({base1, color});
        vertices.push_back({base2, color});
    }
    return vertices;
}

void Sandbox::TransformGizmo::Prepare(const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<Editor>& editor)
{
    editorWeakPtr = std::weak_ptr<Editor>(editor);
    m_editor = editor;
    m_renderer = renderer;

    m_arrowX = GenerateArrowData(AXIS_X);
    // 对 arrowX 中每个点应用绕 y 轴 90 度旋转
    glm::quat rotation = glm::angleAxis(glm::radians(90.0f), AXIS_Y);
    for (auto& vertex : m_arrowX)
    {
        vertex.position = RotateVectorByQuaternion(vertex.position, rotation);
    }
    m_arrowY = GenerateArrowData(AXIS_Y);
    // 对 arrowY 中每个点应用绕 x 轴 -90 度旋转
    rotation = glm::angleAxis(glm::radians(-90.0f), AXIS_X);
    for (auto& vertex : m_arrowY)
    {
        vertex.position = RotateVectorByQuaternion(vertex.position, rotation);
    }
    m_arrowZ = GenerateArrowData(AXIS_Z);
    m_vertices.insert(m_vertices.end(), m_arrowX.begin(), m_arrowX.end());
    m_vertices.insert(m_vertices.end(), m_arrowY.begin(), m_arrowY.end());
    m_vertices.insert(m_vertices.end(), m_arrowZ.begin(), m_arrowZ.end());

    auto bufferSize = sizeof(SimpleVertex) * m_vertices.size();
    m_vertexBuffer = std::make_shared<Buffer>(renderer->device, bufferSize,
                                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    renderer->commandBuffers[0]->CopyDataToBuffer(m_vertices.data(), bufferSize, m_vertexBuffer);
    CreateFramebuffer();
    m_renderer->swapchain->onAfterRecreateSwapchain.BindMember<TransformGizmo, &TransformGizmo::CreateFramebuffer>(this);
    m_prepared = true;
}


void Sandbox::TransformGizmo::CreateFramebuffer()
{
    m_renderAttachments != nullptr ? m_renderAttachments->Cleanup() : void();
    m_renderTarget != nullptr ? m_renderTarget->Cleanup() : void();
    m_renderAttachments = std::make_shared<RenderAttachments>(m_renderer->device, m_renderer->renderPass, m_renderer->swapchain->vkExtent2D, nullptr);
    m_renderTarget = std::make_shared<RenderTarget>(m_renderer->device, m_renderer->renderPass, m_renderer->swapchain->vkExtent2D, m_renderAttachments);
}

void Sandbox::TransformGizmo::UpdateGizmoBoundingBoxes(const glm::mat4& modelToWorld)
{
    m_gizmoBoundingBoxes[0] = std::make_shared<BoundingBox>(m_arrowX);
    m_gizmoBoundingBoxes[0]->PrepareDebugDrawData(m_renderer->device);
    m_gizmoBoundingBoxes[1] = std::make_shared<BoundingBox>(m_arrowY);
    m_gizmoBoundingBoxes[2] = std::make_shared<BoundingBox>(m_arrowZ);
}

void Sandbox::TransformGizmo::DrawGizmo(const std::shared_ptr<CommandBuffer>& commandBuffer, uint32_t frameFlightIndex)
{
    if (m_referenceGameObject == nullptr)
    {
        return;
    }
    constexpr float BASE_SIZE = .25f; // 基础尺寸
    constexpr float DISTANCE_SIZE_FACTOR = 0.25f;
    uint32_t dynamicAlignment = m_editor->GetUniformDynamicAlignment(sizeof(glm::mat4));

    auto translate = m_referenceGameObject->transform->GetModelTranslate();
    float distance = glm::length(m_referenceGameObject->transform->position - m_editor->camera->property->position);
    float currentScaleFactor = distance * DISTANCE_SIZE_FACTOR; // 根据距离计算缩放因子
    glm::mat4 distanceScaledMatrix = glm::scale(translate, glm::vec3(currentScaleFactor * BASE_SIZE));
    if (std::abs(currentScaleFactor - m_previousScaleFactor) > 0.01f || m_previousScaleFactor == 0.0f)
    {
        m_previousScaleFactor = currentScaleFactor;
        UpdateGizmoBoundingBoxes(distanceScaledMatrix);
    }
    m_editor->models[frameFlightIndex]->model[2] = distanceScaledMatrix;
    {
        // TODO:看 descriptorset 用哪个，这里写死了下标
        commandBuffer->BindPipeline(m_editor->pipelineGizmo, m_editor->descriptorSets[frameFlightIndex], {2 * dynamicAlignment});
        commandBuffer->BindVertexBuffers(m_vertexBuffer);
        commandBuffer->Draw(static_cast<uint32_t>(m_vertices.size()));
    }
    // 绘制 包围盒
    {
        // 默认大小，因为顶点已经计算过模型到世界坐标
        commandBuffer->BindPipeline(m_editor->pipelineLineList, m_editor->descriptorSets[frameFlightIndex], {2 * dynamicAlignment});
        commandBuffer->BindVertexBuffers(m_gizmoBoundingBoxes[0]->vertexBuffer);
        commandBuffer->BindIndexBuffer(m_gizmoBoundingBoxes[0]->indexBuffer);
        commandBuffer->DrawIndexed(static_cast<uint32_t>(m_gizmoBoundingBoxes[0]->GetIndices().size()));

        if (cameraRay)
        {
            // 获取射线起点和方向
            commandBuffer->BindPipeline(m_editor->pipelineLineList, m_editor->descriptorSets[frameFlightIndex], {dynamicAlignment});
            commandBuffer->BindVertexBuffers(cameraRay->vertexBuffer);
            commandBuffer->BindIndexBuffer(cameraRay->indexBuffer);
            commandBuffer->DrawIndexed(static_cast<uint32_t>(cameraRay->GetIndices().size()));
        }
    }
}

void Sandbox::TransformGizmo::SetTarget(const std::shared_ptr<GameObject>& target)
{
    m_referenceGameObject = target;
}

void Sandbox::TransformGizmo::Cleanup()
{
    if (!m_prepared)
    {
        return;
    }
    m_prepared = false;
    m_vertexBuffer->Cleanup();
    // m_gizmoBoundingBoxes[0]->Cleanup();
}


void Sandbox::TransformGizmo::UpdateInputs(GLFWwindow* window, const std::shared_ptr<Camera>& camera, const std::shared_ptr<GlfwCallbackBridge>& glfwInputBridge)
{
    auto leftMousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    if (!leftMousePressed)
    {
        m_gizmoActiveX = m_gizmoActiveY = m_gizmoActiveZ = false;
    }
    else if (!m_gizmoActiveX && !m_gizmoActiveY && !m_gizmoActiveZ)
    {
        // auto ray = CursorPositionToWorldRay(window, camera->GetViewMatrix(), camera->GetProjectionMatrix());
        // Logger::Debug(ray.ToString());
        // float factor;
        // glm::vec3 intersectPoint;
        //
        // auto modelToWorld = m_editor->models[m_renderer->frameFlightIndex]->model[2];
        // auto isActiveX = Sandbox::IntersectRayBoundingBox(ray, m_gizmoBoundingBoxes[0]->Multiply(modelToWorld), factor, intersectPoint);
        // auto isActiveY = Sandbox::IntersectRayBoundingBox(ray, m_gizmoBoundingBoxes[1]->Multiply(modelToWorld), factor, intersectPoint);
        // auto isActiveZ = Sandbox::IntersectRayBoundingBox(ray, m_gizmoBoundingBoxes[2]->Multiply(modelToWorld), factor, intersectPoint);
        // m_gizmoActiveX = isActiveX;
        // m_gizmoActiveY = !m_gizmoActiveX && isActiveY;
        // m_gizmoActiveZ = !m_gizmoActiveX && !m_gizmoActiveY && isActiveZ;
        // LOGD("ActiveX {}, ActiveY {}, ActiveZ {}", std::to_string(m_gizmoActiveX), std::to_string(m_gizmoActiveY), std::to_string(m_gizmoActiveZ));
    }
    ApplyGizmoMovement(window, m_gizmoActiveX || m_gizmoActiveY || m_gizmoActiveZ, glfwInputBridge);
}


void Sandbox::TransformGizmo::UpdateGizmoAndObjectPosition(GLFWwindow* window, const std::shared_ptr<Camera>& camera)
{
    auto ray = CursorPositionToWorldRay(window, camera->GetViewMatrix(), camera->GetProjectionMatrix()); // 获取射线起点和方向
    glm::vec3 rayOrigin = ray.origin;
    glm::vec3 rayDir = ray.direction;
    Logger::Debug("UpdateGizmoAndObjectPosition rayOrigin {}, rayDir {}", ToString(rayOrigin), ToString(rayDir));

    // 假设Gizmo距离摄像机的深度为distance
    float distance = glm::length(m_referenceGameObject->transform->position - camera->property->position);

    // 使用射线与深度确定的平面交点来更新Gizmo位置
    // 假设平面方程为Ax + By + Cz + D = 0，这里选择与摄像机视线垂直的平面
    glm::vec3 planeNormal = glm::normalize(camera->front); // 摄像机前向向量作为平面法线
    float D = -glm::dot(planeNormal, camera->property->position + planeNormal * distance); // 计算平面方程的D值

    // 计算射线与平面的交点
    float denom = glm::dot(planeNormal, rayDir);
    if (abs(denom) > FLT_EPSILON)
    {
        // 确保不是平行（避免除以零）
        float t = -(glm::dot(planeNormal, rayOrigin) + D) / denom;
        glm::vec3 intersectionPoint = rayOrigin + rayDir * t;

        // 计算移动量
        glm::vec3 movement = intersectionPoint - m_referenceGameObject->transform->position;
        // 根据激活的Gizmo轴选择移动方向
        glm::vec3 axis = m_gizmoActiveX ? AXIS_X : m_gizmoActiveY ? AXIS_Y : AXIS_Z;

        // 计算沿着Gizmo轴的移动量
        float movementAmount = glm::dot(movement, axis);
        auto movementAlongAxis = movementAmount * axis;
        // 更新Gizmo和关联对象的位置
        m_referenceGameObject->transform->position += movementAlongAxis;
    }
}
