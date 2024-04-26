#include "chess-simulator.h"
#include <random>
#include "monte-carlo-node.h"
#include <chrono> // Needed for when we start limiting monte carlo by time
#include <thread>
#include <mutex>
#include <atomic>
#include "monte-carlo-root-node.h"


int turnCount = 1;

using namespace ChessSimulator;
using namespace jneoy;

const float TIME_TO_MOVE_IN_MILLISECONDS = 500;


//void DoMonteCarlo(MonteCarloNode& root, chess::Board& board);


void DoMonteCarlo(MonteCarloNode* root, chess::Board* board, int beginIndex, int count)
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	int numIterations = 0;
	chess::Color side = board->sideToMove();

	while (true)
	{
		// SELECTION START
		MonteCarloNode* target = root;
		MonteCarloNode* targetHighestUCTChild = target->GetHighestUCTChild(beginIndex, count);

		while (targetHighestUCTChild != nullptr && target->FullyExpanded()) // Get highest UCT leaf node
		{
			target = targetHighestUCTChild;
			targetHighestUCTChild = target->GetHighestUCTChild();
		}
		// SELECTION END

		// EXPANSION START
		MonteCarloNode* expanded = target->ExpandRandomMove();
		//if (expanded == target) // Found what is probably the best choice?
		//{
		//	//break;
		//	//expanded->BackPropagateScore(-20);
		//}
		// EXPANSION END

		// SIMULATION START
		float result = SimulateRandomGame(expanded->boardState, expanded->boardState, side);
		// SIMULATION END

		// BACKPROPAGATION START
		expanded->BackPropagateScore(result);
		// BACKPROPAGATION END

		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		auto timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

		numIterations++;

		if (timePassed > TIME_TO_MOVE_IN_MILLISECONDS) break;
	}
}


std::string ChessSimulator::Move(std::string fen) {
	// create your board based on the board string following the FEN notation
	// search for the best move using minimax / monte carlo tree search /
	// alpha-beta pruning / ... try to use nice heuristics to speed up the search
	// and have better results return the best move in UCI notation you will gain
	// extra points if you create your own board/move representation instead of
	// using the one provided by the library

	//// here goes a random movement
	//chess::Board board(fen);
	

	srand(time(NULL));
	chess::Board board(fen);
	MonteCarloRootNode root = MonteCarloRootNode(board);

	root.FullyExpandChildren();
	
	const int NUM_THREADS = 4;
	std::thread threads[NUM_THREADS];
	int numBranches = root.children.size() / NUM_THREADS;

	for (int i = 0; i < NUM_THREADS - 1; i++)
	{
		threads[i] = std::thread(DoMonteCarlo, &root, &board, numBranches * i, numBranches);
	}

	int startIndex = numBranches * (NUM_THREADS - 1);
	threads[NUM_THREADS - 1] = std::thread(DoMonteCarlo, &root, &board, startIndex, 
		root.children.size() - startIndex);

	for (int i = 0; i < NUM_THREADS; i++)
	{
		threads[i].join();
	}

	/*std::thread thread0(DoMonteCarlo, &root, &board);
	std::thread thread1(DoMonteCarlo, &root, &board);
	std::thread thread2(DoMonteCarlo, &root, &board);
	std::thread thread3(DoMonteCarlo, &root, &board);*/
	
	//thread0.
	//thread0.join();

	/*thread0.join();
	thread1.join();
	thread2.join();
	thread3.join();*/

	//std::cout << "NUM CHILDREN ANY LEVEL: " << root.numChildrenAnyLevel << std::endl;


	//DoMonteCarlo(root, board);


	//std::cout << "NUM ITERATIONS: " << numIterations << std::endl;

	turnCount += 1;
	MonteCarloNode* nextMove = root.GetHighestScoreChild();
	return chess::uci::moveToUci(nextMove->move);

	// Testing SimulateRandomGame()
	//std::cout << ChessSimulator::SimulateRandomGame(board) << std::endl;

	// Random Movement
	//chess::Movelist moves;
	//chess::movegen::legalmoves(moves, board);
	//if (moves.size() == 0)
	//	return "";

	//// Get Random Move
	//std::random_device rd;
	//std::mt19937 gen(rd());
	//std::uniform_int_distribution<> dist(0, moves.size() - 1);
	//auto move = moves[dist(gen)];
	//return chess::uci::moveToUci(move);

	//return "";
}


int CalculateSideScore(const chess::Board& board, const chess::Color& side)
{
	int score = 0;

	score += board.pieces(chess::PieceType::PAWN, side).count();
	score += board.pieces(chess::PieceType::KNIGHT, side).count() * 2;
	score += board.pieces(chess::PieceType::BISHOP, side).count() * 3;
	score += board.pieces(chess::PieceType::ROOK, side).count() * 5;
	score += board.pieces(chess::PieceType::QUEEN, side).count() * 9;

	return score;
}


float ChessSimulator::SimulateRandomGame(const chess::Board& prevBoard, chess::Board board, chess::Color sideToScore)
{
	// record who's turn it is
	//chess::Color startingSide = board.sideToMove();
	chess::Movelist movelist;

	int score = 0;

	int ourSideScore = CalculateSideScore(board, sideToScore);
	int opponentSideScore = CalculateSideScore(board, ~sideToScore);
	int startScoreDiff = ourSideScore - opponentSideScore;

	int numIterations = 0;
	// while the game isnt over, make a random legal move
	while ( board.isGameOver().first == chess::GameResultReason::NONE && numIterations < 100) {
		chess::movegen::legalmoves(movelist, board);
		board.makeMove( movelist[ rand() % movelist.size() ]);
		++numIterations;
	}

	ourSideScore = CalculateSideScore(board, sideToScore);
	opponentSideScore = CalculateSideScore(board, ~sideToScore);
	int endScoreDiff = ourSideScore - opponentSideScore;

	std::string fen = board.getFen();

	// if the current side to move isnt the same one that we started with, then swap sides
	//if (board.sideToMove() != startingSide) {
	//    board.makeNullMove();
	//}

	//switch (board.isGameOver().second) {
	//    case chess::GameResult::WIN:
	//        return 1.0f;
	//    case chess::GameResult::LOSE:
	//        return -1.0f;
	//    //case chess::GameResult::DRAW:
	//    //    return 0.0f;
	//    default:
	//        return 0.0f;
	//}

	// if the game isnt a lose, then its a tie
	if (board.isGameOver().second != chess::GameResult::LOSE) {
		return (endScoreDiff - startScoreDiff);
	}

	// if the loser is the startingSide, then we lose, else then we win
	if (board.sideToMove() == sideToScore) {
		return -10;
	}
	else {
		return 10;
	}
}
