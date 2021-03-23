#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include <imgui_impl_sdl.h>
#include <iostream>

#include "Widgets/SmashScoreboardWindow.h"
#include "Widgets/DialogWindow.h"
#include "Widgets/OKCancelDialogWindow.h"

#include "Widgets/CharacterSelectWindow.h"

#include "Widgets/NameWindow.h"

#include "Widgets/ScoreWindow.h"

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
	
	//Boolean that defines whether or not to show all the windows
	//or just the file select window
	extern bool ISFILEWINDOWOPEN;

	//This variable holds whether or not the perframe should check
	//if there's a result from an OKCancelDialog yet
	extern bool OKCANCELDIALOGCREATEDYET;
	extern bool OKCANCELDIALOGRESULT;

	bool LoadFromSSSB(const char* filename);

/*	class OpenFileWindow : public SmashScoreboardWindow
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
	*/
}