#ifndef SAXA_HPP_INCLUDED
#define SAXA_HPP_INCLUDED

/**            SAXA

 Sistema Artificial de Xadrez Acad�mico.
 The greatest chess engine of all time,

 XUXA built in,

 Prog 2 is coming hard on the way,

          UFS > HARVARD

 **/


#define blacK 0
#define whitE 1

/* a bot that knows how to play better than you.*/

#include <cctype>
#include <iostream>
#include <cstring>
#include <math.h>
#include "Board.hpp"
#include "Piece.hpp"

#define IMESUARABLE 1
#define WORST_THING_POSSIBLE 0
#define BEST_THING_POSSIBLE 1


/*global variables*/

int pawnValue = 1;
int horseValue = 3;
int bishopValue = 3;
int rookValue = 5;
int queenValue = 9;
int kingValue = IMESUARABLE;

float squareValue = 0.1;


typedef struct {
    double grade;
    int to;
    int from;
} saxa_move;


/* gives value to a instant position*/

double evaluatePosition(Board board);

/* returns the best move of a position based on depth*/

saxa_move finalMove(Board board, int depth);

/* test values for position*/

double testMove(Board board, int to, int from);

/* this function returns a value between zero and one*/

double sigmoid(double grade) {
    return 1/(1+ pow(2.718281828,- grade));
}


saxa_move finalMove(Board board, int depth) {

    saxa_move temp = {0,-1,-1};

    int movecounter = 0;

    if(depth >  1){
        for(int i = 0; i < 64; i++) {
            for(int j = 0; j < 64; j++) {

                if(board.move.list[i][j] == true) {
                    movecounter++;
                    double auxGrade = finalMove(board,depth - 1).grade;
                    printf("%f",auxGrade);

                    if(board.state.whoMoves == PIECE_BLACK){
                        if(auxGrade > temp.grade || movecounter == 1) {
                            temp.grade = auxGrade;
                            temp.from = i;
                            temp.to = j;
                        }
                    }else{
                        if(auxGrade < temp.grade || movecounter == 1) {
                            temp.grade = auxGrade;
                            temp.from = i;
                            temp.to = j;
                        }

                    }
                }
            }
        }

        return temp;

    }else {
        for(int i = 0; i < 64; i++) {
            for(int j = 0; j < 64; j++) {

                if(board.move.list[i][j] == true) {
                    movecounter++;
                    double auxGrade = testMove(board,i,j);

                    if(auxGrade > temp.grade || movecounter == 1) {
                        temp.grade = auxGrade;
                        temp.from = i;
                        temp.to = j;
                    }

                }
            }
        }

        return temp;
    }
}


double testMove(Board board,int from,int to) {
    BoardMakeMove(board, from, to,true);

    if(BoardKingInMate(board,PIECE_WHITE)){
        return BEST_THING_POSSIBLE;
    }

    return evaluatePosition(board);
}


double evaluatePosition(Board board) {

    double grade = 0;
    int sinal;
    //contar material
    for(int i = 0; i < 8; i ++) {
        for(int j = 0; j < 8; j ++) {

            int pieceType  = PieceGetType(board.squares[i*8 + j]);
            int pieceColor = PieceGetColor(board.squares[i*8 + j]);
            sinal = (pieceColor == PIECE_BLACK) ? 1: -1;


            switch (pieceType) {

            case PIECE_BISHOP:
                grade += bishopValue*sinal;
                break;
            case PIECE_KNIGHT:
                grade += horseValue*sinal;
                break;
            case PIECE_PAWN:
                grade += pawnValue*sinal;
                break;
            case PIECE_QUEEN:
                grade += queenValue*sinal;
                break;
            case PIECE_ROOK:
                grade += rookValue*sinal;
            default:
                break;

            }
        }
    }

    return sigmoid(grade);
}


#endif // SAXA_HPP_INCLUDED
