#pragma once
#include <vector>
#include <string>
#include <functional>

enum eMenuType
{
	MAIN_MENU,
	SUB_MENU,
	MENU_ACTION
};

class IMenu
{
public:
	virtual const std::vector<IMenu*>& GetChildItems() = 0;
	virtual eMenuType GetType() = 0;
	virtual IMenu* GetParent() = 0;
	virtual std::string& GetTitle() = 0;
	virtual size_t ChildCount() = 0;
};

class CMainMenu : public IMenu
{
public:
	CMainMenu(std::string _title) :title(_title)
	{

	}

	// Унаследовано через IMenu
	virtual const std::vector<IMenu*>& GetChildItems() override;
	virtual eMenuType GetType() override;
	virtual IMenu* GetParent() override;
	virtual std::string& GetTitle() override;

	void AddChildItem(IMenu* child);

private:
	std::vector<IMenu*> childItems;
	std::string title;

	// Унаследовано через IMenu
	virtual size_t ChildCount() override;
};

class CSubMenu : public IMenu
{
public:
	CSubMenu(std::string _title, IMenu* _parent) : title(_title), parent(_parent)
	{

	}

	// Унаследовано через IMenu
	virtual const std::vector<IMenu*>& GetChildItems() override;
	virtual eMenuType GetType() override;
	virtual IMenu* GetParent() override;
	virtual std::string& GetTitle() override;

	void AddChildItem(IMenu* child);

private:
	std::vector<IMenu*> childItems;
	std::string title;
	IMenu* parent;

	// Унаследовано через IMenu
	virtual size_t ChildCount() override;
};

class CMenuAction : public IMenu
{
public:
	CMenuAction(std::string _title, std::function<void(void)> _actionFunc) : title(_title), actionFunc(_actionFunc)
	{

	}

	// Унаследовано через IMenu
	virtual const std::vector<IMenu*>& GetChildItems() override;
	virtual eMenuType GetType() override;
	virtual IMenu* GetParent() override;
	virtual std::string& GetTitle() override;

	std::function<void(void)> GetActionFunc() const;

private:
	std::string title;
	std::function<void(void)> actionFunc;

	// Унаследовано через IMenu
	virtual size_t ChildCount() override;
};
