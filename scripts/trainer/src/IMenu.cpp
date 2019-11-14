#include "pch.h"
#include "IMenu.h"

const std::vector<IMenu*>& CMainMenu::GetChildItems()
{
	return childItems;
}

eMenuType CMainMenu::GetType()
{
	return eMenuType::MAIN_MENU;
}

IMenu* CMainMenu::GetParent()
{
	return nullptr;
}

std::string& CMainMenu::GetTitle()
{
	return title;
}

void CMainMenu::AddChildItem(IMenu* child)
{
	childItems.push_back(child);
}

size_t CMainMenu::ChildCount()
{
	return childItems.size();
}

const std::vector<IMenu*>& CSubMenu::GetChildItems()
{
	return childItems;
}

eMenuType CSubMenu::GetType()
{
	return eMenuType::SUB_MENU;
}

IMenu* CSubMenu::GetParent()
{
	return parent;
}

std::string& CSubMenu::GetTitle()
{
	return title;
}

void CSubMenu::AddChildItem(IMenu* child)
{
	childItems.push_back(child);
}

size_t CSubMenu::ChildCount()
{
	return childItems.size();
}

const std::vector<IMenu*>& CMenuAction::GetChildItems()
{
	static std::vector<IMenu*> empty;
	return empty;
}

eMenuType CMenuAction::GetType()
{
	return eMenuType::MENU_ACTION;
}

IMenu* CMenuAction::GetParent()
{
	return nullptr;
}

std::string& CMenuAction::GetTitle()
{
	return title;
}

std::function<void(void)> CMenuAction::GetActionFunc() const
{
	return actionFunc;
}

size_t CMenuAction::ChildCount()
{
	return 0;
}
