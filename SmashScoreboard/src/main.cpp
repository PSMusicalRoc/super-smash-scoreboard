#include <string>
#include <algorithm>
#include <SDL.h>
#include <SDL_image.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <iostream>

#include "SmashScoreboardFunctions.h"
#include "SmashScoreboardWidgets.h"

int main(int argc, char* argv[])
{
	//SETUP SDL2

	if (SDL_Init(SDL_INIT_EVERYTHING) > 0)
	{
		std::cout << "Error in SDL_Init(): " << SDL_GetError() << std::endl;
	}

	int width = 1280;
	int height = 720;

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Window* window = SDL_CreateWindow("Smash Scoreboard I dunno",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	if (window == NULL)
	{
		std::cout << "Error in SDL_CreateWindow(): " << SDL_GetError() << std::endl;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	if (renderer == NULL)
	{
		std::cout << "Error in SDL_CreateRenderer(): " << SDL_GetError() << std::endl;
	}
	else
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	}

	//Create OPENGL Context for ImGui
	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);

	gladLoadGL();

	std::cout << "Current GL Version: " << glGetString(GL_VERSION) << std::endl;
	
	glClearColor(0, 0, 0, 255);
	

	bool shouldBeRunning = true;
	SDL_Event event;

	//SETUP SMASH SCOREBOARD NAMESPACE

	if (!SmashScoreboard::init("res/ImageCache/Smash Ultimate Full Art/_CharList.txt"))
	{
		try
		{
			throw "Failed to init Smash Scoreboard engine";
		}
		catch (...)
		{
			std::cout << "Failed to init Smash Scoreboard engine" << std::endl;
		}
	}

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

	//SETUP WINDOW STYLES
	ImGui::StyleColorsLight();
	ImGuiStyle& defaultStyle = ImGui::GetStyle();
	ImGuiStyle& p1CharStyle = ImGui::GetStyle();

	//GAME LOOP

	SmashScoreboard::PlayerCharacterSelectWindow* player1CharWindow = new SmashScoreboard::PlayerCharacterSelectWindow();

	while (shouldBeRunning)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				shouldBeRunning = false;
			}

			ImGui_ImplSDL2_ProcessEvent(&event);
		}


		ImGui_ImplSDL2_NewFrame(window);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		/*ImGui::Begin("Main Window");
		ImGui::Text("hello world");
		ImGui::Button("button");
		//ImGui::ImageButton()
		ImGui::End();*/

		player1CharWindow->perframe();

		ImGui::EndFrame();

		ImGui::Render();
		
		//SDL_RenderClear(renderer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//SDL_RenderPresent(renderer);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	//END GAME
	ImGui::DestroyContext();
	ImGui_ImplSDL2_Shutdown();

	SmashScoreboard::uninit();

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_GL_DeleteContext(context);

	return 0;
}