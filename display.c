#include "display.h"
#include <stdbool.h>
#include <stdint.h>
#include <pic32mx.h>
#include "assets.h"
#include "types.h"

#define DISPLAY_VDD PORTFbits.RF6
#define DISPLAY_VBATT PORTFbits.RF5
#define DISPLAY_COMMAND_DATA PORTFbits.RF4
#define DISPLAY_RESET PORTGbits.RG9

#define DISPLAY_VDD_PORT PORTF
#define DISPLAY_VDD_MASK 0x40
#define DISPLAY_VBATT_PORT PORTF
#define DISPLAY_VBATT_MASK 0x20
#define DISPLAY_COMMAND_DATA_PORT PORTF
#define DISPLAY_COMMAND_DATA_MASK 0x10
#define DISPLAY_RESET_PORT PORTG
#define DISPLAY_RESET_MASK 0x200


uint8_t game[128*4] = {0};
uint8_t savedGame[128*4] = {0};


/*
 * Simple delay
 */
void delay(int cyc) {
    int i;
    for(i = cyc; i > 0; i--);
}

/*
 * SPI send/receive
 */
uint8_t spi_send_recv(uint8_t data) {
    while(!(SPI2STAT & 0x08));
    SPI2BUF = data;
    while(!(SPI2STAT & 0x01));
    return SPI2BUF;
}

/*
 * Enable SPI
 */
void spi_init() {
    /* Set up peripheral bus clock */
    OSCCON &= ~0x180000;
    OSCCON |= 0x080000;
    
    /* Set up output pins */
    AD1PCFG = 0xFFFF;
    ODCE = 0x0;
    TRISECLR = 0xFF;
    PORTE = 0x0;
    
    /* Output pins for display signals */
    PORTF = 0xFFFF;
    PORTG = (1 << 9);
    ODCF = 0x0;
    ODCG = 0x0;
    TRISFCLR = 0x70;
    TRISGCLR = 0x200;
    
    /* Set up input pins */
    TRISDSET = (1 << 8);
    TRISFSET = (1 << 1);
    
    /* Set up SPI as master */
    SPI2CON = 0;
    SPI2BRG = 4;
    
    /* Clear SPIROV*/
    SPI2STATCLR &= ~0x40;
    /* Set CKP = 1, MSTEN = 1; */
        SPI2CON |= 0x60;
    
    /* Turn on SPI */
    SPI2CONSET = 0x8000;
}

/*
 * Display startup procedure
 */
void display_wakeup() {
    DISPLAY_COMMAND_DATA_PORT &= ~DISPLAY_COMMAND_DATA_MASK;
    delay(10);
    DISPLAY_VDD_PORT &= ~DISPLAY_VDD_MASK;
    delay(1000000);
    
    spi_send_recv(0xAE);
    DISPLAY_RESET_PORT &= ~DISPLAY_RESET_MASK;
    delay(10);
    DISPLAY_RESET_PORT |= DISPLAY_RESET_MASK;
    delay(10);
    
    spi_send_recv(0x8D);
    spi_send_recv(0x14);
    
    spi_send_recv(0xD9);
    spi_send_recv(0xF1);
    
    DISPLAY_VBATT_PORT &= ~DISPLAY_VBATT_MASK;
    delay(10000000);
    
    spi_send_recv(0xA1);
    spi_send_recv(0xC8);
    
    spi_send_recv(0xDA);
    spi_send_recv(0x20);
    
    spi_send_recv(0xAF);
}

/*
 * Draw single pixel on display by
 * converting coordinates to SPI
 * compatible format.
 */
void lightUpPixel(int x, int y) {
    short offset = 0;
    if (y > 0) { offset = y / 8; }
    game[offset * 128 + x] |= 1 << (y - offset * 8);
}


void drawLetterO(Letter l){

    //load game to screen
    int i;
    for (i = 0; i < sizeof(game); i++) { game[i] = savedGame[i]; }
    PORTE = 0x4; //0100
    delay(500000);


    for (i = 0; i < l.width; i++)
    {
    lightUpPixel(l.x+i, l.y);
    lightUpPixel(l.x+i, l.y+1);
    lightUpPixel(l.x+i, l.y+2);
    lightUpPixel(l.x+i, l.y+3);
    lightUpPixel(l.x+i, l.y+4);
    lightUpPixel(l.x+i, l.y+5);
    lightUpPixel(l.x+i, l.y+6);
    lightUpPixel(l.x+i, l.y+7);
    }
    PORTE = 0x05;
    delay(500000);
}

void drawLetterI(Letter l){
    int i;
    for (i = 0; i < sizeof(game); i++) { game[i] = savedGame[i]; }
    PORTE = 0x4; //0100
    delay(500000);

    for (i = 0; i < l.width; i++)
    {
    lightUpPixel(l.x, l.y+i);
    lightUpPixel(l.x+1, l.y+i);
    lightUpPixel(l.x+2, l.y+i);
    lightUpPixel(l.x+3, l.y+i);
    lightUpPixel(l.x+4, l.y+i);
    lightUpPixel(l.x+5, l.y+i);
    lightUpPixel(l.x+6, l.y+i);
    lightUpPixel(l.x+7, l.y+i);
    }

    PORTE = 0x05;
    delay(500000);
}

