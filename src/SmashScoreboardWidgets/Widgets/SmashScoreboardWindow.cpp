#include "Widgets/SmashScoreboardWindow.h"

SmashScoreboard::SmashScoreboardWindow* SmashScoreboard::SmashScoreboardWindow::CreateWindow()
{
	std::shared_ptr<SmashScoreboardWindow> win = std::make_shared<SmashScoreboardWindow>();
	windowList.push_back(win);
	return win.get();
}

std::vector<std::shared_ptr<SmashScoreboard::SmashScoreboardWindow>> SmashScoreboard::windowList;





/**
 * functions
 */

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