#include <SmashScoreboardFunctions.h>
#include <SmashScoreboardWidgets.h>

namespace fs = std::filesystem;

//Set the UNIQUE IDENTIFIER before any new windows
bool SmashScoreboard::ISFILEWINDOWOPEN = false;

bool SmashScoreboard::OKCANCELDIALOGCREATEDYET = false;
bool SmashScoreboard::OKCANCELDIALOGRESULT = false;

// LoadFromSSSB()

bool SmashScoreboard::LoadFromSSSB(const char* filename)
{
	SmashScoreboard::windowList.clear();

	std::fstream defaultLayout;
	defaultLayout.open(filename, std::ios::in);
	if (defaultLayout.is_open())
	{
		//defines whether or not to search for start
		//or to search for attributes
		bool isWindowStarted = false;

		enum {	CHARACTERWINDOW = 1,
				NAMEWINDOW		= 2,
				SCOREWINDOW		= 3};

		int windowType;
		int styleIndex = 1;
		std::string windowName = "default";

		//start reading lines
		std::string line;
		while (std::getline(defaultLayout, line))
		{
			if (!isWindowStarted)
			{
				if (line.find("start") == 0 && line.find("start") != line.npos)
				{
					std::string windowTypeString = line.replace(line.find("start "), std::string("start ").length(), "");
					if (windowTypeString == "CharacterWindow")
					{
						windowType = CHARACTERWINDOW;
						isWindowStarted = true;
					}
					else if (windowTypeString == "NameWindow")
					{
						windowType = NAMEWINDOW;
						isWindowStarted = true;
					}
					else if (windowTypeString == "ScoreWindow")
					{
						windowType = SCOREWINDOW;
						isWindowStarted = true;
					}
				}
			}
			else
			{
				if (line.find("end") == 0 && line.find("end") != line.npos)
				{
					if (!SmashScoreboard::checkForTakenIdentifier(windowName))
					{
						switch (windowType)
						{
						case CHARACTERWINDOW:
							SmashScoreboard::CharacterSelectWindow::CreateWindow(windowName, styleIndex);
							break;
						case NAMEWINDOW:
							SmashScoreboard::NameWindow::CreateWindow(windowName, styleIndex);
							break;
						case SCOREWINDOW:
							SmashScoreboard::ScoreWindow::CreateWindow(windowName, styleIndex);
							break;
						}

						windowName = "default";
					}
					isWindowStarted = false;
				}
				else if (line.find("styleIndex") == 0 && line.find("styleIndex") != line.npos)
				{
					std::string datastring = line.replace(line.find("styleIndex "), std::string("styleIndex ").length(), "");
					int data = std::stoi(datastring);
					if (0 < data < 5)
						styleIndex = data;
					else
						styleIndex = 1;
				}
				else if (line.find("windowName") == 0 && line.find("windowName") != line.npos)
				{
					std::string datastring = line.replace(line.find("windowName "), std::string("windowName ").length(), "");
					if (!SmashScoreboard::checkForTakenIdentifier(datastring))
						windowName = datastring;
				}
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

// OPENFILEWINDOW IS DOWN HERE, DON'T USE THIS

/*

// [SECTION] OpenFileWindow

std::shared_ptr<SmashScoreboard::OpenFileWindow> SmashScoreboard::OpenFileWindow::filewindowptr = nullptr;
int SmashScoreboard::OpenFileWindow::currentCallback = 0;
bool SmashScoreboard::OpenFileWindow::isInOpenMode = true;

SmashScoreboard::OpenFileWindow* SmashScoreboard::OpenFileWindow::CreateWindow(bool isThisInOpenMode)
{
	if (filewindowptr == nullptr)
	{
		filewindowptr = std::make_shared<OpenFileWindow>();
	}
	ISFILEWINDOWOPEN = true;
	isInOpenMode = isThisInOpenMode;

	return filewindowptr.get();
}

void SmashScoreboard::OpenFileWindow::CloseWindow()
{
	ISFILEWINDOWOPEN = false;	
}

void SmashScoreboard::OpenFileWindow::SetCallback(int callbackFlag) { currentCallback = callbackFlag; }

void SmashScoreboard::OpenFileWindow::perframe()
{
	if (ISFILEWINDOWOPEN)
	{
		SmashScoreboard::StyleColorsFileOpenMenu();
		ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
		ImGui::SetNextWindowPos(ImVec2(0, 0));

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
		ImGuiWindowFlags childflags = 0;
		if (ISDIALOGOPEN)
		{
			flags = flags | ImGuiWindowFlags_NoInputs;
			childflags = childflags | ImGuiWindowFlags_NoInputs;
		}

		//LEAVING A NOTE HERE SO THAT I REMEMBER TO CHANGE THE IDENTIFIER TO FILEMENU AND MAKE THE NAME DYNAMIC
		if (this->isInOpenMode)
			this->windowName = "Open File";
		else
			this->windowName = "Save File";

		ImGui::Begin((this->windowName + "###FileMenu").c_str(), NULL, flags);

		ImGui::Text("Path:"); ImGui::SameLine();
		if (ImGui::InputText("##Path", this->folderpathbuf, IM_ARRAYSIZE(this->folderpathbuf), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			try
			{
				std::string _testpath = std::string(this->folderpathbuf);
				auto __search = _testpath.find('\\');
				if (__search == _testpath.npos)
					_testpath += '\\';

				for (const auto& entry : fs::directory_iterator(_testpath))
				{
				}

				this->readableFolderPath = _testpath;
				strcpy_mac(this->folderpathbuf, _testpath.c_str());
			}
			catch (fs::filesystem_error e)
			{
				strcpy_mac(this->folderpathbuf, this->readableFolderPath.c_str());
			}
		}
		ImGui::SameLine();
		if (ImGui::ImageButton((ImTextureID)FileSelect_GoImage, ImVec2(32, 32)))
		{
			try
			{
				std::string _testpath = std::string(this->folderpathbuf);
				auto __search = _testpath.find('\\');
				if (__search == _testpath.npos)
					_testpath += '\\';

				for (const auto& entry : fs::directory_iterator(_testpath))
				{
				}

				this->readableFolderPath = _testpath;
				strcpy_mac(this->folderpathbuf, _testpath.c_str());
			}
			catch (fs::filesystem_error e)
			{
				strcpy_mac(this->folderpathbuf, this->readableFolderPath.c_str());
			}
		}

		ImGui::SameLine();
		if (ImGui::ImageButton((ImTextureID)FileSelect_UpOneLevelImage, ImVec2(32, 32)))
		{
			if (this->readableFolderPath.back() == '\\')
			{
				//2 because it creates first the length starting at 0 for
				//a minus 1, then takes another one away so we don't consider
				//the first backslash
				auto backpos = this->readableFolderPath.length() - 2;
				auto __search = this->readableFolderPath.rfind('\\', backpos);

				if (__search != this->readableFolderPath.npos)
				{
					size_t len = this->readableFolderPath.length() - __search;
					len -= 1;
					this->readableFolderPath.replace(__search, len, "");
				}
			}
			else
			{
				auto __search = this->readableFolderPath.rfind('\\');
				if (__search != this->readableFolderPath.npos)
				{
					size_t len = this->readableFolderPath.length() - __search;
					this->readableFolderPath.replace(__search, len, "");

					auto __newsearch = this->readableFolderPath.find('\\');
					if (__newsearch == this->readableFolderPath.npos)
					{
						this->readableFolderPath += '\\';
					}
				}

			}

			strcpy_mac(this->folderpathbuf, this->readableFolderPath.c_str());
		}

		ImGui::BeginChild((ImGuiID)1, ImVec2(0, windowHeight - 120), true, childflags);
		std::string path = this->readableFolderPath;
		int i = 0;
		for (const auto& entry : fs::directory_iterator(path))
		{
			std::string name = std::string(entry.path().string());
			if (entry.is_directory())
			{
				if (path.back() == '\\')
				{
					name.replace(name.find(path), std::string(path).length(), "");
					name = "\\" + name;
				}
				else
				{
					name.replace(name.find(path), std::string(path).length(), "");
				}


				ImGui::Image((void*)(intptr_t)FileSelect_Folder, ImVec2(16, 16)); ImGui::SameLine();

				ImGui::TreeNodeEx((void*)(intptr_t)i, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, name.c_str());
				if (ImGui::IsItemClicked())
				{
					if (ImGui::IsMouseDoubleClicked(0))
					{
						if (path.back() == '\\')
						{
							std::string _tempname = name;
							_tempname.replace(name.find('\\'), 1, "");
							this->readableFolderPath = this->readableFolderPath + _tempname;
							strcpy_mac(this->folderpathbuf, this->readableFolderPath.c_str());
						}
						else
						{
							this->readableFolderPath = this->readableFolderPath + name;
							strcpy_mac(this->folderpathbuf, this->readableFolderPath.c_str());
						}
					}
					else
					{
						this->readableFileName = name;
						strcpy_mac(this->filenamebuf, this->readableFileName.c_str());

					}
				}
			}
			else
			{
				if (path.back() == '\\')
				{
					name.replace(name.find(path), std::string(path).length(), "");
				}
				else
				{
					name.replace(name.find(path + "\\"), std::string(path + "\\").length(), "");
				}

				ImGui::Image((void*)(intptr_t)FileSelect_File, ImVec2(16, 16)); ImGui::SameLine();

				ImGui::TreeNodeEx((void*)(intptr_t)i, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, name.c_str());
				if (ImGui::IsItemClicked())
				{
					this->readableFileName = name;
					strcpy_mac(this->filenamebuf, this->readableFileName.c_str());
				}
			}

			i++;
		}
		ImGui::EndChild();

		ImGui::Text("Filename:"); ImGui::SameLine();
		ImGui::InputText("##Filename", this->filenamebuf, IM_ARRAYSIZE(this->filenamebuf));
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ISFILEWINDOWOPEN = false;
		}
		ImGui::SameLine();

		std::string buttonText;
		if (this->isInOpenMode)
			buttonText = "Open File";
		else
			buttonText = "Save File";

		if (ImGui::Button(buttonText.c_str()))
		{
			this->openpath = this->readableFolderPath;
			if (readableFolderPath.back() != '\\')
			{
				this->openpath += '\\';
			}
			this->openpath += this->filenamebuf;

			if (this->isInOpenMode)
			{
				if (this->currentCallback == OpenFileWindowCallback_LOADSSSBWINDOWCONFIG)
				{
					if (this->LoadSSSBWindowConfig(this->openpath))
					{
						CloseWindow();
					}
					else
					{
						DialogWindow::CreateWindow("Failed to open window config",
							"Smash Scoreboard failed to open the window config file at\n\n"
							+ this->openpath
							+ "\n\nPlease check that the file exists and has the extension '.sssb'", SmashScoreboard::Dialogs_Warning);
					}
				}
			}
			else
			{
				if (this->currentCallback == SaveFileWindowCallback_SAVESSSBWINDOWCONFIG)
				{
					if (this->openpath.rfind(".sssb") != this->openpath.length() - 5)
					{
						this->openpath += ".sssb";
					}

					std::fstream checkForFile;
					checkForFile.open(this->openpath, std::ios::in);
					if (checkForFile.is_open())
					{
						checkForFile.close();
						OKCancelDialogWindow::CreateWindow(
							"File already exists",
							"This file at path:\n\n"
							+ this->openpath
							+ "\n\nalready exists. Do you want to overwrite it?",
							this->resultOfDialog,
							Dialogs_Warning
						);
						this->hasCreatedDialogYet = true;
					}
					else
					{
						SaveSSSBWindowConfig(this->openpath);
						CloseWindow();
					}
				}
			}
		}

		if (this->hasCreatedDialogYet && !ISDIALOGOPEN)
		{
			if (this->isInOpenMode)
			{
				if (this->currentCallback == OpenFileWindowCallback_LOADSSSBWINDOWCONFIG)
				{
					this->hasCreatedDialogYet = false;
				}
			}
			else
			{
				if (this->currentCallback == SaveFileWindowCallback_SAVESSSBWINDOWCONFIG)
				{
					if (this->resultOfDialog)
					{
						SaveSSSBWindowConfig(this->openpath);
						this->hasCreatedDialogYet = false;
						CloseWindow();
					}
				}
			}
		}

		ImGui::End();
	}
}

//CALLBACKS

bool SmashScoreboard::OpenFileWindow::LoadSSSBWindowConfig(std::string filepath)
{
	if (filepath.rfind(".sssb") != filepath.length() - 5)
		return false;
	if (!LoadFromSSSB(filepath.c_str()))
		return false;
	return true;
}

void SmashScoreboard::OpenFileWindow::SaveSSSBWindowConfig(std::string filepath)
{
	std::fstream outputFile;
	outputFile.open(filepath, std::ios::out | std::ios::trunc);
	if (outputFile.is_open())
	{
		std::string output = "";
		for (int i = 0; i < SmashScoreboard::windowList.size(); i++)
		{
			output += SmashScoreboard::windowList[i].get()->exportToSSSB();
		}
		outputFile << output;
		outputFile.close();
		SmashScoreboard::DialogWindow::CreateWindow(
			"Succesfully saved!",
			"The file was saved to:\n\n'"
			+ filepath
			+ "'", SmashScoreboard::Dialogs_OK);
	}
	else
	{
		SmashScoreboard::DialogWindow::CreateWindow("Failed to save", "The file could not be saved.", SmashScoreboard::Dialogs_Warning);
	}
}

*/