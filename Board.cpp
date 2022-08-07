#include <cctype>
#include <cstring>

#include "Board.hpp"
#include "Piece.hpp"
#include "Common.hpp"
#include "SAXA.hpp"

/*
 */
Texture2D xuxinha; // Upload da imagem para o menu
Texture2D mao; // Upload da imagem da mãozinha para o menu
Texture2D logo; // Upload da imagem da Logo para o menu


/* Internal helpers functions
 */
static void generateMoves(Board& board, bool onlyLegalMoves);
static void updateTurn(Board& board, bool updateWhoMove);
static bool moveIsValid(Board board, u8 from, u8 to);

/* Internal generate move functions
 * This functions generate moves for specific pieces
 */
static void generateKingMoves(Board& board, u8 square, bool legal);
static void generateSlightMoves(Board& board, u8 square, bool legal);
static void generateKnightMoves(Board& board, u8 square, bool legal);
static void generatePawnMoves(Board& board, u8 square, bool legal);

/* Internal update functions
 */
static void updatePromotionMenu(Board& board);

/* Internal draw functions
 */
static void drawSquare(Board board, Rectangle drawPosition, Color color);
static void drawPiece(Board board, Rectangle drawPosition, u8 square, Color color);
static void drawRankAndFiles(Board board, Rectangle drawPosition, u8 square, Color color);
static void drawMoves(Board board, Rectangle drawPosition, u8 square);
static void drawDraggingPiece(Board board, Rectangle drawPosition);
static void drawPromotionMenu(Board board);



void menuInit(){
    xuxinha = LoadTexture("./assets/xuxinha.png"); // Upload da imagem para o menu
    mao = LoadTexture("./assets/mao.png"); // Upload da imagem da mãozinha para o menu
    logo = LoadTexture("./assets/logo.png"); // Upload da imagem da Logo para o menu
}

void menu(int& menuorboard){
    Vector2 mousePoint = GetMousePosition(); //Pegar as coordenadas do cursor
    Rectangle rec1 = {100, 450, 330, 35}; //Tamanhos dos quadrados das opções do menu
    Rectangle rec2 = {100, 500, 330, 35};
    Rectangle rec3 = {100, 550, 330, 35};
    Rectangle rec4 = {100, 600, 330, 35};


    //Desenhando na tela
    BeginDrawing(); // Comando para iniciar a desenhar
    ClearBackground(BLACK); // Comando para pintar o background

    DrawTexture(xuxinha, 850, 60, RAYWHITE); // Implementando a imagem já feita o upload na variavel "xuxinha"
    DrawTexture(logo, 80, 15, RAYWHITE); // Implementando a imagem já feita o upload na variável "logo"

    DrawRectangleLines(100, 450, 330, 35, PINK); // Desenhando um retangulo com bordas rosas (posição x, posição y, largura, altura)
    DrawText("Jogar", 240, 460, 20, WHITE); // Desenhando um texto "Jogar" (texto, posição x, posição y, tamanho da fonte, cor da fonte)
        if (CheckCollisionPointRec(mousePoint,rec1)){ // Se o mousePoint passar pelo rec1 (mousePoint = posição do mouse) (rec1 = primeira opção do menu)
            DrawText("Xadrez da Xuxinha", 200, 350, 50, WHITE); // Desenho o texto "Xadrez da Xuxinha", (texto, posição x, posição y, tamanho da fonte, cor da fonte)
            DrawTexture(mao,410,450,RAYWHITE); // Desenhando a imagem atribuida na variavel "mao", com posiçãoX=410 e posiçãoY=450 e e tom da imagem branco
            if(IsMouseButtonPressed(0)){ // Se o botão esquerdo do mouse for clicado
                //PlaySound(button);// O som da variável atribuida a "button" toca
                menuorboard = 1;
            }
        };

    DrawRectangleLines(100, 500, 330, 35, PINK);
    DrawText("Tabuleiros", 213, 510, 20, WHITE);
        if (CheckCollisionPointRec(mousePoint,rec2)){
            DrawText("Estilos de Tabuleiros", 200, 350, 50, WHITE);
            DrawTexture(mao,410,500,RAYWHITE);
            if(IsMouseButtonPressed(0)){
               // PlaySound(button);
            }
        };

    DrawRectangleLines(100, 550, 330, 35, PINK);
    DrawText("Em aberto", 215,560, 20, WHITE);
        if (CheckCollisionPointRec(mousePoint,rec3)){
            DrawText("Apenas Deus sabe", 200, 350, 50, WHITE);
            DrawTexture(mao,410,550,RAYWHITE);
            if(IsMouseButtonPressed(0)){
                //PlaySound(button);
            }
        };

    DrawRectangleLines(100, 600, 330, 35, PINK);
    DrawText("Créditos", 223, 610, 20, WHITE);
        if (CheckCollisionPointRec(mousePoint,rec4)){
            DrawText("Grupo", 200, 350, 50, WHITE);
            DrawTexture(mao,410,600,RAYWHITE);
            if(IsMouseButtonPressed(0)){
                //PlaySound(button);
            }
        };

        EndDrawing(); // Fechando a parte do desenho
}




