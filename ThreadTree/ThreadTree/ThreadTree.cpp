// ThreadTree.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Defs.h"
#include <memory>
#include <random>
#include <omp.h>
#include <thread>
#include "windows.h"

using namespace std;

#define MAX_VALUE_FOR_NODE 10

typedef void(*drawFunction)(const wchar_t* path, int x, int y, int width, int heigth);

bool exiting = false;
std::wstring picturePathStr;

std::default_random_engine & global_urng()
{
	static std::default_random_engine u{};
	return u;
}

int pick_a_number(int from, int thru)
{
	static std::uniform_int_distribution<> d;
	return d(global_urng(), std::uniform_int_distribution<>::param_type(from, thru));
}

void GenerateChildNodeOpenMP(shared_ptr<Node> parent, int childNum)
{
	int i = 0;
	omp_set_num_threads(childNum);
#pragma omp parallel shared(parent) firstprivate(i)
	{
#pragma omp for schedule(dynamic,1) nowait 
		for (i = 0; i < childNum; i++)
		{
			//printf("There is %d generation, %d iteration from %d iterations, %d threads\n", parent->Generation, i, childNum, omp_get_num_threads());

			auto threadId = omp_get_thread_num();// std::this_thread::get_id();
			//auto probability = parent->Generation * 0.1f;
			auto minProbability = pick_a_number(1, MAX_VALUE_FOR_NODE);
			//auto minProbability = pick_a_number(1, 10 - parent->Generation);
			if (minProbability > parent->Generation)
			{
				auto value = pick_a_number(1, MAX_VALUE_FOR_NODE);
				shared_ptr<Node> anotherNode(new Node(parent->Generation + 1, value, threadId, i));

				auto threadNum = pick_a_number(0, 5);
				GenerateChildNodeOpenMP(anotherNode, threadNum);
				parent->Nodes.push_back(anotherNode);
			}
			else
			{
				shared_ptr<Node> anotherNode(new Node(parent->Generation + 1, 0, threadId, i));
				parent->Nodes.push_back(anotherNode);
			}
		}
	}

	return;
}

void GenerateChildNodeThread(shared_ptr<Node> parent, int childNum)
{
	int i = 0;
	omp_set_num_threads(childNum);
#pragma omp parallel shared(parent) firstprivate(i)
	{
#pragma omp for schedule(dynamic,1) nowait 
		for (i = 0; i < childNum; i++)
		{
			//printf("There is %d generation, %d iteration from %d iterations, %d threads\n", parent->Generation, i, childNum, omp_get_num_threads());

			auto threadId = omp_get_thread_num();// std::this_thread::get_id();
												 //auto probability = parent->Generation * 0.1f;
			auto minProbability = pick_a_number(1, MAX_VALUE_FOR_NODE);
			//auto minProbability = pick_a_number(1, 10 - parent->Generation);
			if (minProbability > parent->Generation)
			{
				auto value = pick_a_number(1, MAX_VALUE_FOR_NODE);
				shared_ptr<Node> anotherNode(new Node(parent->Generation + 1, value, threadId, i));

				auto threadNum = pick_a_number(0, 5);
				try
				{
					std::thread nextNodeThread(GenerateChildNodeThread, anotherNode, threadNum);
					parent->Nodes.push_back(anotherNode);
					nextNodeThread.join();
				}
				catch (std::exception ex)
				{
					cout << ex.what() << endl;
				}
				catch (std::system_error ex)
				{
					cout << ex.what() << endl;
				}
				catch (...)
				{
					cout << "error(((" << endl;
				}
			}
			else
			{
				shared_ptr<Node> anotherNode(new Node(parent->Generation + 1, 0, threadId, i));
				parent->Nodes.push_back(anotherNode);
			}
		}
	}

	return;
}

shared_ptr<Node> GenerateMasterNode()
{
	auto value = pick_a_number(1, MAX_VALUE_FOR_NODE);
	shared_ptr<Node> head(new Node(0, value, -1, -1));

	auto threadNum = pick_a_number(1, 5);
	GenerateChildNodeOpenMP(head, threadNum);
	return head;
}

std::wstring getexepath()
{
	wchar_t result[MAX_PATH];
	return std::wstring(result, GetModuleFileName(NULL, result, MAX_PATH));
}

