#include "game.h"

// ------------------------------------------------------------------------------ move to the class!!!!!!!!!!!!!!!
const int leftSideGate = (boardWidth - gateWidth) / 2;
const int rightSideGate = leftSideGate + gateWidth;
bool gameOver = false;

//==============================================================================
AirHockey::AirHockey() : mGameUI(new GUI_Renderer)						
{
	mGameElements.resize(3);		
}

void AirHockey::prepareForGame()
{
	//setting the starting coordinates of the game elements
	const double xStartPosBot = (boardWidth - malletDiameter) / 2;
	const double yStartPosBot = (boardHeight - malletDiameter) / 2;
	const double xStartPosPlayer = xStartPosBot;
	const double yStartPosPlayer = yStartPosBot + yStartPosBot / 2;
	const double xStartPosPuck = xStartPosBot;
	const double yStartPosPuck = (boardWidth - puckDiameter) / 2;


	mGameElements[eTypeOfElement_Bot] = { mGameElements[eTypeOfElement_Bot].score, xStartPosBot, yStartPosBot / 2, xStartPosBot, yStartPosBot / 2, 0, 0 };
	mGameElements[eTypeOfElement_Player] = { mGameElements[eTypeOfElement_Player].score, xStartPosBot, yStartPosPlayer, xStartPosBot, yStartPosPlayer, 0, 0 };
	mGameElements[eTypeOfElement_Puck] = { 0, yStartPosPuck, yStartPosBot, yStartPosPuck, yStartPosBot, 0, 0 };
}

void AirHockey::calculatePosition(ETypeOfElement type)
{
	SElement & hitElem = mGameElements[type];
	SElement & puck = mGameElements[eTypeOfElement_Puck];																					
	
	int impactStrength = 1 + abs(abs(hitElem.xSpeed) > abs(hitElem.ySpeed) ? hitElem.xSpeed : hitElem.ySpeed);	//impact force calculation	
	double displacementX = hitElem.xSpeed / impactStrength;
	double displacementY = hitElem.ySpeed / impactStrength;

	bool doHit = false;
	while (impactStrength)
	{
		hitElem.xPrevPos += displacementX;
		hitElem.yPrevPos += displacementY;

		if (pow(malletRadius + puckRadius, 2) >= pow((hitElem.xPrevPos + malletRadius) - (puck.xCurrPos + puckRadius), 2) + pow((hitElem.yPrevPos + malletRadius) - (puck.yCurrPos + puckRadius), 2))
		{
			doHit = true;

			double hyp = hypot(hitElem.xPrevPos - puck.xCurrPos, hitElem.yPrevPos - puck.yCurrPos);			
			double sin = (hitElem.yPrevPos - puck.yCurrPos) / hyp;
			double cos = (puck.xCurrPos - hitElem.xPrevPos) / hyp;
			double nSpeed = puck.xSpeed * cos - puck.ySpeed * sin;
			double tSpeed = puck.xSpeed * sin + puck.ySpeed * cos;
			nSpeed = -nSpeed;

			puck.xSpeed = tSpeed * sin + nSpeed * cos + hitElem.xSpeed;
			puck.ySpeed = tSpeed * cos - nSpeed * sin + hitElem.ySpeed;

			puck.xCurrPos += puck.xSpeed;			
			puck.yCurrPos += puck.ySpeed;
		}
		--impactStrength;
	}

	if (doHit)
		mGameUI->playClashSound(eClash_Hit);			
}


void AirHockey::calcGameState()
{
	SElement & puck = mGameElements[eTypeOfElement_Puck];

	int puckMaxX = boardWidth - puckDiameter;														///////////////////////////////////////////////////////////////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	int puckMaxY = boardHeight - puckDiameter;

	puck.xCurrPos += puck.xSpeed;
	puck.yCurrPos += puck.ySpeed;

	calculatePosition(eTypeOfElement_Player);
	calculatePosition(eTypeOfElement_Bot);

	//boards
	if (puck.xCurrPos > puckMaxX || puck.xCurrPos <= 0)
	{
		puck.xCurrPos = (puck.xCurrPos > puckMaxX ? puckMaxX * 2 - puck.xCurrPos : borderWidth - puck.xCurrPos);        //calculation of the X coordinate after the collision with the field boundary
		puck.xSpeed *= -1;
		mGameUI->playClashSound(eClash_Board);
	}

	//goal
	if ((puck.xCurrPos > leftSideGate) && puck.xCurrPos < (rightSideGate - puckDiameter))
	{
		if (puck.yCurrPos > (puckMaxY + puckDiameter) || puck.yCurrPos <= -puckDiameter) 
		{
			mGameUI->playClashSound(eClash_Goal);
			mGameElements[(puck.yCurrPos > puckMaxY ? eTypeOfElement_Bot : eTypeOfElement_Player)].score++;           //find out who scored
			
			const int maxScore = 2;													//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


			if (mGameElements[eTypeOfElement_Player].score == maxScore)
			{
				mIsPlay = false;
				gameOver = true;
			}

			if (mGameElements[eTypeOfElement_Bot].score == maxScore)
			{
				mIsPlay = false;
				gameOver = true;
			}

			prepareForGame();
			return;
		}	
	}

	else if (puck.yCurrPos > puckMaxY || puck.yCurrPos <= 0) 
	{
		puck.yCurrPos = (puck.yCurrPos > puckMaxY ? puckMaxY * 2 - puck.yCurrPos : borderWidth - puck.yCurrPos);		//calculation of the Ó coordinate after the collision with the field boundary
		puck.ySpeed *= -1;
		mGameUI->playClashSound(eClash_Board);

	}

	// deceleration 
	puck.xSpeed *= 0.98;
	puck.ySpeed *= 0.98;
	if (abs(puck.xSpeed) < 0.7 && abs(puck.ySpeed) < 0.7)	// stops the puck when its speed gets too small
	{
		puck.xSpeed = 0;
		puck.ySpeed = 0;
	}
}




