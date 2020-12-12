#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl.h>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
//#include <iostream>

namespace SmashScoreboard
{
	bool init(const char* pathToList);

	class CharacterName
	{
	private:
		std::string text;
	public:
		CharacterName(std::string str);
		~CharacterName();
		const char* getText();
	};

	//Define variables that will help out other widgets

	//set in init()
	extern std::vector<CharacterName> characterList;

	//END VARIABLE DEF

	void StyleColorsPlayer1(ImGuiStyle* dst = (ImGuiStyle*) 0);

	void HelpMarker(const char* desc);

	bool findSubstring(const char* w1, const char* w2);
	bool findLowerSubstring(std::string str1, std::string str2);

	void uninit();
}