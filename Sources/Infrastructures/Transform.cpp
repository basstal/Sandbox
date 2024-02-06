#include "Transform.hpp"

#include <glm/ext/matrix_transform.hpp>
glm::mat4 Transform::GetModelMatrix() const
{
	// 创建平移矩阵
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);

	// 从四元数创建旋转矩阵
	glm::mat4 rotationMatrix;
	rotation.ToRotationMatrix(rotationMatrix);

	// 创建缩放矩阵
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

	// 合并矩阵: 注意乘法顺序是反向的
	return translation * rotationMatrix * scaleMatrix;
}
