#include "Builder.h"
#include "../app/Images.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <thread>
#include <chrono>
#include <filesystem>

LevelBuilder::LevelBuilder(int window_size) :
    WINDOW_SIZE(window_size),
    window(sf::VideoMode(WINDOW_SIZE + GREED_SIZE, WINDOW_SIZE), "Level Builder")
{
    font.loadFromFile("../../DejaVuSans.ttf");
    window.setFramerateLimit(60);
    Images::load_data();
    square_size = (float)WINDOW_SIZE / (float)board_size;
    board.resize(board_size, std::vector<int>(board_size, 0));
    player_position = {0, 0};
}

void LevelBuilder::input()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        auto mp = sf::Mouse::getPosition(window);
        mouse_position = {(float)mp.x, (float)mp.y};
        if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
        {
            game_over = true;
        }
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Up)
            {
                resize_board(board_size + 1);
            }
            else if (event.key.code == sf::Keyboard::Down && board_size > 1)
            {
                resize_board(board_size - 1);
            }
            else if (event.key.code == sf::Keyboard::R)
            {
                reset();
            }
            else if (event.key.code == sf::Keyboard::S)
            {
                if(conform_level_ok())
                {
                    create_file();
                }
            }
            else if (event.key.code == sf::Keyboard::L)
            {
                ask_and_load_from_file();
            }
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if(event.mouseButton.button == sf::Mouse::Left)
            {
                handle_press();
            }
            else
            {
                handle_right_press();
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased)
        {
            if(mouse_position.x > 0 && mouse_position.x < WINDOW_SIZE && mouse_position.y > 0 && mouse_position.y < WINDOW_SIZE)
            {
                handle_release();
            }
            else
            {
                dragging = -1;
                dragging_from = {-1, -1};
            }
        }
    }
}

void LevelBuilder::render()
{
    window.clear();
    draw_background();
    draw_walls();
    draw_boxes();
    draw_targets();
    draw_player();
    draw_greed();
    draw_dragging();
    window.display();
}

void LevelBuilder::game_loop()
{
    while (!game_over)
    {
        input();
        render();
    }
}

void LevelBuilder::draw_player()
{
    draw_square_at_index(player_position, 0);
}

void LevelBuilder::draw_boxes()
{
    for (int x = 0; x < board_size; x++)
    {
        for (int y = 0; y < board_size; y++)
        {
            if (board[x][y] == 2)  
            {
                draw_square_at_index(Vec2(x, y), 2);
            }
        }
    }
}

void LevelBuilder::draw_targets()
{
    for (auto index : targets)
    {
        int alpha = board[index.x][index.y] == 2 ? 100 : 255;
        draw_square_at_index(index, 4, alpha);
    }
}

void LevelBuilder::draw_walls()
{
    for (int x = 0; x < board_size; x++)
    {
        for (int y = 0; y < board_size; y++)
        {
            if (board[x][y] == 1)
            {
                draw_square_at_index(Vec2(x, y), 1);
            }
        }
    }

    sf::RectangleShape line;
    line.setFillColor(sf::Color::Black);

    for (int x = 1; x < board_size; x++)
    {
        line.setSize(sf::Vector2f(1, WINDOW_SIZE));
        line.setPosition(x * square_size, 0);
        window.draw(line);
    }

    for (int y = 1; y < board_size; y++)
    {
        line.setSize(sf::Vector2f(WINDOW_SIZE, 1));
        line.setPosition(0, y * square_size);
        window.draw(line);
    }
}

void LevelBuilder::draw_background()
{
    for (int x = 0; x < board_size; x++)
    {
        for (int y = 0; y < board_size; y++)
        {
            draw_square_at_index(Vec2(x, y), 3, 128);
        }
    }
}

void LevelBuilder::draw_greed()
{
    draw_square_at_position(Vec2(WINDOW_SIZE, 0), 3, 128, GREED_SIZE, GREED_SIZE); 
    draw_square_at_position(Vec2(WINDOW_SIZE, ITEM_SIZE_Y), 1, 255, GREED_SIZE, ITEM_SIZE_Y); 
    draw_square_at_position(Vec2(WINDOW_SIZE, ITEM_SIZE_Y * 2), 2, 255, GREED_SIZE, ITEM_SIZE_Y);  
    draw_square_at_position(Vec2(WINDOW_SIZE, ITEM_SIZE_Y * 3), 4, 255, GREED_SIZE, ITEM_SIZE_Y);  
}

void LevelBuilder::draw_dragging()
{
    if(dragging < 0)
    {
        return;
    }
    if (dragging == 0)
    {
        draw_square_at_position({mouse_position.x, mouse_position.y}, 3, 128, square_size, square_size, true);
    }
    else
    {
        draw_square_at_position({mouse_position.x, mouse_position.y}, dragging, 255, square_size, square_size, true);
    }
}

