#include "GizmoEditor.hpp"
#include <glm/gtc/type_ptr.hpp> // 包含glm::value_ptr

#include "imgui.h"
#include "ImGuizmo.h"
#include "Infrastructures/DataBinding.hpp"
#include "Rendering/Application.hpp"

std::shared_ptr<GizmoEditor> GizmoEditor::m_instance = nullptr;

GizmoEditor::GizmoEditor(std::shared_ptr<ApplicationEditor> applicationEditor)
{
	m_applicationEditor = applicationEditor;
}

GizmoEditor::~GizmoEditor()
{
	// TODO:bug fix registeredEditors size 0 when shutdown
	// Unregister();
}

void GizmoEditor::DrawFrame()
{
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::BeginFrame();

	ImGuiIO& io = ImGui::GetIO();
	auto identityMatrix = glm::mat4(1.0f);
	auto cameraView = Application::Instance->editorCamera->GetViewMatrix();
	auto cameraProjection = Application::Instance->projection;
	auto viewProjection = cameraProjection * cameraView;

	auto settings = m_applicationEditor->settings;
	float x = settings->IsWindow ? static_cast<float>(settings->WindowPositionX) : 0;
	float y = settings->IsWindow ? static_cast<float>(settings->WindowPositionY) : 0;
	ImGuizmo::SetRect(x, y, io.DisplaySize.x, io.DisplaySize.y);

	DrawGrid(viewProjection, identityMatrix, 100.f);
	// ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], gizmoCount);
	// ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL,
	//                      boundSizingSnap ? boundsSnap : NULL);
	//
	// ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);
}

void ComputeFrustumPlanes(glm::vec4* frustum, const float* clip)
{
	frustum[0].x = clip[3] - clip[0];
	frustum[0].y = clip[7] - clip[4];
	frustum[0].z = clip[11] - clip[8];
	frustum[0].w = clip[15] - clip[12];

	frustum[1].x = clip[3] + clip[0];
	frustum[1].y = clip[7] + clip[4];
	frustum[1].z = clip[11] + clip[8];
	frustum[1].w = clip[15] + clip[12];

	frustum[2].x = clip[3] + clip[1];
	frustum[2].y = clip[7] + clip[5];
	frustum[2].z = clip[11] + clip[9];
	frustum[2].w = clip[15] + clip[13];

	frustum[3].x = clip[3] - clip[1];
	frustum[3].y = clip[7] - clip[5];
	frustum[3].z = clip[11] - clip[9];
	frustum[3].w = clip[15] - clip[13];

	frustum[4].x = clip[3] - clip[2];
	frustum[4].y = clip[7] - clip[6];
	frustum[4].z = clip[11] - clip[10];
	frustum[4].w = clip[15] - clip[14];

	frustum[5].x = clip[3] + clip[2];
	frustum[5].y = clip[7] + clip[6];
	frustum[5].z = clip[11] + clip[10];
	frustum[5].w = clip[15] + clip[14];

	for (int i = 0; i < 6; i++)
	{
		frustum[i] = glm::normalize(frustum[i]);
	}
}

static float DistanceToPlane(const glm::vec4& point, const glm::vec4& plan)
{
	return glm::dot(glm::vec3(plan), glm::vec3(point)) + plan.w;
}

static void Lerp(glm::vec4& a, const glm::vec4& v, float t)
{
	a.x += (v.x - a.x) * t;
	a.y += (v.y - a.y) * t;
	a.z += (v.z - a.z) * t;
	a.w += (v.w - a.w) * t;
}

void GizmoEditor::DrawGrid(glm::mat4 viewProjection, glm::mat4 matrix, const float gridSize)
{
	// matrix_t viewProjection = *(matrix_t*)view * *(matrix_t*)projection;
	glm::vec4 frustum[6];
	ComputeFrustumPlanes(frustum, glm::value_ptr(viewProjection));
	glm::mat4 res = viewProjection * matrix;

	for (float f = -gridSize; f <= gridSize; f += 1.f)
	{
		for (int dir = 0; dir < 2; dir++)
		{
			glm::vec4 ptA = glm::vec4(dir ? -gridSize : f, dir ? f : -gridSize, 0, 0);
			glm::vec4 ptB = glm::vec4(dir ? gridSize : f, dir ? f : gridSize, 0, 0);
			bool visible = true;
			for (int i = 0; i < 6; i++)
			{
				float dA = DistanceToPlane(ptA, frustum[i]);
				float dB = DistanceToPlane(ptB, frustum[i]);
				if (dA < 0.f && dB < 0.f)
				{
					visible = false;
					break;
				}
				if (dA > 0.f && dB > 0.f)
				{
					continue;
				}
				if (dA < 0.f)
				{
					float len = fabsf(dA - dB);
					float t = fabsf(dA) / len;
					Lerp(ptA, ptB, t);
				}
				if (dB < 0.f)
				{
					float len = fabsf(dB - dA);
					float t = fabsf(dB) / len;
					Lerp(ptB, ptA, t);
				}
			}
			if (visible)
			{
				ImU32 col = IM_COL32(0x80, 0x80, 0x80, 0xFF);
				col = (fmodf(fabsf(f), 10.f) < FLT_EPSILON) ? IM_COL32(0x90, 0x90, 0x90, 0xFF) : col;
				col = (fabsf(f) < FLT_EPSILON) ? IM_COL32(0x40, 0x40, 0x40, 0xFF) : col;

				float thickness = 1.f;
				thickness = (fmodf(fabsf(f), 10.f) < FLT_EPSILON) ? 1.5f : thickness;
				thickness = (fabsf(f) < FLT_EPSILON) ? 2.3f : thickness;


				// ImGuizmo::AddLineToContextDrawList(ptA.x, ptA.y, ptA.z, ptB.x, ptB.y, ptB.z, glm::value_ptr(res), col, thickness);
			}
		}
	}
}
void GizmoEditor::Create(std::shared_ptr<ApplicationEditor> applicationEditor)
{
	if (m_instance != nullptr)
	{
		return;
	}
	m_instance = std::make_shared<GizmoEditor>(applicationEditor);
	m_instance->Register(m_instance);
}
