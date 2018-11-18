#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <omp.h>
#include "windows.h"

static std::vector<std::vector<double>> LoadMatrix()
{
	std::vector<std::vector<double>> data;

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
			return std::vector<std::vector<double>>();
		}

		std::string line;
		bool firstLine = false;
		int len = -1;

		std::getline(file, line, '\n');

		do
		{
			std::vector<double> lineData;
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
					return std::vector<std::vector<double>>();
				}
			}

			data.push_back(lineData);

		} while (!std::getline(file, line, '\n').eof());

		if (data.size() != len)
		{
			std::cout << "Incorrect matrix, should be squared" << std::endl;
			return std::vector<std::vector<double>>();
		}
	}

	return data;
}

static double CalcDeterminantNormal(std::vector<std::vector<double>> Matrix)
{
	//this function is written in c++ to calculate the determinant of matrix
	// it's a recursive function that can handle matrix of any dimension
	double det = 0; // the determinant value will be stored here
	int n = Matrix.size();
	if (n == 1)
	{
		return Matrix[0][0]; // no calculation needed
	}
	else if (n == 2)
	{
		//in this case we calculate the determinant of a 2-dimensional matrix in a 
		//default procedure
		det = (Matrix[0][0] * Matrix[1][1] - Matrix[0][1] * Matrix[1][0]);
		return det;
	}
	else
	{
		//in this case we calculate the determinant of a squared matrix that have 
		// for example 3x3 order greater than 2
		for (int p = 0; p < n; p++)
		{
			//this loop iterate on each elements of the first row in the matrix.
			//at each element we cancel the row and column it exist in
			//and form a matrix from the rest of the elements in the matrix
			std::vector<std::vector<double>> TempMatrix; // to hold the shaped matrix;
			for (int i = 1; i < n; i++)
			{
				// iteration will start from row one cancelling the first row values
				std::vector<double> TempRow;
				for (int j = 0; j < n; j++)
				{
					// iteration will pass all cells of the i row excluding the j 
					//value that match p column
					if (j != p)
					{
						TempRow.push_back(Matrix[i][j]);//add current cell to TempRow 
					}
				}
				if (TempRow.size() > 0)
				{
					TempMatrix.push_back(TempRow);
				}
				//after adding each row of the new matrix to the vector tempx
				//we add it to the vector temp which is the vector where the new 
				//matrix will be formed
			}
			det = det + Matrix[0][p] * pow(-1, p) * CalcDeterminantNormal(TempMatrix);
			//then we calculate the value of determinant by using a recursive way
			//where we re-call the function by passing to it the new formed matrix
			//we keep doing this until we get our determinant
		}
		return det;
	}
}

static double CalcDeterminantOpenMP(std::vector<std::vector<double>> Matrix)
{
	//this function is written in c++ to calculate the determinant of matrix
	// it's a recursive function that can handle matrix of any dimension
	double det = 0; // the determinant value will be stored here
	int n = Matrix.size();
	if (n == 1)
	{
		return Matrix[0][0]; // no calculation needed
	}
	else if (n == 2)
	{
		//in this case we calculate the determinant of a 2-dimensional matrix in a 
		//default procedure
		det = (Matrix[0][0] * Matrix[1][1] - Matrix[0][1] * Matrix[1][0]);
		return det;
	}
	else
	{
		//in this case we calculate the determinant of a squared matrix that have 
		// for example 3x3 order greater than 2

		//auto chunk = n;
		int p = 0, i = 1, j = 0;

#pragma omp parallel shared(Matrix,det) firstprivate(p) private(i,j)
		{
			//printf("#%d thread \n", omp_get_thread_num());
#pragma omp for schedule(guided,1) nowait
			for (p = 0; p < n; p++)
			{
				//this loop iterate on each elements of the first row in the matrix.
				//at each element we cancel the row and column it exist in
				//and form a matrix from the rest of the elements in the matrix
				std::vector<std::vector<double>> TempMatrix; // to hold the shaped matrix;
				for (i = 1; i < n; i++)
				{
					// iteration will start from row one cancelling the first row values
					std::vector<double> TempRow;
					for (j = 0; j < n; j++)
					{
						// iteration will pass all cells of the i row excluding the j 
						//value that match p column
						if (j != p)
						{
							TempRow.push_back(Matrix[i][j]);//add current cell to TempRow 
						}
					}
					if (TempRow.size() > 0)
					{
						TempMatrix.push_back(TempRow);
					}
					//after adding each row of the new matrix to the vector tempx
					//we add it to the vector temp which is the vector where the new 
					//matrix will be formed
				}
				det += Matrix[0][p] * pow(-1, p) * CalcDeterminantOpenMP(TempMatrix);
				//then we calculate the value of determinant by using a recursive way
				//where we re-call the function by passing to it the new formed matrix
				//we keep doing this until we get our determinant
			}
		}
		return det;
	}
}



