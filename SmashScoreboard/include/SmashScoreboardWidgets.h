#pragma once
#include <filesystem>
#include <fstream>
#include <imgui_impl_sdl.h>
#include <iostream>

namespace SmashScoreboard
{
	class SmashScoreboardWindow
	{
	public:
		SmashScoreboardWindow() {};
		~SmashScoreboardWindow() {};

		virtual void perframe() {};
	};

	class PlayerCharacterSelectWindow : public SmashScoreboardWindow
	{
	private:
		char pName[128];

	public:
		PlayerCharacterSelectWindow();
		~PlayerCharacterSelectWindow() {}

		void perframe() override;
	};

	void PP();
}