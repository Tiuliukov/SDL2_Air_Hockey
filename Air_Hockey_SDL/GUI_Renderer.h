#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_TTF.h>
#include <SDL_mixer.h>

#include <iostream>
#include <vector>

#include "types.h"

const int boardWidth = 450;           //standard air hockey field size 232/124. Ratio ~1.855                         
const int boardHeight = 840;
const int malletDiameter = 50;
const int speakerDiameter = 40;
const int puckDiameter = 40;
const int malletRadius = 25;
const int puckRadius = 20;
const int gateWidth = 170;
const int borderWidth = 5;					//border thickness around the playing field



#define MAX_SPEED puckDiameter / 2 

class GUI_Renderer
{
public:
	GUI_Renderer();
	~GUI_Renderer();

	void newGame(EDifficulty inDifficulty); 
	EEvent checkEvent(SElement & inMallet) const;
	void draw(const std::vector<SElement> & inElements);
	bool enableSound();
	void playClashSound(EClash s);

private:

	void loadFont();
	void loadImages();
	void loadSounds();
	void drawSpeaker();

	bool soundOn = true;
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	SDL_Surface* mTTF;
	SDL_Texture* mGameField;
	SDL_Texture* mDifficultyEasy;
	SDL_Texture* mDifficultyNormal;
	SDL_Texture* mMallet;
	SDL_Texture* mMalletBot;
	SDL_Texture* mPuck;
	SDL_Texture* mGameScore;
	SDL_Texture* mSpeakerOn;
	SDL_Texture* mSpeakerOff;
	TTF_Font *mFont;
	
	
	Mix_Music* mSound;
	Mix_Chunk* mBoard, *mHit, *mGoal;
};