#include "WindowThemeData.h"
#include "Renderer.h"
#include "resources/Font.h"
#include "Sound.h"
#include "resources/TextureResource.h"
#include "Log.h"
#include "Settings.h"
#include "pugixml/pugixml.hpp"
#include <boost/assign.hpp>
#include <fstream>
#include <sstream>

#include "components/ImageComponent.h"
#include "components/TextComponent.h"

namespace fs = boost::filesystem;

WindowThemeData* WindowThemeData::sInstance = NULL;


WindowThemeData* WindowThemeData::getInstance() {
	if (sInstance == NULL)
		sInstance = new WindowThemeData();

	return sInstance;
}


WindowTheme* WindowThemeData::get() {
	return getInstance()->getCurrentTheme();
}


WindowThemeData::WindowThemeData() {
	// get all from WindowTheme folder
	fs::path wtPath(getHomePath() + "/.emulationstation/WindowThemes");
	string_path = getHomePath() + "/.emulationstation/WindowThemes";

	generateFiles();

	fs::directory_iterator end_itr;
	for (fs::directory_iterator itr(wtPath); itr != end_itr; ++itr) {
		parseFile(itr->path().generic_string());
	}

	// check if selected theme exists and set it
	if (Settings::getInstance()->getString("WindowTheme") == "") {
		setTheme("ES-Light");
		Settings::getInstance()->setString("Windowtheme", "ES-Light");
	}
	else
		setTheme(Settings::getInstance()->getString("WindowTheme"));

}


// helper
unsigned int WindowThemeData::getHexColor(const char* str)
{
	ThemeException error;
	if (!str)
		throw error << "Empty color";

	size_t len = strlen(str);
	if (len != 6 && len != 8)
		throw error << "Invalid color (bad length, \"" << str << "\" - must be 6 or 8)";

	unsigned int val;
	std::stringstream ss;
	ss << str;
	ss >> std::hex >> val;

	if (len == 6)
		val = (val << 8) | 0xFF;

	return val;
}


void WindowThemeData::generateFiles() {
	// make sure path exists or create it
	if (!fs::exists(getHomePath() + "/.emulationstation/WindowThemes"))
		fs::create_directory(getHomePath() + "/.emulationstation/WindowThemes");

	// create light and dark themes if they do not exist
	if (!fs::exists(getHomePath() + "/.emulationstation/WindowThemes/es-light.xml")) {
		// Create light theme
		std::fstream file(getHomePath() + "/.emulationstation/WindowThemes/es-light.xml", std::fstream::out);
		file << "<WindowTheme name=\"ES-Light\">\n";
		file << "    <text-default>\n";
		file << "        <color>777777FF</color>\n";
		file << "        <font></font>\n";
		file << "    </text-default>\n";
		file << "    <window>\n";
		file << "        <background>\n";
		file << "            <color>FFFFFFFF</color>\n";
		file << "            <path></path>\n";
		file << "        </background>\n";
		file << "        <title>\n";
		file << "            <color>888888FF</color>\n";
		file << "            <alignment>center</alignment>\n";
		file << "            <font></font>\n";
		file << "        </title>\n";
		file << "        <footer>\n";
		file << "            <color>999999FF</color>\n";
		file << "            <alignment>center</alignment>\n";
		file << "            <font></font>\n";
		file << "        </footer>\n";
		file << "        <spacer>\n";
		file << "            <color>AAAACCFF</color>\n";
		file << "        </spacer>\n";
		file << "    </window>\n";
		file << "</WindowTheme>\n";
		file.close();
	}

	// Create dark theme if none exist
	if (!fs::exists(getHomePath() + "/.emulationstation/WindowThemes/es-dark.xml")) {
		// Create light theme
		std::fstream file(getHomePath() + "/.emulationstation/WindowThemes/es-dark.xml", std::fstream::out);
		file << "<WindowTheme name=\"ES-Dark\">\n";
		file << "    <text-default>\n";
		file << "        <color>AAAAAAFF</color>\n";
		file << "        <font></font>\n";
		file << "    </text-default>\n";
		file << "    <window>\n";
		file << "        <highlight>\n";
		file << "            <color>00000000</color>\n";
		file << "        </highlight>\n";
		file << "        <background>\n";
		file << "            <color>555555FF</color>\n";
		file << "            <path></path>\n";
		file << "        </background>\n";
		file << "        <title>\n";
		file << "            <color>CCCCCCFF</color>\n";
		file << "            <alignment>center</alignment>\n";
		file << "            <font></font>\n";
		file << "        </title>\n";
		file << "        <footer>\n";
		file << "            <color>AAAAFFFF</color>\n";
		file << "            <alignment>center</alignment>\n";
		file << "            <font></font>\n";
		file << "        </footer>\n";
		file << "        <spacer>\n";
		file << "            <color>AAAACCFF</color>\n";
		file << "        </spacer>\n";
		file << "        <button>\n";
		file << "            <color>AAAAFFFF</color>\n";
		file << "            <color_focused>AAAAFFFF</color_focused>\n";
		file << "        </button>\n";
		file << "    </window>\n";
		file << "</WindowTheme>\n";
		file.close();
	}
}


