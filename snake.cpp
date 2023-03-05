#include <iostream>
#include <Windows.h>
#include <list>
#include <thread>

using namespace std;

int screenWidth = 120;
int screenHeight = 30;
int screenArea = screenWidth * screenHeight;

struct Coordinate
{
    int xPos;
    int yPos;
    int pos(int width)
    {
        return yPos * width + xPos;
    }
};

enum Direction
{
    // clockwise
    north   =     0,
    east    =     1,
    south   =     2,
    west    =     3
};


struct Key
{
    bool left;
    bool right;
};


int main()
{
    /************************************************************/
    /******************Creating the Window***********************/
    wchar_t* screen = new wchar_t[screenArea];
    for (int i = 0; i < screenArea; i++)
        screen[i] = L' ';

    HANDLE hConsole = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );

    list<Coordinate> snake = {
        {56, 15},
        {57, 15},
        {58, 15},
        {59, 15},
        {60, 15}
    };
    Coordinate food = {45, 15};
    Direction snakeDirection = west;
    Key currKeyInput {false, false};
    Key prevKeyInput {false, false};
    int score = 0;

    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    /************************************************************/
    /***********************Game Loop****************************/
    bool gameOver = false;

    while(gameOver == false)
    {
    /**************Timing*****************/
        
        this_thread::sleep_for(300ms);

    /**************Input******************/


        currKeyInput.right = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;
        currKeyInput.left = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;

        if (currKeyInput.right && !prevKeyInput.right)
        {
            switch (snakeDirection)
            {
                case north:
                    snakeDirection = east;
                    break; 
                case east:
                    snakeDirection = south;
                    break;
                case south:
                    snakeDirection = west;
                    break;
                case west:
                    snakeDirection = north;
            }
        }

        if (currKeyInput.left && !prevKeyInput.left)
        {
            switch (snakeDirection)
            {
                case east:
                    snakeDirection = north;
                    break; 
                case south:
                    snakeDirection = east;
                    break;
                case west:
                    snakeDirection = south;
                    break;
                case north:
                    snakeDirection = west;
            }
        }

        prevKeyInput.right = currKeyInput.right;
        prevKeyInput.left = currKeyInput.left;

    /**************Logic******************/

        //Movement
        switch (snakeDirection)
        {
        case north: 
            snake.push_front( {snake.front().xPos, snake.front().yPos - 1} );
            break;
        case east: 
            snake.push_front( {snake.front().xPos + 1, snake.front().yPos} );
            break;
        case south: 
            snake.push_front( {snake.front().xPos, snake.front().yPos + 1} );
            break;
        case west: 
            snake.push_front( {snake.front().xPos - 1, snake.front().yPos} );
            break;
        }

        snake.pop_back();

        //Collision with the wall
        if (snake.front().xPos < 0 || snake.front().xPos >= screenWidth)
			gameOver = true;
		if (snake.front().yPos < 3 || snake.front().yPos >= screenHeight)
			gameOver = true;

        //Collision with the food
        if (snake.front().xPos == food.xPos && snake.front().yPos == food.yPos)
        {
            score += 10;
            while (screen[food.pos(screenWidth)] != L' ')
            {
                food.xPos = rand() % screenWidth;
                food.yPos = ( rand() % (screenHeight-3) ) + 3;
            }

            for (int i = 0; i < 5; i++)
                snake.push_back({ snake.back().xPos, snake.back().yPos });
        }

    /**************Dsiplay****************/

        //Clear the Screen
        for (int i = 0; i < screenArea; i++)
            screen[i] = L' ';

        //Draw border
        for (int i = 0; i < screenWidth; i++)
        {
            screen[i] = L'*';
            screen[2 * screenWidth + i] = L'*';
        }
        wsprintf(&screen[screenWidth + 55], L"SCORE: %d", score);

        //Draw snake body
        for (auto segment : snake)
            screen[segment.pos(screenWidth)] = gameOver ? L'x' : L'o';

        //Draw snake head
        screen[snake.front().pos(screenWidth)] = gameOver ? L'X' : L'O';

        //Draw food
        screen[food.pos(screenWidth)] = L'@';

        //Display frame
        WriteConsoleOutputCharacter(
            hConsole,
            screen,
            screenArea,
            { 0, 0 },
            &dwBytesWritten
        );
    }

    return 0;
}