void LevelBuilder::handle_press()
{
    if (mouse_position.x > WINDOW_SIZE)  
    {
        handle_greed_press();
        return;
    }

    Vec2 index = get_index_by_position(mouse_position);
    if (index.x < 0 || index.x >= board_size || index.y < 0 || index.y >= board_size)
    {
        return;
    }

    if (index == player_position)
    {
        dragging = 5;  
        return;
    }

    dragging_from = index;

    for (int i = 0; i < targets.size(); i++)
    {
        if (targets[i] == index)
        {
            dragging = 4; 
            dragging_from.x = i;
            return;
        }
    }

    dragging = board[index.x][index.y];  
}

void LevelBuilder::handle_release()
{
    if (dragging < 0) return;

    Vec2 index = get_index_by_position(mouse_position);

    if (index.x < 0 || index.x >= board_size || index.y < 0 || index.y >= board_size)
    {
        dragging_from = {-1, -1};
        dragging = -1;
        return;
    }

    if (dragging == 4)
    {
        if (dragging_from.x >= 0 && dragging_from.x < targets.size())
        {
            targets.erase(targets.begin() + dragging_from.x);
        }
        if (!found_in_target(index)) 
        {
            targets.push_back(index);
            if(board[index.x][index.y] != 2) board[index.x][index.y] = 0;
        }
    }
    else if (dragging == 5)
    {
        player_position = index;
    }
    else
    {
        board[index.x][index.y] = dragging;
        if(dragging != 2)
        {
            int index_in_tar = index_of(index);
            if (index_in_tar != -1)
            {
                targets.erase(targets.begin() + index_in_tar);
            }
        }

        if (dragging_from != index && dragging_from.x >= 0 && dragging_from.x < board_size && dragging_from.y >= 0 && dragging_from.y < board_size)
        {
            board[dragging_from.x][dragging_from.y] = 0;
        }
    }

    dragging_from = {-1, -1};
    dragging = -1;
}

void LevelBuilder::handle_greed_press()
{
    int item = mouse_position.y / ITEM_SIZE_Y;
    if (item >= 0 && item <= 4)
    {
        dragging = item == 3 ? 4: item;  // Set the item to be dragged
    }
}

void LevelBuilder::handle_right_press()
{
    Vec2 index = get_index_by_position(mouse_position);
    if(index.x < 0 || index.x >= board_size || index.y < 0 || index.y >= board_size)
    {
        return;
    }
    int tar_index = index_of(index);
    if(tar_index != -1)
    {
        targets.erase(targets.begin() + tar_index);
    }
    else
    {
        board[index.x][index.y] = 0;
    }
}

void LevelBuilder::resize_board(int new_size)
{
    targets.clear();
    board.clear();
    board.resize(new_size, std::vector<int>(new_size, 0));
    board_size = new_size;
    square_size = (float)WINDOW_SIZE / (float)board_size;
    player_position = {0, 0};  // Reset player position
}

void LevelBuilder::draw_square_at_index(const Vec2& index, int type, int alpha)
{
    if (index.x < 0 || index.x >= board_size || index.y < 0 || index.y >= board_size) {
        return;  // Ensure valid index before drawing
    }
    Vec2 square_position = get_position_by_index(index);
    sf::Sprite sprite = Images::get_image(type, square_size, square_size);
    sprite.setColor(sf::Color(255, 255, 255, alpha));
    sprite.setPosition(square_position.x, square_position.y);
    window.draw(sprite);
}

void LevelBuilder::draw_square_at_position(const Vec2 &position, int type, int alpha, int size_x, int size_y, bool center)
{
    sf::Sprite sprite = Images::get_image(type, size_x, size_y);
    sprite.setColor(sf::Color(255, 255, 255, alpha));
    sprite.setPosition(position.x - (center ? size_x / 2.0: 0), position.y - (center ? size_y / 2.0: 0));
    window.draw(sprite);
}

Vec2 LevelBuilder::get_position_by_index(const Vec2& index)
{
    return Vec2(index.x * square_size, index.y * square_size);
}

Vec2 LevelBuilder::get_index_by_position(const sf::Vector2f& position)
{
    int x = position.x / square_size;
    int y = position.y / square_size;
    if (x < 0 || x >= board_size || y < 0 || y >= board_size) {
        return Vec2(-1, -1);  // Return an invalid index if out of bounds
    }
    return Vec2(x, y);
}

void LevelBuilder::reset()
{
    resize_board(board_size);  // Reset board to current size
}

bool LevelBuilder::found_in_target(const Vec2 &index)
{
    return index_of(index) != -1;
}

int LevelBuilder::index_of(const Vec2 &index)
{
    for (int i = 0; i < targets.size(); i++)
    {    
        if (targets[i] == index)
        {
            return i;
        }
    }

    return -1;
}

