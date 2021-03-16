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

#include <ft2build.h>
#include FT_FREETYPE_H

#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <map>
#include <thread>
#include <time.h>

#include <ImFileDialog.h>

#include "SmashScoreboardFunctions.h"
#include "SmashScoreboardWidgets.h"

//Version semantics
#define SSSBMAJOR 0
#define SSSBMINOR 0
#define SSSBMICRO 6
#define SSSBNANO  0
#define SSSBBETA  true

#define IMGBASEDIR "res/ImageCache"

namespace fs = std::filesystem;

/**
* Global variable to store the text's VBO because I need it for
* renderWhiteText()
*/
GLuint TextVBO;

/**
* Real quick, we're gonna define a struct for the characters in
* the font, so we can not only write good looking text, but also
* center it horizontally on the screen
*
* Keep in mind that these textures will start at 500, and go until
* about 630. You have been warned
*/


struct Character {
	GLuint			TextureID;		//GLuint for the character
	ImVec2			Size;			//Size of the character
	ImVec2			Bearing;		//Offset from the baseline to the top/left of the character
	unsigned int	Advance;		//Offset to advance to next glyph
};

std::map<char, Character> Characters;

/**
* Quick helper function to translate from screen space
* to the more cartesian plane that OpenGL uses for its
* screen viewport
*/
void CoordsToOpenGL(double& width , double& height, bool absVal = false)
{
	width = (width / SmashScoreboard::windowWidth);
	width = (width -.5) * 2;
	if (absVal)
		width = abs(width);

	height = (height / SmashScoreboard::windowHeight);
	height = (height - .5) * 2;
	if (absVal)
		height = abs(height);
}

/**
* Basically just doesn't make the local coordinates ig
*/
void SizeToOpenGL(double& width, double& height)
{
	width = width / (SmashScoreboard::windowWidth / 2);
	height = height / (SmashScoreboard::windowHeight / 2);
}


/**
* Before calling this function, set the program to use
* the shader that renders to completely white text, as
* well as binding the text VAO (should be gluint 496)
* 
* In this function, we will be binding the VBO a lot,
* so this comment strives to remind you that the VBO
* has a GLuint of 498. It should be binded before using
* this function.
* 
* The height value is based on the window height. The origin
* is the bottom left. Basically, height is how high off the
* baseline you want to be (ie. 60 = 60 px)
*/
void renderWhiteText(std::string text, unsigned int height)
{
	std::string::const_iterator c;

	//This is updated with the width of the overall string
	int overallWidth = 0.0;

	//This holds the starting x value of the string. It will
	//be set on the first trip around, but if this fails for
	//whatever reason, it should default to 0
	double starting_x = 0.0;

	//This, like x, will be set after the first trip around, but
	//is instead based on user input to the function
	double starting_y = 0.0;

	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];
		overallWidth += (ch.Advance >> 6);
	}

	//Calculate starting x value and y value
	starting_x = (SmashScoreboard::windowWidth - overallWidth) / 2;
	starting_y = height;

	CoordsToOpenGL(starting_x, starting_y);

	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];
		
		double xpos = ch.Bearing.x;
		double ypos = (double)ch.Size.y - (double)ch.Bearing.y;

		SizeToOpenGL(xpos, ypos);
		xpos = starting_x + xpos;
		ypos = starting_y - ypos;

		double w = ch.Size.x;
		double h = ch.Size.y;
		SizeToOpenGL(w, h);

		//Update VBO for each character
		float verticies[6][4] = {
			{ xpos,		ypos + h,	0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		//render that shizz
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticies), verticies);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		double increment = ch.Advance >> 6;
		double dummy = 0;
		SizeToOpenGL(increment, dummy);
		starting_x += increment;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}


