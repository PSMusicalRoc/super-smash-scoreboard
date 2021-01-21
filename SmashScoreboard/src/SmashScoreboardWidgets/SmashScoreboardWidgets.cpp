#include <SmashScoreboardFunctions.h>
#include <SmashScoreboardWidgets.h>

namespace fs = std::filesystem;

//Set the UNIQUE IDENTIFIER before any new windows
unsigned int SmashScoreboard::UNIQUE_INT_CTR = 0;
bool SmashScoreboard::ISFILEWINDOWOPEN = false;

std::vector<std::shared_ptr<SmashScoreboard::SmashScoreboardWindow>> SmashScoreboard::windowList;

// LoadFromSSSB()

void SmashScoreboard::LoadFromSSSB(const char* filename)
{
	std::fstream defaultLayout;
	defaultLayout.open(filename, std::ios::in);
	if (defaultLayout.is_open())
	{
		//defines whether or not to search for start
		//or to search for attributes
		bool isWindowStarted = false;

		enum {	CHARACTERWINDOW = 1,
				NAMEWINDOW		= 2};

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
							SmashScoreboard::PlayerOneSelectWindow::CreateWindow(windowName, styleIndex);
							break;
						case NAMEWINDOW:
							SmashScoreboard::PlayerTextWindow::CreateWindow(windowName, styleIndex);
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
}

// [SECTION] SmashScoreboardWindow()

SmashScoreboard::SmashScoreboardWindow* SmashScoreboard::SmashScoreboardWindow::CreateWindow()
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<SmashScoreboardWindow>();
	windowList.push_back(win);
	return win.get();
}

// [SECTION] OpenFileWindow

//CONSTRUCTOR DEFINED IN SMASHSCOREBOARDWIDGETS.H AS IT IS PROTECTED

std::shared_ptr<SmashScoreboard::OpenFileWindow> SmashScoreboard::OpenFileWindow::filewindowptr = nullptr;
int SmashScoreboard::OpenFileWindow::currentCallback = 0;

SmashScoreboard::OpenFileWindow* SmashScoreboard::OpenFileWindow::CreateWindow()
{
	if (filewindowptr == nullptr)
	{
		filewindowptr = std::make_shared<OpenFileWindow>();
	}
	ISFILEWINDOWOPEN = true;

	return filewindowptr.get();
}

void SmashScoreboard::OpenFileWindow::CloseWindow()
{
	ISFILEWINDOWOPEN = false;	
}

void SmashScoreboard::OpenFileWindow::SetCallback(int callbackFlag) { currentCallback = callbackFlag; }

