#pragma once
#include <fstream>

#include <imgui.h>

#include "SmashScoreboardFunctions.h"
#include "Widgets/SmashScoreboardWindow.h"

namespace SmashScoreboard
{
    class NameWindow : public SmashScoreboardWindow
	{
	private:
		//Stores the input box data
		char pName[128];
		char comparison[128];

		int styleIndex;

	public:

		NameWindow(std::string winName = "", int styleIndex = 1);
		~NameWindow() {}
		static NameWindow* CreateWindow(std::string winName = "", int styleIndex = 1);

		void perframe() override;
		std::string exportToSSSB() override;
	};

	class NameWindowCreator : public SmashScoreboardWindow
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
		NameWindowCreator();
		~NameWindowCreator() {}
		static NameWindowCreator* CreateWindow();

		void perframe() override;
	};
}