Board BoardInit(int screenWidth, int screenHeight)
{
    Board board;
    std::memset(&board, 0, sizeof(Board));

    board.squareBlackColor = PINK;
    board.squareWhiteColor = WHITE;

#ifndef BOARD_PERFT
    board.pieceSpriteSheet = LoadTexture("sprites/chess_pieces.png");
    board.pieceSpriteSize = {
        board.pieceSpriteSheet.width / 6.f,
        board.pieceSpriteSheet.height / 2.f,
    };
#endif /* BOARD_PERFT */

    BoardResize(board, screenWidth, screenHeight);

    return board;
}

bool BoardLoadFEN(Board& board, const char fen[BOARD_FEN_LENGTH])
{
    const char pieces_data[12] = {
        'k', PIECE_KING,
        'q', PIECE_QUEEN,
        'b', PIECE_BISHOP,
        'n', PIECE_KNIGHT,
        'r', PIECE_ROOK,
        'p', PIECE_PAWN,
    };

    u8 i = 0;
    u8 rank = 0;
    u8 file = 0;

    /* Load the pieces location */
    for (; fen[i] != ' '; i++) {
        if (fen[i] == '/') {
            rank++;
            file = 0;
        } else if (std::isdigit(fen[i])) {
            /* Convert to a number and add to file */
            file += fen[i] - '0';
        } else {
            for (int j = 0; j < 12; j += 2) {
                /* if the fen symbol isn't the same of pieces data the jump to
                 * continue to the next pieces data symbol
                 */
                if (std::tolower(fen[i]) != pieces_data[j])
                    continue;

                board.squares[rank * 8 + file] = pieces_data[j + 1] + (std::isupper(fen[i]) ? PIECE_WHITE : PIECE_BLACK);
                file++;

                break;
            }
        }
    }

    /* Load who should move */
    i++;
    if (fen[i] == 'w')
        board.state.whoMoves = PIECE_WHITE;
    else if (fen[i] == 'b')
        board.state.whoMoves = PIECE_BLACK;
    else
        return false;

    for (i += 2; fen[i] != ' '; i++) {
        if (fen[i] == 'K')
            board.state.castlingWhite |= CASTLING_KING_SIDE;
        else if (fen[i] == 'Q')
            board.state.castlingWhite |= CASTLING_QUEEN_SIDE;
        else if (fen[i] == 'k')
            board.state.castlingBlack |= CASTLING_KING_SIDE;
        else if (fen[i] == 'q')
            board.state.castlingBlack |= CASTLING_QUEEN_SIDE;
        else if (fen[i] == '-')
            break;
        else
            return false;
    }

    i++;
    if (fen[i] != '-') {
        board.state.enPassantSquare = fen[i] - 'a' + fen[i + 1] - '0';
        i += 2;
    } else {
        board.state.enPassantSquare = 0;
        i++;
    }

    /* TODO: half moves and full moves data */
    updateTurn(board, false);

    return true;
}

void BoardUnload(Board& board)
{
#ifndef BOARD_PERFT
    UnloadTexture(board.pieceSpriteSheet);
#endif /* BOARD_PERFT */
}

void BoardResize(Board& board, int screenWidth, int screenHeight)
{
    board.squareLength = MIN(screenWidth, screenHeight) / BOARD_SQUARE_LENGTH_FACTOR;

    board.drawPosition = {
        (screenWidth / 2.f) - (board.squareLength * 4),
        (screenHeight / 2.f) - (board.squareLength * 4),
    };
}

