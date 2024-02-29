#pragma once
#include <string>
#include <glm/mat4x4.hpp>

#include "IEditor.hpp"

class ApplicationEditor;

class ValueEditor : public IEditor
{
private:
	static std::shared_ptr<ValueEditor> m_instance;
	void ConvertGlmMat4ToImGui(const glm::mat4& mat, float out[4][4])
	{
		for (int row = 0; row < 4; ++row)
		{
			for (int col = 0; col < 4; ++col)
			{
				out[row][col] = mat[col][row]; // glm是列主序，而ImGui通常是行主序
			}
		}
	}

	void ConvertImGuiToGlmMat4(const float in[4][4], glm::mat4& out)
	{
		for (int row = 0; row < 4; ++row)
		{
			for (int col = 0; col < 4; ++col)
			{
				out[col][row] = in[row][col]; // glm是列主序，而ImGui通常是行主序
			}
		}
	}

	// 使用示例
	void DisplayMatrixInImGui(glm::mat4& mat, const std::string& matrixName);

	std::shared_ptr<ApplicationEditor> m_applicationEditor;

public:
	void DrawFrame() override;
	static void Create(std::shared_ptr<ApplicationEditor> applicationEditor);
	ValueEditor(std::shared_ptr<ApplicationEditor> applicationEditor)
	{
		m_applicationEditor = applicationEditor;
	}
};
