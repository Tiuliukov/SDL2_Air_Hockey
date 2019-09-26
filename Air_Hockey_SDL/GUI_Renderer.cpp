#include "GUI_Renderer.h"
#include <string>

//------------------------------------------------------------------------------
static const int sizeOfSRC_Rect = 550;

static const SDL_Color cColorRed = { 255, 0, 0 };
static const SDL_Color cColorBlue = { 0, 0, 255 };
static const int fontScoreH = 40;
static const int fontScoreW = 20;

SDL_Rect Src; 

//==============================================================================
GUI_Renderer::GUI_Renderer()
{
	mWindow = SDL_CreateWindow("Air Hockey", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, boardWidth, boardHeight, SDL_WINDOW_SHOWN);
	if (!mWindow)
	{
		std::cerr << "SDL error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer)
	{
		std::cerr << "SDL error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	loadFont();
	loadImages();
	loadSounds();

	Src = { 0, 0, sizeOfSRC_Rect, sizeOfSRC_Rect };
}

GUI_Renderer::~GUI_Renderer()
{
	SDL_DestroyTexture(mGameField);
	SDL_DestroyTexture(mDifficultyEasy);
	SDL_DestroyTexture(mDifficultyNormal);
	SDL_DestroyTexture(mMallet);
	SDL_DestroyTexture(mMalletBot);
	SDL_DestroyTexture(mPuck);
	SDL_DestroyTexture(mGameScore);
	SDL_DestroyTexture(mSpeakerOn);
	SDL_DestroyTexture(mSpeakerOff);
	TTF_CloseFont(mFont);
	Mix_FreeMusic(mSound);
	Mix_FreeChunk(mBoard);
	Mix_FreeChunk(mHit);
	Mix_FreeChunk(mGoal);
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	Mix_CloseAudio();
	Mix_Quit();
	TTF_Quit();
	SDL_Quit();
}

//------------------------------------------------------------------------------
void GUI_Renderer::loadFont()
{
	if (TTF_Init() < 0)
	{
		std::cerr << "TTF error: " << TTF_GetError() << std::endl;
		exit(3);
	}

	mFont = TTF_OpenFont("assets/ArialBlack.ttf", 45);

	if (!mFont)
	{
		std::cerr << "TTF error: " << TTF_GetError() << std::endl;
		exit(3);
	}
}

void GUI_Renderer::loadImages()
{
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) || !(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG))
	{
		std::cerr << "IMG error: " << IMG_GetError() << std::endl;
		exit(2);
	}

	mPuck = IMG_LoadTexture(mRenderer, "assets/puck2.png");
	if (!mPuck)
	{
		std::cerr << "IMG error: " << IMG_GetError() << std::endl;
		exit(2);
	}
	mMallet = IMG_LoadTexture(mRenderer, "assets/Mallet.png");
	if (!mMallet)
	{
		std::cerr << "IMG error: " << IMG_GetError() << std::endl;
		exit(2);
	}
	mMalletBot = IMG_LoadTexture(mRenderer, "assets/MalletBot.png");
	if (!mMalletBot)
	{
		std::cerr << "IMG error: " << IMG_GetError() << std::endl;
		exit(2);
	}

	mSpeakerOn = IMG_LoadTexture(mRenderer, "assets/on.png");							
	if (!mSpeakerOn)
	{
		std::cerr << "IMG error: " << IMG_GetError() << std::endl;
		exit(2);
	}

	mSpeakerOff = IMG_LoadTexture(mRenderer, "assets/mute.png");							
	if (!mSpeakerOff)
	{
		std::cerr << "IMG error: " << IMG_GetError() << std::endl;
		exit(2);
	}

	mGameField = IMG_LoadTexture(mRenderer, "assets/field.jpg");
	if (!mGameField)
	{
		std::cerr << "IMG error: " << IMG_GetError() << std::endl;
		exit(2);
	}

	mDifficultyEasy  = IMG_LoadTexture(mRenderer,"assets/Difficulty_easy.jpg");
	if (!mDifficultyEasy)
	{
		std::cerr << "IMG error: " << IMG_GetError() << std::endl;
		exit(2);
	}

	mDifficultyNormal = IMG_LoadTexture(mRenderer, "assets/Difficulty_normal.jpg");
	if (!mDifficultyNormal)
	{
		std::cerr << "IMG error: " << IMG_GetError() << std::endl;
		exit(2);
	}
}

