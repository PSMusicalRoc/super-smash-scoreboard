#include "Widgets/CharacterSelectWindow.h"

#ifdef _WIN32
	#include <Windows.h>
	#include <direct.h>
	#pragma push_macro("CreateWindow")
	#undef CreateWindow
	#define GetCurrentDir _getcwd
#endif

namespace fs = std::filesystem;

// [SECTION] CharacterSelectWindow()

SmashScoreboard::CharacterSelectWindow::CharacterSelectWindow(std::string winName, int styleIndex)
	:SmashScoreboardWindow(winName), pName(""), styleIndex(styleIndex)
{
	if (this->windowName == "" or this->windowName == "default")
		this->windowName == "Character Selection Window";
}

SmashScoreboard::CharacterSelectWindow* SmashScoreboard::CharacterSelectWindow::CreateWindow(std::string winName, int styleIndex)
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<CharacterSelectWindow>(winName, styleIndex);
	windowList.push_back(win);
	return (CharacterSelectWindow*)win.get();
}

void SmashScoreboard::CharacterSelectWindow::perframe()
{
	if (this->isVisible)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);

		StyleColorsFromIndex(this->styleIndex);
		
		ImGuiWindowFlags flags = 0;

		ImGui::Begin((this->windowName + "###" + this->windowName).c_str() , &(this->isVisible), flags);
		ImGui::InputTextWithHint("", "Player 1's character", pName, IM_ARRAYSIZE(pName)); ImGui::SameLine();
		SmashScoreboard::HelpMarker("Input the name of Player 1's character (eg. Mario)");

		int WidthTakenUp = 0;

		for (int i = 0; i < SmashScoreboard::characterList.size(); i++)
		{
			CharacterName& cname = SmashScoreboard::characterList[i];
			if (SmashScoreboard::findLowerSubstring(cname.text.c_str(), pName))
			{
				if (ImGui::CollapsingHeader(cname.text.c_str()))
				{
					//if (ImGui::Button((cname.text + "##button").c_str()))
					for (int j = cname.indexLow; j < cname.indexLow + cname.numImages; j++)
					{
						if (ImGui::ImageButton((ImTextureID)textureList[j], ImVec2(64, 64)))
						{
							int imageNum = j - cname.indexLow + 1;

							const auto copyOptions = fs::copy_options::overwrite_existing;
							std::string topath = "Output/" + this->windowName + ".png";
							fs::path to = topath;
							std::string frompath = SmashScoreboard::resPath;
							frompath += SmashScoreboard::characterList[i].text.c_str();
							frompath += "/";
							frompath += SmashScoreboard::characterList[i].text.c_str();
							if (imageNum < 10)
								frompath += "_0" + std::to_string(imageNum);
							else if (imageNum >= 10)
								frompath += "_" + std::to_string(imageNum);
							frompath += ".png";
							fs::path from = frompath;
							fs::copy_file(from, to, copyOptions);

#ifdef __linux__
							std::fstream changingModifiedTime;
							changingModifiedTime.open(topath, std::ios::out | std::ios::app);
							if (changingModifiedTime.is_open())
							{
								changingModifiedTime.close();
							}
#elif _WIN32
							
							SYSTEMTIME currentSystemTime;
							GetSystemTime(&currentSystemTime);

							FILETIME modifiedfileTime;
							SystemTimeToFileTime(&currentSystemTime, &modifiedfileTime);

							char pathname[FILENAME_MAX];
							GetCurrentDir(pathname, FILENAME_MAX);

							std::string s_pathname = pathname;
							if (s_pathname.back() != '\\')
								s_pathname += '\\';

							HANDLE winHandleFilename = CreateFileA((LPCSTR)(s_pathname + topath).c_str(),
								FILE_GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL, OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL, NULL);

							if (winHandleFilename == INVALID_HANDLE_VALUE)
								std::cout << GetLastError() << std::endl;

							BOOL b = SetFileTime(winHandleFilename, (LPFILETIME)NULL, (LPFILETIME)NULL, &modifiedfileTime);

							CloseHandle(winHandleFilename);

							if (!b)
								std::cout << GetLastError() << std::endl;
							else
								std::cout << "OK" << std::endl;
#endif
						}
						WidthTakenUp += ImGui::GetItemRectSize().x;

						ImVec2 contentSize = ImGui::GetContentRegionAvail();
						if (j + 1 != cname.indexLow + cname.numImages && !(WidthTakenUp + ImGui::GetItemRectSize().x >= contentSize.x))
						{
							ImGui::SameLine();
						}
						else
						{
							WidthTakenUp = 0;
						}
						
					}
				}
			}
		}
		ImGui::End();
	}
}

std::string SmashScoreboard::CharacterSelectWindow::exportToSSSB()
{
	std::string output = "start CharacterWindow\n";
	output += "styleIndex " + std::to_string(this->styleIndex) + "\n";
	output += "windowName " + this->windowName + "\n";
	output += "end\n\n";

	return output;
}

// [SECTION] CharacterSelectWindowCreator()

SmashScoreboard::CharacterSelectWindowCreator::CharacterSelectWindowCreator()
	:SmashScoreboardWindow("Create New Character Select Window"), newWindowName("") {}

SmashScoreboard::CharacterSelectWindowCreator* SmashScoreboard::CharacterSelectWindowCreator::CreateWindow()
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<CharacterSelectWindowCreator>();
	windowList.push_back(win);
	return (CharacterSelectWindowCreator*)win.get();
}

void SmashScoreboard::CharacterSelectWindowCreator::perframe()
{
	if (this->isVisible)
	{
		StyleColorsFromIndex(this->styleIndex);

		ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;

		ImGui::Begin("Add a new Character Selector Window", &this->isVisible, flags);
		const char* items[] = { "Red", "Blue", "Yellow", "Green"};
		int item_current_idx = this->styleIndex - 1;            // Here our selection data is an index.
		const char* combo_label = items[item_current_idx];  // Label to preview before opening the combo (technically it could be anything)
		if (ImGui::BeginCombo("New Window Color", combo_label))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (item_current_idx == n);
				if (ImGui::Selectable(items[n], is_selected))
				{
					item_current_idx = n;
					this->styleIndex = n + 1;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::InputText("Name of new Window", newWindowName, IM_ARRAYSIZE(newWindowName));

		if (ImGui::Button("Cancel"))
			this->isVisible = false;
		ImGui::SameLine();

		bool shouldDisable = checkForTakenIdentifier(std::string(this->newWindowName)) || strcmp(this->newWindowName, "") == 0;

		if (!shouldDisable)
		{
			if (ImGui::Button("Create Window!"))
			{
				CharacterSelectWindow::CreateWindow(std::string(this->newWindowName), this->styleIndex);
				this->isVisible = false;
			}
		}
		else
		{
			SmashScoreboard::HelpMarker("Choose a name not already taken by another window, not blank, and not 'default'");
		}
		ImGui::End();
	}
}



#ifdef _WIN32
#pragma pull_macro("CreateWindow")
#endif