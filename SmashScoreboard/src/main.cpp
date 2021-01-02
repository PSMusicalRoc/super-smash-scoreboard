#include <string>
#include <algorithm>
#include <SDL.h>
#include <SDL_image.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <iostream>
#include <thread>
#include <typeinfo>

#include "SmashScoreboardFunctions.h"
#include "SmashScoreboardWidgets.h"

int main(int argc, char* argv[])
{
	//SETUP SDL2

	if (SDL_Init(SDL_INIT_EVERYTHING) > 0)
	{
		std::cout << "Error in SDL_Init(): " << SDL_GetError() << std::endl;
		return -1;
	}

	//Window width
	int width = 1280;

	//Window height
	int height = 720;

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		std::cout << "Error in IMG_Init(): " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_Surface* winicon = IMG_Load("res/loader/icon.png");

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Window* window = SDL_CreateWindow("Smash Scoreboard I dunno",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

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
	destRect.x = (width / 2) - (destRect.w / 2);
	destRect.y = (height / 2) - (destRect.h / 2);

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, loader, NULL, &destRect);
	SDL_RenderPresent(renderer);

	SmashScoreboard::init("res/ImageCache/Smash Ultimate Full Art/_CharList.txt");

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

	//GAME LOOP

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
						width = event.window.data1;
						height = event.window.data2;
						break;
					}
				}

				ImGui_ImplSDL2_ProcessEvent(&event);
			}


			ImGui_ImplSDL2_NewFrame(window);
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

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
			backgrounddrawlist->AddImage((ImTextureID)backgroundImage, ImVec2(0, 0), ImVec2(width, height), ImVec2(0, 0), ImVec2(1, 1), ImU32(3439329279));

			ImGui::EndFrame();

			ImGui::Render();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			SDL_GL_SwapWindow(window);
		}
	}

	//Create Windows
	SmashScoreboard::PlayerOneSelectWindow::CreateWindow("Helfen");

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
					width = event.window.data1;
					height = event.window.data2;
					break;
				}
			}

			ImGui_ImplSDL2_ProcessEvent(&event);
		}


		ImGui_ImplSDL2_NewFrame(window);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		//Menu Bar

		ImGui::StyleColorsDark();

		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit"))
				shouldBeRunning = false;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Add Window"))
		{
			if (ImGui::MenuItem("Add Character Select Window"))
			{
				SmashScoreboard::AddPlayerSelectWindowWindow::CreateWindow();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

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
		backgrounddrawlist->AddImage((ImTextureID)backgroundImage, ImVec2(0, 0), ImVec2(width, height), ImVec2(0, 0), ImVec2(1, 1), ImU32(3439329279));

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