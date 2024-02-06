#include "Quaternion.hpp"
#include <glm/ext/matrix_transform.hpp>

Quaternion Quaternion::identity = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

void Quaternion::ToRotationMatrix(glm::mat4& matrix) const
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
	matrix[0][1] = 2 * (xy + zw);
	matrix[0][2] = 2 * (xz - yw);
	matrix[0][3] = 0.0f;

	matrix[1][0] = 2 * (xy - zw);
	matrix[1][1] = 1 - 2 * (xx + zz);
	matrix[1][2] = 2 * (yz + xw);
	matrix[1][3] = 0.0f;

	matrix[2][0] = 2 * (xz + yw);
	matrix[2][1] = 2 * (yz - xw);
	matrix[2][2] = 1 - 2 * (xx + yy);
	matrix[2][3] = 0.0f;

	matrix[3][0] = 0.0f;
	matrix[3][1] = 0.0f;
	matrix[3][2] = 0.0f;
	matrix[3][3] = 1.0f;
}
