#include <string>
#include <algorithm>
#ifdef __linux__ 
    #include <SDL2/SDL.h>
	#include <SDL2/SDL_image.h>
#elif _WIN32
    #include <SDL.h>
	#include <SDL_image.h>
#else
#endif

#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <typeinfo>

#include "SmashScoreboardFunctions.h"
#include "SmashScoreboardWidgets.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
	//SETUP SDL2

	if (SDL_Init(SDL_INIT_EVERYTHING) > 0)
	{
		std::cout << "Error in SDL_Init(): " << SDL_GetError() << std::endl;
		return -1;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		std::cout << "Error in IMG_Init(): " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_Surface* winicon = IMG_Load("res/loader/icon.png");

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Window* window = SDL_CreateWindow("Smash Scoreboard I dunno",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SmashScoreboard::windowWidth, SmashScoreboard::windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	if (window == NULL)
	{
		std::cout << "Error in SDL_CreateWindow(): " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_SetWindowIcon(window, winicon);
	SDL_FreeSurface(winicon);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == NULL)
	{
		std::cout << "Error in SDL_CreateRenderer(): " << SDL_GetError() << std::endl;
		return -1;
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	//Load Loading image
	SDL_Surface* loader_surf = IMG_Load("res/loader/loader.png");
	SDL_Texture* loader = SDL_CreateTextureFromSurface(renderer, loader_surf);

	//Create OPENGL Context for ImGui
	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);

	//init GLAD
	gladLoadGL();

	std::cout << "Current GL Version: " << glGetString(GL_VERSION) << std::endl;
	
	glClearColor(0, 0, 0, 255);
	
	bool shouldBeRunning = true;
	SDL_Event event;

	//LOAD SMASH SCOREBOARD
	GLuint backgroundImage = SmashScoreboard::LoadAndInitTex("res/loader/background.png");

	SDL_Rect destRect;
	destRect.w = loader_surf->w;
	destRect.h = loader_surf->h;
	destRect.x = (SmashScoreboard::windowWidth / 2) - (destRect.w / 2);
	destRect.y = (SmashScoreboard::windowHeight / 2) - (destRect.h / 2);

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, loader, NULL, &destRect);
	SDL_RenderPresent(renderer);

	SmashScoreboard::internalsInit();
	SmashScoreboard::init("res/ImageCache/Smash Ultimate Full Art/_CharList.txt", renderer, loader, destRect);

	//DESTROY RENDERER AND LOADER TEXTURES

	SDL_DestroyRenderer(renderer);
	SDL_FreeSurface(loader_surf);
	SDL_DestroyTexture(loader);

	//DONE NOW

	//SETUP IMGUI

	//SETUP DEAR IMGUI CONTEXT
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	//ImFont* font = io.Fonts->AddFontDefault();
	ImFont* arial = io.Fonts->AddFontFromFileTTF("res/font/ARLRDBD.ttf", 18);
	io.Fonts->Build();

	//SETUP PLATFORM/RENDERER BINDINGS
	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init();

	//Add a nullptr to the beginning of windowList
	//so that it doesn't crash?
	//SmashScoreboard::windowList.push_back(nullptr);

	//[SECTION] If Initialization Fails

	if (!SmashScoreboard::initSuccessful)
	{
		while (shouldBeRunning)
		{
			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
				case SDL_QUIT:
					shouldBeRunning = false;
					break;
				case SDL_WINDOWEVENT:
					switch (event.window.event) {
					case SDL_WINDOWEVENT_SIZE_CHANGED:
						SmashScoreboard::windowWidth = event.window.data1;
						SmashScoreboard::windowHeight = event.window.data2;
						break;
					}
				}

				ImGui_ImplSDL2_ProcessEvent(&event);
			}

			ImGui_ImplSDL2_NewFrame(window);
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			//[IF FILE WINDOW IS OPEN]

			if (SmashScoreboard::ISFILEWINDOWOPEN)
			{
				SmashScoreboard::OpenFileWindow* win = SmashScoreboard::OpenFileWindow::getWindowPtr();
				if (!win->isVisible)
					win->CloseWindow();
				else
					win->perframe();
#ifndef NDEBUG
				//ImGui::StyleColorsLight();
				ImGui::ShowDemoWindow();
#endif
			}
			else
			{
				ImGui::BeginMainMenuBar();

				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Quit"))
						shouldBeRunning = false;
					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();

				//ImGui::StyleColorsLight();
				ImGui::Begin("Failed to initialize");
				ImGui::Text("Smash Scoreboard failed to initialize...");
				ImGui::Text("Please make sure that your path to the .txt file is correct!");

#ifndef NDEBUG
				//ImGui::StyleColorsLight();
				ImGui::ShowDemoWindow();
#endif

				ImGui::End();

				auto backgrounddrawlist = ImGui::GetBackgroundDrawList();
				backgrounddrawlist->AddImage((ImTextureID)backgroundImage, ImVec2(0, 0), ImVec2(SmashScoreboard::windowWidth,
					SmashScoreboard::windowHeight), ImVec2(0, 0), ImVec2(1, 1), ImU32(3439329279));
			}

			ImGui::EndFrame();

			ImGui::Render();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			SDL_GL_SwapWindow(window);
		}
	}

	//Create Windows
	
	SmashScoreboard::LoadFromSSSB("default.sssb");

	//[SECTION] Main Loop

	while (shouldBeRunning)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				shouldBeRunning = false;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					SmashScoreboard::windowWidth = event.window.data1;
					SmashScoreboard::windowHeight = event.window.data2;
					break;
				}
			}

			ImGui_ImplSDL2_ProcessEvent(&event);
		}


		ImGui_ImplSDL2_NewFrame(window);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		//Menu Bar
		if (SmashScoreboard::ISFILEWINDOWOPEN)
		{
			SmashScoreboard::OpenFileWindow* win = SmashScoreboard::OpenFileWindow::getWindowPtr();
			if (!SmashScoreboard::ISFILEWINDOWOPEN)
				win->CloseWindow();
			else
			{
				if (SmashScoreboard::windowList.size() != 0)
				{
					for (int i = 0; i < SmashScoreboard::windowList.size(); i++)
					{
						if (!SmashScoreboard::windowList[i].get()->isVisible)
						{
							SmashScoreboard::SmashScoreboardWindow::DeleteWindow(SmashScoreboard::windowList[i].get(), SmashScoreboard::windowList);
							i--;
						}
						else
						{
							if (SmashScoreboard::windowList[i].get()->windowTypeIdentifier == "Dialog")
								SmashScoreboard::windowList[i].get()->perframe();
						}

					}
				}
				win->perframe();
				auto backgrounddrawlist = ImGui::GetBackgroundDrawList();
				backgrounddrawlist->AddImage((ImTextureID)backgroundImage, ImVec2(0, 0), ImVec2(SmashScoreboard::windowWidth,
					SmashScoreboard::windowHeight), ImVec2(0, 0), ImVec2(1, 1), ImU32(3439329279));
			}
		}
		else
		{
			ImGui::StyleColorsDark();

			ImGui::BeginMainMenuBar();

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("Layout"))
				{
					if (ImGui::MenuItem("Load Window Config"))
					{
						SmashScoreboard::OpenFileWindow::CreateWindow(true);
						SmashScoreboard::OpenFileWindow::getWindowPtr()->SetCallback(OpenFileWindowCallback_LOADSSSBWINDOWCONFIG);
					}
					if (ImGui::MenuItem("Save Window Config"))
					{
						SmashScoreboard::OpenFileWindow::CreateWindow(false);
						SmashScoreboard::OpenFileWindow::getWindowPtr()->SetCallback(SaveFileWindowCallback_SAVESSSBWINDOWCONFIG);
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Quit"))
					shouldBeRunning = false;
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Add Window"))
			{
				if (ImGui::MenuItem("Add Character Select Window"))
				{
					if (!SmashScoreboard::checkForTakenIdentifier("Create New Character Select Window"))
						SmashScoreboard::AddPlayerSelectWindowWindow::CreateWindow();
				}
				if (ImGui::MenuItem("Add Player Name Window"))
				{
					if (!SmashScoreboard::checkForTakenIdentifier("Create New Player Name Window"))
						SmashScoreboard::AddPlayerTextWindow::CreateWindow();
						//SmashScoreboard::PlayerTextWindow::CreateWindow("Test Player", 1);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();

			// [SECTION] Right Click Context Menu

			ImGui::StyleColorsLight();
			if (ImGui::BeginPopupContextVoid("MainMenuPopup"))
			{
				if (ImGui::BeginMenu("New Window"))
				{
					ImGui::Separator();

					if (ImGui::MenuItem("Character Select Window"))
					{
						if (!SmashScoreboard::checkForTakenIdentifier("Create New Character Select Window"))
							SmashScoreboard::AddPlayerSelectWindowWindow::CreateWindow();
					}

					if (ImGui::MenuItem("Name Input Window"))
					{
						if (!SmashScoreboard::checkForTakenIdentifier("Create New Player Name Window"))
							SmashScoreboard::AddPlayerTextWindow::CreateWindow();
					}

					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Quit"))
					shouldBeRunning = false;
				ImGui::EndPopup();
			}

			//Frame

#ifndef NDEBUG
			ImGui::StyleColorsLight();
			ImGui::ShowDemoWindow();
#endif
			if (SmashScoreboard::windowList.size() != 0)
			{
				for (int i = 0; i < SmashScoreboard::windowList.size(); i++)
				{
					if (!SmashScoreboard::windowList[i].get()->isVisible)
					{
						SmashScoreboard::SmashScoreboardWindow::DeleteWindow(SmashScoreboard::windowList[i].get(), SmashScoreboard::windowList);
						i--;
						if (SmashScoreboard::windowList.size() <= 0)
							goto NoWindows;
					}
					else
					{
						SmashScoreboard::windowList[i].get()->perframe();
					}

				}
			}
			NoWindows: auto backgrounddrawlist = ImGui::GetBackgroundDrawList();
			backgrounddrawlist->AddImage((ImTextureID)backgroundImage, ImVec2(0, 0), ImVec2(SmashScoreboard::windowWidth,
				SmashScoreboard::windowHeight), ImVec2(0, 0), ImVec2(1, 1), ImU32(3439329279));
		}
		ImGui::EndFrame();

		ImGui::Render();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	//END GAME
	ImGui::DestroyContext();
	ImGui_ImplSDL2_Shutdown();

	SmashScoreboard::uninit();

	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);

	return 0;
}