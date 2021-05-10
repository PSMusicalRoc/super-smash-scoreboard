#include "Widgets/DialogWindow.h"

SmashScoreboard::DialogWindow::DialogWindow(std::string dialogTitle, std::string dialogContents, GLuint iconImg)
	:SmashScoreboardWindow("default", "Dialog"), dialogTitle(dialogTitle), dialogContents(dialogContents), iconImg(iconImg)
{}

SmashScoreboard::DialogWindow* SmashScoreboard::DialogWindow::CreateWindow(std::string dialogTitle, std::string dialogContents, GLuint iconImg)
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<DialogWindow>(dialogTitle, dialogContents, iconImg);
	windowList.push_back(win);
	return (DialogWindow*)win.get();
}

void SmashScoreboard::DialogWindow::perframe()
{
	if (this->isVisible)
	{
		ImGui::OpenPopup(this->dialogTitle.c_str(), ImGuiPopupFlags_AnyPopupLevel);

		ImGui::StyleColorsLight();
		ImGui::SetNextWindowSize(ImVec2(-1, -1), ImGuiCond_Always);

		if (ImGui::BeginPopupModal((this->dialogTitle).c_str(), &this->isVisible))
		{
			if (this->iconImg != 0)
			{
				ImGui::Image((void*)this->iconImg, ImVec2(64, 64));
				ImGui::SameLine();
			}
			ImGui::Text(this->dialogContents.c_str());

			ImGui::EndPopup();
		}
	}
}