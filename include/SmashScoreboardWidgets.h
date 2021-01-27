#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include <imgui_impl_sdl.h>
#include <iostream>

//for current working directory stuff
#ifdef __linux__
#include <unistd.h>
#define GetCurrentDir getcwd
#elif _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#endif

		//enum values for setting open window callback
enum { OpenFileWindowCallback_LOADSSSBWINDOWCONFIG = 100 };
enum { SaveFileWindowCallback_SAVESSSBWINDOWCONFIG = 100 };

namespace SmashScoreboard
{
	//Int holder to make unique window instances
	extern unsigned int UNIQUE_INT_CTR;
	
	//Boolean that defines whether or not to show all the windows
	//or just the file select window
	extern bool ISFILEWINDOWOPEN;
	extern bool ISDIALOGOPEN;

	bool LoadFromSSSB(const char* filename);

	struct SmashScoreboardWindow
	{
		std::string windowName;

		//Please don't use this, use CreateWindow instead
		SmashScoreboardWindow(std::string winName = "default", std::string winType = "Normal Window")
			: windowName(winName), windowTypeIdentifier(winType)
		{
			windowTitleIdentifier = "##" + std::to_string(UNIQUE_INT_CTR);
			UNIQUE_INT_CTR++;
		}

		~SmashScoreboardWindow() {}
		static SmashScoreboardWindow* CreateWindow();

		std::string windowTitleIdentifier;
		std::string windowTypeIdentifier;

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
		virtual std::string exportToSSSB() { return ""; };
	};

	class DialogWindow : public SmashScoreboardWindow
	{
	protected:
		std::string dialogTitle;
		std::string dialogContents;
		GLuint iconImg = 0;

	public:
		DialogWindow(std::string dialogTitle, std::string dialogContents, GLuint iconImg = 0);
		~DialogWindow() { ISDIALOGOPEN = false; }

		static DialogWindow* CreateWindow(std::string dialogTitle, std::string dialogContents, GLuint iconImg = 0);

		void perframe() override;

	};

	//Subclasses of DialogWindow

	class OKCancelDialogWindow : public DialogWindow
	{
	protected:
		bool& flagToChange;

	public:
		OKCancelDialogWindow(std::string dialogTitle, std::string dialogContents, bool& flag, GLuint iconImg = 0);
		~OKCancelDialogWindow() { ISDIALOGOPEN = false; }

		static OKCancelDialogWindow* CreateWindow(std::string dialogTitle, std::string dialogContents, bool& flag, GLuint iconImg = 0);

		void perframe() override;
	};

	//End Subclasses

	class OpenFileWindow : public SmashScoreboardWindow
	{
	private:
		static std::shared_ptr<OpenFileWindow> filewindowptr;
		char folderpathbuf[FILENAME_MAX];
		char filenamebuf[FILENAME_MAX];

		std::string readableFolderPath;
		std::string readableFileName;

		//This value dictates whether or not the window
		//is in save or open mode
		static bool isInOpenMode;

		//This allows the file menu to know whether or
		//not it has opened a dialog for a particular
		//action (i.e. whether it should do the action
		//it thinks it should do yet)
		bool hasCreatedDialogYet = false;

		//This variable holds the result of the Y/N
		//or OK/Cancel dialog that was created
		bool resultOfDialog;

		//Stores the path for the last opened file for
		//callbacks using a Y/N or OK/Cancel Dialog
		std::string openpath = "";

		//start defining callbacks for "OnOpen" button.
		//Template is returns bool, true if successful,
		//false if not.
		//Must take a string value as a parameter.

		//Firstly, here's the variable that stores the
		//currently used callback.
		static int currentCallback;

		//Now for the callback functions themselves.

		//This function loads a .sssb file and changes
		//the windowList to suit it
		bool LoadSSSBWindowConfig(std::string inFile);

		//This function saves a .sssb file from the current
		//layout
		void SaveSSSBWindowConfig(std::string filepath);
	
	public:
		OpenFileWindow()
			:SmashScoreboardWindow("Open File"), filenamebuf("")
		{
			GetCurrentDir(folderpathbuf, FILENAME_MAX);
			readableFolderPath = folderpathbuf;
			readableFileName = filenamebuf;
		}
		~OpenFileWindow() {}

		static OpenFileWindow* CreateWindow(bool isThisInOpenMode = true);
		static void CloseWindow();

		std::string windowTitleIdentifier;
 
		void perframe() override;

		//and a function to change that value when the
		//user loads the file select window
		static void SetCallback(int callbackFlag = 0);

		static OpenFileWindow* getWindowPtr()
		{
			return filewindowptr.get();
		}
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
		std::string exportToSSSB() override;
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

	class PlayerTextWindow : public SmashScoreboardWindow
	{
	private:
		//Stores the input box data
		char pName[128];
		char comparison[128];

		int styleIndex;

	public:

		PlayerTextWindow(std::string winName = "", int styleIndex = 1);
		~PlayerTextWindow() {}
		static PlayerTextWindow* CreateWindow(std::string winName = "", int styleIndex = 1);

		void perframe() override;
		std::string exportToSSSB() override;
	};

	class AddPlayerTextWindow : public SmashScoreboardWindow
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
		AddPlayerTextWindow();
		~AddPlayerTextWindow() {}
		static AddPlayerTextWindow* CreateWindow();

		void perframe() override;
	};

	extern std::vector<std::shared_ptr<SmashScoreboardWindow>> windowList;

	bool checkForTakenIdentifier(std::string ident);
}