std::string LevelBuilder::generate_level_str()
{
    std::string text = "";
    text += "Board_size: " + std::to_string(board_size) + "\n";
    text += "Player_starting_index: " + std::to_string(player_position.x) + " " + std::to_string(player_position.y) + "\n";
    text += "Boxes_indexes: ";

    bool first = true;
    for (int x = 0; x < board_size; x++)
    {
        for (int y = 0; y < board_size; y++)
        {
            if (board[x][y] == 2)
            {
                if (!first) text += " , ";
                text += std::to_string(x) + " " + std::to_string(y);
                first = false;
            }
        }
    }

    first = true;
    text += "\nTargets_indexes: ";
    for (auto i: targets)
    {
        if (!first) text += " , ";
        text += std::to_string(i.x) + " " + std::to_string(i.y);
        first = false;
    }

    first = true;
    text += "\nWall_indexes: ";
    for (int x = 0; x < board_size; x++)
    {
        for (int y = 0; y < board_size; y++)
        {
            if (board[x][y] == 1)
            {
                if (!first) text += " , ";
                text += std::to_string(x) + " " + std::to_string(y);
                first = false;
            }
        }
    }

    text += "\n";
    return text;
}

void LevelBuilder::create_file()
{
    std::string name = aks_for_str("Save this level as:");

    if (!all_digits(name))
    {
        std::cout << "File was not created!\n";
        return;
    }

    std::string path = "levels/" + name + ".txt";
    bool was = std::filesystem::exists(path);
    
    std::ofstream level_file(path);
    if (level_file)
    {
        level_file << generate_level_str();
        level_file.close();
        std::cout << "Level " << name << " was" <<  (was ? " overwrite": " created") << " successfully\n";
    }
    else
    {
        std::cout << "Level creation failed, reason: Unknown\n";
    }
}

bool LevelBuilder::all_digits(const std::string &str)
{
    for (char c: str)
    {
        if (!isdigit(c))
        {
            return false;
        }
    }
    return str.length() > 0;
}

void LevelBuilder::play()
{
    game_loop(); 
}

std::string LevelBuilder::aks_for_str(const std::string& prompt_text)
{
    // for the love of f god i cant make it not capture the last entered input before the function was called to i did this
    // yes i tried while(window.poolEvent()) and still capturing
    using c = std::chrono::high_resolution_clock;
    auto start_time = c::now();
    sf::Text prompt(prompt_text, font, 24);
    prompt.setPosition(50, 50);
    prompt.setFillColor(sf::Color::White);

    sf::Text user_input("", font, 24);
    user_input.setPosition(50, 100);
    user_input.setFillColor(sf::Color::White);

    std::string input;
    bool enter_pressed = false;

    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 128)); 
    overlay.setSize(sf::Vector2f(WINDOW_SIZE + GREED_SIZE, WINDOW_SIZE));
    overlay.setPosition(0, 0);

    sf::Event event;
    while (!game_over) 
    {
        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed)
             {
                game_over = true;
                return "";
            }

            if (event.type == sf::Event::TextEntered && (c::now() - start_time).count() > 100000000LL) 
            {
                if (event.text.unicode == '\b') 
                { 
                    if (!input.empty()) {
                        input.pop_back();
                    }
                } else if (event.text.unicode == '\r') 
                {
                    enter_pressed = true;
                } else if (event.text.unicode < 128) 
                { 
                    input += static_cast<char>(event.text.unicode);
                }

                user_input.setString(input);
            }
        }

        if (enter_pressed) 
        {
            return input;
        }

        window.clear();
        draw_background();
        draw_walls();
        draw_boxes();
        draw_targets();
        draw_player();
        draw_greed();

        window.draw(overlay);  
        window.draw(prompt);
        window.draw(user_input);

        window.display();
    }

    return "";
}

void LevelBuilder::load_data_of_level(const std::string& level)
{
    std::string file_path = "../../levels/" + level + ".txt";
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file of level " << level << std::endl;
        return;
    }
    
    std::string documentation;
    std::string comma;
    int x, y;

    file >> documentation >> board_size;
    square_size = WINDOW_SIZE / board_size;
    
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
    while (file >> x >> y)
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

void LevelBuilder::ask_and_load_from_file()
{
    std::string name = aks_for_str("Load level:");
    load_data_of_level(name);
}

bool LevelBuilder::conform_level_ok()
{
    int amount_of_targets = targets.size();
    int amount_of_boxes = 0;
    for(auto x: board)
    {
        for(auto y: x)
        {
            if(y == 2)
            {
                amount_of_boxes++;
            }
        }
    }

    if(amount_of_boxes == amount_of_targets)
    {
        return true;
    }


    std::string prompt = "There are more ";
    prompt += amount_of_boxes > amount_of_targets ? "boxes that targets" : "targets than boxes";
    prompt += "(" + std::to_string(amount_of_boxes) + ", " + std::to_string(amount_of_targets) + ").\n";
    prompt += "Would you like to create level anyway? (y or n)";
    std::string ans = aks_for_str(prompt);
    return !ans.empty() && (ans[0] == 'y' || ans[0] == 'Y');
    
}
