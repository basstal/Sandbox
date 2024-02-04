#pragma once
#include <memory>

#include "ApplicationEditor.hpp"
#include "IEditor.hpp"

class SettingsEditor : public IEditor
{
private:
	std::shared_ptr<ApplicationEditor> m_applicationEditor;
	static std::shared_ptr<SettingsEditor> m_instance;

public:
	SettingsEditor(std::shared_ptr<ApplicationEditor> applicationEditor);
	~SettingsEditor() override;
	void DrawFrame() override;
	static void Create(std::shared_ptr<ApplicationEditor> applicationEditor);
};
