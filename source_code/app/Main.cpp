// g++ lib links: -lsfml-graphics -lsfml-window -lsfml-system

#include"Game.h"

int main()
{
	Game* game = new Game(800);
	game->play();
	delete game;
	return 0;
}
