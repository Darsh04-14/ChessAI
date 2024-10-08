#include "engine4.h"

bool Engine4::cmp::operator()(const Move& lhs, const Move& rhs) {
  if (!lhs.capture() || !rhs.capture()) return lhs.capture();
  short valueA = pieceValue[lhs.capture()] - pieceValue[pieceAt(board, lhs.start())];
  int valueB = pieceValue[rhs.capture()] - pieceValue[pieceAt(board, rhs.start())];

  return valueA > valueB;
}

bool Engine4::PairCmp::operator()(const pair<int, Move>& lhs, const pair<int, Move>& rhs) {
  return lhs.first > rhs.first;
}

int Engine4::boardEvaluation() {
  Colour c = chess->getCurrentPlayer();
  const short* board = chess->getBoard();
  ULL friendAttack = chess->attackBitboards[colourInd(c)], enemyAttack = chess->attackBitboards[!colourInd(c)];

  int score = 0;

  for (int i = 0; i < 64; ++i) {
    Piece piece = Piece(board[i] & PieceType);
    Colour pieceColour = Colour(board[i] & ColourType);

    if (piece != NoPiece) {
      if (pieceColour == c) {
        if (!getBit(enemyAttack, i)) {
          score += pieceValue[piece];
        } else if (getBit(friendAttack, i)) {
          score += int(pieceValue[piece] / (1.4));
        } else {
          score += pieceValue[piece] / 4;
        }
      } else {
        if (!getBit(friendAttack, i)) {
          score -= pieceValue[piece];
        } else if (getBit(enemyAttack, i)) {
          score -= int(pieceValue[piece] / (1.6));
        } else {
          score -= pieceValue[piece] / 16;
        }
      }
    }

    if (getBit(friendAttack, i)) score += SQUARE_VALUE;
    if (getBit(enemyAttack, i)) score -= SQUARE_VALUE;
  }

  return score;
}

int Engine4::moveEvaluation(int depth, int alpha, int beta, int moveCounter = 0) {
  if (!depth) {
    ++nodeCount;
    return boardEvaluation();
  }

  chess->genLegalMoves();

  if (chess->end()) return chess->end() != (White | Black) ? (-2e6 + moveCounter) : 0;
  if (chess->check() && moveCounter <= 16) depth += 1;

  short legalMovesLen = chess->getLegalMovesLen();
  const Move* legalMoves = chess->getLegalMoves();
  Move* currentMoves = new Move[legalMovesLen];
  std::copy(legalMoves, legalMoves + legalMovesLen, currentMoves);
  sort(currentMoves, currentMoves + legalMovesLen, c);

  int valuation = -2e6;
  for (int i = 0; i < legalMovesLen; ++i) {
    chess->makeMove(currentMoves[i]);
    valuation = max(-moveEvaluation(depth - 1, -beta, -alpha, moveCounter + 1), valuation);
    chess->undoMove();
    alpha = max(alpha, valuation);
    if (valuation >= beta) break;
  }

  delete[] currentMoves;
  currentMoves = nullptr;

  return valuation;
}

bool Engine4::notify() {
  string cmd;
  cin >> cmd;
  if (cmd == "move") {
    short legalMovesLen = chess->getLegalMovesLen();

    if (!legalMovesLen) return false;

    const Move* legalMoves = chess->getLegalMoves();

    vector<pair<int, Move>> moves;
    for (int i = 0; i < legalMovesLen; ++i) moves.push_back({0, legalMoves[i]});

    auto t1 = std::chrono::high_resolution_clock::now();
    Move bestMove;
    int alpha;
    for (int d = 1; d <= MAX_DEPTH; ++d) {
      nodeCount = 0;
      bestMove = moves[0].second;
      alpha = -2e6;
      for (int j = 0; j < moves.size(); ++j) {
        chess->makeMove(moves[j].second);
        int value = -moveEvaluation(d - 1, -2e6, -alpha);
        moves[j].first = value;
        if (value > alpha) {
          bestMove = moves[j].second;
          alpha = value;
        }
        chess->undoMove();
      }
      sort(moves.begin(), moves.end(), PairCmp());
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms_double = (t2 - t1);

    cout << "Best evaluation: " << alpha << " nodes searched " << nodeCount << " | ";
    cout << ms_double.count() << "ms, at " << nodeCount / (ms_double.count() / 1000) << "node/s\n";

    chess->makeMove(bestMove);
    chess->genLegalMoves();

    return true;
  } else
    cout << "Invalid command!\n";
  return false;
}

Engine4::~Engine4() {}
