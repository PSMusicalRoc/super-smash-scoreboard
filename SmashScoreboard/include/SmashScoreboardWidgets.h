#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include <imgui_impl_sdl.h>
#include <iostream>

namespace SmashScoreboard
{
	//Int holder to make unique window instances
	extern unsigned int UNIQUE_INT_CTR;

	struct SmashScoreboardWindow
	{
		//Please don't use this, use CreateWindow instead
		SmashScoreboardWindow()
		{
			windowTitleIdentifier = "##" + std::to_string(UNIQUE_INT_CTR);
			UNIQUE_INT_CTR++;
		}

		~SmashScoreboardWindow() {}
		static SmashScoreboardWindow* CreateWindow();

		std::string windowTitleIdentifier;

		template<typename T>
		static void DeleteWindow(T* win, std::vector<std::shared_ptr<SmashScoreboardWindow>> &list)
		{
			if (list.size() != 0)
			{
				for (int i = 0; i < list.size(); i++)
				{
					if ((SmashScoreboardWindow*)win == list[i].get())
					{
						std::vector<std::shared_ptr<SmashScoreboardWindow>>::iterator removeThis = list.begin() + i;
						list.erase(removeThis);
						return;
					}
				}
			}
			else
				std::cout << "Empty Vector" << std::endl;
		}

		bool isVisible = true;
		virtual void perframe() {};
	};

	class PlayerOneSelectWindow : public SmashScoreboardWindow
	{
	private:
		std::string windowName;

		//Stores the input box data
		char pName[128];

		int styleIndex;

	public:
		PlayerOneSelectWindow(std::string winName = "", int styleIndex = 1);
		~PlayerOneSelectWindow() {}
		static PlayerOneSelectWindow* CreateWindow(std::string winName = "", int styleIndex = 1);

		void perframe() override;
	};

	class AddPlayerSelectWindowWindow : public SmashScoreboardWindow
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
		char windowName[128];

	public:
		AddPlayerSelectWindowWindow();
		~AddPlayerSelectWindowWindow() {}
		static AddPlayerSelectWindowWindow* CreateWindow();

		void perframe() override;
	};

	extern std::vector<std::shared_ptr<SmashScoreboardWindow>> windowList;
}