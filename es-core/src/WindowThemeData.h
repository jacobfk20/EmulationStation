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
};

struct WindowTheme {
	std::string name;
	WindowThemeElement background;
	WindowThemeElement title;
	WindowThemeElement footer;
	WindowThemeElement default_text;
	WindowThemeElement button{ 0xFFFFFFFF, 0x777777FF, "", "", ALIGN_CENTER };
	WindowThemeElement option_list{ default_text.color_focused, default_text.color, ":/option_arrow.svg", ":/option_arrow.svg", ALIGN_CENTER };
	unsigned int spacer_color;
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

private:
	WindowThemeData();

	bool parseFile(std::string path);
	void generateDefault();
	Alignment getAlignment(std::string align);

	std::map<std::string, WindowTheme> mThemeMap;

	unsigned int getHexColor(const char* str);
	void getElementData(pugi::xml_node node, WindowThemeElement* themeElement);

	WindowTheme* mCurrentTheme;

	static WindowThemeData* sInstance;
};
