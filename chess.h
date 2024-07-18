#define CHESS_H

#include <iostream>
#include <vector>
using namespace std;

typedef unsigned long long ULL;

// The special piece is simply needed to make the castle and en passant flag bits
enum Piece { None, King, Queen, Knight, Bishop, Rook, Pawn, Special };
enum Colour { White = 8, Black = 16 };

class Chess {
    bool debugHelper = false;
    static const short NUM_ROWS = 8;
    static const short NUM_COLS = 8;

    static const short ColourType = (3 << 3);
    static const short PieceType = (1 << 3) - 1;

    // Precomputed ray masks used to determine pinned pieces
    unsigned long long diagonalMasks[16];
    unsigned long long rdiagonalMasks[16];
    unsigned long long horizontalMasks[8];
    unsigned long long verticalMasks[8];

    enum MoveFlag { CAPTURE, PROMOTION, ENPASSANT = 14, CASTLE = 15 };
    struct Move {
        static const short positionMask = (1 << 6) - 1;
        short moveData;
        short capturePromotionData;
        // moveData will be represented as SSSSSS TTTTTT F PPP
        // S - Starting square bits
        // T - Target Sqaure bits
        // F - Flag bits, capture -> 0, promotion -> 1
        // P - Piece type for captures or promotions
        // Special Cases: En Passant and Castling use FPPP bits together, with values 0111 and 1111 respectively
        // * You may capture nothing, which is just a regular move
        // * The capture promotion variable stores promoted piece data when a pawn capture leads to a promotion

        Move();
        Move(int, int, MoveFlag = CAPTURE, Piece = None);
        Move(int, int, MoveFlag, int);
        bool flag() const;
        short piece() const;
        short start() const;
        short target() const;
        short isCapture() const;
        bool isEnPassant() const;
        bool isCastle() const;
        short isPromotion() const;
        short isCapturePromotion() const;
        bool operator==(const Move&) const;
    };

    Move legalMoves[321];  // Max number of pseudolegal moves in any position
    short legalMovesLen;

    vector<Move> previousMoves;

    short castlingRights[2][2];

    // Boolean value to determine if generated moves should be added to vector or capture mask
    bool getAttackMasks;
    Colour colourToMove;

    // Used to efficiently compute pins
    ULL bitBoard[2][6];

    // A piece will be represented as CCPPP
    // C - Colour bits
    // P - Piece type bits
    short board[64];

    // Used to keep track of squares that each colour is able to capture or move to
    ULL attackMask[2];

    // Used to track pins on pieces
    vector<ULL> pinRayMask;

    Piece getPiece(char);
    // ULL& BitBoard(Colour, Piece);

    void getAttackSquares();
    short getKing(Colour);
    bool isSquareAttacked(short, short);
    ULL isSquarePinned(short);
    void generateRayMasks();
    // void genPins();

    void genCaptures();

    // Generates legal moves
    void generatePseudoLegalMoves();
    void generateLegalMoves();
    void genCastleMove();
    void genKingMoves(short);
    void genPawnMoves(short);
    void genKnightMoves(short);
    void genSlidingMoves(short);

    // Responsible for adding moves to vector, updating captureMask state, and filtering out any illegal moves
    void addMove(const Move&);

    void makeMove(const Move&);

   public:
    Chess();
    Chess(string);
    bool playMove(short, short);
    bool playMove(string, string);
    void unmakeMove();
    void print();
    void printLegalMoves();
    void printCastleRights();
    int perft(int, int = 0);
    friend ostream& operator<<(ostream&, Chess&);
};