/*
 * Reset display
 */
void clearGame() {
    int i;
    for (i = 0; i < sizeof(game); i++) { game[i] = 0; }
}

void saveGame(){
    int i;
    for (i = 0; i < sizeof(game); i++) { savedGame[i] = game[i]; }

    PORTE = 0xA; //1010
}

// /*
//  * Print current score
//  */
// void drawScore(Paddle p1, Paddle p2) {
//     int i, x = 52;
//     p1.score = p1.score % 128;
//     p2.score = p2.score % 128;
//     char c = p1.score + '0';

//     for (i = 0; i < 8; i++) {
//         game[x + i] = font[c * 8 + i];
//     }

//     x = 60;
//     c = ':';
//     for (i = 0; i < 8; i++) {
//         game[x + i] = font[c * 8 + i];
//     }

//     x = 68;
//     c = p2.score + '0';
//     for (i = 0; i < 8; i++) {
//         game[x + i] = font[c * 8 + i];
//     }
// }

/*
 * Print high score
 */
// void drawHighScore() {
//     int winner = p1.score > p2.score ? 1 : 2;
//     int i, c, x = 64;
//     char sequence[] = {'P', 'l', 'a', 'y', 'e', 'r', ' ', winner + '0', '!'};
//     int offset = 3 * 128;

//     for (c = 0; c < sizeof(sequence); c++) {
//         for (i = 0; i < 8; i++) {
//             game[offset + x + c * 8 + i] = font[sequence[c] * 8 + i];
//         }
//     }
// }


/**
 * Renders the full screen
 */
void renderScreen(uint8_t arr[]) {
    int i, j;
    
    for(i = 0; i < 4; i++) {
        DISPLAY_COMMAND_DATA_PORT &= ~DISPLAY_COMMAND_DATA_MASK;
        spi_send_recv(0x22);
        spi_send_recv(i);

        spi_send_recv(0 & 0xF);
        spi_send_recv(0x10 | ((0 >> 4) & 0xF));

        DISPLAY_COMMAND_DATA_PORT |= DISPLAY_COMMAND_DATA_MASK;

        for(j = 0; j < 128; j++)
            spi_send_recv(arr[i*128 + j]);
    }
}

/*
 * Send the next frame to the display
 */
void draw(Letter myletterO, Letter myletterI) {
    int i, j;

    clearGame();
    PORTE = 0x3;
    delay(500000);

    if (myletterO.enabled)
    {
        drawLetterO(myletterO);
    } else if (myletterI.enabled)
    {
        drawLetterI(myletterI);
    }

    renderScreen(game);
    PORTE = 0x6;
    delay(500000);
}

bool isBottomYet(Letter myletter){
    int pageNumber = myletter.y / 8;
    if (myletter.x == 0 | game[pageNumber * 128 + myletter.x - 1] > 0)
    {
        return 1;
    }

    pageNumber = pageNumber + myletter.width / 8 - 1;
    if (myletter.x == 0 | game[pageNumber * 128 + myletter.x - 1] > 0)
    {
        // clearScreenRow();
        return 1;
    } else return 0;
}


int checkPage(int col)
{
    int i;
    for (i = 0; i < 4; i++) //check pages
    {
        if (checkSegment(i, col) == 0)
        {
            return 0;

        }
    }
    return 1;
}

int checkSegment(int i, int col)
{
    int j;
    for(j = 0; j < 8; j++) //check segments
    {
        if (savedGame[i * 128 + col * 8 + j] != 0xFF) //not lighten
        {
            return 0;
        }
    }
    return 1;
}
void clearScreenRow()
{
    int i, j, col, columnToClear; // total 15 columns, except the highist one
    bool fullSegment = 0;
    bool fullPage = 0;
    bool fullColumn = 0;

    bool checkNewColumn = 0;
    for (col = 0; col < 15; col++) //check columns
    {
        if (checkPage(col) == 1)
        {
            fullColumn = 1;
            columnToClear = col;
            break;
        }
    }

    if (fullColumn)
    {
        fullColumn = 0;
        PORTE = 0xff;
        delay(8000000);
        //clear the column
        for (i = 0; i < 4; i++) //check pages
        {
            for(j = 0; j < 8; j++) //check segments
            {
                savedGame[i * 128 + columnToClear * 8 + j] = 0;
            }
        }

        //columns above should fall one column down
        for (i = 0; i < 4; i++)
        {
            for (j = columnToClear * 8; j < 120; j++)
            {
                savedGame[i * 128 + j] = savedGame[i * 128 + j + 8]; //move down 8 segments
            }
        }
    }

}


// /*
//  * Starting screen
//  */
// void drawLogo() {
//     renderScreen(logo);
// }

/*
 * Ending Screen
 */
// void drawEnding(Paddle p1, Paddle p2) {
//     clearGame();
//     int i;
//     for (i = 0; i < sizeof(game); i++) {
//         game[i] = minion[i];
//     }
    
//     drawWinnerInverted(p1, p2);
//     renderScreen(game);
// }


