#include <iostream>
#include <Windows.h>
using namespace std;



wstring blocks[7]; // 4x4
int blockLength = 4;

int screenWidth = 120;
int screenHeight = 30;
int screenArea = screenWidth * screenHeight;

int fieldWidth = 12;
int fieldHeight = 18;
int fieldArea = fieldWidth * fieldHeight;
unsigned char* pField = nullptr;

/****************************************************************
x -> x coordinates
y -> y coordiantes
r -> rotating direction

0 degree            90 degree       180 degree      270 degree
0   1   2   3       12 .... 0       15 .... 12      3 .... 15
4   5   6   7 ->    .       . ->    .       . ->    .       .
8   9  10  11       .       .       .       .       .       .
12 13  14  15       15 .... 3       3  .... 0       0 .... 12
****************************************************************/
int Rotate(int x, int y, int r);

bool BlockCollision(int block, int rotation, int posX, int posY);

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

    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    /************************************************************/
    /******************Creating the Blocks***********************/
    blocks[0].append(L"..X.");
    blocks[0].append(L"..X.");
    blocks[0].append(L"..X.");
    blocks[0].append(L"..X.");

    blocks[1].append(L"....");
    blocks[1].append(L".XX.");
    blocks[1].append(L"..X.");
    blocks[1].append(L"..X.");

    blocks[2].append(L"....");
    blocks[2].append(L".XX.");
    blocks[2].append(L".X..");
    blocks[2].append(L".X..");

    blocks[3].append(L"....");
    blocks[3].append(L".XX.");
    blocks[3].append(L".XX.");
    blocks[3].append(L"....");

    blocks[4].append(L"....");
    blocks[4].append(L"..XX");
    blocks[4].append(L".XX.");
    blocks[4].append(L"....");

    blocks[5].append(L"....");
    blocks[5].append(L"XX..");
    blocks[5].append(L".XX.");
    blocks[5].append(L"....");

    blocks[6].append(L"....");
    blocks[6].append(L".XXX");
    blocks[6].append(L"..X.");
    blocks[6].append(L"....");

    /************************************************************/
    /***************Creating the playing field*******************/
    pField = new unsigned char[fieldArea];
    for (int x = 0; x < fieldWidth; x++)
        for (int y = 0; y < fieldHeight; y++)
            if (x == 0 || x == fieldWidth - 1 || y == 0 || y == fieldHeight - 1)
                pField[y * fieldWidth + x] = 3; // #
            else
                pField[y * fieldWidth + x] = 0; // blank space

    /************************************************************/
    /***********************Game Loop****************************/
    bool gameOver = false;
    int fieldBorder = 5; // how far is the field away from the upper left
    wstring fieldSymbol = L" +-#"; // field elements

    int currentBlock = 0;
    int currentRotation = 0;
    int currentX = fieldWidth / 2;
    int currentY = 0;

    while (gameOver == false)
    {
        /**************Timing*****************/

        /**************Input******************/

        /**************Logic******************/

        /**************Output*****************/

        /*********Draw field********/
        for (int x = 0; x < fieldWidth; x++)
            for (int y = 0; y < fieldHeight; y++)
                screen[(y + fieldBorder) * screenWidth + (x + fieldBorder)] = fieldSymbol[pField[y * fieldWidth + x]];

        /*****Draw Current Block****/
        for (int x = 0; x < blockLength; x++)
            for (int y = 0; y < blockLength; y++)
                if (blocks[currentBlock][Rotate(x, y , currentBlock)] != L'.')
                    screen[(currentY + y + fieldBorder) * screenWidth + (currentX + x + 2)] = fieldSymbol[1];

        /******Display frame********/
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

int Rotate(int x, int y, int r)
{
    int posZero000 = 0;
    int posZero090 = 12;
    int posZero180 = 15;
    int posZero270 = 3;

    int index = 0;
    switch (r % blockLength)
    {
    case 0:     // 0 degree
        index = posZero000 + (y * blockLength) + x;
        break;
    case 1:     // 90 degrees
        index = posZero090 + y - (x * blockLength);
        break;
    case 2:     // 180 degree
        index = posZero180 - (y * blockLength) - x;
        break;
    case 3:     // 270 degree
        index = posZero270 - y + (x * blockLength);
    }

    return index;
}

bool BlockCollision(int block, int rotation, int posX, int posY)
{
    for (int x = 0; x < blockLength; x++)
        for (int y = 0; y < blockLength; y++)
        {
            int blockIndex = Rotate(x, y, rotation);
            int fieldIndex = (posY + y) * fieldWidth + (posX + x);

            // Check if the dropping block is within the field
            if (posX + x >= 0 && posX + x < fieldWidth)
                if(posY + y >= 0 && posY +y < fieldHeight)
                    if (blocks[block][blockIndex] != L'.' && pField[fieldIndex] != 0)
                        return false; // dropped on a block
        }
    return true;
}