void GUI_Renderer::loadSounds()
{
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096))
	{
		std::cerr << "MIX error: " << Mix_GetError() << std::endl;
		exit(4);
	}

	mSound = Mix_LoadMUS("assets/music.mp3");
	if (!mSound)
	{
		std::cerr << "MIX error: " << Mix_GetError() << std::endl;
		exit(4);
	}
	mBoard = Mix_LoadWAV("assets/hitBoard.wav");
	if (!mBoard)
	{
		std::cerr << "MIX error: " << Mix_GetError() << std::endl;
		exit(4);
	}
	mHit = Mix_LoadWAV("assets/hitPuck.wav");
	if (!mHit)
	{
		std::cerr << "MIX error: " << Mix_GetError() << std::endl;
		exit(4);
	}
	mGoal = Mix_LoadWAV("assets/goal.wav");
	if (!mGoal)
	{
		std::cerr << "MIX error: " << Mix_GetError() << std::endl;
		exit(4);
	}
	Mix_PlayMusic(mSound, -1);
}

EEvent GUI_Renderer::checkEvent(SElement & inMallet) const
{
	SDL_Event event;

	if (!SDL_PollEvent(&event))
		return eEvent_NoEvent;

	switch (event.type)
	{
	case SDL_QUIT:
		return eEvent_esc;

	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			return eEvent_Menu;
		case SDLK_m:
			return eEvent_Sound;
		}
		break;

	case SDL_MOUSEMOTION:
		inMallet.xPrevPos = inMallet.xCurrPos;
		inMallet.yPrevPos = inMallet.yCurrPos;
		inMallet.xCurrPos = event.motion.x - malletRadius;
		inMallet.yCurrPos = event.motion.y - malletRadius;
		break;

	case SDL_MOUSEBUTTONUP:
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			if ((event.button.y > 125) && (event.button.y < 175) && 				// Play button borders
				(event.button.x > 135) && (event.button.x < 300))
				return eEvent_Play;

			if ((event.button.y > 225) && (event.button.y < 265)) 					// Difficulty button borders
				return eEvent_ChangeDifficulty;

			//if ((event.button.y > 305) && (event.button.y < 345) && 				// Settings button borders
			//	(event.button.x > 135) && (event.button.x < 300))
			//	return eEvent_Settings;
			
			if (event.button.y > boardHeight - speakerDiameter - borderWidth && 				// SoundOn/SouundOff button borders
				event.button.x < speakerDiameter + borderWidth)
				return eEvent_Sound;
		}
		if (event.button.button == SDL_BUTTON_RIGHT)
			return eEvent_Menu;
		break;

	default:
		break;
	}

	return eEvent_NoEvent;
}

