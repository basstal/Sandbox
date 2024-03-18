#pragma once

#include <memory>
#include <glm/matrix.hpp>
#include <boost/hana.hpp>
#include <vulkan/vulkan.hpp>
#include "yaml-cpp/node/node.h"

namespace Sandbox
{
    class UniformBuffer;
    class Buffer;

    struct ViewAndProjection
    {
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 projection;
    };

    // One big uniform buffer that contains all matrices
    // Note that we need to manually allocate the data to cope for GPU-specific uniform buffer offset alignments
    struct Models
    {
        glm::mat4* model = nullptr;
    };

    struct MVPUboObjects
    {
        std::shared_ptr<UniformBuffer> vpUbo;
        std::shared_ptr<UniformBuffer> modelsUbo;
    };

    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum ECameraMovement
    {
        FORWARD = 0,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        MAX,
    };


    /**
 * \brief 相机持久化数据
 */
    struct CameraProperty
    {
        /**
         * \brief 位置
         */
        glm::vec3 position = glm::vec3(0.0f);
        /**
         * \brief X 轴旋转角度
         */
        float rotationX = 0.0f;
        /**
         * \brief Z 轴旋转角度
         */
        float rotationZ = 0.0f;
        /**
         * \brief 视野
         */
        float fieldOfView = 45.0f;
        /**
         * \brief 近平面
         */
        float nearPlane = 0.1f;
        /**
         * \brief 远平面
         */
        float farPlane = 100.0f;
        /**
         * \brief 纵横比
         */
        float aspectRatio = 1.0f;
    };

    class Camera
    {
    public:
        Camera(glm::vec3 inWorldUp, float aspectRatio, std::shared_ptr<CameraProperty> inProperty);

        /**
         * Returns the view matrix calculated using Euler Angles and the LookAt Matrix
         */
        glm::mat4 GetViewMatrix();

        /**
         * Returns the projection matrix
         */
        glm::mat4 GetProjectionMatrix();

        /**
         * Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
         */
        void ProcessKeyboard(ECameraMovement direction, float deltaTime);

        /**
         * \brief 处理相机的 Yaw 旋转
         * \param delta 旋转角度
         */
        void CameraYawRotate(float delta);

        /**
         * \brief 处理相机的 Pitch 旋转
         * \param delta 旋转角度
         */
        void CameraPitchRotate(float delta);

        /**
         * Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
         */
        void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

        /**
         * \brief 重置相机
         */
        void Reset();

        /**
         * Calculates the front vector from the Camera's (updated) Euler Angles
         */
        void UpdateCameraVectors();

        std::shared_ptr<CameraProperty> property;
        glm::vec3 worldUp;
        glm::vec3 front;
        glm::vec3 right;
        glm::vec3 up;
    };
}

BOOST_HANA_ADAPT_STRUCT(Sandbox::CameraProperty, position, rotationX, rotationZ);


namespace YAML
{
    template <>
    struct convert<Sandbox::CameraProperty>
    {
        static Node encode(const Sandbox::CameraProperty& rhs)
        {
            Node node;
            node["position"] = rhs.position;
            node["rotationX"] = rhs.rotationX;
            node["rotationZ"] = rhs.rotationZ;
            return node;
        }

        static bool decode(const Node& node, Sandbox::CameraProperty& rhs)
        {
            if (!node["position"] || !node["rotationX"] || !node["rotationZ"])
            {
                return false;
            }
            rhs.position = node["position"].as<glm::vec3>();
            rhs.rotationX = node["rotationX"].as<float>();
            rhs.rotationZ = node["rotationZ"].as<float>();
            return true;
        }
    };

    template <>
    struct convert<std::shared_ptr<Sandbox::CameraProperty>>
    {
        static Node encode(const std::shared_ptr<Sandbox::CameraProperty>& rhs)
        {
            return convert<Sandbox::CameraProperty>::encode(*rhs);
        }

        static bool decode(const Node& node, std::shared_ptr<Sandbox::CameraProperty>& rhs)
        {
            Sandbox::CameraProperty value;
            bool decoded = convert<Sandbox::CameraProperty>::decode(node, value);
            if (decoded)
            {
                rhs = std::make_shared<Sandbox::CameraProperty>(value);
            }
            return decoded;
        }
    };
}
