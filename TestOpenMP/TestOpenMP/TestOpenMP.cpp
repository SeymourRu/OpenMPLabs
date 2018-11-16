// TestOpenMP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <omp.h>
#include <iostream>
#include "Windows.h"

int main_one(int argc, _TCHAR* argv[])
{
	int nthreads, tid;

#pragma omp parallel private(tid)
	{
		tid = omp_get_thread_num();
		std::cout<<"Hello  from thread =" << tid<<std::endl;
		if (tid==0)
		{
			nthreads = omp_get_num_threads();
			std::cout<<"Num of threads = "<<nthreads<<std::endl;
		}
	}

	system("pause");
	return 0;
}

int main_two(int argc, _TCHAR* argv[])
{
#define CHUNK 100
#define NMAX 1000
	int i,n,chunk;
	float a[NMAX],b[NMAX],c[NMAX];
	for (i = 0;i<NMAX;i++)
	{
		a[i] = b[i] = i * 1.0;
	}
	n = NMAX;
	chunk = CHUNK;
#pragma omp parallel shared(a,b,c,n,chunk) private(i)
	{
#pragma omp for schedule(dynamic,chunk) nowait
		for (i = 0;i<n;i++)
		{
			c[i] = a[i] + b[i];
		}
	}
}

int main_three(int argc, _TCHAR* argv[])
{
#define CHUNK 100
#define NMAX 1000
	int i,n,chunk;
	float a[NMAX],b[NMAX],c[NMAX];
	for (i = 0;i<NMAX;i++)
	{
		a[i] = b[i] = i * 1.0;
	}
	n = NMAX;
	chunk = CHUNK;
	#pragma omp parallel for shared(a,b,c,n) schedule(static,chunk)
	{
		for (i = 0;i<n;i++)
		{
			c[i] = a[i] + b[i];
		}
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	//main_one(argc,argv);
	//main_two(argc,argv);
	main_three(argc,argv);
	system("pause");
	return 0;
}