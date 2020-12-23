#include <SmashScoreboardFunctions.h>
#include <SDL.h>
#include <SDL_image.h>

std::vector<SmashScoreboard::CharacterName> SmashScoreboard::characterList;
std::map<std::string, GLuint> SmashScoreboard::textureList;
bool SmashScoreboard::doneWithInit = false;
bool SmashScoreboard::initSuccessful = false;

SmashScoreboard::CharacterName::CharacterName(std::string str)
	:text(str) {}

SmashScoreboard::CharacterName::~CharacterName() {}

void SmashScoreboard::init(const char* pathToFile)
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
			initSuccessful = false;
		}
		else
		{
			for (int i = 0; i < characterList.size(); i++)
			{
				std::string path("res/ImageCache/Smash Ultimate Full Art/");
				path += characterList[i].text;
				path += "/";
				path += characterList[i].text;
				path += "_01";
				path += ".png";

				GLuint loadedImage = LoadAndInitTex(path.c_str());

				textureList.emplace(characterList[i].text, loadedImage);
			}
			initSuccessful = true;
		}
	}
	else
	{
		initSuccessful = false;
	}
	doneWithInit = true;
}

void SmashScoreboard::StyleColorsPlayer1(ImGuiStyle* dst)
{
	ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 0.69f, 0.69f, 0.84f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(1.00f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.00f, 0.00f, 0.82f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.46f, 0.54f, 0.80f, 0.60f);
	colors[ImGuiCol_Button] = ImVec4(0.82f, 0.00f, 0.00f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.82f, 0.00f, 0.00f, 0.65f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.82f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(1.00f, 0.00f, 0.00f, 0.28f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.28f, 0.31f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 0.14f, 0.09f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 0.62f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.90f);
	colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
	colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
	colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_NavHighlight] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
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

GLuint SmashScoreboard::LoadAndInitTex(const char* path)
{
	SDL_Surface* surf = IMG_Load(path);

	static int idNum = 999;
	idNum += 1;

	GLuint id(idNum);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	int mode = GL_RGB;
	if (surf->format->BytesPerPixel == 4)
		mode = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, mode, surf->w, surf->h, 0, mode, GL_UNSIGNED_BYTE, surf->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	SDL_FreeSurface(surf);

	return id;
}

void SmashScoreboard::uninit() {}