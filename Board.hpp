#ifndef BOARD_HPP
#define BOARD_HPP

#include "raylib.h"
#include "Common.hpp"

#define BOARD_SQUARE_LENGTH_FACTOR 10.f

#define BOARD_FEN_LENGTH 85
#define BOARD_FEN_DEFAULT "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"



typedef enum {
    MOVE_NONE,
    MOVE_NORMAL,

    MOVE_PAWN_TWO_FORWARD,
    MOVE_PAWN_EN_PASSANT,
    MOVE_PAWN_PROMOTE,

    MOVE_CASTLING_KING,
    MOVE_CASTLING_QUEEN,
} MoveType;

typedef enum {
    CASTLING_NONE,
    CASTLING_KING_SIDE,
    CASTLING_QUEEN_SIDE,
} CastlingType;

typedef struct {
    /* The position that's start draw the board */
    Vector2 drawPosition;

    /* The color of the squares */
    Color squareBlackColor;
    Color squareWhiteColor;

    /* Pieces sprite sheet and the size of a sprite */
    Texture2D pieceSpriteSheet;
    Vector2 pieceSpriteSize;

    /* The length in pixels of a single square and the square data, i.e., if has
     * piece or not and which type of piece
     */
    f32 squareLength;
    u8 squares[64];

    /* Drag information store if is dragging and which piece is dragging */
    struct {
        u16 position     : 6;
        u16 dragging     : 1;

        f32 ringRotation;
    } movingPiece;

    struct {
        u16 enPassantSquare : 6;
        u16 castlingBlack   : 2;
        u16 castlingWhite   : 2;
        u16 waitPromotion   : 4;
        u16 whoMoves        : 5;
    } state;

    struct {
        /* Number of generated moves */
        u32 count;

        /* List of squares that are attacked by the enemy */
        u8 pseudoLegalMoves[64];

        /* List of move that can be made for which square (the move for the type of
        * the piece in some square)
        */
        u8 list[64][64];
    } move;
} Board;

void menu(int& menuorboard);

void menuInit();
/*
 * Initialize the board related to current screen width and height
 */
Board BoardInit(int screenWidth, int screenHeight);

/*
 * Loads pieces positions from FEN (Forsyth-Edwards Notation) string and return
 * true on success and false on error
 */
bool BoardLoadFEN(Board& board, const char fen[BOARD_FEN_LENGTH]);

/*
 * Unload some board internal data
 */
void BoardUnload(Board& board);

/*
 * Resize the board with some new screen width and height
 */
void BoardResize(Board& board, int screenWidth, int screenHeight);

/* Make some piece move to another position, return true on success and false on
 * failure
 */
bool BoardMakeMove(Board& board, u8 from, u8 to, bool updateWhoMoves);

/* Check if the king with a specific color is in check
 */
bool BoardKingInCheck(Board board, u8 kingColor);

/* Check if the king with a specific color is in check-mate
 */
bool BoardKingInMate(Board board, u8 kingColor);

/* Performance test, move enumeration function
 */
u64 BoardPerft(Board& board, u8 depth);

/*
 * Drag some piece in the board
 */
void BoardUpdate(Board& board);

/*
 * Draw the board
 */
void BoardDraw(Board board);

#endif /* BOARD_HPP */

