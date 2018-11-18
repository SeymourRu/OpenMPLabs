#include "stdafx.h"
#include "Matrix.h"
#include <ctime>
#include <chrono>

void BenchForNormal(std::vector<std::vector<double>> matrix)
{
	auto det = 0;
	auto matrixSize = matrix.size();
	int start_s = clock();
	det = CalcDeterminantNormal(matrix);
	int stop_s = clock();

	double durationD = (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000.0;
	std::cout << "Normal calculation speed: " << durationD << std::endl;
	std::cout << "Det normal = " << det << " [" << matrixSize << "x" << matrixSize << "]" << std::endl;

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	det = CalcDeterminantNormal(matrix);
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	auto durationL = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

	std::cout << "Normal calculation speed (ms): " << durationL << std::endl;
	std::cout << "Det normal = " << det << " [" << matrixSize << "x" << matrixSize << "]" << std::endl;
}

void BenchForOpenMP(std::vector<std::vector<double>> matrix)
{
	auto det = 0;
	auto matrixSize = matrix.size();

	int start_s = clock();
	det = CalcDeterminantOpenMP(matrix);
	int stop_s = clock();

	double durationD = (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000.0;
	std::cout << "OpenMP calculation speed: " << durationD << std::endl;
	std::cout << "Det OpenMP = " << det << " [" << matrixSize << "x" << matrixSize << "]" << std::endl;

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	det = CalcDeterminantOpenMP(matrix);
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	auto durationL = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	std::cout << "OpenMP calculation speed (ms): " << durationL << std::endl;
	std::cout << "Det OpenMP = " << det << " [" << matrixSize << "x" << matrixSize << "]" << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	auto matrix = LoadMatrix();

	if (matrix.size() != 0)
	{
		//omp_set_nested(0);

		auto iterationNum = 0;

		std::cout << "Enter num of iterations:" << std::endl;
		std::cin >> iterationNum;

		for (int i = 0; i < iterationNum; i++)
		{
			std::cout << "========================" << std::endl;
			BenchForNormal(matrix);
			BenchForOpenMP(matrix);
			std::cout << "========================" << std::endl;
		}
	}
	else
	{
		std::cout << "Empty matrix, no calculation performed" << std::endl;
	}

	system("pause");

	return 0;
}