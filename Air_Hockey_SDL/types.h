#pragma once

enum ETypeOfElement {
	eTypeOfElement_Bot,
	eTypeOfElement_Player,
	eTypeOfElement_Puck
};

enum EDifficulty {
	eDifficulty_Easy,
	eDifficulty_Normal
};

enum EClash {
	eClash_Board,
	eClash_Hit,
	eClash_Goal
};

enum EEvent {
	eEvent_NoEvent,
	eEvent_esc,
	eEvent_PrepareToPlay,
	eEvent_Play,
	eEvent_ChangeDifficulty,
	eEvent_Sound,
	eEvent_Menu
};

struct SElement {
	int8_t score;
	double xCurrPos;
	double yCurrPos;
	double xPrevPos;
	double yPrevPos;
	double xSpeed;	
	double ySpeed; 
};


