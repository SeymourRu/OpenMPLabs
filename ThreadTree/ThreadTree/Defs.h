#include "SafeVector.h"

struct Node
{
	int Generation;
	int Value;
	safe_vector<Node> Nodes;

	Node(int gen, int value)
	{
		this->Generation = gen;
		this->Value = value;
		Nodes = safe_vector<Node>();
	}
};