#include "Widgets/NameWindow.h"

// [SECTION] PlayerTextWindow()

SmashScoreboard::NameWindow::NameWindow(std::string winName, int styleIndex)
	:SmashScoreboardWindow(winName), pName(""), styleIndex(styleIndex)
{
	if (this->windowName == "" or this->windowName == "default")
		this->windowName == "Player Name Window";

	std::fstream outputFile;
	outputFile.open(("Output/" + this->windowName + ".txt"), std::ios::in);
	if (outputFile.is_open())
	{
		outputFile.getline(this->comparison, IM_ARRAYSIZE(this->comparison));
		outputFile.close();
	}
}

SmashScoreboard::NameWindow* SmashScoreboard::NameWindow::CreateWindow(std::string winName, int styleIndex)
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<NameWindow>(winName, styleIndex);
	windowList.push_back(win);
	return (NameWindow*)win.get();
}

void SmashScoreboard::NameWindow::perframe()
{
	if (this->isVisible)
	{
		std::fstream outputFile;
		outputFile.open(("Output/" + this->windowName + ".txt"), std::ios::in);
		if (outputFile.is_open())
		{
			outputFile.getline(this->comparison, IM_ARRAYSIZE(this->comparison));
			outputFile.close();
		}

		ImGui::SetNextWindowSize(ImVec2(400, 100), ImGuiCond_FirstUseEver);

		SmashScoreboard::StyleColorsFromIndex(this->styleIndex);

		ImGuiWindowFlags flags = 0;

		ImGui::Begin((this->windowName + "###" + this->windowName).c_str(), &this->isVisible, flags);

		ImGui::Text("Player's Name");
		ImGui::SameLine();
		SmashScoreboard::HelpMarker("Input the name of the player currently competing (ex. 'Hungrybox')");
		ImGui::SameLine();
		if (ImGui::InputText("##nametext", this->pName, IM_ARRAYSIZE(this->pName), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			goto SetFileContents;
		}

		if (strcmp(this->comparison, this->pName) == 0)
		{
			ImGui::Text("Name Updated!");
			ImGui::SameLine();
			SmashScoreboard::HelpMarker("To update the name again, change the text box to something other than what's currently in the .txt file (or what you last typed)");
		}
		else
		{
			if (ImGui::Button("Set Name"))
			{
			SetFileContents: std::fstream outputFile;
				outputFile.open(("Output/" + this->windowName + ".txt"), std::ios::in);
				if (outputFile.is_open())
				{
					outputFile.close();
					outputFile.open("Output/" + this->windowName + ".txt", std::ios::out | std::ios::trunc);
					outputFile << this->pName;
					outputFile.close();
				}
				else
				{
					outputFile.open("Output/" + this->windowName + ".txt", std::ios::out);
					outputFile << this->pName;
					outputFile.close();
				}
			}
			ImGui::SameLine();
			SmashScoreboard::HelpMarker("Click this button when you want to update the file containing the player's name.");
		}

		ImGui::End();
	}
}

std::string SmashScoreboard::NameWindow::exportToSSSB()
{
	std::string output = "start NameWindow\n";
	output += "styleIndex " + std::to_string(this->styleIndex) + "\n";
	output += "windowName " + this->windowName + "\n";
	output += "end\n\n";

	return output;
}

// [SECTION] AddPlayerTextWindow()

SmashScoreboard::NameWindowCreator::NameWindowCreator()
	:SmashScoreboardWindow("Create New Player Name Window"), newWindowName("") {}

SmashScoreboard::NameWindowCreator* SmashScoreboard::NameWindowCreator::CreateWindow()
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<NameWindowCreator>();
	windowList.push_back(win);
	return (NameWindowCreator*)win.get();
}

void SmashScoreboard::NameWindowCreator::perframe()
{
	if (this->isVisible)
	{
		StyleColorsFromIndex(this->styleIndex);
		ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;

		ImGui::Begin("Add a new Character Selector Window", &this->isVisible, flags);
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
				NameWindow::CreateWindow(std::string(this->newWindowName), this->styleIndex);
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
