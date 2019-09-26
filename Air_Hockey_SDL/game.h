#pragma once

#include "GUI_Renderer.h"

class AirHockey
{
public:
	AirHockey();
	void startGame();

private:
	void prepareForGame();
	void calcBotPos();
	void calcGameState();
	void hitPuck(ETypeOfElement type);
	void checkBoardLimitsFor(ETypeOfElement type);
	std::unique_ptr<GUI_Renderer> mGameUI;
	std::vector<SElement> mGameElements;
	EDifficulty gameDifficulty = eDifficulty_Easy;
	bool mIsPlay = false;
	bool mMute = false;

	


};
