#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "Widgets/SmashScoreboardWindow.h"
#include "Widgets/DialogWindow.h"

namespace SmashScoreboard
{
	class OKCancelDialogWindow : public DialogWindow
	{
	protected:
		bool& flagToChange;

	public:
		OKCancelDialogWindow(std::string dialogTitle, std::string dialogContents, bool& flag, GLuint iconImg = 0);
		~OKCancelDialogWindow()
		{
			ImGui::CloseCurrentPopup();
		}

		static OKCancelDialogWindow* CreateWindow(std::string dialogTitle, std::string dialogContents, bool& flag, GLuint iconImg = 0);

		void perframe() override;
	};
}