#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl.h>
#include <glad/glad.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <fstream>
//#include <iostream>

namespace SmashScoreboard
{
	extern bool doneWithInit;
	extern bool initSuccessful;

	void init(const char* pathToList);

	struct CharacterName
	{
		std::string text;
		int indexLow, numImages;
		CharacterName(std::string str);
		~CharacterName();
	};

	//Define variables that will help out other widgets

	//set in init()
	extern std::vector<CharacterName> characterList;
	extern std::vector<GLuint> textureList;

	//END VARIABLE DEF

	void StyleColorsRed(ImGuiStyle* dst = (ImGuiStyle*)0);
	void StyleColorsBlue(ImGuiStyle* dst = (ImGuiStyle*)0);
	void StyleColorsYellow(ImGuiStyle* dst = (ImGuiStyle*)0);
	void StyleColorsGreen(ImGuiStyle* dst = (ImGuiStyle*)0);

	void StyleColorsFromIndex(int index);

	void HelpMarker(const char* desc);

	bool findSubstring(const char* w1, const char* w2);
	bool findLowerSubstring(std::string str1, std::string str2);

	GLuint LoadAndInitTex(const char* path);

	void uninit();
}