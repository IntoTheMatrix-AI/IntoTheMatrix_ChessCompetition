#pragma once
#include "chess.hpp"
#include <vector>
#include "random"


namespace jneoy
{
	struct MonteCarloNode
	{
		const float C = sqrtf(2);
		const float SAME_THRESHOLD = .01;

		MonteCarloNode* parent;
		chess::Board boardState;
		std::vector<MonteCarloNode*> children;

		int numVisits = 1;
		float score = 0;


		MonteCarloNode(MonteCarloNode* _parent, chess::Board _boardState) : parent(_parent), boardState(_boardState) 
		{
		}

		MonteCarloNode* GetHighestScoreChild();
		float GetUCT();
	};
}