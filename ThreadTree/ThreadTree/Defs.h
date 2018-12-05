#include "SafeVector.h"
#include "SafeVectorCut.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <memory>

struct Node
{
	int Generation;
	int Value;
	int ThreadId;
	int Iteration;
	std::chrono::high_resolution_clock::time_point createTime;

	SafeVector<std::shared_ptr<Node>> Nodes;

	Node(int gen, int value, int thread, int iteration)
	{
		this->Generation = gen;
		this->Value = value;
		this->ThreadId = thread;
		this->Iteration = iteration;
		this->createTime = std::chrono::high_resolution_clock::now();
		Nodes = SafeVector<std::shared_ptr<Node>>();
	}

	~Node()
	{
		//std::cout << "Node ~()" << std::endl;
	}

	void DrawTree(std::ofstream& output, int depth, bool markUnused, std::chrono::high_resolution_clock::time_point startTime, int& sum)
	{
		if (Nodes.size() > 0)
		{
			for (int i = 0; i < floor(Nodes.size() / 2.0); i++)
			{
				Nodes[i]->DrawTree(output, depth + 5, markUnused, startTime, sum);
			}
		}
		else
		{
			if (markUnused)
			{
				DrawRepeats(output, depth + 5, '\t');
				output << "X\n";
			}
		}

		DrawRepeats(output, depth, '\t');
		output << this->Value << "[" << this->Generation << "/" << this->Nodes.size() << "/" << this->ThreadId << "/" << this->Iteration << "]{" << std::chrono::duration_cast<std::chrono::milliseconds>(this->createTime - startTime).count() << " ms}\n";
		sum += this->Value;

		if (Nodes.size() > 0)
		{
			for (int i = floor(Nodes.size() / 2.0); i < Nodes.size(); i++)
			{
				Nodes[i]->DrawTree(output, depth + 5, markUnused, startTime, sum);
			}
		}
		else
		{
			if (markUnused)
			{
				DrawRepeats(output, depth + 5, '\t');
				output << "X\n";
			}
		}
	}

private:
	void DrawRepeats(std::ofstream& output, int num, char sym)
	{
		for (int i = 0; i < num; i++)
		{
			output << sym;
		}
	}
};