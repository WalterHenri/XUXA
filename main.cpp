#include <cctype>
#include <cstring>

#include "raylib.h"
#include "Board.hpp"

#include <cstdio>

#define SCREEN_TITLE  "Xuxa"


#ifndef BOARD_PERFT


int main(void)
{

    int menuorboard = 0;
    int screenWidth, screenHeight;

    Board board;

    InitWindow(0, 0, SCREEN_TITLE);
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();




    /* Set the initial mouse position one pixel at bottom left of
     * the center of the window. This prevent some bugs when start
     * drawing the square that's the mouse is in.
     */
    SetMousePosition(screenWidth / 2 + 1, screenHeight / 2 + 1);

    board = BoardInit(screenWidth, screenHeight);
    menuInit();
    BoardLoadFEN(board, "2r3k1/5ppp/8/8/2Q5/8/5PPP/4R1K1 w - - 0 1");


    while (!WindowShouldClose()) {

        if(menuorboard){
               /* Update section:
             *   update the game logic, doesn't use any draw function here
             */
            BoardUpdate(board);

            /* Draw section:
             *  this is the correct place to use draw functions, but this time logic
             *  update should be avoided
             */
            BeginDrawing();
            ClearBackground(BLACK);

            DrawFPS(0, 0);
            BoardDraw(board);

            EndDrawing();
        }else{
            menu(menuorboard);
        }
    }

    BoardUnload(board);
    //  Unloads dos arquivos que foram feitos uploads


    CloseWindow();

    fclose(stdout);

    return 0;
}
#else
#include <iostream>

int main(void)
{
    u64 nodes;
    u16 depth;

    Board board = BoardInit(0, 0);
    BoardLoadFEN(board, BOARD_FEN_DEFAULT);

    std::cout << "Enter the test depth: ";
    std::cin >> depth;

    nodes = BoardPerft(board, depth);

    std::cout << "Nodes: " << nodes << std::endl;
    return 0;
}

#endif /* BOARD_PERFT */
