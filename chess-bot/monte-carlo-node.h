#pragma once
#include "chess.hpp"
#include <vector>
#include "random"
#include <atomic>
#include <mutex>


namespace jneoy
{
	// TODO: 
	// Handle edge case of no valid children to expand with this node
	// Optimization of keeping branch we already started exploring

	struct MonteCarloNode
	{
		const float C = sqrtf(2);
		const float SAME_THRESHOLD = .01;

		std::atomic<bool> locked = false;
		std::atomic<int> numChildrenLocked = 0;

		std::mutex mutexLock;

		MonteCarloNode* parent;
		chess::Board boardState;
		std::vector<MonteCarloNode*> children;
		chess::Movelist nonExpandedMoves;
		int numNonExpandedMoves;
		chess::Move move;

		int numVisits = 1;
		float score = 0;

		std::string fen;


		MonteCarloNode(MonteCarloNode* _parent, chess::Board _boardState, chess::Move _move = chess::Move()) : parent(_parent), boardState(_boardState), move(_move)
		{
			if(move != chess::Move())
				boardState.makeMove(move);

			chess::movegen::legalmoves(nonExpandedMoves, boardState);
			numNonExpandedMoves = nonExpandedMoves.size();
			children.reserve(numNonExpandedMoves);

			fen = boardState.getFen();
		}

		~MonteCarloNode()
		{
			for (int i = 0; i < children.size(); ++i)
			{
				delete children[i];
			}

			children.clear();
		}

		MonteCarloNode(const MonteCarloNode& other) = delete;




		MonteCarloNode* GetHighestUCTChild();
		float GetUCT();

		MonteCarloNode* GetHighestScoreChild();

		MonteCarloNode* ExpandRandomMove();
		bool FullyExpanded() { return numNonExpandedMoves == 0; }

		void BackPropagateScore(float scoreToPropagate);

		void BackPropagateLock();
		void BackPropagateUnlock();
	};
}