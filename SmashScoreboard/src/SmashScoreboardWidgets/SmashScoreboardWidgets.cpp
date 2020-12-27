#include <SmashScoreboardFunctions.h>
#include <SmashScoreboardWidgets.h>

namespace fs = std::filesystem;

/*
//
//Player Character Selection Window
//
//Creates a reference to a window where the player may select
//the active character and skin combo to update a file with.
//
*/
SmashScoreboard::PlayerCharacterSelectWindow::PlayerCharacterSelectWindow()
	:SmashScoreboardWindow(), pName("") {}


void SmashScoreboard::PlayerCharacterSelectWindow::perframe()
{
	ImGui::Begin("Character Selector Player 1");
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
						fs::path to = "Output/image0.png";
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

void SmashScoreboard::PP()
{
	ImGui::Begin("Character Selector Player 1");

	static char p1Name[128] = "";
	ImGui::InputTextWithHint("", "Player 1's character", p1Name, IM_ARRAYSIZE(p1Name)); ImGui::SameLine();
	SmashScoreboard::HelpMarker("Input the name of Player 1's character (eg. Mario)");

	const char* names[] = { "Mario", "Link", "Kirby" };

	for (int i = 0; i < IM_ARRAYSIZE(names); i++)
	{
		if (SmashScoreboard::findLowerSubstring(names[i], p1Name))
		{
			if (ImGui::CollapsingHeader(names[i]))
			{
				ImGui::Button(names[i]);
			}
		}
	}

	ImGui::End();
}