#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <vector>
#include <deque>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/variant.hpp>
#include <Eigen/Dense>
#include "pugixml/pugixml.hpp"
#include "GuiComponent.h"
#include "components/TextComponent.h"

template<typename T>
class TextListComponent;

class Sound;
class ImageComponent;
class NinePatchComponent;
class TextComponent;
class Window;


struct WindowThemeElement {
	unsigned int color_focused;
	unsigned int color;
	std::string path;
	std::string path_focused;
	Alignment alignment;
	std::string path_selected;		// Mostly for switch component "on"
	float size;
};

struct WindowTheme {
	std::string name;
	WindowThemeElement background;
	WindowThemeElement title;
	WindowThemeElement footer;
	WindowThemeElement default_text;
	WindowThemeElement button{ 0xFFFFFFFF, 0x777777FF, "", "", ALIGN_CENTER, "", 1 };
	WindowThemeElement option_list{ default_text.color_focused, default_text.color, ":/option_arrow.svg", ":/option_arrow.svg", ALIGN_CENTER, "", 1 };
	WindowThemeElement Switch{ 0xFFFFFFFF, 0xFFFFFFFF, ":/off.svg", "", ALIGN_CENTER, ":/on.svg", 1 };
	WindowThemeElement slider{ 0xFFFFFFFF, 0xFFFFFFFF, ":/slider_knob.svg", "", ALIGN_CENTER, "", 1 };
	unsigned int spacer_color = 0xFFFFFFFF;
	unsigned int highlight_color = 0x777777FF;
	std::string arrow = ":/arrow.svg";
};


class WindowThemeData
{
public:
	static WindowThemeData* getInstance();

	// Returns a vector of names for all themes (mainly for showing in a window list)
	std::vector<std::string> getThemeNames();

	// Sets the WindowTheme to this
	bool setTheme(std::string name);

	// Returns the amount of themes that are loaded in
	int getThemeCount() { return mThemeMap.size(); };

	// Returns the current selected theme
	WindowTheme* getCurrentTheme() { return mCurrentTheme; };

	// Gets instance and returns current theme.  example: WindowThemeData::get()->footer.size;
	// Instead of WindowThemeData::getInstance()->getCurrentTheme()->footer.size;
	static WindowTheme* get();

private:
	WindowThemeData();

	std::string string_path;

	bool parseFile(std::string path);
	void generateDefault();
	Alignment getAlignment(std::string align);
	void generateFiles();

	std::map<std::string, WindowTheme> mThemeMap;

	unsigned int getHexColor(const char* str);
	void getElementData(pugi::xml_node node, WindowThemeElement* themeElement);

	WindowTheme* mCurrentTheme;

	static WindowThemeData* sInstance;
};
