#include "IEditor.hpp"

std::vector<std::shared_ptr<IEditor>> IEditor::registeredEditors = std::vector<std::shared_ptr<IEditor>>();

int IEditor::m_nextId = 0;


void IEditor::Register(std::shared_ptr<IEditor> editor)
{
	for (auto it = registeredEditors.begin(); it != registeredEditors.end(); ++it)
	{
		if (*it->get() == *editor)
		{
			return;
		}
	}
	registeredEditors.push_back(editor);
}

void IEditor::Unregister()
{
	for (auto it = registeredEditors.cbegin(); it != registeredEditors.cend(); ++it)
	{
		if (*it->get() == *this)
		{
			registeredEditors.erase(it);
		}
	}
}
