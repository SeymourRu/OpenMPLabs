#pragma once
#include <chrono>

enum CellState
{
	OutOfField = -2,
	Exit = -1,
	Free = 0,
	Gold = 1,
	Player = 2,
};

enum Direction
{
	Left = 0,
	Right = 1,
	Up = 2,
	Down = 3,
	Last = 4,
};

struct SimplePlayer
{
	int id;
	int coordX;
	int coordY;
	int goldCount;
	bool finished = false;
	std::chrono::high_resolution_clock::time_point finishTime;
	//Direction previousDirection;

	SimplePlayer(int id, int x, int y, int money = 0)
	{
		this->id = id;
		this->coordX = x;
		this->coordY = y;
		this->goldCount = money;
		this->finished = false;
		//this->previousDirection = Last;
	}
};