void SmashScoreboard::OpenFileWindow::perframe()
{
	if (this->isVisible)
	{
		SmashScoreboard::StyleColorsFileOpenMenu();
		ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin(this->windowName.c_str(), &this->isVisible, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);

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
				strcpy_s(this->folderpathbuf, _testpath.c_str());
			}
			catch (fs::filesystem_error e)
			{
				strcpy_s(this->folderpathbuf, this->readableFolderPath.c_str());
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
				strcpy_s(this->folderpathbuf, _testpath.c_str());
			}
			catch (fs::filesystem_error e)
			{
				strcpy_s(this->folderpathbuf, this->readableFolderPath.c_str());
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

			strcpy_s(this->folderpathbuf, this->readableFolderPath.c_str());
		}

		ImGui::BeginChild((ImGuiID)1, ImVec2(0, windowHeight - 120), true);
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
							strcpy_s(this->folderpathbuf, this->readableFolderPath.c_str());
						}
						else
						{
							this->readableFolderPath = this->readableFolderPath + name;
							strcpy_s(this->folderpathbuf, this->readableFolderPath.c_str());
						}
					}
					else
					{
						this->readableFileName = name;
						strcpy_s(this->filenamebuf, this->readableFileName.c_str());

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
					strcpy_s(this->filenamebuf, this->readableFileName.c_str());
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
			this->isVisible = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Open File"))
		{
			std::string openpath = this->readableFolderPath;
			if (readableFolderPath.back() != '\\')
			{
				openpath += '\\';
			}
			openpath += this->readableFileName;

			if (this->currentCallback == this->LOADSSSBWINDOWCONFIG)
			{
				if (this->LoadSSSBWindowConfig(openpath))
				{
					this->isVisible = false;
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
	{
		return false;
	}
	LoadFromSSSB(filepath.c_str());
	return true;
}

// [SECTION] PlayerOneSelectWindow()

/*
//
//Player Character Selection Window
//
//Creates a reference to a window where the player may select
//the active character and skin combo to update a file with.
//
*/
SmashScoreboard::PlayerOneSelectWindow::PlayerOneSelectWindow(std::string winName, int styleIndex)
	:SmashScoreboardWindow(winName), pName(""), styleIndex(styleIndex)
{
	if (this->windowName == "" or this->windowName == "default")
		this->windowName == "Character Selection Window";
}

SmashScoreboard::PlayerOneSelectWindow* SmashScoreboard::PlayerOneSelectWindow::CreateWindow(std::string winName, int styleIndex)
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<PlayerOneSelectWindow>(winName, styleIndex);
	windowList.push_back(win);
	return (PlayerOneSelectWindow*)win.get();
}

void SmashScoreboard::PlayerOneSelectWindow::perframe()
{
	if (this->isVisible)
	{
		StyleColorsFromIndex(this->styleIndex);

		ImGui::Begin((this->windowName + "###" + this->windowName).c_str() , &(this->isVisible));
		ImGui::InputTextWithHint("", "Player 1's character", pName, IM_ARRAYSIZE(pName)); ImGui::SameLine();
		SmashScoreboard::HelpMarker("Input the name of Player 1's character (eg. Mario)");

		int WidthTakenUp = 0;

		for (int i = 0; i < SmashScoreboard::characterList.size(); i++)
		{
			CharacterName& cname = SmashScoreboard::characterList[i];
			if (SmashScoreboard::findLowerSubstring(cname.text.c_str(), pName))
			{
				if (ImGui::CollapsingHeader(cname.text.c_str()))
				{
					//if (ImGui::Button((cname.text + "##button").c_str()))
					for (int j = cname.indexLow; j < cname.indexLow + cname.numImages; j++)
					{
						if (ImGui::ImageButton((ImTextureID)textureList[j], ImVec2(64, 64)))
						{
							int imageNum = j - cname.indexLow + 1;

							const auto copyOptions = fs::copy_options::overwrite_existing;
							std::string topath = "Output/" + this->windowName + ".png";
							fs::path to = topath;
							std::string frompath = "res/ImageCache/Smash Ultimate Full Art/";
							frompath += SmashScoreboard::characterList[i].text.c_str();
							frompath += "/";
							frompath += SmashScoreboard::characterList[i].text.c_str();
							if (imageNum < 10)
								frompath += "_0" + std::to_string(imageNum);
							else if (imageNum >= 10)
								frompath += "_" + std::to_string(imageNum);
							frompath += ".png";
							fs::path from = frompath;
							std::cout << fs::copy_file(from, to, copyOptions) << std::endl;
							std::cout << SmashScoreboard::characterList[i].text.c_str() << std::endl;
						}
						WidthTakenUp += ImGui::GetItemRectSize().x;

						ImVec2 contentSize = ImGui::GetContentRegionAvail();
						if (j + 1 != cname.indexLow + cname.numImages && !(WidthTakenUp + ImGui::GetItemRectSize().x >= contentSize.x))
						{
							ImGui::SameLine();
						}
						else
						{
							WidthTakenUp = 0;
						}
						
					}
				}
			}
		}
		ImGui::End();
	}
}

// [SECTION] AddPlayerSelectWindowWindow()

SmashScoreboard::AddPlayerSelectWindowWindow::AddPlayerSelectWindowWindow()
	:SmashScoreboardWindow("Create New Character Select Window"), newWindowName("") {}

SmashScoreboard::AddPlayerSelectWindowWindow* SmashScoreboard::AddPlayerSelectWindowWindow::CreateWindow()
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<AddPlayerSelectWindowWindow>();
	windowList.push_back(win);
	return (AddPlayerSelectWindowWindow*)win.get();
}

void SmashScoreboard::AddPlayerSelectWindowWindow::perframe()
{
	if (this->isVisible)
	{
		StyleColorsFromIndex(this->styleIndex);

		ImGui::Begin(("Add a new Character Selector Window" + this->windowTitleIdentifier).c_str(), &this->isVisible, ImGuiWindowFlags_AlwaysAutoResize);
		const char* items[] = { "Red", "Blue", "Yellow", "Green"};
		int item_current_idx = this->styleIndex - 1;            // Here our selection data is an index.
		const char* combo_label = items[item_current_idx];  // Label to preview before opening the combo (technically it could be anything)
		if (ImGui::BeginCombo("New Window Color", combo_label))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (item_current_idx == n);
				if (ImGui::Selectable(items[n], is_selected))
				{
					item_current_idx = n;
					this->styleIndex = n + 1;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::InputText("Name of new Window", newWindowName, IM_ARRAYSIZE(newWindowName));

		if (ImGui::Button("Cancel"))
			this->isVisible = false;
		ImGui::SameLine();

		bool shouldDisable = checkForTakenIdentifier(std::string(this->newWindowName)) || strcmp(this->newWindowName, "");

		if (!shouldDisable)
		{
			if (ImGui::Button("Create Window!"))
			{
				PlayerOneSelectWindow::CreateWindow(std::string(this->newWindowName), this->styleIndex);
				this->isVisible = false;
			}
		}
		else
		{
			SmashScoreboard::HelpMarker("Choose a name not already taken by another window, not blank, and not 'default'");
		}
		ImGui::End();
	}
}

// [SECTION] PlayerTextWindow()

SmashScoreboard::PlayerTextWindow::PlayerTextWindow(std::string winName, int styleIndex)
	:SmashScoreboardWindow(winName), pName(""), styleIndex(styleIndex)
{
	if (this->windowName == "" or this->windowName == "default")
		this->windowName == "Player Name Window";

	std::fstream outputFile;
	outputFile.open(("Output/" + this->windowName + ".txt"), std::ios::in);
	if (outputFile.is_open())
	{
		outputFile.getline(this->comparison, IM_ARRAYSIZE(this->comparison));
		outputFile.close();
	}
}

SmashScoreboard::PlayerTextWindow* SmashScoreboard::PlayerTextWindow::CreateWindow(std::string winName, int styleIndex)
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<PlayerTextWindow>(winName, styleIndex);
	windowList.push_back(win);
	return (PlayerTextWindow*)win.get();
}

void SmashScoreboard::PlayerTextWindow::perframe()
{
	if (this->isVisible)
	{
		std::fstream outputFile;
		outputFile.open(("Output/" + this->windowName + ".txt"), std::ios::in);
		if (outputFile.is_open())
		{
			outputFile.getline(this->comparison, IM_ARRAYSIZE(this->comparison));
			outputFile.close();
		}

		ImGui::SetNextWindowSize(ImVec2(400, 100), ImGuiCond_FirstUseEver);

		SmashScoreboard::StyleColorsFromIndex(this->styleIndex);
		ImGui::Begin((this->windowName + "###" + this->windowName).c_str(), &this->isVisible);

		ImGui::Text("Player's Name");
		ImGui::SameLine();
		SmashScoreboard::HelpMarker("Input the name of the player currently competing (ex. 'Hungrybox')");
		ImGui::SameLine();
		if (ImGui::InputText("##nametext", this->pName, IM_ARRAYSIZE(this->pName), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			goto SetFileContents;
		}

		if (strcmp(this->comparison, this->pName) == 0)
		{
			ImGui::Text("Name Updated!");
			ImGui::SameLine();
			SmashScoreboard::HelpMarker("To update the name again, change the text box to something other than what's currently in the .txt file (or what you last typed)");
		}
		else
		{
			if (ImGui::Button("Set Name"))
			{
			SetFileContents: std::fstream outputFile;
				outputFile.open(("Output/" + this->windowName + ".txt"), std::ios::in);
				if (outputFile.is_open())
				{
					outputFile.close();
					outputFile.open("Output/" + this->windowName + ".txt", std::ios::out | std::ios::trunc);
					outputFile << this->pName;
					outputFile.close();
				}
				else
				{
					outputFile.open("Output/" + this->windowName + ".txt", std::ios::out);
					outputFile << this->pName;
					outputFile.close();
				}
			}
			ImGui::SameLine();
			SmashScoreboard::HelpMarker("Click this button when you want to update the file containing the player's name.");
		}

		ImGui::End();
	}
}

// [SECTION] AddPlayerTextWindow()

SmashScoreboard::AddPlayerTextWindow::AddPlayerTextWindow()
	:SmashScoreboardWindow("Create New Player Name Window"), newWindowName("") {}

SmashScoreboard::AddPlayerTextWindow* SmashScoreboard::AddPlayerTextWindow::CreateWindow()
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<AddPlayerTextWindow>();
	windowList.push_back(win);
	return (AddPlayerTextWindow*)win.get();
}

void SmashScoreboard::AddPlayerTextWindow::perframe()
{
	if (this->isVisible)
	{
		StyleColorsFromIndex(this->styleIndex);

		ImGui::Begin(("Add a new Character Selector Window" + this->windowTitleIdentifier).c_str(), &this->isVisible, ImGuiWindowFlags_AlwaysAutoResize);
		const char* items[] = { "Red", "Blue", "Yellow", "Green" };
		int item_current_idx = this->styleIndex - 1;                    // Here our selection data is an index.
		const char* combo_label = items[item_current_idx];  // Label to preview before opening the combo (technically it could be anything)
		if (ImGui::BeginCombo("New Window Color", combo_label))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (item_current_idx == n);
				if (ImGui::Selectable(items[n], is_selected))
				{
					item_current_idx = n;
					this->styleIndex = n + 1;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::InputText("Name of new Window", newWindowName, IM_ARRAYSIZE(newWindowName));
		ImGui::SameLine();
		SmashScoreboard::HelpMarker("This will become not only the name of the new window, but also the name of the .txt file created in the Output folder.");

		if (ImGui::Button("Cancel"))
			this->isVisible = false;
		ImGui::SameLine();

		bool shouldDisable = checkForTakenIdentifier(std::string(this->newWindowName)) || strcmp(this->newWindowName, "") == 0;

		if (!shouldDisable)
		{
			if (ImGui::Button("Create Window!"))
			{
				PlayerTextWindow::CreateWindow(std::string(this->newWindowName), this->styleIndex);
				this->isVisible = false;
			}
		}
		else
		{
			SmashScoreboard::HelpMarker("Choose a name not already taken by another window, not blank, and not 'default'");
		}
		ImGui::End();
	}
}

// [SECTION] Other Functions

bool SmashScoreboard::checkForTakenIdentifier(std::string ident)
{
	if (ident == "default")
		return true;

	for (int i = 0; i < windowList.size(); i++)
	{
		if (windowList[i].get()->windowName == ident)
			return true;
	}
	return false;
}