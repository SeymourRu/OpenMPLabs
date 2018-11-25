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
#include <queue>
#include <sstream>
#include <random>
#include "windows.h"
#include "Defs.h"

using namespace std;

std::default_random_engine & global_urng()
{
	static std::default_random_engine u{};
	return u;
}

int pick_a_number(int from, int thru)
{
	static std::uniform_int_distribution<> d{};
	using parm_t = decltype(d)::param_type;
	return d(global_urng(), parm_t{ from, thru });
}

static CellState CheckCell(int value)
{
	return (CellState)value;
}

static char ReplaceCellView(int value)
{
	switch ((CellState)value)
	{
	case Exit: return 'E';
	case Player: return '@';
	case Gold: return '$';
	case Free: return '.';
	}
	return '!';
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
	ofn.lpstrFilter = L"Field file (.txt)\0*.txt\0All\0*.*\0";
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

	auto maxValue = field.size();
	vector<SimplePlayer> players;

	std::cout << "========================" << std::endl;
	std::cout << "Setting players:" << std::endl;
	std::cout << "========================" << std::endl;

	for (int i = 0; i < iterationNum; i++)
	{
		int randX = rand() % maxValue;
		int randY = rand() % maxValue;

		auto cellState = CheckCell(field[randX][randY]);

		if (cellState != Exit)
		{
			if (cellState == Free)
			{
				players.push_back(SimplePlayer(i, randX, randY));
			}
			else if (cellState == Gold)
			{
				players.push_back(SimplePlayer(i, randX, randY, 1));
			}
			else
			{
				cout << "Cell will be ignored" << endl;
				i--;
				continue;
			}

			field[randX][randY] = Player;
			std::cout << "Player " << i << " - x:" << randX << ",y:" << randY << std::endl;
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
	cout << endl;
	for (int i = 0; i < maxValue; i++)
	{
		cout << "|";
		for (int j = 0; j < maxValue; j++)
		{
			cout << ReplaceCellView(field[i][j]) << "|";
		}
		cout << endl;
	}
}

static void RenderNewLocation(Direction nextMove, int& x, int& y)
{
	switch (nextMove)
	{
	case Direction::Left:
	{
		x -= 1;
	}
	break;
	case Direction::Right:
	{
		x += 1;
	}
	break;
	case Direction::Up:
	{
		y -= 1;
	}
	break;
	case Direction::Down:
	{
		x += 1;
	}
	break;
	}
}

static bool ValidCoords(int x, int y, int dimension)
{
	if ((x >= 0) && (x < dimension) && (y >= 0) && (y < dimension))
	{
		return true;
	}
	return false;
}

static bool RenderPlayer(SimplePlayer& player, int x, int y, std::vector<std::vector<int>>& field)
{
	cout << "Moving player " << player.id << " to coords " << x << "," << y << endl;

	field[player.coordX][player.coordY] = Free;

	player.coordX = x;
	player.coordY = y;

	auto curState = CheckCell(field[x][y]);
	switch (curState)
	{
	case OutOfField:
	{
		cout << "Ahtung!!" << endl;
	}
	break;
	case Exit:
	{
		player.finishTime = std::chrono::high_resolution_clock::now();
		player.finished = true;
		cout << "Player " << player.id << " finished with " << endl;
	}
	break;
	case Free:
	{
		//ok
		field[x][y] = Player;
	}
	break;
	case Gold:
	{
		player.goldCount += 1;
		field[x][y] = Player;
		cout << "Player " << player.id << " collected a coin " << endl;
	}
	break;
	case Player:
	{
		//skip now
		field[x][y] = Player;
		cout << "Player " << player.id << " crashed into collision " << endl;
	}
	break;
	}

	return player.finished;
}

void PlayGame()
{
	auto field = LoadPlayField();

	if (field.size() != 0)
	{
		auto players = SetPlayers(field);
		ShowField(field);

		cout << endl <<"Legend:" << endl;
		cout << "Exit - E" << endl;
		cout << "Player - @" << endl;
		cout << "Gold - $" << endl;
		cout << "Free cell - ." << endl;

		cout << endl << "When ready to play - hit any button" << endl;
		system("pause");

		auto fieldSize = field.size();
		vector<string> results;

		auto gameStart = std::chrono::high_resolution_clock::now();

		while (players.size() > 0)
		{
			int playersCount = players.size();
			int i = 0, j = 0;

			system("CLS");
			ShowField(field);
			cout << endl;
#pragma omp parallel sections shared(field,players,fieldSize) firstprivate(playersCount,i,j)
			{

#pragma omp section 
				{
					for (int i = 0; i < playersCount / 2; i++)
					{
						auto x = 0, y = 0;
						do
						{
							x = players[i].coordX, y = players[i].coordY;
							
							//auto nextMove = (Direction)(rand() % Direction::Last);
							auto nextMove = (Direction)pick_a_number(0,3);

							RenderNewLocation(nextMove, x, y);
						} while (!ValidCoords(x, y, fieldSize));

#pragma omp critical 
						{
							RenderPlayer(players[i], x, y, field);
						}
					}
				}

#pragma omp section 
				{
					for (int j = playersCount / 2; j < playersCount; j++)
					{
						auto x = 0, y = 0;
						do
						{
							x = players[j].coordX, y = players[j].coordY;
							//auto nextMove = (Direction)(rand() % Direction::Last);
							auto nextMove = (Direction)pick_a_number(0, 3);
							RenderNewLocation(nextMove, x, y);
						} while (!ValidCoords(x, y, fieldSize));

#pragma omp critical 
						{
							RenderPlayer(players[j], x, y, field);
						}
					}
				}
			}

			for (int i = 0; i < players.size(); i++)
			{
				if (players[i].finished)
				{
					auto duration = std::chrono::duration_cast<std::chrono::seconds>(players[i].finishTime - gameStart).count();
					std::ostringstream stream;
					stream << "Player with ID = " << players[i].id << " finished with " << players[i].goldCount << " coins with play time = " << duration << " seconds" << endl;
					results.push_back(stream.str());
					players.erase(players.begin() + i);
					i = 0;
				}
			}

			ShowField(field);
			Sleep(1000);
		}

		system("CLS");

		std::cout << "Game finished!" << std::endl;
		for (int i = 0; i < results.size(); i++)
		{
			cout << results[i];
		}

		std::cout << endl << "========================" << std::endl;
		ShowField(field);
		std::cout << endl << "========================" << std::endl;
	}
	else
	{
		std::cout << "Empty field, can not play" << std::endl;
	}

	system("pause");
}

static void SavePlayField(std::vector<std::vector<int>> field)
{
	OPENFILENAME ofn;
	TCHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetConsoleWindow();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"Field file (.txt)\0*.txt\0All\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST;

	if (GetSaveFileName(&ofn) == TRUE)
	{
		ofstream outfile; // Opens file in write mode
		outfile.open(ofn.lpstrFile); // Opens the quotations file

		auto maxValue = field.size();
		for (int i = 0; i < maxValue; i++)
		{
			for (int j = 0; j < maxValue; j++)
			{
				outfile << field[i][j];
				if (j < maxValue - 1)
				{
					outfile << ",";
				}
			}
			outfile << endl;
		}

		outfile.close();
	}

	return;
}

void GenerateField()
{
	auto dimension = 0;
	std::vector<std::vector<int>> field;
	cout << "Enter dimension" << endl;
	cin >> dimension;

	for (int i = 0; i < dimension; i++)
	{
		std::vector<int> row;
		for (int j = 0; j < dimension; j++)
		{
			row.push_back(rand() % 2);
		}
		field.push_back(row);
	}


	auto exitX = rand() % dimension;
	auto exitY = rand() % dimension;

	field[exitX][exitY] = Exit;

	SavePlayField(field);
}

int _tmain(int argc, _TCHAR* argv[])
{
	srand(time(NULL));
	int input;
	while (true)
	{
		system("CLS");
		cout << " 0 : Exit\n"
			<< " 1 : Play a game\n"
			<< " 2 : Generate field\n"
			<< "Enter #:\n";
		cin >> input;
		if (cin.good())
		{
			switch (input)
			{
			case 0: cout << "Exiting.." << endl; system("pause"); return 0;
			case 1: PlayGame(); break;
			case 2: GenerateField(); break;
			default:cout << "Incorrect value" << endl;
			}
		}
		else
		{
			cout << "Please, repeat request..!" << endl;
		}
	}
}