std::vector<std::string> WindowThemeData::getThemeNames() {
	std::vector<std::string> vNames;

	for (auto m = mThemeMap.begin(); m != mThemeMap.end(); m++) {
		vNames.push_back((*m).first);
	}

	return vNames;
}


bool WindowThemeData::setTheme(std::string name) {
	if (mThemeMap[name].name == "")
		return false;

	mCurrentTheme = &mThemeMap[name];

	if (mCurrentTheme == NULL) return false;

	return true;
}


bool WindowThemeData::parseFile(std::string path) {
	if (!fs::exists(path)) {
		LOG(LogWarning) << "The WindowTheme could not be found: " << path;
		return false;
	}

	// check file is xml
	if (path.find(".xml") == std::string::npos) return false;

	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file(path.c_str());
	if (!res)
		LOG(LogError) << "XML parsing error: \n    " << res.description();

	pugi::xml_node root = doc.child("WindowTheme");
	if (!root)
		LOG(LogError) << "Missing <WindowTheme> tag!";

	// create a new WindowTheme Object
	WindowTheme wintheme;

	// get theme name
	if (!root.attribute("name")) {
		LOG(LogError) << "WindowTheme is missing a name.  Please use <WindowTheme name=\"THEME_NAME\">  Bailing...";
		return false;
	}

	wintheme.name = root.attribute("name").as_string();

	// get text-default:
	pugi::xml_node node = root.child("text-default");
	if (node) getElementData(node, &wintheme.default_text);

	// Start getting Window stuff:
	pugi::xml_node window = root.child("window");
	pugi::xml_node element;

	// ------------------ UNIQUE THEME ITEMS ---------------
	// Spacer (the line between list items)
	element = window.child("spacer");
	if (element) {
		if (element.child("color")) wintheme.spacer_color = getHexColor(element.child("color").text().as_string());
	}

	element = window.child("highlight");
	if (element.child("color")) wintheme.highlight_color = getHexColor(element.child("color").text().as_string());

	// Arrow
	element = window.child("arrow");
	if (element)
		if (element.child("path")) wintheme.arrow = string_path + "/" + element.child("path").text().as_string();


	// ----------------- NORMAL THEME ITEMS ----------------
	// Background
	element = window.child("background");
	if (element) getElementData(element, &wintheme.background);

	// Get Title theme
	element = window.child("title");
	if (element) getElementData(element, &wintheme.title);

	// Footer
	element = window.child("footer");
	if (element) getElementData(element, &wintheme.footer);

	// Button
	element = window.child("button");
	if (element) getElementData(element, &wintheme.button);

	// Option List
	element = window.child("option_list");
	// get default text color in case theme doesn't apply one
	wintheme.option_list.color = wintheme.default_text.color;
	if (element) getElementData(element, &wintheme.option_list);

	// Switch Component
	element = window.child("switch");
	if (element) getElementData(element, &wintheme.Switch);

	// Slider Component
	element = window.child("slider");
	if (element) getElementData(element, &wintheme.slider);

	// push this new theme to the map
	mThemeMap[wintheme.name] = wintheme;
}


void WindowThemeData::generateDefault() {
	WindowTheme dtheme;
	dtheme.background.color = 0xFFFFFFFF;
	dtheme.default_text.color = 0x777777FF;
	dtheme.footer.color = 0xAAAAAAFF;
	dtheme.footer.alignment = ALIGN_CENTER;
	dtheme.spacer_color = 0xC6C7C6FF;
	dtheme.title.color = 0x999999FF;
	dtheme.title.alignment = ALIGN_CENTER;
	dtheme.option_list.color = dtheme.default_text.color;

	dtheme.name = "default";

	mThemeMap[dtheme.name] = dtheme;
}


Alignment WindowThemeData::getAlignment(std::string align) {
	if (align == "left") return ALIGN_LEFT;
	if (align == "right") return ALIGN_RIGHT;
	if (align == "center") return ALIGN_CENTER;
	return ALIGN_CENTER;
}


void WindowThemeData::getElementData(pugi::xml_node node, WindowThemeElement* themeElement) {
	if (node.child("color")) themeElement->color = getHexColor(node.child("color").text().as_string());
	if (node.child("color_focused")) themeElement->color_focused = getHexColor(node.child("color_focused").text().as_string());
	if (node.child("size")) themeElement->size = node.child("size").text().as_float();
	else themeElement->size = 1;
	if (node.child("path")) themeElement->path = string_path + "/" + node.child("path").text().as_string();
	if (node.child("path_focused")) themeElement->path_focused = string_path + "/" + node.child("path_focused").text().as_string();
	if (node.child("path_selected")) themeElement->path_selected = string_path + "/" + node.child("path_selected").text().as_string();
	if (node.child("alignment")) themeElement->alignment = getAlignment(node.child("alignment").text().as_string());
	else themeElement->alignment = ALIGN_CENTER;
}