int main(int argc, char* argv[])
{

	char cwd[400];
	getcwd(cwd, 400);
	
	std::cout << cwd << std::endl;

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

	std::string winTitle = "Smash Scoreboard "
		+ std::to_string(SSSBMAJOR) + "."
		+ std::to_string(SSSBMINOR) + "."
		+ std::to_string(SSSBMICRO);
	
	if (SSSBNANO)
		winTitle += "." + std::to_string(SSSBNANO);
	if (SSSBBETA)
		winTitle += "b";

	SDL_Window* window = SDL_CreateWindow(winTitle.c_str(),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SmashScoreboard::windowWidth, SmashScoreboard::windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	if (window == NULL)
	{
		std::cout << "Error in SDL_CreateWindow(): " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_SetWindowIcon(window, winicon);
	SDL_FreeSurface(winicon);

	//Create OPENGL Context for ImGui
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GLContext threadContext = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);

	//init GLAD
	gladLoadGL();

	std::cout << "Current GL Version: " << glGetString(GL_VERSION) << std::endl;
	
	glClearColor(0, 0, 0, 255);



	/**
	* ALRIGHT, TIME TO IMPLEMENT SOME CUSTOMIZABILITY
	* 
	* This should open an ImGuiWindow that has a radiobutton
	* allowing the user to choose a folder from the res/imgcache folder
	* to load into memory and use. At this point, the only way
	* to change these sets out are by restarting the program.
	*/
	//SETUP IMGUI

	//SETUP DEAR IMGUI CONTEXT
	IMGUI_CHECKVERSION();
	ImGuiContext* tempContext =  ImGui::CreateContext();
	ImGuiIO& init_io = ImGui::GetIO();

	//ImFont* font = io.Fonts->AddFontDefault();
	ImFont* init_arial = init_io.Fonts->AddFontFromFileTTF("./res/font/ARLRDBD.ttf", 18);
	init_io.Fonts->Build();










	/*
	* Define the functions for the ImFileDialog
	* Specifically, CreateTexture and DeleteTexture
	* 
	* Copy-pasted from the ImFileDialog example code
	*/
	ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void* {
		GLuint tex;

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		return (void*)tex;
	};
	ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
		GLuint texID = (GLuint)((uintptr_t)tex);
		glDeleteTextures(1, &texID);
	};









	SDL_Event init_event;
	bool doneWithSetPath = false;
	bool shouldClose = false;

	int selection = 0;

	std::vector<std::string> DirectoriesInPath;
	std::vector<std::string> DirectoriesDisplay;
	std::string ChosenPath;

	//SETUP PLATFORM/RENDERER BINDINGS
	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init();

	while (!doneWithSetPath)
	{
		while (SDL_PollEvent(&init_event))
		{
			switch (init_event.type)
			{
			case SDL_QUIT:
				doneWithSetPath = true;
				shouldClose = true;
				break;
			case SDL_WINDOWEVENT:
				switch (init_event.window.event) {
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					SmashScoreboard::windowWidth = init_event.window.data1;
					SmashScoreboard::windowHeight = init_event.window.data2;
					break;
				}
			}

			ImGui_ImplSDL2_ProcessEvent(&init_event);
		}

		ImGui_ImplSDL2_NewFrame(window);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		ImGui::StyleColorsLight();
#ifndef NDEBUG
		ImGui::ShowDemoWindow();
#endif

		DirectoriesInPath.clear();
		DirectoriesDisplay.clear();

		for (const auto& entry : fs::directory_iterator(IMGBASEDIR))
		{
			if (entry.is_directory())
			{
				std::string path = entry.path().string();
				DirectoriesInPath.push_back(path);

				std::string name = path;
				//We add one to compensate for the slash
				name.erase(0, std::string(IMGBASEDIR).size() + 1);
				DirectoriesDisplay.push_back(name);
			}
		}

		ImGui::Begin("Choose Image Set", (bool*)0, ImGuiColumnsFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

		int i = 0;
		//ImGui::RadioButton("##Choice", true);
		for (std::string disp : DirectoriesDisplay)
		{
			ImGui::RadioButton(disp.c_str(), &selection, i);
			i++;
		}

		ImGui::Separator();

		if (ImGui::Button("Load!"))
		{
			doneWithSetPath = true;
		}

		ImGui::End();

		ImGui::EndFrame();

		ImGui::Render();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	ImGui_ImplSDL2_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();

	ImGui::DestroyContext(tempContext);

	std::string DirectoryToLoad = DirectoriesInPath[selection];
	std::cout << DirectoryToLoad << std::endl << std::endl;
	if (DirectoryToLoad.back() != '/' and DirectoryToLoad.back() != '\\')
	{
		DirectoryToLoad += '/';
	}

	if (shouldClose)
	{
		SmashScoreboard::uninit();

		SDL_DestroyWindow(window);
		SDL_GL_DeleteContext(threadContext);
		SDL_GL_DeleteContext(context);

		return 0;
	}



	/*
	* Initializing the FT2 Library
	* 
	* This library is likely only going to be used for the loading screen
	* function, however it has a massive user experience boost if
	* implemented
	*/

	FT_Library library;
	FT_Error error;

	error = FT_Init_FreeType(&library);
	if (error)
	{
		std::cout << "An error has occured during the loading of FreeType2!" << std::endl;
		std::cout << "Error Code " << error << std::endl;
		return -1;
	}

	FT_Face arial_font_face;

	error = FT_New_Face(library, "res/font/ARLRDBD.ttf", 0, &arial_font_face);
	if (error == FT_Err_Unknown_File_Format)
	{
		std::cout << "An error has occured during the loading of FreeType2!" << std::endl;
		std::cout << "It seems that the font face is readable, but not of the correct type" << std::endl;
		std::cout << "Is the font face supported by FT2?" << std::endl;
		return -1;
	}
	else if (error)
	{
		std::cout << "An error has occured during the loading of FreeType2!" << std::endl;
		std::cout << "It seems that the font face is unreadable... is it broken?" << std::endl;
		return -1;
	}

	error = FT_Set_Pixel_Sizes(arial_font_face, 0, 18);
	if (error)
	{
		std::cout << "Error setting pixel size, this is weird..." << std::endl;
		return -1;
	}

	/**
	* Load the glyphs into the Characters map
	*/

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLuint texture = 500;
	
	for (unsigned char c = 0; c < 128; c++)
	{
		//load that glyph
		if (FT_Load_Char(arial_font_face, c, FT_LOAD_RENDER))
		{
			std::cout << "Didn't load glyph " << char(c) << std::endl;
			continue;
		}

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			arial_font_face->glyph->bitmap.width,
			arial_font_face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			arial_font_face->glyph->bitmap.buffer
		);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//store that glyph
		Character character = {
			texture,
			ImVec2(arial_font_face->glyph->bitmap.width, arial_font_face->glyph->bitmap.rows),
			ImVec2(arial_font_face->glyph->bitmap_left, arial_font_face->glyph->bitmap_top),
			arial_font_face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));

		texture++;
	}

	FT_Done_Face(arial_font_face);
	FT_Done_FreeType(library);




	/**
	* Defining the VAO/VBO for the text
	*/

	GLuint TextVAO = 496;
	glGenVertexArrays(1, &TextVAO);
	glBindVertexArray(TextVAO);


	TextVBO = 498;
	glGenBuffers(1, &TextVBO);
	glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);




	/**
	* Alrighty, now we get to define the charaacter shaders
	*/

	const char* textvertexShaderSource = "#version 330 core\n"
		"layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\n"
		"out vec2 TexCoords;\n\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(vertex.xy, 0.0, 1.0);\n"
		"	TexCoords = vertex.zw;\n"
		"}";

	GLuint textvertexShader;
	textvertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(textvertexShader, 1, &textvertexShaderSource, NULL);
	glCompileShader(textvertexShader);

	int  success;
	char infoLog[512];
	glGetShaderiv(textvertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(textvertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Frag Shader

	const char* textfragmentShaderSource = "#version 330 core\n"
		"in vec2 TexCoords;\n"
		"out vec4 color;\n"
		"uniform sampler2D text;\n"
		"void main()\n"
		"{\n"
		"vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
		"color = vec4(1.0, 1.0, 1.0, 1.0) * sampled;\n"
		"}";
	GLuint textfragmentShader;
	textfragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(textfragmentShader, 1, &textfragmentShaderSource, NULL);
	glCompileShader(textfragmentShader);

	glGetShaderiv(textfragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(textfragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Shader Program

	GLuint textShaderProgram;
	textShaderProgram = glCreateProgram();

	glAttachShader(textShaderProgram, textvertexShader);
	glAttachShader(textShaderProgram, textfragmentShader);
	glLinkProgram(textShaderProgram);

	glGetProgramiv(textShaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(textShaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(textvertexShader);
	glDeleteShader(textfragmentShader);

	glUseProgram(textShaderProgram);




	bool shouldBeRunning = true;
	SDL_Event event;

	//LOAD SMASH SCOREBOARD

	//Load Loading image
	GLuint loader = SmashScoreboard::LoadAndInitTex("res/loader/loader.png", 0, false);

	GLuint backgroundImage = SmashScoreboard::LoadAndInitTex("res/loader/background.png", 0, false);

	GLfloat headerRect_verts[] = {
		
		//positions			//texture coords
		-0.98,	0.98,		0.0,	0.0,
		0.98,	0.98,		1.0,	0.0,
		-0.98,	0.0,		0.0,	1.0,
		-0.98,	0.0,		0.0,	1.0,
		0.98,	0.0,		1.0,	1.0,
		0.98,	0.98,		1.0,	0.0
	};

	GLuint headerRect = 10;
	glGenVertexArrays(1, &headerRect);
	glBindVertexArray(headerRect);

	GLuint headerRect_buf = 11;
	glGenBuffers(1, &headerRect_buf);
	glBindBuffer(GL_ARRAY_BUFFER, headerRect_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(headerRect_verts), headerRect_verts, GL_STATIC_DRAW);

	//Binding Vertex Attributes
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//spinner textures and model

	GLuint spinner_texs[7];

	for (int i = 0; i < 7; i++)
	{
		spinner_texs[i] = SmashScoreboard::LoadAndInitTex(("res/loader/spin_anim/frame-" + std::to_string(i) + ".png").c_str(), 0, false);
	}

	double spinner_xpos = (SmashScoreboard::windowWidth / 2) - 50;
	double spinner_ypos = (SmashScoreboard::windowHeight / 2) - 50;

	double spinner_widthlength = 100;
	double spinner_heightlength = 100;

	CoordsToOpenGL(spinner_xpos, spinner_ypos);
	SizeToOpenGL(spinner_widthlength, spinner_heightlength);

#ifndef NDEBUG
	std::cout << "Spinner_XPOS: " << spinner_xpos << std::endl;
	std::cout << "Spinner_YPOS: " << spinner_ypos << std::endl;
	std::cout << "Spinner_widthlength: " << spinner_widthlength << std::endl;
	std::cout << "Spinner_heightlength: " << spinner_heightlength << std::endl;
#endif

	GLfloat spinnerRect_verts[] = {
		
		//positions																	//texture coords
		spinner_xpos,						spinner_ypos,							0.0,	0.0,
		spinner_xpos + spinner_widthlength,	spinner_ypos,							1.0,	0.0,
		spinner_xpos + spinner_widthlength,	spinner_ypos - spinner_heightlength,	1.0,	1.0,
		spinner_xpos + spinner_widthlength,	spinner_ypos - spinner_heightlength,	1.0,	1.0,
		spinner_xpos,						spinner_ypos - spinner_heightlength,	0.0,	1.0,
		spinner_xpos,						spinner_ypos,							0.0,	0.0,
	};

	GLuint spinnerRect = 12;
	glGenVertexArrays(1, &spinnerRect);
	glBindVertexArray(spinnerRect);

	GLuint spinnerRect_buf = 13;
	glGenBuffers(1, &spinnerRect_buf);
	glBindBuffer(GL_ARRAY_BUFFER, spinnerRect_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(spinnerRect_verts), spinnerRect_verts, GL_STATIC_DRAW);

	//Spinner Vertex Attributes

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Vertex Shader

	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec2 aPos;\n"
		"layout (location = 1) in vec2 aTexCoord;\n"
		"\n"
		"out vec2 TexCoord;\n"
		"\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, 0, 1.0);\n"
		"	TexCoord = aTexCoord;\n"
		"}\0";
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Frag Shader

	const char* fragmentShaderSource = "#version 330 core\n"
		"in vec2 TexCoord;\n"
		"out vec4 FragColor;\n"
		"uniform sampler2D textureData;\n"
		"void main()\n"
		"{\n"
		"FragColor = texture(textureData, TexCoord);\n"
		"}";
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Shader Program

	GLuint shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);

	int i = 0;

	SmashScoreboard::internalsInit();

	std::thread initThread(SmashScoreboard::init, DirectoryToLoad, std::ref(window), threadContext);

	clock_t time_started;
	clock_t time_to_delay;
	
	while (!SmashScoreboard::doneWithInit)
	{
		time_started = clock();

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			}
		}

		i++;
		if (i >= 7)
			i = 0;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		
		glBindTexture(GL_TEXTURE_2D, loader);
		glBindVertexArray(headerRect);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindTexture(GL_TEXTURE_2D, spinner_texs[i]);
		glBindVertexArray(spinnerRect);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glUseProgram(textShaderProgram);
		glBindVertexArray(TextVAO);
		std::string textToRender = SmashScoreboard::processMessage;
		renderWhiteText(textToRender, 150);

		SDL_GL_SwapWindow(window);

		time_to_delay = clock() - time_started;
		time_to_delay = (1000 / 20) - time_to_delay;
		if (time_to_delay <= 0)
			time_to_delay = 0;

		SDL_Delay(time_to_delay);
	}

	initThread.join();

	SDL_GL_DeleteContext(threadContext);

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
			/*SmashScoreboard::OpenFileWindow* win = SmashScoreboard::OpenFileWindow::getWindowPtr();
			if (!SmashScoreboard::ISFILEWINDOWOPEN)
				win->CloseWindow();
			else
			{*/
			ImGui::StyleColorsLight();

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
			//win->perframe();

			if (ifd::FileDialog::Instance().IsDone("WindowConfigLoad"))
			{
				bool result = false;
				if (ifd::FileDialog::Instance().HasResult())
				{
					std::string filepath = ifd::FileDialog::Instance().GetResult().string();

					if (SmashScoreboard::LoadFromSSSB(filepath.c_str()))
						result = true;


					if (result)
					{
						SmashScoreboard::ISFILEWINDOWOPEN = false;
						ifd::FileDialog::Instance().Close();
					}
					else
					{
						SmashScoreboard::DialogWindow::CreateWindow("Loading SSSB File Failed!", "The window config file failed to load!", SmashScoreboard::Dialogs_Warning);
					}
				}
				else
				{
					SmashScoreboard::ISFILEWINDOWOPEN = false;
					ifd::FileDialog::Instance().Close();
				}
			}

			if (ifd::FileDialog::Instance().IsDone("WindowConfigSave"))
			{
				if (ifd::FileDialog::Instance().HasResult())
				{
					std::string filepath = ifd::FileDialog::Instance().GetResult().string();

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
							"The file was saved to:\n\n\""
							+ filepath
							+ "\"", SmashScoreboard::Dialogs_OK);
					}
					else
					{
						SmashScoreboard::DialogWindow::CreateWindow("Failed to save", "The file could not be saved.", SmashScoreboard::Dialogs_Warning);
					}

					SmashScoreboard::ISFILEWINDOWOPEN = false;
					ifd::FileDialog::Instance().Close();
				}
				else
				{
					SmashScoreboard::ISFILEWINDOWOPEN = false;
					ifd::FileDialog::Instance().Close();
				}
			}

			

#ifndef NDEBUG
				ImGui::StyleColorsLight();
				ImGui::ShowDemoWindow();
#endif

				auto backgrounddrawlist = ImGui::GetBackgroundDrawList();
				backgrounddrawlist->AddImage((ImTextureID)backgroundImage, ImVec2(0, 0), ImVec2(SmashScoreboard::windowWidth,
					SmashScoreboard::windowHeight), ImVec2(0, 0), ImVec2(1, 1), ImU32(3439329279));
			//}
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
						SmashScoreboard::ISFILEWINDOWOPEN = true;
						ifd::FileDialog::Instance().Open("WindowConfigLoad", "Load a .SSSB window config", "SSSB Window Config (*.sssb){.sssb}");
						/*SmashScoreboard::OpenFileWindow::CreateWindow(true);
						SmashScoreboard::OpenFileWindow::getWindowPtr()->SetCallback(OpenFileWindowCallback_LOADSSSBWINDOWCONFIG);*/
					}
					if (ImGui::MenuItem("Save Window Config"))
					{
						SmashScoreboard::ISFILEWINDOWOPEN = true;
						ifd::FileDialog::Instance().Save("WindowConfigSave", "Save a .SSSB window config", "SSSB Window Config (*.sssb){.sssb}");
						/*SmashScoreboard::OpenFileWindow::CreateWindow(false);
						SmashScoreboard::OpenFileWindow::getWindowPtr()->SetCallback(SaveFileWindowCallback_SAVESSSBWINDOWCONFIG);*/
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
				if (ImGui::MenuItem("Add Score Window"))
				{
					if (!SmashScoreboard::checkForTakenIdentifier("Score Window"))
					{
						SmashScoreboard::ScoreWindow::CreateWindow("Score Window", 1);
					}
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