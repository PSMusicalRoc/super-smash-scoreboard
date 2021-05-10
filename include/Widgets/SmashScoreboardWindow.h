#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>



/**
 * Project wide global variables are kept in this file, as it'll
 * inevitably get shared around through inheritance. When creating
 * variables, note what the variable does and what windows it's
 * affecting in comments so that VSCode and Visual Studio pick it
 * up.
 */









/**
 * Window File Layout Notes
 * 
 * This isn't a clear example of the style guide around the
 * Window files, but I hope this is good enough for now.
 * 
 * Each window file should contain two things:
 *      1) The Window itself, defined first and containing all
 *          of the code it needs to function properly.
 *      2) A creator window, which defines the way that the first
 *          window is created. For an example, look at ScoreWindow.h
 * 
 * Let's go into detail on both of these things real quickly.
 * 
 * 1) Base Window
 *      This should always be defined first in the header, and its
 *      section should be first in the implementation file. Its
 *      internal name should be clear and concise (ScoreWindow rather
 *      than SmashScoreboardPlayerScoreWindow). It should have definitions
 *      and implementations for all of the following functions:
 *          Constructor
 *          Destructor (can be blank)
 *          CreateWindow (type is a pointer to the window itself)
 *          void perframe
 *          std::string exportToSSSB
 *      Below is the acual code for the virtual functions, so take a look at
 *      what their parameters are and code appropriately.
 * 
 *      The implementation file should also follow this standard, and functions
 *      should be defined in this order:
 *          Constructor
 *          Destructor (if it's more than just {})
 *          CreateWindow
 *          void perframe
 *          std::string exportToSSSB
 *      There should also be a full line of whitespace between each implementation
 *      function.
 * 
 * 2) Creation Window
 *      This is always second in the header file and second in the implementation
 *      file. The internal name of this window should be the above window's name +
 * 		"Creator". (ie ScoreWindowCreator). These windows again follow the same
 * 		virtual functions that the above windows do. The difference is that the
 * 		perframe function must contain:
 * 			a) A space to put the new window's name (and a check to make sure
 * 				that the name isn't already taken)
 * 			b) A color selector that sets the colorindex of the new window (and
 * 				the current creator window)
 * 			c) A button that when clicked creates a new instance of the base window
 * 				with the configurations selected in the Creator window.
 */


namespace SmashScoreboard {
    struct SmashScoreboardWindow
	{
		std::string windowName;

		//Please don't use this, use CreateWindow instead
		SmashScoreboardWindow(std::string winName = "default", std::string winType = "Normal Window")
			: windowName(winName), windowTypeIdentifier(winType)
		{}

		~SmashScoreboardWindow() {}
		static SmashScoreboardWindow* CreateWindow();

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


    /**
     * This is the focal point of the app. This window list stores each
     * active window and dynamically manages the windows as the app runs.
     * This will be shared throughout the application, but has to be defined
     * here for ease of access to all the child windows. It makes things harder
     * in this class, but for now, it's fine.
     */
    extern std::vector<std::shared_ptr<SmashScoreboardWindow>> windowList;


	/**
	 * Another non usual inclusion here, these are functions that are universal
	 * across all of the window classes but aren't members of those classes.
	 */

	/**
	 * This funtion scans through windowList and checks if the given identifier is
	 * taken by another window.
	 * 
	 * Returns true if it's taken, false if not
	 */
	bool checkForTakenIdentifier(std::string ident);
}