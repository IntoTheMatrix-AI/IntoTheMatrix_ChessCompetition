#include "monte-carlo-node.h"


namespace jneoy
{
	struct MonteCarloRootNode : public MonteCarloNode
	{
		MonteCarloRootNode(chess::Board _boardState, chess::Move _move = chess::Move()) : 
			MonteCarloNode(nullptr, _boardState, _move) {}


		void BackPropagateScore(float scoreToPropagate) override {}
	};
}