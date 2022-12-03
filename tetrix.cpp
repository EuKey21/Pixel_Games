#include <iostream>
#include <thread>
#include <Windows.h>
#include <stdlib.h>
using namespace std;


const int nBlocks = 7;
wstring blocks[nBlocks]; // 4x4
int blockLength = 4;

int screenWidth = 120;
int screenHeight = 30;
int screenArea = screenWidth * screenHeight;

int fieldWidth = 12;
int fieldHeight = 18;
int fieldArea = fieldWidth * fieldHeight;
unsigned char* pField = nullptr;

int fieldBorderDistance = 5; // how far is the field away from the upper left
wstring fieldElement = L" +#"; // field elements
const int fieldSpace = 0;
const int fieldBlock = 1;
const int fieldBorder = 2;

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
int BlockRotate(int x, int y, int r);

bool BlockFitsInField(int block, int rotation, int posX, int posY);

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
            pField[y * fieldWidth + x] = (x == 0 || x == fieldWidth - 1 ||y == fieldHeight - 1) ? fieldBorder : fieldSpace;

    /************************************************************/
    /***********************Game Loop****************************/
    bool gameOver = false;

    int currentBlock = rand() % nBlocks;
    int currentRotation = 0;
    int currentX = fieldWidth / 2;
    int currentY = 0;

    const int keyNum = 4;
    bool key[keyNum];
    bool rotateHold = false;

    int speed = 20;
    int speedCounter = 0;
    bool forceDown = false;

    while (gameOver == false)
    {
        /**************Timing*****************/
        this_thread::sleep_for(50ms); // Game Tick
        speedCounter++;
        forceDown = (speedCounter == speed);
        /**************Input******************/
        for (int i = 0; i < keyNum; i++)
            key[i] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x26"[i]))) != 0;
        /**************Logic******************/
        // Right Arrow Key
        currentX += (key[0] && BlockFitsInField(currentBlock, currentRotation, currentX + 1, currentY)) ? 1 : 0;
        // Left Arrow Key
        currentX -= (key[1] && BlockFitsInField(currentBlock, currentRotation, currentX - 1, currentY)) ? 1 : 0;
        // Down Arrow Key
        currentY += (key[2] && BlockFitsInField(currentBlock, currentRotation, currentX, currentY + 1)) ? 1 : 0;
        // Up Arrow Key
        if(key[3])
        {
            currentRotation += (!rotateHold && BlockFitsInField(currentBlock, currentRotation + 1, currentX, currentY)) ? 1 : 0;
            rotateHold = true;
        }
        else
        {
            rotateHold = false;
        }

        if(forceDown)
        {
            if(BlockFitsInField(currentBlock, currentRotation, currentX, currentY + 1))
            {
                currentY++;
            }
            else
            {
                // lock the current blocks in the field
                for (int x = 0; x < blockLength; x++)
                    for (int y = 0; y < blockLength; y++)
                        if(blocks[currentBlock][BlockRotate(x, y, currentRotation)] != L'.')
                            pField[(currentY + y) * fieldWidth + (currentX + x)] = fieldBlock;

                // check if the row are filled
                for (int y = 0; y < blockLength; y++)
                {
                    if (currentY + y < fieldHeight - 1)
                    {
                        bool filled = true;
                        for(int x = 1; x < fieldWidth - 1; x++)
                        {
                            filled &= (pField[(currentY + y) * fieldWidth + x]) != 0;
                        }

                        if (filled)
                        {
                            // removed the row that is filled    
                            for(int x = 1; x < fieldWidth - 1; x++)
                            {
                                for (int y2 = currentY + y; y2 > 0; y2--)
						            pField[y2 * fieldWidth + x] = pField[(y2 - 1) * fieldWidth + x];
					            pField[x] = fieldSpace;
                            }
                        }
                    }
                }

                // Next blcok
                currentX = fieldWidth / 2;
                currentY = 0;
                currentRotation = 0;
                currentBlock = rand() % nBlocks;
                

                // Gameover - Blocks go out of field
                gameOver = !BlockFitsInField(currentBlock, currentRotation, currentX, currentY);
            }
            speedCounter = 0;
        }
        
        


        /**************Output*****************/

        /*********Draw field********/
        for (int x = 0; x < fieldWidth; x++)
            for (int y = 0; y < fieldHeight; y++)
                screen[(y + fieldBorder) * screenWidth + (x + fieldBorder)] = fieldElement[pField[y * fieldWidth + x]];

        /*****Draw Current Block****/
        for (int x = 0; x < blockLength; x++)
            for (int y = 0; y < blockLength; y++)
                if (blocks[currentBlock][BlockRotate(x, y, currentRotation)] != L'.')
                    screen[(currentY + y + fieldBorder) * screenWidth + (currentX + x + fieldBorder)] = fieldElement[fieldBlock];

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

int BlockRotate(int x, int y, int r)
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

bool BlockFitsInField(int block, int rotation, int posX, int posY)
{
    for (int x = 0; x < blockLength; x++)
        for (int y = 0; y < blockLength; y++)
        {
            int blockIndex = BlockRotate(x, y, rotation);
            int fieldIndex = (posY + y) * fieldWidth + (posX + x);

            // Check if the dropping block is within the field
            if (posX + x >= 0 && posX + x < fieldWidth)
                if (posY + y >= 0 && posY + y < fieldHeight)
                    if (blocks[block][blockIndex] != L'.' && pField[fieldIndex] != 0)
                        return false; // dropped on a block
        }
    return true;
}