bool BoardMakeMove(Board& board, u8 from, u8 to, bool updateWhoMoves)
{
    if (board.move.list[from][to] == MOVE_NONE || from == to)
        return false;

    const u8 enPassantSquare = board.state.enPassantSquare + (PieceHasColor(board.squares[from], PIECE_BLACK) ? -8 : 8);

    u8 castlingSide;

    board.state.enPassantSquare = 0;
    switch (board.move.list[from][to]) {
    case MOVE_PAWN_EN_PASSANT:
        board.squares[enPassantSquare] = PIECE_NONE;
        board.squares[to] = board.squares[from];
        break;

    case MOVE_PAWN_TWO_FORWARD:
        board.state.enPassantSquare = to + (PieceHasColor(board.squares[from], PIECE_BLACK) ? -8 : 8);

        /* fall through */

    case MOVE_NORMAL:
        board.squares[to] = board.squares[from];
        break;

    case MOVE_PAWN_PROMOTE:
        /* The +1 ensures that has a promotion even if it's for a black piece
         * with the file 0.
         */
        board.state.waitPromotion = PieceFile(to) + 1;

        if (PieceHasColor(board.squares[from], PIECE_WHITE))
            board.state.waitPromotion |= 0b1000;

        break;

    case MOVE_CASTLING_KING:
        board.squares[to] = board.squares[from];
        board.squares[from + 1] = board.squares[from + 3];
        board.squares[from + 3] = PIECE_NONE;
        break;

    case MOVE_CASTLING_QUEEN:
        board.squares[to] = board.squares[from];
        board.squares[from - 1] = board.squares[from - 4];
        board.squares[from - 4] = PIECE_NONE;
        break;

    default:
        break;
    }

    /* Disable any castling because the kings has moved */
    if (PieceHasType(board.squares[from], PIECE_KING)) {
        if (PieceHasColor(board.squares[from], PIECE_WHITE))
            board.state.castlingWhite = CASTLING_NONE;
        else
            board.state.castlingBlack = CASTLING_NONE;
    /* Disable one side castling because the rook has moved */
    } else if (PieceHasType(board.squares[from], PIECE_ROOK)) {
        /* Queen side rooks */
        if (from == 56 || from == 0)
            castlingSide = CASTLING_KING_SIDE;
        /* King side rooks */
        else if (from == 63 || from == 7)
            castlingSide = CASTLING_QUEEN_SIDE;

        if (PieceHasColor(board.squares[from], PIECE_WHITE))
            board.state.castlingWhite &= castlingSide;
        else
            board.state.castlingBlack &= castlingSide;
    }

    board.squares[from] = PIECE_NONE;
    if (updateWhoMoves)
        updateTurn(board, true);

    return true;
}

bool BoardKingInCheck(Board board, u8 kingColor)
{
    for (u8 square = 0; square < 64; square++)
        if (board.move.pseudoLegalMoves[square]
                && PieceHasType(board.squares[square], PIECE_KING)
                && PieceHasColor(board.squares[square], kingColor))
            return true;

    return false;
}

bool BoardKingInMate(Board board, u8 kingColor)
{
    if (BoardKingInCheck(board, kingColor) && board.move.count == 0)
        return true;

    return false;
}

u64 BoardPerft(Board& board, u8 depth)
{
    u64 nodes = 0;

#ifdef BOARD_PERFT
    Board copiedBoard;

    if (depth == 0)
        return 1;

    for (u8 from = 0; from < 64; from++) {
        for (u8 to = 0; to < 64; to++) {
            std::memcpy(&copiedBoard, &board, sizeof(board));

            if (BoardMakeMove(copiedBoard, from, to, true))
                nodes += BoardPerft(copiedBoard, depth - 1);
        }
    }
#endif /* BOARD_PERFT */

    return nodes;
}

