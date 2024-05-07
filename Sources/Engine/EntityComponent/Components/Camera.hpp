﻿#pragma once

#include <glm/matrix.hpp>
#include <memory>

#include "Engine/EntityComponent/IComponent.hpp"
#include "Engine/RendererSource/RendererSource.hpp"
#include "Generated/Camera.rfkh.h"
#include "Math/Vector3.hpp"
#include "Standard/Dictionary.hpp"

namespace Sandbox NAMESPACE()
{
    class UniformBuffer;
    class ShaderSource;
    class PostProcess;
    class Renderer;
    class Skybox;

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


    class CLASS() Camera : public IComponent
    {
    public:
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

        Camera();

        // ~Camera();
        Camera(float inAspectRatio);

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
        void ProcessMouseMovement(float xOffset, float yOffset);

        /**
         * \brief 重置相机
         */
        void Reset();

        /**
         * Calculates the front vector from the Camera's (updated) Euler Angles
         */
        void UpdateCameraVectors();

        bool IsValid();

        // /**
        //  * \brief 位置
        //  */
        // FIELD()
        // Vector3 position;
        /**
         * \brief X 轴旋转角度
         */
        FIELD()
        float rotationX = 0.0f;
        /**
         * \brief Z 轴旋转角度
         */
        FIELD()
        float rotationZ = 0.0f;
        /**
         * \brief 视野
         */
        FIELD()
        float fieldOfView = 45.0f;
        /**
         * \brief 近平面
         */
        FIELD()
        float nearPlane = 0.1f;
        /**
         * \brief 远平面
         */
        FIELD()
        float farPlane = 100.0f;
        /**
         * \brief 纵横比
         */
        FIELD()
        float aspectRatio = 16 / 9.0f;

        FIELD()
        bool useSkybox = true;

        std::shared_ptr<Skybox> skybox;

        // TODO:临时数据
        std::vector<std::string> skyboxImagePaths = {
            "Textures/skybox/left.png", "Textures/skybox/right.png", "Textures/skybox/front.png",
            "Textures/skybox/back.jpg", "Textures/skybox/top.png",   "Textures/skybox/bottom.jpg",
        };
        glm::vec3 worldUp;
        Vector3   worldUp1;
        glm::vec3 front;
        Vector3   front1;
        glm::vec3 right;
        Vector3   right1;
        glm::vec3 up;
        Vector3   up1;


        std::string GetDerivedClassName() override;

        const rfk::Class* GetDerivedClass() override;

        Vector3 NdcToWorld(const float& x, const float& y, const float& z);

        Vector3 NdcToWorld(const Vector3& ndcCoordinate);

        void Cleanup() override;

        Dictionary<std::shared_ptr<ShaderSource>, bool> postProcessFragShaders;
        std::shared_ptr<PostProcess>                    postProcess;

        static Event<const std::shared_ptr<Camera>&> onCameraChange;

    private:
        bool m_cleaned;
        Sandbox_Camera_GENERATED
	};
} // namespace Sandbox NAMESPACE()

File_Camera_GENERATED