void GUI_Renderer::draw(const std::vector<SElement> & inElements)
{
	SDL_RenderClear(mRenderer);
	SDL_RenderCopy(mRenderer, mGameField, NULL, NULL);

	SDL_Rect Dst;
	Dst.x = boardWidth - fontScoreW * 2;
	Dst.h = fontScoreH;
	Dst.w = fontScoreW;

	// Draw current score for Bot
	Dst.y = boardHeight / 2 - fontScoreH;
	SDL_Color mColScoreBot = (inElements[eTypeOfElement_Bot].score >= inElements[eTypeOfElement_Player].score) ? cColorRed : cColorBlue ;
	mTTF = TTF_RenderText_Solid(mFont, std::to_string(inElements[eTypeOfElement_Bot].score).c_str(), mColScoreBot);
	mGameScore = SDL_CreateTextureFromSurface(mRenderer, mTTF);
	SDL_RenderCopy(mRenderer, mGameScore, 0, &Dst);
	SDL_FreeSurface(mTTF);
	SDL_DestroyTexture(mGameScore);

	// Draw current score for Player
	Dst.y = boardHeight / 2;
	SDL_Color mColScorePlayer = (inElements[eTypeOfElement_Player].score > inElements[eTypeOfElement_Bot].score ? cColorRed : cColorBlue);
	mTTF = TTF_RenderText_Solid(mFont, std::to_string(inElements[eTypeOfElement_Player].score).c_str(), mColScorePlayer);
	mGameScore = SDL_CreateTextureFromSurface(mRenderer, mTTF);
	SDL_RenderCopy(mRenderer, mGameScore, 0, &Dst);
	SDL_FreeSurface(mTTF);
	SDL_DestroyTexture(mGameScore);

	// Draw Puck
	Dst.h = Dst.w = puckDiameter;
	Dst.x = inElements[eTypeOfElement_Puck].xCurrPos; 
	Dst.y = inElements[eTypeOfElement_Puck].yCurrPos;
	SDL_RenderCopy(mRenderer, mPuck, &Src, &Dst);

	// Draw Bot
	Dst.h = Dst.w = malletDiameter;
	Dst.x = inElements[eTypeOfElement_Bot].xCurrPos;
	Dst.y = inElements[eTypeOfElement_Bot].yCurrPos;
	SDL_RenderCopy(mRenderer, mMalletBot, &Src, &Dst);

	// Draw Player
	Dst.x = inElements[eTypeOfElement_Player].xCurrPos;
	Dst.y = inElements[eTypeOfElement_Player].yCurrPos;
	SDL_RenderCopy(mRenderer, mMallet, &Src, &Dst);

	// Draw SoundOn/SoundOff button
	drawSpeaker();
	SDL_RenderPresent(mRenderer);
}

void GUI_Renderer::newGame(EDifficulty difficulty)
{
	SDL_RenderClear(mRenderer);
	SDL_RenderCopy(mRenderer, mDifficultyEasy, NULL, NULL);
	SDL_RenderPresent(mRenderer);

	if (difficulty == eDifficulty_Easy) {
		SDL_RenderClear(mRenderer);
		SDL_RenderCopy(mRenderer, mDifficultyEasy, NULL, NULL);
		SDL_RenderPresent(mRenderer);
	}
	if (difficulty == eDifficulty_Normal) {
		SDL_RenderClear(mRenderer);
		SDL_RenderCopy(mRenderer, mDifficultyNormal, NULL, NULL);
		SDL_RenderPresent(mRenderer);
	}
	drawSpeaker();
	SDL_RenderPresent(mRenderer);
}

bool GUI_Renderer::enableSound()
{
	soundOn = !soundOn;
	if (soundOn)
		Mix_ResumeMusic();
	else
		Mix_PauseMusic();
	return soundOn;
}



void GUI_Renderer::drawSpeaker()
{																													
	if (soundOn){
		SDL_Rect mDst{ borderWidth, boardHeight - speakerDiameter - borderWidth, speakerDiameter, speakerDiameter }; 
		SDL_RenderCopy(mRenderer, mSpeakerOn, &Src, &mDst);
	}
	else {
		SDL_Rect mDst{ borderWidth, boardHeight - speakerDiameter - borderWidth, speakerDiameter, speakerDiameter }; 
		SDL_RenderCopy(mRenderer, mSpeakerOff, &Src, &mDst);
	}

	
}

void GUI_Renderer::playClashSound(EClash sound)
{
	if (!soundOn)
		return;

	if (sound == eClash_Board)
		Mix_PlayChannel(-1, mBoard, 0);
	else if (sound == eClash_Hit)
		Mix_PlayChannel(-1, mHit, 0);
	else {
		Mix_PlayChannel(-1, mGoal, 0);
		SDL_Delay(300);						//pause 0.3s before a new round
	}
}

