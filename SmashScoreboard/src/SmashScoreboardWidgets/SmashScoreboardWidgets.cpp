#include <SmashScoreboardFunctions.h>
#include <SmashScoreboardWidgets.h>

namespace fs = std::filesystem;

//Set the UNIQUE IDENTIFIER before any new windows
unsigned int SmashScoreboard::UNIQUE_INT_CTR = 0;

std::vector<std::shared_ptr<SmashScoreboard::SmashScoreboardWindow>> SmashScoreboard::windowList;

SmashScoreboard::SmashScoreboardWindow* SmashScoreboard::SmashScoreboardWindow::CreateWindow()
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<SmashScoreboardWindow>();
	windowList.push_back(win);
	return win.get();
}

// [SECTION] PlayerOneSelectWindow()

/*
//
//Player Character Selection Window
//
//Creates a reference to a window where the player may select
//the active character and skin combo to update a file with.
//
*/
SmashScoreboard::PlayerOneSelectWindow::PlayerOneSelectWindow(std::string winName, int styleIndex)
	:SmashScoreboardWindow(winName), pName(""), styleIndex(styleIndex)
{
	if (this->windowName == "" or this->windowName == "default")
		this->windowName == "Character Selection Window";
}

SmashScoreboard::PlayerOneSelectWindow* SmashScoreboard::PlayerOneSelectWindow::CreateWindow(std::string winName, int styleIndex)
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<PlayerOneSelectWindow>(winName, styleIndex);
	windowList.push_back(win);
	return (PlayerOneSelectWindow*)win.get();
}

void SmashScoreboard::PlayerOneSelectWindow::perframe()
{
	if (this->isVisible)
	{
		StyleColorsFromIndex(this->styleIndex);

		ImGui::Begin((this->windowName + "###" + this->windowName).c_str() , &(this->isVisible));
		ImGui::InputTextWithHint("", "Player 1's character", pName, IM_ARRAYSIZE(pName)); ImGui::SameLine();
		SmashScoreboard::HelpMarker("Input the name of Player 1's character (eg. Mario)");

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
							std::string frompath = "res/ImageCache/Smash Ultimate Full Art/";
							frompath += SmashScoreboard::characterList[i].text.c_str();
							frompath += "/";
							frompath += SmashScoreboard::characterList[i].text.c_str();
							if (imageNum < 10)
								frompath += "_0" + std::to_string(imageNum);
							else if (imageNum >= 10)
								frompath += "_" + std::to_string(imageNum);
							frompath += ".png";
							fs::path from = frompath;
							std::cout << fs::copy_file(from, to, copyOptions) << std::endl;
							std::cout << SmashScoreboard::characterList[i].text.c_str() << std::endl;
						}
					}
				}
			}
		}
		ImGui::End();
	}
}

// [SECTION] AddPlayerSelectWindowWindow()

SmashScoreboard::AddPlayerSelectWindowWindow::AddPlayerSelectWindowWindow()
	:SmashScoreboardWindow("Create New Character Select Window"), newWindowName("") {}

SmashScoreboard::AddPlayerSelectWindowWindow* SmashScoreboard::AddPlayerSelectWindowWindow::CreateWindow()
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<AddPlayerSelectWindowWindow>();
	windowList.push_back(win);
	return (AddPlayerSelectWindowWindow*)win.get();
}

void SmashScoreboard::AddPlayerSelectWindowWindow::perframe()
{
	if (this->isVisible)
	{
		StyleColorsFromIndex(this->styleIndex);

		ImGui::Begin(("Add a new Character Selector Window" + this->windowTitleIdentifier).c_str(), &this->isVisible, ImGuiWindowFlags_AlwaysAutoResize);
		const char* items[] = { "Red", "Blue", "Yellow", "Green"};
		int item_current_idx = 0;                    // Here our selection data is an index.
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

		bool shouldDisable = checkForTakenIdentifier(std::string(this->newWindowName));

		if (!shouldDisable)
		{
			if (ImGui::Button("Create Window!"))
			{
				PlayerOneSelectWindow::CreateWindow(std::string(this->newWindowName), this->styleIndex);
				this->isVisible = false;
			}
		}
		else
		{
			SmashScoreboard::HelpMarker("Choose a name not already taken by another window and not 'default'");
		}
		ImGui::End();
	}
}

bool SmashScoreboard::checkForTakenIdentifier(std::string ident)
{
	if (ident == "default")
		return true;

	for (int i = 0; i < windowList.size(); i++)
	{
		if (windowList[i].get()->windowName == ident)
			return true;
	}
	return false;
}