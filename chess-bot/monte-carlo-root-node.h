#include "monte-carlo-node.h"
#include <atomic>


namespace jneoy
{
	struct MonteCarloRootNode : public MonteCarloNode
	{
		std::atomic<int> atomicNumVisits;

		MonteCarloRootNode(chess::Board _boardState, chess::Move _move = chess::Move()) : 
			MonteCarloNode(nullptr, _boardState, _move) {}


		void BackPropagateScore(float scoreToPropagate) override { atomicNumVisits++; }
		int GetNumVisits() override { return atomicNumVisits; }
	};
}