void DrawAscii()
{
	//Thanks to
	//http://www.ascii-art.de/ascii/ab/anime.txt

	cout << "MMMMSSSSSSSSSSSSSSSSSMSS;.     .dMMMMSSSSSSMMSSSSSSSSS" << endl;
	cout << "MMSSSSSSSMSSSSSMSSSSMMMSS.\"-.-\":MMMMMSSSSMMMMSSMSSSMMS" << endl;
	cout << "MSSSSSSSMSSSSMMMSSMMMPTMM;\"-/\\\":MMM ^ \"     MMMSSMMMSSMM" << endl;
	cout << "SSSSSSSMMSSMMMMMMMMMP-.MMM :  ;.;P       dMMMMMMMMMP' " << endl;
	cout << "SSMSSSMMMSMMMMMMMMMP   :M;`:  ;.'+\"\"\"t+dMMMMMMMMMMP   " << endl;
	cout << "MMMSSMMMMMMMMPTMMMM\"\"\"\":P `.\// '    \"\"^^MMMMMMMP'    " << endl;
	cout << "MMMMMMPTMMMMP=\"TMMMsg,      \/   db`c\"  dMMMMMP\"      " << endl;
	cout << "MMMMMM  TMMM   d$$$b ^          /T$; ;-/TMMMP         " << endl;
	cout << "MMMMM; .^`M; d$P^T$$b          :  $$ ::  \"T(          " << endl;
	cout << "MMMMMM   .-+d$$   $$$;         ; d$$ ;;  __           " << endl;
	cout << "MMMMMMb   _d$$$   $$$$         :$$$; :MmMMMMp.        " << endl;
	cout << "MMMMMM\"  \" T$$$._.$$$;          T$P.'MMMSSSSSSb.      " << endl;
	cout << "MMM`TMb   -\")T$$$$$$P'       `._ \"\"  :MMSSSMMP'       " << endl;
	cout << "MMM / \    '  \"T$$P\"           /     :MMMMMMM         " << endl;
	cout << "MMSb`. ;                      \"      :MMMMMMM         " << endl;
	cout << "MMSSb_lSSSb.      \ `.   .___.       MMMMMMMM         " << endl;
	cout << "MMMMSSSSSSSSb.                     .MMMMMMMMM         " << endl;
	cout << "MMMMMMMMMMMSSSb                  .dMMMMMMMMM'         " << endl;
	cout << "MMMMMMMMMMMMMSS;               .dMMMMMMMMMMP          " << endl;
	cout << "MMMMMMMMMMMMMb`;\"-.          .dMMMMMMMMMMP'           " << endl;
	cout << "MMMMMMMMMMMMMMb    \"\"--.___.dMMMMMMMMMP^\"             " << endl << endl;
}

void Painting(std::wstring picPath)
{
	auto hinstLib = LoadLibrary(L"ConsoleDrawingLibWrapper.dll");
	if (hinstLib == NULL)
	{
		cout << "ERROR: unable to load DLL\n" << endl;
		return;
	}

	auto pictureFunction = (drawFunction)GetProcAddress(hinstLib, "DrawPicture");

	if (pictureFunction == NULL)
	{
		cout << "ERROR: unable to find draw function in DLL (What a shame, bro!)\n" << endl;
		FreeLibrary(hinstLib);
		return;
	}

	while (!exiting)
	{
		pictureFunction(picPath.c_str(), 70, 5, 35, 20);
	}

	FreeLibraryAndExitThread(hinstLib, 0);
}

void LoadResource()
{
	wchar_t picturePathBuffer[MAX_PATH];
	GetCurrentDirectory(sizeof(picturePathBuffer), picturePathBuffer);
	picturePathStr = std::wstring(picturePathBuffer).append(L"\\result.jpg");
}

int _tmain(int argc, _TCHAR* argv[])
{
	LoadResource();

	OPENFILENAME ofn;
	TCHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetConsoleWindow();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"Run result file (.txt)\0*.txt\0All\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST;

	if (GetSaveFileName(&ofn) == TRUE)
	{
		auto generationStart = std::chrono::high_resolution_clock::now();
		auto tree = GenerateMasterNode();

		ofstream outfile;
		outfile.open(ofn.lpstrFile);

		outfile << "======================================================================================================================" << endl;
		outfile << "Format: {Value Of Node}[Generation / Num of childs / Id of thread/ Iteration number]{Generated after x ms from start}" << endl;
		outfile << "======================================================================================================================" << endl << endl;

		tree->DrawTree(outfile, 0, true, generationStart);

		outfile.close();

		DrawAscii();
		std::thread paintThread(Painting, picturePathStr);

		cout << "Cool! Result saved!!~" << endl;
		system("pause");
		exiting = true;
		paintThread.join();
	}
	else
	{
		cout << "Could not save run result, sorry~" << endl;
		system("pause");
	}

	return 0;
}