void BoardUpdate(Board& board)
{
#ifndef BOARD_PERFT
    i8 rank;
    i8 file;
    i8 square;

    /* Resize the board if screen size has changed */
    if (IsWindowResized())
        BoardResize(board, GetScreenWidth(), GetScreenHeight());

    if (board.state.waitPromotion) {
        updatePromotionMenu(board);
    } else {
        rank = (i8) ((GetMouseY() - board.drawPosition.y) / board.squareLength);
        file = (GetMouseX() - board.drawPosition.x) / board.squareLength;

        square = PieceSquare(rank, file);

        if (rank >= 0 && rank <= 7 && file >= 0 && file <= 7) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !board.movingPiece.dragging
                    && !PieceHasType(board.squares[square], PIECE_NONE)
                    && PieceHasColor(board.squares[square], board.state.whoMoves)) {
                board.movingPiece.dragging = true;
                board.movingPiece.position = square;
            } else if (IsMouseButtonUp(MOUSE_BUTTON_LEFT) && board.movingPiece.dragging) {
                board.movingPiece.dragging = false;

                if (PieceHasColor(board.squares[board.movingPiece.position], board.state.whoMoves))
                    BoardMakeMove(board, board.movingPiece.position, square, true);
            }
        }
    }
    if(board.state.whoMoves == PIECE_BLACK){
        saxa_move saxaMove = finalMove(board,3);
        printf("->%f",saxaMove.grade);
        BoardMakeMove(board,saxaMove.from,saxaMove.to,true);

    }//else if (board.state.whoMoves == PIECE_WHITE){
       // saxa_move saxaMove = finalMove(board,1);
        //BoardMakeMove(board,saxaMove.from,saxaMove.to,true);
    //}
    if (board.movingPiece.dragging)
        board.movingPiece.ringRotation += 150 * GetFrameTime();
    else
        board.movingPiece.ringRotation = 0;

    if (board.movingPiece.ringRotation > 360)
        board.movingPiece.ringRotation = 0;
#endif /* BOARD_PERFT */
}

void BoardDraw(Board board)
{
#ifndef BOARD_PERFT
    Rectangle squarePosition = {
        board.drawPosition.x,
        board.drawPosition.y,

        board.squareLength,
        board.squareLength
    };

    Color color, opositeColor;

    for (u8 square = 0; square < 64; square++) {
        squarePosition.x = board.drawPosition.x + squarePosition.width
            * PieceFile(square);
        squarePosition.y = board.drawPosition.y + squarePosition.height
            * PieceRank(square);

        if ((PieceRank(square) + PieceFile(square)) % 2) {
            color = board.squareBlackColor;
            opositeColor = board.squareWhiteColor;
        } else {
            color = board.squareWhiteColor;
            opositeColor = board.squareBlackColor;
        }

        /* Draw the square */
        drawSquare(board, squarePosition, color);

        /* Draw the piece, if it's a valid piece and isn't dragging it
         */
        drawPiece(board, squarePosition, square, opositeColor);

        /* Draw the rank and the file on the board */
        drawRankAndFiles(board, squarePosition, square, opositeColor);

        /* Draw the possible moves */
        drawMoves(board, squarePosition, square);

        // if (board.move.pseudoLegalMoves[square])
            // DrawRectangleRec(squarePosition, { 0, 255, 0, 100 });
    }

    if (!board.state.waitPromotion)
        drawDraggingPiece(board, squarePosition);

    drawPromotionMenu(board);
#endif /* BOARD_PERFT */
}

static void generateMoves(Board& board, bool onlyLegalMoves)
{
    bool legalMoves = false;

    /* Reset the moves list, setting all bytes to zero */
    std::memset(&board.move, 0, sizeof(board.move));

    u8 squareValue;
    u8 kingsSquareIndex = 0;
    u8 kingsSquare[2];

    for (u8 square = 0; square < (64 + kingsSquareIndex); square++) {
        /* If it's generating for the board or "after king generation" */
        squareValue = square < 64 ? square : kingsSquare[square - 64];

        /* If the piece in this square hasn't the color that's move this turn,
         * only generate pseudo-legal moves for it.
         */
        if (!PieceHasColor(board.squares[squareValue], board.state.whoMoves) || !onlyLegalMoves)
            legalMoves = false;
        /* If the piece has the same color that's move this turn but check if
         * it's needed to generate legal moves for it.
         */
        else if (onlyLegalMoves)
            legalMoves = true;
        else
            continue;

        switch (PieceGetType(board.squares[squareValue])) {
        case PIECE_KING:
            /* The king moves need to be generated after all other pieces move
             * generation because of the king castling that must now if it's in
             * check, this information only can be fetch after all move
             * generation.
             *
             * Therefore, we defer the king move generation to end of the
             * generation, this ensures the check informations was fetched.
             */
            if (square < 64)
                kingsSquare[kingsSquareIndex++] = square;
            else
                generateKingMoves(board,squareValue, legalMoves);

            break;

        case PIECE_QUEEN: case PIECE_BISHOP: case PIECE_ROOK:
            generateSlightMoves(board, squareValue, legalMoves);
            break;

        case PIECE_KNIGHT:
            generateKnightMoves(board, squareValue, legalMoves);
            break;

        case PIECE_PAWN:
            generatePawnMoves(board, squareValue, legalMoves);
            break;

        default:
            break;
        }
    }
}

