#pragma once

#include <imgui.h>
#include <glad/glad.h>

#include "SmashScoreboardFunctions.h"
#include "Widgets/SmashScoreboardWindow.h"

namespace SmashScoreboard
{
    class CharacterSelectWindow : public SmashScoreboardWindow
	{
	private:
		//Stores the input box data
		char pName[128];

		int styleIndex;

	public:
		
		CharacterSelectWindow(std::string winName = "", int styleIndex = 1);
		~CharacterSelectWindow() {}
		static CharacterSelectWindow* CreateWindow(std::string winName = "", int styleIndex = 1);

		void perframe() override;
		std::string exportToSSSB() override;
	};








    

	class CharacterSelectWindowCreator : public SmashScoreboardWindow
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
		CharacterSelectWindowCreator();
		~CharacterSelectWindowCreator() {}
		static CharacterSelectWindowCreator* CreateWindow();

		void perframe() override;
	};
}