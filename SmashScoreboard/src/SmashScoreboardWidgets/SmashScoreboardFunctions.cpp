#include <SmashScoreboardFunctions.h>

std::vector<SmashScoreboard::CharacterName> SmashScoreboard::characterList;

SmashScoreboard::CharacterName::CharacterName(std::string str)
	:text(str) {}

SmashScoreboard::CharacterName::~CharacterName() {}

const char* SmashScoreboard::CharacterName::getText()
{
	return text.c_str();
}

bool SmashScoreboard::init(const char* pathToFile)
{
	std::fstream characterListFile;
	characterListFile.open(pathToFile, std::ios::in);
	if (characterListFile.is_open())
	{
		std::string line;
		while (std::getline(characterListFile, line))
		{
			characterList.push_back(CharacterName(line));
		}

		if (characterList.size() == 0)
		{
			return false;
		}
	}
	return true;
}

/*
//Smash Scoreboard Help Marker
//
//Code yoinked from imgui_demo.cpp
//Displays a '?' with a tooltop when hovered over
*/
void SmashScoreboard::HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

bool SmashScoreboard::findSubstring(const char* w1, const char* w2)
{
	int i = 0;
	int j = 0;

	if (w1 == "")
	return true;

	if (w2 == NULL)
	w2 = "";

	while (w1[i] != '\0')
	{
		while (w1[i] == w2[j] && w2[j] != '\0')
		{
			i++;
			j++;
		}
		if (w2[j] == '\0') { return true; }

		if (w1[i] != w2[j] && j != 0) { j = 0; }

		i++;
	}
	return false;
}

bool SmashScoreboard::findLowerSubstring(std::string str1, std::string str2)
{
	static std::string wStr1;
	wStr1 = str1;
	std::transform(wStr1.begin(), wStr1.end(), wStr1.begin(),
		[](unsigned char c) { return std::tolower(c); });

	static std::string wStr2;
	wStr2 = str2;
	std::transform(wStr2.begin(), wStr2.end(), wStr2.begin(),
		[](unsigned char c) { return std::tolower(c); });

	const char* w1 = &wStr1[0];
	const char* w2 = &wStr2[0];

	return findSubstring(w1, w2);
}

void SmashScoreboard::uninit() {}