static void generateKingMoves(Board& board, u8 square, bool legalMove)
{
    static const i8 directionOffsets[8][2] = {
        {  0, -1 }, {  0,  1 },
        { -1, -1 }, { -1,  1 },
        {  1, -1 }, {  1,  1 },
        { -1,  0 }, {  1,  0 },
    };

    const bool castlingKing = (board.state.whoMoves == PIECE_WHITE ?
        board.state.castlingWhite : board.state.castlingBlack) & CASTLING_KING_SIDE;

    const bool castlingQueen = (board.state.whoMoves == PIECE_WHITE ?
        board.state.castlingWhite : board.state.castlingBlack) & CASTLING_QUEEN_SIDE;

    i8 targetSquare;
    i8 targetRank;
    i8 targetFile;

    u8 kingMoves;
    u8 castlingSide;

    for (u8 direction = 0; direction < 8; direction++) {
        /* If the king is in check can't do any cast this turn */
        if (BoardKingInCheck(board, PieceGetColor(board.squares[square])))
            kingMoves = 1;
        /* If the move is to left and can cast to queen side */
        else if (direction == 6 && castlingQueen)
            kingMoves = 2, castlingSide = MOVE_CASTLING_QUEEN;
        /* If the move is to right and can cast to king side */
        else if (direction == 7 && castlingKing)
            kingMoves = 2, castlingSide = MOVE_CASTLING_KING;
        else
            kingMoves = 1;

        for (u8 moves = 1; moves <= kingMoves; moves++) {
            targetRank = PieceRank(square) + directionOffsets[direction][1];
            targetFile = PieceFile(square) + directionOffsets[direction][0] * moves;

            targetSquare = PieceSquare(targetRank, targetFile);

            if (targetRank < 0 || targetRank > 7 || targetFile < 0 || targetFile > 7
                    || PieceHasColor(board.squares[targetSquare], PieceGetColor(board.squares[square])))
                continue;

            if (!legalMove) {
                board.move.pseudoLegalMoves[targetSquare] = true;
            } else if (moveIsValid(board, square, targetSquare)) {
                board.move.list[square][targetSquare] = moves == 1 ?
                MOVE_NORMAL : castlingSide;
                board.move.count++;
            } else {
                /* Always break the moves loop only, this stop the castling
                 * if doing one and stop the move loop in anyway.
                 */
                break;
            }
        }
    }
}

static void generateSlightMoves(Board& board, u8 square, bool legalMove)
{
    static const i8 directionOffsets[8][2] = {
        {  0, -1 }, {  0,  1 },
        { -1,  0 }, {  1,  0 },
        {  1, -1 }, { -1,  1 },
        { -1, -1 }, {  1,  1 },
    };

    /* If it's bishop only use the up four offsets that's diagonal offsets */
    const u8 startDirection = PieceHasType(board.squares[square], PIECE_BISHOP) ?
        4 : 0;

    /* If it's rook only use the bottom four offsets that's vertical and
     * horizontal offsets.
     */
    const u8 endDirection = PieceHasType(board.squares[square], PIECE_ROOK) ?
        4 : 8;

    i8 targetSquare;
    i8 targetRank;
    i8 targetFile;

    for (u8 direction = startDirection; direction < endDirection; direction++) {
        /* Generate moves until reach some impossible place to move */
        for (u8 moves = 1 ;; moves++) {
            targetRank = PieceRank(square) + directionOffsets[direction][1] * moves;
            targetFile = PieceFile(square) + directionOffsets[direction][0] * moves;

            targetSquare = PieceSquare(targetRank, targetFile);

            if (targetRank < 0 || targetRank > 7 || targetFile < 0 || targetFile > 7
                    || PieceHasColor(board.squares[targetSquare], PieceGetColor(board.squares[square])))
                break;

            if (!legalMove) {
                board.move.pseudoLegalMoves[targetSquare] = true;
            } else if (moveIsValid(board, square, targetSquare)) {
                board.move.list[square][targetSquare] = MOVE_NORMAL;
                board.move.count++;
            }

            if (!(PieceHasType(board.squares[targetSquare], PIECE_NONE)
                    || PieceHasColor(board.squares[targetSquare], PieceGetColor(board.squares[square]))))
                break;
        }
    }
}

