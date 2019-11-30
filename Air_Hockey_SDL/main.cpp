#include "game.h"

#undef main

int main()
{
	try
	{
		AirHockey ah;
		ah.startGame();
	}
	catch (const std::exception& ex)
	{
		std::cout << "error: " << ex.what() << std::endl;
	}

	return 0;
}
