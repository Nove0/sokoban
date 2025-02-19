#include "Game.h"
#include "Images.h"
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

Game::Game(int window_size) :
	WINDOW_SIZE(window_size),
	window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Cool Game")
{
	window.setFramerateLimit(60);
	Images::load_data();
	level = get_current_level();
	load_data_of_level();
}

void Game::input()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
		{
			game_over = true;
		}
		else if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D)
			{
				move_player_x(true);
			}
			else if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) 
			{
				move_player_x(false);
			}
			else if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W)
			{
				move_player_y(true);
			}
			else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
			{
				move_player_y(false);
			}
			else if(event.key.code == sf::Keyboard::R)
			{
				reset();
			}
			else if(event.key.code == sf::Keyboard::T)
			{
				reset(true);
			}
		}
	}
}

void Game::render()
{
	window.clear();
	draw_background();
	draw_walls();
	draw_boxes();
	draw_targets();
	draw_player();
	window.display();
}

void Game::do_win()
{
	if(check_win())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		level_up();
	}
}

void Game::game_loop()
{
	while (!game_over)
	{
		input();
		render();
		do_win();
	}
}

void Game::draw_player()
{
	draw_square_at_index(player_position, 0);
}

void Game::draw_boxes()
{
	for(int x = 0; x < board_size; x++)
	{
		for(int y = 0; y < board_size; y++)
		{
			if(board[x][y] == 2)
			{
				draw_square_at_index(Vec2(x, y), 2);
			}
		}
	}
}

void Game::draw_targets()
{
	for(auto index: targets)
	{
		int alpha = board[index.x][index.y] == 2 ? 100 : 255;
		draw_square_at_index(index, 4, alpha);
	}
}

void Game::draw_walls()
{
	for(int x = 0; x < board_size; x++)
	{
		for(int y = 0; y < board_size; y++)
		{
			if(board[x][y] == 1)
			{
				draw_square_at_index(Vec2(x, y), 1);
			}
		}
	}

    sf::RectangleShape line;
    line.setFillColor(sf::Color::Black);  

    for(int x = 1; x < board_size; x++)
    {
        line.setSize(sf::Vector2f(1, WINDOW_SIZE)); 
        line.setPosition(x * square_size, 0);  
        window.draw(line);
    }

    // Draw horizontal lines
    for(int y = 1; y < board_size; y++)
    {
        line.setSize(sf::Vector2f(WINDOW_SIZE, 1));  
        line.setPosition(0, y * square_size); 
        window.draw(line);
    }
}

void Game::draw_background()
{
	for(int x = 0; x < board_size; x++)
	{
		for(int y = 0; y < board_size; y++)
		{
			draw_square_at_index(Vec2(x, y), 3, 128);
		}
	}
}

void Game::draw_square_at_index(const Vec2 &index, int type, int alpha)
{
	Vec2 square_position = get_position_by_index(Vec2(index.x, index.y));
	sf::Sprite sprite = Images::get_image(type, square_size, square_size);
	sprite.setColor(sf::Color(255, 255, 255, alpha));
	sprite.setPosition(square_position.x, square_position.y);
	window.draw(sprite);
}

bool Game::move_player_x(bool to_right)
{
	int direction = to_right ? 1 : -1;
	Vec2 res_index(player_position.x + direction, player_position.y);

	move_box(true, direction, res_index);

	if (!can_be_at(res_index))
	{
		return false;
	}

	player_position.x = res_index.x;
	return true;
}

bool Game::move_player_y(bool to_up)
{
	int direction = to_up ? -1: 1;
	Vec2 res_index(player_position.x, player_position.y + direction);

	move_box(false, direction, res_index);

	if (!can_be_at(res_index))
	{
		return false;
	}
	
	player_position.y = res_index.y;
	return true;
}

void Game::move_box(bool x_axis, int direction, const Vec2& box_index)
{
    if(box_index.x < 0 || box_index.x >= board_size || box_index.y < 0 || box_index.y >= board_size || board[box_index.x][box_index.y] != 2)
    {
        return;
    }

    if(x_axis) 
    {
        Vec2 res_index = Vec2(box_index.x + direction, box_index.y);
        if(can_be_at(res_index))
        {
            place_at(res_index, 2);
            place_at(box_index, 0);   
        }
    }
    else 
    {
        Vec2 res_index = Vec2(box_index.x, box_index.y + direction);
        if(can_be_at(res_index))
        {
            place_at(res_index, 2);
            place_at(box_index, 0);      
        }
    }
}

void Game::place_at(const Vec2 &index, int value)
{
	board[index.x][index.y] = value;
}

bool Game::can_be_at(const Vec2 &index)
{
    return index.x >= 0 && index.x < board_size &&
           index.y >= 0 && index.y < board_size && 
           board[index.x][index.y] == 0;
}

bool Game::check_win()
{
	for(auto index: targets)
	{
		if(board[index.x][index.y] != 2)
		{
			return false;
		}
	}
    return true;
}

Vec2 Game::get_position_by_index(const Vec2& index)
{
	return Vec2(index.x * square_size, index.y * square_size);
}

void Game::level_up()
{
	set_level(level + 1);
	load_data_of_level();
}

int Game::get_current_level()
{
    std::ifstream level_file("../../levels/level.txt");
	if (!level_file.is_open()) 
	{
        std::cerr << "Error: Unable to open level file" << std::endl;
        std::exit(1);
    }

	std::string documentation;

	int l;
	level_file >> documentation >> l;

	level_file.close();
	return l;
}

void Game::load_data_of_level()
{
    std::string file_path = "../../levels/" + std::to_string(level) + ".txt";
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file of level " << level << std::endl;
        std::exit(1);
    }
    
    std::string documentation;
    std::string comma;
    int x, y;

    file >> documentation >> board_size;
	square_size = WINDOW_SIZE / (float)board_size;
    
    board.clear();
    board.resize(board_size, std::vector<int>(board_size, 0));

    file >> documentation >> player_position.x >> player_position.y; 

	// Boxes: 
    file >> documentation;
    while (file >> x >> y) {
        board[x][y] = 2;
        if (file.peek() == '\n') break;
		file >> comma;
    }

	targets.clear();
	// Targets: 
	file >> documentation;
	while(file >> x >> y)
	{
		targets.push_back(Vec2(x, y));
		if (file.peek() == '\n') break;
		file >> comma;
	}

	// Walls: 
    file >> documentation;
    while (file >> x >> y) {
        board[x][y] = 1;
		if (file.peek() == '\n') break;
		file >> comma;
    }

    file.close();
}

void Game::set_level(int level)
{
	std::ofstream file("levels/level.txt");
	this->level = level;
	file.clear();
	std::string text = "\n\nlevel writing format:\n\tBoard_size: width\n\tPlayer_starting_index: x y\n\tBoxes_indexes: x y , x y ...\n\tTargets_indexes: x y , x y ...\n\tWalls_indexes: x y , x y ...";

	file << "current_level: " + std::to_string(level);
	file << text;
	file.close();
}

void Game::reset(bool total)
{
	if(total)
	{
		level = 1;
	}
	set_level(level);
	load_data_of_level();
}

void Game::play()
{
	game_loop();
}
