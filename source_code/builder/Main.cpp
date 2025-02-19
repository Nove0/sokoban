#include"Builder.h"

int main()
{
	LevelBuilder* game = new LevelBuilder(800);
	game->play();
	delete game;
	return 0;
}
