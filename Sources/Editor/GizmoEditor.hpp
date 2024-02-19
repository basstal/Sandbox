#pragma once
#include "ApplicationEditor.hpp"
#include "IEditor.hpp"

class GizmoEditor : public IEditor
{
private:
	std::shared_ptr<ApplicationEditor> m_applicationEditor;
	static std::shared_ptr<GizmoEditor> m_instance;

public:
	GizmoEditor(std::shared_ptr<ApplicationEditor> applicationEditor);
	~GizmoEditor() override;
	void DrawFrame() override;
	static void Create(std::shared_ptr<ApplicationEditor> applicationEditor);
};