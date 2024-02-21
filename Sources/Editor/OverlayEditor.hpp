#pragma once
#include "ApplicationEditor.hpp"
#include "IEditor.hpp"

class OverlayEditor : public IEditor
{
private:
	std::shared_ptr<ApplicationEditor> m_applicationEditor;
	static std::shared_ptr<OverlayEditor> m_instance;
	bool m_open = true;
public:
	OverlayEditor(std::shared_ptr<ApplicationEditor> applicationEditor);
	~OverlayEditor() override;
	void DrawFrame() override;
	static void Create(std::shared_ptr<ApplicationEditor> applicationEditor);
};