static void generateKnightMoves(Board& board, u8 square, bool legalMove)
{
    static const i8 directionOffsets[8][2] = {
        { -1, -2 }, {  1, -2 },
        { -1,  2 }, {  1,  2 },
        { -2, -1 }, { -2,  1 },
        {  2, -1 }, {  2,  1 },
    };

    i8 targetSquare;
    i8 targetRank;
    i8 targetFile;

    for (u8 direction = 0; direction < 8; direction++) {
        targetRank = PieceRank(square) + directionOffsets[direction][1];
        targetFile = PieceFile(square) + directionOffsets[direction][0];

        targetSquare = PieceSquare(targetRank, targetFile);

        if (targetRank < 0 || targetRank > 7 || targetFile < 0 || targetFile > 7
                || PieceHasColor(board.squares[targetSquare], PieceGetColor(board.squares[square])))
            continue;

        if (!legalMove) {
            board.move.pseudoLegalMoves[targetSquare] = true;
        } else if (moveIsValid(board, square, targetSquare)) {
            board.move.list[square][targetSquare] = MOVE_NORMAL;
            board.move.count++;
        }
    }
}

static void generatePawnMoves(Board& board, u8 square, bool legalMove)
{
    static const i8 directionOffsets[8][2] = {
        {  0,  1 },
        {  1,  1 },
        { -1,  1 },
    };

    /* If the piece is white the pawn moves up, i.e., subtracting the rank value
     * of the pawn, otherwise add the rank value
     */
    const i8 direction = PieceHasColor(board.squares[square], PIECE_WHITE) ? -1 : 1;

    /* If it's the first move of the pawn it can go two square forward */
    const u8 forwardMoves = PieceRank(square) == 1 || PieceRank(square) == 6 ? 2 : 1;

    i8 targetRank;
    i8 targetFile;
    i8 targetSquare;

    for (u8 moves = 1; moves <= forwardMoves; moves++) {
        targetRank = PieceRank(square) + directionOffsets[0][1] * moves * direction;
        targetFile = PieceFile(square) + directionOffsets[0][0];

        targetSquare = PieceSquare(targetRank, targetFile);

        if (targetRank < 0 || targetRank > 7 || targetFile < 0 || targetFile > 7
                || !PieceHasType(board.squares[targetSquare], PIECE_NONE))
            break;

        if (!legalMove) {
            board.move.pseudoLegalMoves[targetSquare] = true;
        } else if (moveIsValid(board, square, targetSquare)) {
            if (moves == 1) {
                if (targetSquare > 7 && targetSquare < 56)
                    board.move.list[square][targetSquare] = MOVE_NORMAL;
                else
                    board.move.list[square][targetSquare] = MOVE_PAWN_PROMOTE;
            } else {
                board.move.list[square][targetSquare] = MOVE_PAWN_TWO_FORWARD;
            }

            board.move.count++;
        }
    }

    for (u8 moves = 1; moves <= 2; moves++) {
        targetRank = PieceRank(square) + directionOffsets[moves][1] * direction;
        targetFile = PieceFile(square) + directionOffsets[moves][0];

        targetSquare = PieceSquare(targetRank, targetFile);

        if (targetRank < 0 || targetRank > 7 || targetFile < 0 || targetFile > 7
                || PieceHasColor(board.squares[targetSquare], PieceGetColor(board.squares[square])))
            continue;

        if (!legalMove) {
            board.move.pseudoLegalMoves[targetSquare] = true;
        } else if (moveIsValid(board, square, targetSquare)) {
             if (targetSquare == board.state.enPassantSquare) {
                 board.move.list[square][targetSquare] = MOVE_PAWN_EN_PASSANT;
                 board.move.count++;
             }

            if (!PieceHasType(board.squares[targetSquare], PIECE_NONE)
                    && !PieceHasColor(board.squares[targetSquare], PieceGetColor(board.squares[square]))) {
                if (targetSquare > 7 && targetSquare < 56)
                    board.move.list[square][targetSquare] = MOVE_NORMAL;
                else
                    board.move.list[square][targetSquare] = MOVE_PAWN_PROMOTE;

                board.move.count++;
            }
        }
    }
}

