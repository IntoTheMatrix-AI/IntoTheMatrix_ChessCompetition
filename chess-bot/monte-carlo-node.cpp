#include "monte-carlo-node.h"
using namespace jneoy;

#define UPDATE_HIGHEST_SCORE_CHILD highestChild = children.data() + i; highestUCT = possibleUCT;
MonteCarloNode* MonteCarloNode::GetHighestScoreChild()
{
	if (children.size() <= 0) return nullptr;

	MonteCarloNode* highestChild = children.data(); // Gets first element
	float highestUCT = highestChild->GetUCT();

	for (int i = 1; i < children.size(); ++i)
	{
		float possibleUCT = (children.data() + i)->GetUCT();
		if (highestUCT < possibleUCT)
		{
			UPDATE_HIGHEST_SCORE_CHILD
		}
		else if (std::abs(highestUCT - possibleUCT) < SAME_THRESHOLD) // Handle case where highest children are roughly equal
		{
			if (rand() % 2 == 0)
			{
				UPDATE_HIGHEST_SCORE_CHILD
			}
		}
	}

	return highestChild;
}


float jneoy::MonteCarloNode::GetUCT()
{
	float firstTerm = score / numVisits;
	float secondTerm = log(parent->numVisits) / numVisits;
	secondTerm = sqrtf(secondTerm);
	secondTerm *= C;

	return firstTerm + secondTerm;
}


MonteCarloNode jneoy::MonteCarloNode::ExpandRandomMove()
{
	if (numNonExpandedMoves == 0) throw std::out_of_range("No non-expanded moves!");

	int index = rand() % numNonExpandedMoves;
	
	// Make new board with move applied
	chess::Board newNodeBoard = boardState;
	newNodeBoard.makeMove(nonExpandedMoves[index]);

	// Send just used move to back of array so we don't pick it again
	chess::Move temp = nonExpandedMoves[numNonExpandedMoves - 1];
	nonExpandedMoves[numNonExpandedMoves - 1] = nonExpandedMoves[index];
	nonExpandedMoves[index] = temp;

	children.push_back(MonteCarloNode(this, newNodeBoard));
}
