#include "Widgets/OKCancelDialogWindow.h"

// [SECTION] OKCancelDialogWindow

SmashScoreboard::OKCancelDialogWindow::OKCancelDialogWindow(std::string dialogTitle, std::string dialogContents, bool& flag, GLuint iconImg)
	:DialogWindow(dialogTitle, dialogContents, iconImg), flagToChange(flag)
{
	flagToChange = false;
}

SmashScoreboard::OKCancelDialogWindow* SmashScoreboard::OKCancelDialogWindow::CreateWindow(std::string dialogTitle, std::string dialogContents, bool& flag, GLuint iconImg)
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<OKCancelDialogWindow>(dialogTitle, dialogContents, flag, iconImg);
	windowList.push_back(win);
	return (OKCancelDialogWindow*)win.get();
}

void SmashScoreboard::OKCancelDialogWindow::perframe()
{
	if (this->isVisible)
	{
		ImGui::OpenPopup(this->dialogTitle.c_str(), ImGuiPopupFlags_AnyPopupLevel);

		ImGui::StyleColorsLight();
		ImGui::SetNextWindowSize(ImVec2(-1, -1), ImGuiCond_Always);

		ImGui::BeginPopupModal(this->dialogTitle.c_str(), &this->isVisible);

		if (this->iconImg != 0)
		{
			ImGui::Image((void*)this->iconImg, ImVec2(64, 64));
			ImGui::SameLine();
		}
		ImGui::Text(this->dialogContents.c_str());

		ImGui::Separator();
		
		ImGui::Dummy(ImVec2(64, 20));
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			this->isVisible = false;
		ImGui::SameLine();
		if (ImGui::Button("OK"))
		{
			this->flagToChange = true;
			this->isVisible = false;
		}

		ImGui::EndPopup();
	}
}