static void updateTurn(Board& board, bool updateWhoMove)
{
    if (updateWhoMove) {
        if (board.state.whoMoves == PIECE_WHITE)
            board.state.whoMoves = PIECE_BLACK;
        else
            board.state.whoMoves = PIECE_WHITE;
    }

    generateMoves(board, true);
}

static bool moveIsValid(Board board, u8 from, u8 to)
{
    Board copiedBoard;

    std::memcpy(&copiedBoard, &board, sizeof(board));
    copiedBoard.move.list[from][to] = MOVE_NORMAL;

    BoardMakeMove(copiedBoard, from, to, false);
    generateMoves(copiedBoard, false);

    for (u8 square = 0; square < 64; square++)
        if (copiedBoard.move.pseudoLegalMoves[square]
            && PieceHasType(copiedBoard.squares[square], PIECE_KING)
            && PieceHasColor(copiedBoard.squares[square], copiedBoard.state.whoMoves))
            return false;

    return true;
}

static void updatePromotionMenu(Board& board)
{
    static u8 promotionSelected = 5;

    const u8 rank = (board.state.waitPromotion & 0b1000) == 0 ? 7 : 0;
    const u8 file = (board.state.waitPromotion - 1) & 0b0111;

    const Rectangle menuRectangle = {
        board.drawPosition.x + board.squareLength,
        board.drawPosition.y + board.squareLength * 3,

        board.squareLength * 6,
        board.squareLength * 2,
    };

    Rectangle spritePosition = {
        menuRectangle.x + menuRectangle.width / 18.f,
        menuRectangle.y + menuRectangle.height / 7.f,

        menuRectangle.width / 4.5f,
        menuRectangle.height / 1.5f,
    };

    if (!board.state.waitPromotion)
        return;

    for (u8 promotion = 1; promotion <= 4; promotion++) {
        if (!CheckCollisionPointRec(GetMousePosition(), spritePosition)) {
            spritePosition.x += spritePosition.width;
            continue;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && promotionSelected > 4) {
            promotionSelected = promotion;
        } else if (IsMouseButtonUp(MOUSE_BUTTON_LEFT) && promotionSelected <= 4) {
            board.squares[PieceSquare(rank, file)] = promotionSelected + PIECE_KING;

            if (board.state.waitPromotion & 0b1000)
                board.squares[PieceSquare(rank, file)] += PIECE_WHITE;
            else
                board.squares[PieceSquare(rank, file)] += PIECE_BLACK;

            updateTurn(board, false);

            promotionSelected = 5;
            board.state.waitPromotion = 0;
            break;
        }
    }
}

static void drawSquare(Board board, Rectangle drawPosition, Color color)
{
    if (CheckCollisionPointRec(GetMousePosition(), drawPosition)
            && !board.state.waitPromotion)
        DrawRectangleRec(drawPosition, RED);
    else
        DrawRectangleRec(drawPosition, color);
}

static void drawPiece(Board board, Rectangle drawPosition, u8 square, Color color)
{
    Rectangle spritePosition = {
        0, 0,

        board.pieceSpriteSize.x,
        board.pieceSpriteSize.y
    };

    if (PieceHasType(board.squares[square], PIECE_NONE))
        return;

    /* Draw a red square if the king is in check */
    if (PieceHasType(board.squares[square], PIECE_KING)
            && BoardKingInCheck(board, PieceGetColor(board.squares[square])))
        DrawRectangleRec(drawPosition, RED);

    /* Draw the piece image if isn't dragging any piece or if is dragging
     * doesn't the piece is this square.
     */
    if (!board.movingPiece.dragging || board.movingPiece.position != square) {
        spritePosition.x = spritePosition.width * (PieceGetType(board.squares[square]) - 1);
        spritePosition.y = spritePosition.height * PieceHasColor(board.squares[square], PIECE_BLACK);

        DrawTexturePro(board.pieceSpriteSheet, spritePosition, drawPosition, { 0,0 }, 0, WHITE);
    /* In here draw a replacement of the piece image that's dragging in this
     * case.
     */
    } else {
        drawPosition.x += drawPosition.width / 2.f;
        drawPosition.y += drawPosition.height / 2.f;

        DrawRing({ drawPosition.x, drawPosition.y }, board.squareLength * 0.4, board.squareLength * 0.5,
            board.movingPiece.ringRotation, 360 + board.movingPiece.ringRotation, 6, color);
    }
}

