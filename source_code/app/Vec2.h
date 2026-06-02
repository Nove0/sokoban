#pragma once
#include<iostream>

class Vec2
{
public:
	float x;
	float y;
	Vec2() : Vec2(0, 0) {};
	Vec2(float x, float y) : x(x), y(y) {};

	Vec2& operator=(const Vec2& other)
	{
		x = other.x;
		y = other.y;
		return *this;
	}

	bool operator==(const Vec2& other)
	{
		return x == other.x && y == other.y;
	}

	bool operator!=(const Vec2& other)
	{
		return !(*this == other);
	}
	std::string str() const
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}
};

class IntVec2
{
public:
	int x;
	int y;
	IntVec2() : IntVec2(0, 0) {};
	IntVec2(int x, int y) : x(x), y(y) {};

	IntVec2& operator=(const IntVec2& other)
	{
		x = other.x;
		y = other.y;
		return *this;
	}

	bool operator==(const IntVec2& other)
	{
		return x == other.x && y == other.y;
	}

	bool operator!=(const IntVec2& other)
	{
		return !(*this == other);
	}
	std::string str() const
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}
};
