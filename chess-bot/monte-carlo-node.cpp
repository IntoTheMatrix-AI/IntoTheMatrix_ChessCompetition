#include "monte-carlo-node.h"
using namespace jneoy;

#define UPDATE_HIGHEST_SCORE_CHILD highestChild = children[i]; highestUCT = possibleUCT;
MonteCarloNode* MonteCarloNode::GetHighestScoreChild()
{
	if (children.size() <= 0) return nullptr;

	MonteCarloNode* highestChild = children[0];
	float highestUCT = highestChild->GetUCT();

	for (int i = 1; i < children.size(); ++i)
	{
		float possibleUCT = children[i]->GetUCT();
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
