#include<SFML/Graphics.hpp>
#include<iostream>
#include<vector>
#include"../app/Vec2.h"

class LevelBuilder
{
	int board_size = 8;
	float square_size;
	const int WINDOW_SIZE;
    const int GREED_SIZE = 200;
	const int ITEMS = 4;
	const int ITEM_SIZE_Y = WINDOW_SIZE / ITEMS;

	int dragging = -1;
	Vec2 dragging_from = {-1, -1};

	std::vector<std::vector<int>> board;
	std::vector<Vec2> targets;

	Vec2 player_position;
	sf::RenderWindow window;

	sf::Font font;

	sf::Vector2f mouse_position = {0, 0};

	bool game_over = false;

	// Game loop handler:
	void game_loop();

	// Systems:
	void input();
	void render();

	// Render System:
	void draw_player();
	void draw_boxes();
	void draw_targets();
	void draw_walls();
	void draw_background();
	void draw_greed();
	void draw_dragging();
	
	void handle_press();
	void handle_release();
	void handle_greed_press();
	void handle_right_press();

	void resize_board(int new_size);

	void draw_square_at_index(const Vec2& index, int type, int alpha = 255);
	void draw_square_at_position(const Vec2& position, int type, int alpha, int size_x, int size_y, bool center = false);

	Vec2 get_position_by_index(const Vec2& index);
	Vec2 get_index_by_position(const sf::Vector2f& position);

    void reset();

	bool found_in_target(const Vec2& index);
	int index_of(const Vec2& index);

	std::string generate_level_str();
	void create_file();
	bool all_digits(const std::string& str);

	std::string aks_for_str(const std::string& promp);
	void load_data_of_level(const std::string& level);
	void ask_and_load_from_file();

	bool conform_level_ok();
public:
	LevelBuilder(int window_size);
	void play();
};