static void drawRankAndFiles(Board board, Rectangle drawPosition, u8 square, Color color)
{
    const u16 textFontSize = board.squareLength / 5;
    const u16 textSize = MeasureText("a", textFontSize);

    const u8 file = PieceFile(square);
    const u8 rank = PieceRank(square);

    if (rank == 7) {
        drawPosition.x += drawPosition.width - textSize - 5;
        drawPosition.y += drawPosition.height - textFontSize - 5;

        DrawText(TextFormat("%c", 'a' + file), drawPosition.x, drawPosition.y, textFontSize, color);
    }

    if (file == 0) {
        if (rank == 7) {
            drawPosition.x -= drawPosition.width - textSize - 5;
            drawPosition.y -= drawPosition.height - textFontSize - 5;
        }

        drawPosition.x += 5;
        drawPosition.y += 5;

        DrawText(TextFormat("%d", 8 - rank), drawPosition.x, drawPosition.y, textFontSize, color);
    }
}

static void drawMoves(Board board, Rectangle drawPosition, u8 square)
{
    static const Color color = { 128, 128, 128, 100 };

    if (!board.movingPiece.dragging
            || !board.move.list[board.movingPiece.position][square]
            || !PieceHasColor(board.squares[board.movingPiece.position], board.state.whoMoves))
        return;

    drawPosition.x += drawPosition.width / 2.f;
    drawPosition.y += drawPosition.height / 2.f;

    if (PieceHasType(board.squares[square], PIECE_NONE))
        DrawCircle(drawPosition.x, drawPosition.y, drawPosition.width / 6.f, color);
    else
        DrawRing({ drawPosition.x, drawPosition.y }, board.squareLength * 0.5,
                 board.squareLength * 0.4, 0, 360, 0, color);
}

static void drawDraggingPiece(Board board, Rectangle drawPosition)
{
    Rectangle spritePosition = {
        0, 0,

        board.pieceSpriteSize.x,
        board.pieceSpriteSize.y
    };

    if (!board.movingPiece.dragging)
        return;

    spritePosition.x = spritePosition.width * (PieceGetType(board.squares[board.movingPiece.position]) - 1);
    spritePosition.y = spritePosition.height * PieceHasColor(board.squares[board.movingPiece.position], PIECE_BLACK);

    drawPosition.x = GetMouseX() - board.squareLength / 2.f;
    drawPosition.y = GetMouseY() - board.squareLength / 2.f;

    DrawTexturePro(board.pieceSpriteSheet, spritePosition, drawPosition, { 0, 0 }, 0, WHITE);
}

static void drawPromotionMenu(Board board)
{
    const Rectangle menuRectangle = {
        board.drawPosition.x + board.squareLength,
        board.drawPosition.y + board.squareLength * 3,

        board.squareLength * 6,
        board.squareLength * 2,
    };

    Rectangle spriteRectangle = {
        board.pieceSpriteSize.x,
        board.pieceSpriteSize.y * ((board.state.waitPromotion & 0b1000) == 0),

        board.pieceSpriteSize.x,
        board.pieceSpriteSize.y,
    };

    Rectangle spritePosition = {
        menuRectangle.x + menuRectangle.width / 18.f,
        menuRectangle.y + menuRectangle.height / 7.f,

        menuRectangle.width / 4.5f,
        menuRectangle.height / 1.5f,
    };

    if (!board.state.waitPromotion)
        return;

    DrawRectangleRounded(menuRectangle, 0.2f, 0, board.squareBlackColor);
    DrawRectangleRoundedLines(menuRectangle, 0.2f, 0, 3, board.squareWhiteColor);

    for (u8 i = 1; i <= 4; i++) {
        if (CheckCollisionPointRec(GetMousePosition(), spritePosition))
            DrawRectangleRounded(spritePosition, 0.2f, 0, RED);

        DrawTexturePro(board.pieceSpriteSheet, spriteRectangle, spritePosition, { 0,0 }, 0, WHITE);

        spritePosition.x += spritePosition.width;
        spriteRectangle.x += spriteRectangle.width;
    }
}




