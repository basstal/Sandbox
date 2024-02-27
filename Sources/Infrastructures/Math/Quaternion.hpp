#pragma once

#include <cmath>
#include <glm/fwd.hpp>

#include "yaml-cpp/yaml.h"


class Quaternion
{
public:
	// 四元数的组成部分
	float w, x, y, z;
	static Quaternion identity;
	// 默认构造函数
	Quaternion(float w = 1.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f) :
		w(w), x(x), y(y), z(z)
	{
	}

	// 从另一个四元数复制构造函数
	Quaternion(const Quaternion& q) :
		w(q.w), x(q.x), y(q.y), z(q.z)
	{
	}

	// 四元数加法
	Quaternion operator+(const Quaternion& q) const
	{
		return Quaternion(w + q.w, x + q.x, y + q.y, z + q.z);
	}

	// 四元数减法
	Quaternion operator-(const Quaternion& q) const
	{
		return Quaternion(w - q.w, x - q.x, y - q.y, z - q.z);
	}

	// 四元数乘法
	Quaternion operator*(const Quaternion& q) const
	{
		return Quaternion(
			w * q.w - x * q.x - y * q.y - z * q.z,
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y - x * q.z + y * q.w + z * q.x,
			w * q.z + x * q.y - y * q.x + z * q.w
		);
	}

	// 四元数的模
	float Norm() const
	{
		return sqrt(w * w + x * x + y * y + z * z);
	}

	// 四元数的共轭
	Quaternion Conjugate() const
	{
		return Quaternion(w, -x, -y, -z);
	}

	// 四元数的归一化
	Quaternion &Normalize()
	{
		float n = Norm();
		if (n > 0.0f)
		{
			float invNorm = 1.0f / n;
			w *= invNorm;
			x *= invNorm;
			y *= invNorm;
			z *= invNorm;
		}
		return *this;
	}

	// 转换为旋转矩阵
	void ToRotationMatrix(float matrix[3][3]) const
	{
		float xx = x * x;
		float xy = x * y;
		float xz = x * z;
		float xw = x * w;
		float yy = y * y;
		float yz = y * z;
		float yw = y * w;
		float zz = z * z;
		float zw = z * w;

		matrix[0][0] = 1 - 2 * (yy + zz);
		matrix[0][1] = 2 * (xy - zw);
		matrix[0][2] = 2 * (xz + yw);

		matrix[1][0] = 2 * (xy + zw);
		matrix[1][1] = 1 - 2 * (xx + zz);
		matrix[1][2] = 2 * (yz - xw);

		matrix[2][0] = 2 * (xz - yw);
		matrix[2][1] = 2 * (yz + xw);
		matrix[2][2] = 1 - 2 * (xx + yy);
	}

	void ToRotationMatrix(glm::mat4& matrix) const;
};

namespace YAML
{
	template <>
	struct convert<Quaternion>
	{
		static Node encode(const Quaternion& rhs)
		{
			Node node;
			node["x"] = rhs.x;
			node["y"] = rhs.y;
			node["z"] = rhs.z;
			node["w"] = rhs.w;
			return node;
		}

		static bool decode(const Node& node, Quaternion& rhs)
		{
			if (!node["x"] || !node["y"] || !node["z"] || !node["w"])
			{
				return false;
			}
			rhs.x = node["x"].as<float>();
			rhs.y = node["y"].as<float>();
			rhs.z = node["z"].as<float>();
			rhs.w = node["w"].as<float>();
			return true;
		}
	};
}
