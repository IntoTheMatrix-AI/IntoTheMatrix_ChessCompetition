#pragma once
#include <string>
// disservin's lib. drop a star on his hard work!
// https://github.com/Disservin/chess-library
#include "chess.hpp"

namespace ChessSimulator {
/**
 * @brief Move a piece on the board
 *
 * @param fen The board as FEN
 * @return std::string The move as UCI
 */
std::string Move(std::string fen);


float SimulateRandomGame(const chess::Board& prevBoard, chess::Board board, chess::Color side);

} // namespace ChessSimulator