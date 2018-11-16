#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <omp.h>
#include "windows.h"

static int CalcDeterminantNormal(std::vector<std::vector<int>> Matrix)
{
	//this function is written in c++ to calculate the determinant of matrix
	// it's a recursive function that can handle matrix of any dimension
	int det = 0; // the determinant value will be stored here
	if (Matrix.size() == 1)
	{
		return Matrix[0][0]; // no calculation needed
	}
	else if (Matrix.size() == 2)
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
		for (int p = 0; p < Matrix[0].size(); p++)
		{
			//this loop iterate on each elements of the first row in the matrix.
			//at each element we cancel the row and column it exist in
			//and form a matrix from the rest of the elements in the matrix
			std::vector<std::vector<int>> TempMatrix; // to hold the shaped matrix;
			for (int i = 1; i < Matrix.size(); i++)
			{
				// iteration will start from row one cancelling the first row values
				std::vector<int> TempRow;
				for (int j = 0; j < Matrix[i].size(); j++)
				{
					// iteration will pass all cells of the i row excluding the j 
					//value that match p column
					if (j != p)
					{
						TempRow.push_back(Matrix[i][j]);//add current cell to TempRow 
					}
				}
				if (TempRow.size() > 0)
					TempMatrix.push_back(TempRow);
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

static int CalcDeterminantOpenMP(std::vector<std::vector<int>> Matrix)
{
	//this function is written in c++ to calculate the determinant of matrix
	// it's a recursive function that can handle matrix of any dimension
	int det = 0; // the determinant value will be stored here
	if (Matrix.size() == 1)
	{
		return Matrix[0][0]; // no calculation needed
	}
	else if (Matrix.size() == 2)
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

		auto chunk = Matrix.size();

		#pragma omp parallel shared(chunk)
		{
			#pragma omp for schedule(dynamic,chunk) nowait
			for (int p = 0; p < Matrix[0].size(); p++)
			{
				//this loop iterate on each elements of the first row in the matrix.
				//at each element we cancel the row and column it exist in
				//and form a matrix from the rest of the elements in the matrix
				std::vector<std::vector<int>> TempMatrix; // to hold the shaped matrix;
				for (int i = 1; i < Matrix.size(); i++)
				{
					// iteration will start from row one cancelling the first row values
					std::vector<int> TempRow;
					for (int j = 0; j < Matrix[i].size(); j++)
					{
						// iteration will pass all cells of the i row excluding the j 
						//value that match p column
						if (j != p)
						{
							TempRow.push_back(Matrix[i][j]);//add current cell to TempRow 
						}
					}
					if (TempRow.size() > 0)
						TempMatrix.push_back(TempRow);
					//after adding each row of the new matrix to the vector tempx
					//we add it to the vector temp which is the vector where the new 
					//matrix will be formed
				}
				det = det + Matrix[0][p] * pow(-1, p) * CalcDeterminantOpenMP(TempMatrix);
				//then we calculate the value of determinant by using a recursive way
				//where we re-call the function by passing to it the new formed matrix
				//we keep doing this until we get our determinant
			}
		}
		return det;
	}
}