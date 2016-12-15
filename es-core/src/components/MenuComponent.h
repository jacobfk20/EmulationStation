#pragma once

#include "components/NinePatchComponent.h"
#include "components/ComponentList.h"
#include "components/TextComponent.h"
#include "components/ComponentGrid.h"
#include "WindowThemeData.h"
#include "Util.h"

class ButtonComponent;
class ImageComponent;

std::shared_ptr<ComponentGrid> makeButtonGrid(Window* window, const std::vector< std::shared_ptr<ButtonComponent> >& buttons);
std::shared_ptr<ImageComponent> makeArrow(Window* window);

#define TITLE_VERT_PADDING (Renderer::getScreenHeight()*0.0637f)

class MenuComponent : public GuiComponent
{
public:
	MenuComponent(Window* window, const char* title, const std::shared_ptr<Font>& titleFont = Font::get(FONT_SIZE_LARGE, WindowThemeData::get()->title.path));

	void onSizeChanged() override;

	void setTheme();

	inline void addRow(const ComponentListRow& row, bool setCursorHere = false) { mList->addRow(row, setCursorHere); updateSize(); }

	inline void addWithLabel(const std::string& label, const std::shared_ptr<GuiComponent>& comp, bool setCursorHere = false, bool invert_when_selected = true)
	{
		ComponentListRow row;
		row.addElement(std::make_shared<TextComponent>(mWindow, strToUpper(label), Font::get(FONT_SIZE_MEDIUM), getTextColor()), true);
		row.addElement(comp, false, invert_when_selected);
		addRow(row, setCursorHere);
	}

	void addButton(const std::string& label, const std::string& helpText, const std::function<void()>& callback);

	void setTitle(const char* title, const std::shared_ptr<Font>& font);
	void setFooter(const std::string& footer, const std::shared_ptr<Font>& font = Font::get(FONT_SIZE_SMALL * WindowThemeData::get()->footer.size, WindowThemeData::get()->footer.path));

	inline void setCursorToList() { mGrid.setCursorTo(mList); }
	inline void setCursorToButtons() { assert(mButtonGrid); mGrid.setCursorTo(mButtonGrid); }

	virtual std::vector<HelpPrompt> getHelpPrompts() override;

	// Returns the default text color from Window Theme.
	unsigned int getTextColor() { return mTextColor; };

private:
	void updateSize();
	void updateGrid();
	float getButtonGridHeight() const;

	NinePatchComponent mBackground;
	ComponentGrid mGrid;

	unsigned int mTextColor = 0x777777FF;

	std::shared_ptr<TextComponent> mTitle;
	std::shared_ptr<TextComponent> mFooter;
	std::shared_ptr<ComponentList> mList;
	std::shared_ptr<ComponentGrid> mButtonGrid;
	std::vector< std::shared_ptr<ButtonComponent> > mButtons;
};
