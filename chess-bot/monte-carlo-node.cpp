#include "monte-carlo-node.h"
#include <thread>

using namespace jneoy;


MonteCarloNode* MonteCarloNode::GetHighestUCTChild()
{
	if (children.size() <= 0) return nullptr;

	std::vector<MonteCarloNode*> highestChildren;
	highestChildren.reserve(children.size());

	float highestUCT = -INT_MAX;

	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i]->locked) continue;

		float possibleUCT = children[i]->GetUCT();

		if (highestUCT < possibleUCT)
		{
			highestChildren.clear();
			highestChildren.push_back(children[i]);
			highestUCT = possibleUCT;
		}
		else if (std::abs(highestUCT - possibleUCT) < SAME_THRESHOLD) // Handle case where highest children are roughly equal
		{
			highestChildren.push_back(children[i]);
		}
	}

	if (highestChildren.size() <= 0) return nullptr;

	return highestChildren[rand() % highestChildren.size()];
}


float jneoy::MonteCarloNode::GetUCT()
{
	float firstTerm = score / numVisits;
	float secondTerm = log(parent->numVisits) / numVisits;
	secondTerm = sqrtf(secondTerm);
	secondTerm *= C;

	return firstTerm + secondTerm;
}


MonteCarloNode* jneoy::MonteCarloNode::GetHighestScoreChild()
{
	if (children.size() <= 0) return nullptr;

	std::vector<MonteCarloNode*> highestChildren;
	highestChildren.reserve(children.size());

	highestChildren.push_back(*children.data()); // Gets first element
	float highestScore = highestChildren[0]->score;

	for (int i = 1; i < children.size(); ++i)
	{
		float possibleUCT = children[i]->score;
		if (highestScore < possibleUCT)
		{
			highestChildren.clear();
			highestChildren.push_back(children[i]);
			highestScore = possibleUCT;
		}
		else if (std::abs(highestScore - possibleUCT) < SAME_THRESHOLD) // Handle case where highest children are roughly equal
		{
			highestChildren.push_back(children[i]);
		}
	}

	return highestChildren[rand() % highestChildren.size()];
}


MonteCarloNode* jneoy::MonteCarloNode::ExpandRandomMove()
{

	mutexLock.lock();
	if (numNonExpandedMoves == 0) { mutexLock.unlock(); return this; } /*throw std::out_of_range("No non-expanded moves!")*/

	int index = rand() % numNonExpandedMoves;
	
	// Make new board with move applied
	/*chess::Board newNodeBoard = boardState;
	newNodeBoard.makeMove(nonExpandedMoves[index]);*/

	// Send just used move to back of array so we don't pick it again
	chess::Move temp = nonExpandedMoves[numNonExpandedMoves - 1];
	nonExpandedMoves[numNonExpandedMoves - 1] = nonExpandedMoves[index];
	nonExpandedMoves[index] = temp;
	numNonExpandedMoves--;

	children.push_back(new MonteCarloNode(this, boardState, nonExpandedMoves[index]));

	mutexLock.unlock();
	return children.back();
}


void jneoy::MonteCarloNode::BackPropagateScore(float scoreToPropagate)
{
	score += scoreToPropagate;
	numVisits++;

	if(parent != nullptr) parent->BackPropagateScore(scoreToPropagate);
}


void jneoy::MonteCarloNode::BackPropagateLock()
{
	mutexLock.lock();
	locked = true;

	if (parent != nullptr)
	{
		parent->numChildrenLocked++;
		
		if (parent->children.size() + parent->nonExpandedMoves.size() <= parent->numChildrenLocked)
		{
			parent->BackPropagateLock();
		}
	}

	mutexLock.unlock();
}


void jneoy::MonteCarloNode::BackPropagateUnlock()
{
	mutexLock.lock();
	locked = false;

	if (parent != nullptr)
	{
		parent->numChildrenLocked--;

		if (parent->locked) parent->BackPropagateUnlock();
	}

	mutexLock.unlock();
}
