#include "chess-simulator.h"
#include <random>
#include "monte-carlo-node.h"
#include <chrono> // Needed for when we start limiting monte carlo by time

using namespace ChessSimulator;
using namespace jneoy;

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
    MonteCarloNode root = MonteCarloNode(nullptr, board);










    // Random Movement
    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);
    if (moves.size() == 0)
        return "";

    // Get Random Move
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, moves.size() - 1);
    auto move = moves[dist(gen)];
    return chess::uci::moveToUci(move);

    return "";
}


float ChessSimulator::SimulateRandomGame(chess::Board board)
{
    // record who's turn it is
    chess::Color startingSide = board.sideToMove();
    srand(time(NULL));
    chess::Movelist movelist;

    // while the game isnt over, make a random legal move
    while ( board.isGameOver().first == chess::GameResultReason::NONE ) {
        chess::movegen::legalmoves(movelist, board);
        board.makeMove( movelist[ rand() % movelist.size() ]);
    }

    // if the current side to move isnt the same one that we started with, then swap sides
    if (board.sideToMove() != startingSide) {
        board.makeNullMove();
    }

    switch (board.isGameOver().second) {
        case chess::GameResult::WIN:
            return 1.0f;
        case chess::GameResult::LOSE:
            return -1.0f;
        case chess::GameResult::DRAW:
            return 0.0f;
        default:
            return 0.0f;
    }
}