void AirHockey::checkBoardLimitsFor(ETypeOfElement type) 
{
	SElement & bat = mGameElements[type];

	//Does not allow opponents to go beyond the boundaries of their field
	if (type == eTypeOfElement_Player && bat.yCurrPos < boardHeight / 2)
		bat.yCurrPos = boardHeight / 2;
	/*if (type == eTypeOfElement_Bot && bat.yCurrPos > boardHeight / 2 - malletDiameter)
		bat.yCurrPos = boardHeight / 2 - malletDiameter;*/

	//touching the bat boundary of the field
	if (bat.xCurrPos > boardWidth - malletDiameter - borderWidth)
	{
		mGameUI->playClashSound(eClash_Board);
		bat.xCurrPos = boardWidth - malletDiameter - borderWidth;
	}
	if (bat.xCurrPos < borderWidth)
	{
		mGameUI->playClashSound(eClash_Board);
		bat.xCurrPos = borderWidth;
	}
	if (type == eTypeOfElement_Player && bat.yCurrPos > boardHeight - malletDiameter - borderWidth)
	{
		mGameUI->playClashSound(eClash_Board);
		bat.yCurrPos = boardHeight - malletDiameter - borderWidth;
	}
	if (type == eTypeOfElement_Bot && bat.yCurrPos < borderWidth)
	{
		mGameUI->playClashSound(eClash_Board);
		bat.yCurrPos = borderWidth;
	}
	bat.xSpeed = bat.xCurrPos - bat.xPrevPos;
	bat.ySpeed = bat.yCurrPos - bat.yPrevPos;
}

void AirHockey::botMoveStrategy()
{
	SElement & bot = mGameElements[eTypeOfElement_Bot];
	SElement & puck = mGameElements[eTypeOfElement_Puck];
	double speed = (gameDifficulty == eDifficulty_Normal ? MAX_SPEED / 3 : MAX_SPEED / 5);

	bot.xPrevPos = bot.xCurrPos;
	bot.yPrevPos = bot.yCurrPos;

	//predict position (coordinates) puck	
	double preX = puck.xCurrPos + puck.xSpeed;
	double preY = puck.yCurrPos + puck.ySpeed;
	double distX = abs(preX - bot.xCurrPos);														
	double distY = preY - bot.yCurrPos;																

	bot.xSpeed = (speed < distX ? speed : distX) * (preX < bot.xCurrPos ? -1 : 1);
	if (preY > boardHeight / 2)
		bot.ySpeed -= (bot.yCurrPos - speed > 50 ? speed : 0);
	else if (preY < bot.yCurrPos + malletRadius)
		bot.ySpeed = -speed;
	else if (distY > distX || speed > distX - puckRadius)
		bot.ySpeed = (speed < distY ? speed : distY);
	else
		bot.ySpeed = distY / (distX / speed);

	speed *= speed;
	while (speed < pow(bot.xSpeed, 2) + pow(bot.ySpeed, 2))
	{
		bot.xSpeed *= 0.9;
		bot.ySpeed *= 0.9;
	}
	bot.xCurrPos += bot.xSpeed;
	bot.yCurrPos += bot.ySpeed;

	checkBoardLimitsFor(eTypeOfElement_Bot);
}

void AirHockey::startGame()
{
	mGameUI->gameMenu(gameDifficulty);

	while (true)
	{
		frameStart = SDL_GetTicks();

		EEvent event = mGameUI->checkEvent(mGameElements[eTypeOfElement_Player]);
		switch (event)
		{
		default:
		case eEvent_NoEvent:
			break;
		case eEvent_esc:
			return;
		case eEvent_PrepareToPlay:
			if (!mIsPlay)
			{
				mGamePreparation = true;
			}
			break;
		case eEvent_Play:
			if (!mIsPlay)
			{
				mGamePreparation = false;
				mIsPlay = true;
			}
			break;
		case eEvent_ChangeDifficulty:
			if (!mIsPlay){
				(gameDifficulty == eDifficulty_Easy ? gameDifficulty = eDifficulty_Normal : gameDifficulty = eDifficulty_Easy); 
				mGameUI->gameMenu(gameDifficulty);
			}
			break;

		case eEvent_Sound:
			mMute = mGameUI->enableSound();
			if (!mIsPlay)
				mGameUI->gameMenu(gameDifficulty);
			break;
		case eEvent_Menu:
			if (mIsPlay){
				mIsPlay = false;
				mGameElements[eTypeOfElement_Bot].score = 0;
				mGameElements[eTypeOfElement_Player].score = 0;
				mGameUI->gameMenu(gameDifficulty);
			}
			break;
		}
		if (gameOver == true)
		{
			mGameUI->gameMenu(gameDifficulty);
			mGameElements[eTypeOfElement_Bot].score = 0;
			mGameElements[eTypeOfElement_Player].score = 0;
			gameOver = false;
		}
			

		if (mGamePreparation && !mIsPlay) {
			prepareForGame();
			mGameUI->drawGame(mGameElements, mGamePreparation);
			
		}

		if (!mGamePreparation && mIsPlay){
			botMoveStrategy();
			checkBoardLimitsFor(eTypeOfElement_Player);
			calcGameState();
			mGameUI->drawGame(mGameElements, mGamePreparation);
		}

		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}
}
