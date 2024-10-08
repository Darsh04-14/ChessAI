#include "move.h"

Move::Move() : moveData{0} {}
Move::Move(int start, int target, MoveFlag flag, Piece pieceType) : Move() {
  moveData = start | (target << 6) | (flag << 12);
  if (flag == PROMOTION) {
    moveData |= (pieceType << 18);
  } else {
    moveData |= (pieceType << 15);
  }
}

Move::Move(int start, int target, MoveFlag flag, int pieceType) : Move{start, target, flag, Piece(pieceType)} {}

Move::Move(int start, int target, Piece captured, Piece promoted) : Move(start, target, CAPTURE_PROMOTION, captured) {
  moveData |= (promoted << 18);
}

MoveFlag Move::flag() const { return MoveFlag((moveData >> 12) & 7); }

short Move::start() const { return moveData & positionMask; }
short Move::target() const { return (moveData >> 6) & positionMask; }

Piece Move::capture() const { return Piece((moveData >> 15) & 7); }

bool Move::isEnPassant() const { return flag() == ENPASSANT; }

bool Move::isCastle() const { return flag() == LEFT_CASTLE || flag() == RIGHT_CASTLE; }

Piece Move::promotion() const { return Piece((moveData >> 18) & 7); }

bool Move::isCapturePromotion() const { return flag() == CAPTURE_PROMOTION; }

bool Move::operator==(const Move& other) const { return moveData == other.moveData; }
