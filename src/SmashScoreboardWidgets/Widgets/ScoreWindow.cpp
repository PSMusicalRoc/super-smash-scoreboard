#include "Widgets/ScoreWindow.h"

// [SECTION] ScoreWindow()

SmashScoreboard::ScoreWindow::ScoreWindow(std::string winName, int styleIndex)
	:SmashScoreboardWindow(winName), styleIndex(styleIndex)
{
	if (this->windowName == "" or this->windowName == "default")
		this->windowName == "Player Name Window";

	std::fstream outputFile;
	outputFile.open(("Output/" + this->windowName + ".txt"), std::ios::in);
	if (outputFile.is_open())
	{
		char instring[128];
		outputFile.getline(instring, IM_ARRAYSIZE(instring));
		outputFile.close();
		std::string instr = std::string(instring);
		this->scoreInt = this->comparisonScoreInt = std::stoi(instring);
	}
	else
	{
		this->scoreInt = 0;
		this->comparisonScoreInt = 0;
	}
}

SmashScoreboard::ScoreWindow* SmashScoreboard::ScoreWindow::CreateWindow(std::string winName, int styleIndex)
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<ScoreWindow>(winName, styleIndex);
	windowList.push_back(win);
	return (ScoreWindow*)win.get();
}

void SmashScoreboard::ScoreWindow::perframe()
{
	if (this->isVisible)
	{
		std::fstream outputFile;
		outputFile.open(("Output/" + this->windowName + ".txt"), std::ios::in);
		if (outputFile.is_open())
		{
			char instring[128];
			outputFile.getline(instring, IM_ARRAYSIZE(instring));
			outputFile.close();
			std::string instr = instring;
			this->comparisonScoreInt = std::stoi(instring);
		}

		ImGui::SetNextWindowSize(ImVec2(310, 122), ImGuiCond_FirstUseEver);

		SmashScoreboard::StyleColorsFromIndex(this->styleIndex);

		ImGuiWindowFlags flags = 0;

		ImGui::Begin((this->windowName + "###" + this->windowName).c_str(), &this->isVisible, flags);

		ImGui::Text("Player Score:");
		ImGui::InputInt("##scoreinput", &this->scoreInt);
		if (this->scoreInt < 0) {this->scoreInt = 0;}
		else if (this->scoreInt > 9999) {this->scoreInt = 9999;}
		ImGui::SameLine();
		if (ImGui::Button("Reset to 0"))
		{
			this->scoreInt = 0;
		}

		if (this->comparisonScoreInt == this->scoreInt)
		{
			ImGui::TextColored(ImVec4(0.0, 0.6, 0.0, 1.0), "Updated Score!");
		}
		else
		{
			if (ImGui::Button("Set Score"))
			{
				outputFile.open(("Output/" + this->windowName + ".txt"), std::ios::out | std::ios::trunc);
				if (outputFile.is_open())
				{
					outputFile << this->scoreInt;
					outputFile.close();
				}
			}
		}

		ImGui::End();
	}
}

std::string SmashScoreboard::ScoreWindow::exportToSSSB()
{
	std::string output = "start ScoreWindow\n";
	output += "styleIndex " + std::to_string(this->styleIndex) + "\n";
	output += "windowName " + this->windowName + "\n";
	output += "end\n\n";

	return output;
}

// [SECTION] AddPlayerTextWindow()

SmashScoreboard::ScoreWindowCreator::ScoreWindowCreator()
	:SmashScoreboardWindow("Create New Score Window"), newWindowName("") {}

SmashScoreboard::ScoreWindowCreator* SmashScoreboard::ScoreWindowCreator::CreateWindow()
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<ScoreWindowCreator>();
	windowList.push_back(win);
	return (ScoreWindowCreator*)win.get();
}

void SmashScoreboard::ScoreWindowCreator::perframe()
{
	if (this->isVisible)
	{
		StyleColorsFromIndex(this->styleIndex);
		ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;

		ImGui::Begin("Create New Score Window", &this->isVisible, flags);
		const char* items[] = { "Red", "Blue", "Yellow", "Green" };
		int item_current_idx = this->styleIndex - 1;                    // Here our selection data is an index.
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
		ImGui::SameLine();
		SmashScoreboard::HelpMarker("This will become not only the name of the new window, but also the name of the .txt file created in the Output folder.");

		if (ImGui::Button("Cancel"))
			this->isVisible = false;
		ImGui::SameLine();

		bool shouldDisable = checkForTakenIdentifier(std::string(this->newWindowName)) || strcmp(this->newWindowName, "") == 0;

		if (!shouldDisable)
		{
			if (ImGui::Button("Create Window!"))
			{
				ScoreWindow::CreateWindow(std::string(this->newWindowName), this->styleIndex);
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
