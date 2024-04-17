#pragma once
#include "chess.hpp"
#include <vector>
#include "random"


namespace jneoy
{
	// TODO: 
	// Handle edge case of no valid children to expand with this node
	// Handle side switching for legal moves

	struct MonteCarloNode
	{
		const float C = sqrtf(2);
		const float SAME_THRESHOLD = .01;

		MonteCarloNode* parent;
		chess::Board boardState;
		std::vector<MonteCarloNode> children;
		chess::Movelist nonExpandedMoves;
		int numNonExpandedMoves;

		int numVisits = 1;
		float score = 0;


		MonteCarloNode(MonteCarloNode* _parent, chess::Board _boardState) : parent(_parent), boardState(_boardState) 
		{
			chess::movegen::legalmoves(nonExpandedMoves, boardState);
			numNonExpandedMoves = nonExpandedMoves.size();
			children.reserve(numNonExpandedMoves);
		}

		MonteCarloNode* GetHighestScoreChild();
		float GetUCT();

		MonteCarloNode ExpandRandomMove();
		bool FullyExpanded() { return nonExpandedMoves.size() == 0; }
	};
}