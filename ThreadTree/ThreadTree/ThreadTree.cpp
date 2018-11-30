// ThreadTree.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Defs.h"
#include <random>



using namespace std;

std::default_random_engine & global_urng()
{
	static std::default_random_engine u;
	return u;
}

int pick_a_number(int from, int thru)
{
	static std::uniform_int_distribution<> d;
	return d(global_urng(), std::uniform_int_distribution<>::param_type(from, thru));
}

void GenerateChildNode(Node* parent, int childNum)
{

	return ;
}

Node* GenerateMasterNode()
{
	auto value = pick_a_number(1,10);
	Node* head = new Node(0,value);

	auto threadNum = pick_a_number(1,5);
	GenerateChildNode(head,threadNum);

	return head;
}

int _tmain(int argc, _TCHAR* argv[])
{

	return 0;
}