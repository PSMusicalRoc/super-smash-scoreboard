#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include <imgui.h>
#include <glad/glad.h>

#include "Widgets/SmashScoreboardWindow.h"

namespace SmashScoreboard
{
    class DialogWindow : public SmashScoreboardWindow
	{
	protected:
		std::string dialogTitle;
		std::string dialogContents;
		GLuint iconImg = 0;

	public:
		DialogWindow(std::string dialogTitle, std::string dialogContents, GLuint iconImg = 0);
		~DialogWindow()
		{
			ImGui::CloseCurrentPopup();
		}

		static DialogWindow* CreateWindow(std::string dialogTitle, std::string dialogContents, GLuint iconImg = 0);

		void perframe() override;

	};
}