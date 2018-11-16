// MatrixMultiplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Matrix.h"
#include <ctime>
#include <chrono>

void TimerOne(std::vector<std::vector<int>> matrix)
{
	auto det = 0;
	int start_s = clock();
	det = CalcDeterminantNormal(matrix);
	int stop_s = clock();

	std::cout << "Normal calculation speed: " << (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << std::endl;
	std::cout << "Det normal = " << det << std::endl;

	start_s = clock();
	det = CalcDeterminantOpenMP(matrix);
	stop_s = clock();
	std::cout << "OpenMP calculation speed: " << (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << std::endl;
	std::cout << "Det OpenMP = " << det << std::endl;
}

void TimerTwo(std::vector<std::vector<int>> matrix)
{
	auto det = 0;
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	det = CalcDeterminantNormal(matrix);
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

	std::cout << "Normal calculation speed (ms): " << duration << std::endl;
	std::cout << "Det normal = " << det << std::endl;

	t1 = std::chrono::high_resolution_clock::now();
	det = CalcDeterminantOpenMP(matrix);
	t2 = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

	std::cout << "OpenMP calculation speed (ms): " << duration << std::endl;
	std::cout << "Det OpenMP = " << det << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	auto matrix = LoadMatrix(std::string("matrix.txt"));

	for (int i = 0; i < 5; i++)
	{
		TimerOne(matrix);
		TimerTwo(matrix);
		std::cout << "========" << std::endl;
	}

	system("pause");

	return 0;
}