#pragma once
#include<SFML/Graphics.hpp>
#include<iostream>
#include<vector>
#include"Vec2.h"

class Game
{
	int board_size;
	float square_size;
	const int WINDOW_SIZE;
	std::vector<std::vector<int>> board;
	std::vector<Vec2> targets;

	int level;

	Vec2 player_position;
	sf::RenderWindow window;

	bool game_over = false;

	// Game loop handler:
	void game_loop();

	// Systems:
	void input();
	void render();
	void do_win();

	// Render System:
	void draw_player();
	void draw_boxes();
	void draw_targets();
	void draw_walls();
	void draw_background();
	
	void draw_square_at_index(const Vec2& index, int type, int alpha = 255);

	// Logic:
	bool move_player_x(bool to_right);
	bool move_player_y(bool to_up);

	void move_box(bool x_axis, int direction, const Vec2& box_index);

	void place_at(const Vec2& index, int value);
	bool can_be_at(const Vec2& index);

	Vec2 get_position_by_index(const Vec2& index);

	bool check_win();
	void level_up();

	int get_current_level();
	void load_data_of_level();
	void set_level(int level);

	void reset(bool total = false);

public:
	Game(int window_size);
	void play();
};