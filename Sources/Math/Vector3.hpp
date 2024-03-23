#pragma once

#include "Generated/Vector3.rfkh.h"
#include "glm/vec3.hpp"
#include "Serialization/ISerializable.hpp"

namespace Sandbox
NAMESPACE()
{
    class CLASS() Vector3 : public ISerializable<Vector3>
    {
    public:
        FIELD()
        float x = 0.0f;
        FIELD()
        float y = 0.0f;
        FIELD()
        float z = 0.0f;


        Vector3();

        Vector3(float scalar);

        Vector3(const glm::vec3& inGlmVec3);

        Vector3 operator-() const;

        void operator+=(const glm::vec3& inGlmVec3);

        void operator+=(const Vector3& inGlmVec3);

        const Vector3 operator*(const glm::vec3& inGlmVec3) const;

        const Vector3 operator*(float velocity) const;

        glm::vec3 ToGlmVec3();

    private:
        glm::vec3 m_vec = glm::vec3(0.0f);

        void Sync();

        Sandbox_Vector3_GENERATED
    };
}

File_Vector3_GENERATED
