#include "piece.h"
#include <iostream>

Piece::Piece(Color c, std::string name, int id, Square pos)
    : color_(c), name_(std::move(name)), id_(id), position_(pos), has_moved_(false),
      captured_(false) {}

void Piece::display() const {
    std::cout << name_;
}
