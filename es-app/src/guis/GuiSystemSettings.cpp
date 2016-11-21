#include "EmulationStation.h"
#include "guis/GuiStorageInfo.h"
#include "guis/GuiSystemSettings.h"
#include "Window.h"
#include "Sound.h"
#include "Log.h"
#include "Settings.h"
#include "guis/GuiMsgBox.h"
#include "guis/GuiSettings.h"
#include "guis/GuiScraperStart.h"
#include "guis/GuiDetectDevice.h"
#include "guis/GuiWifi.h"
#include "views/ViewController.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <stdlib.h>

#include "components/ButtonComponent.h"
#include "components/SwitchComponent.h"
#include "components/SliderComponent.h"
#include "components/TextComponent.h"
#include "components/OptionListComponent.h"
#include "components/MenuComponent.h"
#include "VolumeControl.h"
#include "scrapers/GamesDBScraper.h"
#include "scrapers/TheArchiveScraper.h"
#include "guis/GuiTextEditPopup.h"

GuiSystemSettings::GuiSystemSettings(Window* window) : GuiComponent(window), mMenu(window, "SYSTEM SETTINGS")
{
	// SYSTEM SETTINGS

	// UPDATES >
	// NETWORK SETTINGS >
	// STORAGE >

	// [version]

	addEntry("SYSTEM UPDATE", mMenu.getTextColor(), true, [this, window] { 

			auto s = new GuiSettings(mWindow, "SYSTEM UPDATE");

			ComponentListRow row;
			auto cb = [this] { 
				system("./systemupdate.sh");
				SDL_Event ev;
				ev.type = SDL_QUIT;
				SDL_PushEvent(&ev);
			};
			row.addElement(std::make_shared<TextComponent>(mWindow, "GET LATEST BINARY", Font::get(FONT_SIZE_MEDIUM), 0x777777FF), true);
			row.makeAcceptInputHandler(cb);
			s->addRow(row);


			mWindow->pushGui(s);
	});

	/// Change network settings
	addEntry("NETWORK SETTINGS", mMenu.getTextColor(), true, [this, window] {
		mWindow->pushGui(new GuiWifi(mWindow));
	});

	/// See storage on internal memory card.
	addEntry("STORAGE", mMenu.getTextColor(), true, [this] {
		mWindow->pushGui(new GuiStorageInfo(mWindow));
	});

	mMenu.setFooter("BUILD DATE: " + strToUpper(PROGRAM_BUILT_STRING));

	addChild(&mMenu);

	setSize(mMenu.getSize());
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, Renderer::getScreenHeight() * 0.15f);
}

void GuiSystemSettings::onSizeChanged()
{

}

void GuiSystemSettings::addEntry(const char* name, unsigned int color, bool add_arrow, const std::function<void()>& func)
{
	std::shared_ptr<Font> font = Font::get(FONT_SIZE_MEDIUM);
	
	// populate the list
	ComponentListRow row;
	row.addElement(std::make_shared<TextComponent>(mWindow, name, font, color), true);

	if(add_arrow)
	{
		std::shared_ptr<ImageComponent> bracket = makeArrow(mWindow);
		row.addElement(bracket, false);
	}
	
	row.makeAcceptInputHandler(func);

	mMenu.addRow(row);
}



bool GuiSystemSettings::input(InputConfig* config, Input input)
{
	if(GuiComponent::input(config, input))
		return true;

	if((config->isMappedTo("b", input) || config->isMappedTo("start", input)) && input.value != 0)
	{
		delete this;
		return true;
	}

	return false;
}

std::vector<HelpPrompt> GuiSystemSettings::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts;
	prompts.push_back(HelpPrompt("up/down", "choose"));
	prompts.push_back(HelpPrompt("a", "select"));
	prompts.push_back(HelpPrompt("start", "close"));
	return prompts;
}
