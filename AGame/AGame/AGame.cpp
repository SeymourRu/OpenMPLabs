// AGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <omp.h>
#include <time.h>
#include "windows.h"

using namespace std;

enum CellState
{
	Exit = -1,
	Free = 0,
	Gold = 1,
	Player = 2,
};

struct SimplePlayer
{
	int id;
	int coordX;
	int coordY;
	SimplePlayer(int id,int x, int y)
	{
this->id = id;
this->coordX = x;
this->coordY = y;
	}
};

static CellState CheckCell(int value)
{
	return (CellState)value;
}

static std::vector<std::vector<int>> LoadPlayField()
{
	std::vector<std::vector<int>> data;

	OPENFILENAME ofn;
	TCHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetConsoleWindow();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"Matrix file (.txt)\0*.txt\0All\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		std::ifstream file(ofn.lpstrFile, std::ios::in | std::ios::out);

		if (!file.is_open())
		{
			return std::vector<std::vector<int>>();
		}

		std::string line;
		bool firstLine = false;
		int len = -1;

		std::getline(file, line, '\n');

		do
		{
			std::vector<int> lineData;
			int value;
			std::string textedValue;
			for (int i = 0; i < line.length(); i++)
			{
				if (line[i] != ',')
				{
					textedValue += line[i];
				}
				else
				{
					value = std::stoi(textedValue);
					lineData.push_back(value);
					textedValue.clear();
				}
			}

			value = std::stoi(textedValue);
			lineData.push_back(value);
			textedValue.clear();

			if (!firstLine)
			{
				len = lineData.size();
				firstLine = !firstLine;
			}
			else
			{
				if (lineData.size() != len)
				{
					std::cout << "Incorrect matrix, should be balanced" << std::endl;
					return std::vector<std::vector<int>>();
				}
			}

			data.push_back(lineData);

		} while (!std::getline(file, line, '\n').eof());

		if (data.size() != len)
		{
			std::cout << "Incorrect matrix, should be squared" << std::endl;
			return std::vector<std::vector<int>>();
		}
	}

	return data;
}

static vector<SimplePlayer> SetPlayers(std::vector<std::vector<int>>& field)
{
	auto iterationNum = 0;

	std::cout << "Enter num of players:" << std::endl;
	std::cin >> iterationNum;

	srand(time(NULL));

	auto maxValue = field.size();
	vector<SimplePlayer> players;

	std::cout << "========================" << std::endl;
	std::cout << "Setting players:" << std::endl;
	std::cout << "========================" << std::endl;
	for (int i = 0; i < iterationNum;i++)
	{
		int randX = rand() % maxValue;
		int randY = rand() % maxValue;

		if (CheckCell(field[randX][randY]) == Free)
		{
			field[randX][randY] = Player;
			std::cout << "Player "<< i << " - x:"<< randX << ",y:"<< randY<< std::endl;
			players.push_back(SimplePlayer(i, randX,randY));
		}
		else
		{
			i--;
		}
	}
	return players;
}

static void ShowField(std::vector<std::vector<int>> field)
{
	auto maxValue = field.size();
	cout<<endl;
	for (int i = 0;i < maxValue; i++)
	{
		cout <<"|";
		for (int j = 0; j < maxValue; j++)
		{
			cout<<field[i][j]<<"|";
		}
		cout << endl;
	}
}

static void Play(std::vector<std::vector<int>> field)
{

}

int _tmain(int argc, _TCHAR* argv[])
{
	auto field = LoadPlayField();

	if (field.size() != 0)
	{
		auto players = SetPlayers(field);
		ShowField(field);

		#pragma omp parallel shared(field)
		{

		}

		//omp_set_nested(0);
		//std::cout << "========================" << std::endl;
		//std::cout << "========================" << std::endl;
	}
	else
	{
		std::cout << "Empty field, can not play" << std::endl;
	}

	system("pause");
	return 0;
}