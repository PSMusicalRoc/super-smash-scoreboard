#include <SmashScoreboardFunctions.h>
#include <SmashScoreboardWidgets.h>

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
		if (SmashScoreboard::findLowerSubstring(SmashScoreboard::characterList[i].getText(), pName))
		{
			if (ImGui::CollapsingHeader(SmashScoreboard::characterList[i].getText()))
			{
				ImGui::Button(SmashScoreboard::characterList[i].getText());
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