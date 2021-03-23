#pragma once
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include "SmashScoreboardFunctions.h"
#include "Widgets/SmashScoreboardWindow.h"

namespace SmashScoreboard {
    class ScoreWindow : public SmashScoreboardWindow
	{
	private:
		//Stores the input box data
		int scoreInt;
		int comparisonScoreInt;

		int styleIndex;

	public:

		ScoreWindow(std::string winName = "", int styleIndex = 1);
		~ScoreWindow() {}
		static ScoreWindow* CreateWindow(std::string winName = "", int styleIndex = 1);

		void perframe() override;
		std::string exportToSSSB() override;
	};

	class ScoreWindowCreator : public SmashScoreboardWindow
	{
	private:
		/// <summary>
		/// Red = 1
		/// Blue = 2
		/// Yellow = 3
		/// Green = 4
		/// </summary>
		int styleIndex = 1;

		//Name of the new Window
		char newWindowName[128];

	public:
		ScoreWindowCreator();
		~ScoreWindowCreator() {}
		static ScoreWindowCreator* CreateWindow();

		void perframe() override;
	};
}