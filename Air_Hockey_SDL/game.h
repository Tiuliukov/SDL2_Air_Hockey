#pragma once

#include "GUI_Renderer.h"

class AirHockey
{
public:
	AirHockey();
	void startGame();

private:
	void prepareForGame();
	void botMoveStrategy();
	void calcGameState();
	void calculatePosition(ETypeOfElement type);
	void checkBoardLimitsFor(ETypeOfElement type);
	std::unique_ptr<GUI_Renderer> mGameUI;
	std::vector<SElement> mGameElements;
	EDifficulty gameDifficulty = eDifficulty_Easy;
	bool mGamePreparation = false;
	bool mIsPlay = false;
	bool mMute = false;

	const int FPS = 120;
	const int frameDelay = 1000 / FPS;

	unsigned int frameStart;
	int frameTime;


};
