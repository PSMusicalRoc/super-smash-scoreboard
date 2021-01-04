#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include <imgui_impl_sdl.h>
#include <iostream>

//for current working directory stuff
#include <direct.h>
#define GetCurrentDir _getcwd

namespace SmashScoreboard
{
	//Int holder to make unique window instances
	extern unsigned int UNIQUE_INT_CTR;
	
	//Boolean that defines whether or not to show all the windows
	//or just the file select window
	extern bool ISFILEWINDOWOPEN;

	struct SmashScoreboardWindow
	{
		std::string windowName;

		//Please don't use this, use CreateWindow instead
		SmashScoreboardWindow(std::string winName = "default") : windowName(winName)
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

	class OpenFileWindow : public SmashScoreboardWindow
	{
	private:
		static std::shared_ptr<OpenFileWindow> filewindowptr;
		char folderpathbuf[FILENAME_MAX];
		char filenamebuf[FILENAME_MAX];

		std::string readableFolderPath;
		std::string readableFileName;
	
	public:
		OpenFileWindow()
			:SmashScoreboardWindow("Open File"), filenamebuf("")
		{
			GetCurrentDir(folderpathbuf, FILENAME_MAX);
			readableFolderPath = folderpathbuf;
			readableFileName = filenamebuf;
		}
		~OpenFileWindow() {}

		static OpenFileWindow* CreateWindow();
		static void CloseWindow();

		std::string windowTitleIdentifier;

		bool isVisible = true;
		void perframe() override;

		static OpenFileWindow* getWindowPtr() { return filewindowptr.get(); }
	};

	class PlayerOneSelectWindow : public SmashScoreboardWindow
	{
	private:
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
		char newWindowName[128];

	public:
		AddPlayerSelectWindowWindow();
		~AddPlayerSelectWindowWindow() {}
		static AddPlayerSelectWindowWindow* CreateWindow();

		void perframe() override;
	};

	extern std::vector<std::shared_ptr<SmashScoreboardWindow>> windowList;

	bool checkForTakenIdentifier(std::string ident);
}