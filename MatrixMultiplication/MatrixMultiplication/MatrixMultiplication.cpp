// MatrixMultiplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Matrix.h"
#include <ctime>

#ifndef SYSOUT_F
#define SYSOUT_F(f, ...)      _RPT1( 0, f, __VA_ARGS__ ) // For Visual studio
#endif

#ifndef speedtest__             
#define speedtest__(data)   for (long blockTime = NULL; (blockTime == NULL ? (blockTime = clock()) != NULL : false); SYSOUT_F(data "%.9fs", (double) (clock() - blockTime) / CLOCKS_PER_SEC))
#endif

std::vector<std::vector<int> > LoadMatrix(const std::string &filename)
{
	std::ifstream file;
	file.open(filename, std::ios::in | std::ios::out);

	if(!file.is_open())
	{
		return std::vector<std::vector<int>>();
	}

	std::vector<std::vector<int>> data;
	std::string line;

	while(!std::getline(file, line, '\n').eof())
	{
		std::vector<int> lineData;
		int value;
		std::string textedValue;
		for (int i = 0;i<line.length();i++)
		{
			if (line[i]!=',')
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
		data.push_back(lineData);
	}

	return data;
}

int _tmain(int argc, _TCHAR* argv[])
{
	auto matrix = LoadMatrix(std::string("matrix.txt"));

	auto det = 0;

	int start_s=clock();
	det = CalcDeterminantNormal(matrix);
	int stop_s=clock();
	std::cout << "Normal calculation speed: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;
	std::cout<<"Det normal = " << det << std::endl;

	start_s=clock();
	det = CalcDeterminantOpenMP(matrix);
	stop_s=clock();
	std::cout << "OpenMP calculation speed: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;
	std::cout<<"Det openmp = " << det << std::endl;

	system("pause");

	return 0;
}