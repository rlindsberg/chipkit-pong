#include <pic32mx.h>
#include "types.h"
#include "helpers.h"
#include "assets.h"
#include "display.h"
// #include "sound.h"
// #include "freqmap.h"

#define GAME_SPEED          800
// #define GAME_WIN_SCORE      3 //uless
// #define MAX_X               128 //uless
// #define MAX_Y               32 //uless
// #define PADDLE_HEIGHT       8 //uless
#define CONTROLLER_SPEED    40

#define STATE_START     0
#define STATE_PONG      1
#define STATE_END       2
#define STATE_MENU      3

// #define MENU_MULTI      1 //uless
// #define MENU_CPUBAS     0 //uless
// #define MENU_CPUADV     2 //uless

int gameState = STATE_PONG;
// int menuState = MENU_MULTI;

Letter letterO;
Letter letterI;

/*
 * One frame of the game
 */
void advance() {

    if (letterO.enabled == 1)
    {
        PORTE = 0x20;
        delay(500000);
    }

    if (letterI.enabled == 1)
    {
        // PORTE = 0xAA;
        delay(500000);
    }



    //letter O
    if (letterO.enabled == 1 && letterO.x >= 0)
    {
        letterO.x = (letterO.x - letterO.speedX);

        //if BTN2 pressed, move letter to right
        if (getbtns() & 1) // btnStatus is 0010. BTN2 is pressed
        {
            if (letterO.y + letterO.speedY <=24)
            {
                letterO.y = (letterO.y + letterO.speedY);
            }
        }

        //BTN3 pressed
        if (getbtns() & 2) //0100, BTN3 pressed
        {
            if (letterO.y - letterO.speedY >= 0)
            {
                letterO.y = (letterO.y - letterO.speedY);
            }
        }

        PORTE = 0x8;
        delay(500000);
        draw(letterO, letterI);

    //Letter I
    } else if (letterI.enabled == 1 && letterI.x >= 0)
    {
        letterI.x = (letterI.x - letterI.speedX);


        //if BTN2 pressed, move letter to right
        if (getbtns() & 1) // btnStatus is 0010. BTN2 is pressed
        {
            if (letterI.y + letterI.speedY <=16)
            {
                letterI.y = (letterI.y + letterI.speedY);
            }
        }

        //BTN3 pressed
        if (getbtns() & 2) //0100, BTN3 pressed
        {
            if (letterI.y - letterI.speedY >= 0)
            {
                letterI.y = (letterI.y - letterI.speedY);
            }
        }

        PORTE = 0x8; //1000
        delay(500000);
        draw(letterO, letterI);

    }

    // checks if the letter falls to the bottom
    if (letterO.enabled == 1 && isBottomYet(letterO))
    {
        PORTE = 0x9; //1001
        saveGame();
        clearScreenRow();

        letterO.x = 80;
        letterO.y =  16;
        letterO.enabled = 0;
        letterI.enabled = 1;

        PORTE = 0xB; //1011
    }
    if (letterI.enabled == 1 && isBottomYet(letterI))
    {
        PORTE = 0x9; //1001
        saveGame();
        clearScreenRow();

        letterI.x = 80;
        letterI.y =  16;
        letterI.enabled = 0;
        letterO.enabled = 1;

        PORTE = 0xB; //1011
    }

    //BTN4 pressed, speedy down
    if (getbtns() & 4) //0100, BTN4 pressed
    {
        if (letterO.enabled)
        {
            
        PORTE = 0xC; //1100
        delay(500000);

        draw(letterO, letterI);

        if (letterO.x - letterO.speedX >= 0)
        {
            PORTE = 0x0;
            delay(500000);
            letterO.x = (letterO.x - 24);
        }

        PORTE = 0xD; //1101
        delay(500000);

        draw(letterO, letterI);
    
        PORTE = 0xE;
    if (isBottomYet(letterO))
    {
        PORTE = 0x9; //1001
        delay(500000);
        saveGame();
        clearScreenRow();

        letterO.x = 80;
        letterO.y =  16;
        letterO.enabled = 0;
        letterI.enabled = 1;

        PORTE = 0xB; //1011
        delay(500000);
    }







        
        } else {



            PORTE = 0xC; //1100
            delay(500000);

            draw(letterI, letterO);

            if (letterI.x - letterI.speedX >= 0)
            {
                PORTE = 0x0;
                delay(500000);
                letterI.x = (letterI.x - 16);
            }

            PORTE = 0xD; //1101
            delay(500000);

            draw(letterI, letterO);
        
            PORTE = 0xE;
            if (isBottomYet(letterI))
            {
                PORTE = 0x9; //1001
                delay(500000);
                saveGame();
                clearScreenRow();

                letterI.x = 80;
                letterI.y =  16;
                letterI.enabled = 0;
                letterO.enabled = 1;

                PORTE = 0xB; //1011
                delay(500000);
             }


        }
    }


}

/*
 * Set up game configuration
 */
void init_game() {
    letterO.x = 8;
    letterO.y = 16;
    letterO.width = 8;
    letterO.speedX = 8;
    letterO.speedY = 8;
    letterO.enabled = 1;

    letterI.x = 80;
    letterI.y = 8;
    letterI.width = 24;
    letterI.speedX = 8;
    letterI.speedY = 8;
    letterI.enabled = 0;
}

int tuneCount = 1;
int tuneScale = 0;
// /*
//  * Plays a single note. Call repetitively for
//  * music sequences.
//  */
// void playTune(int tune[], int tempo, int toneVolume) {
//     tone(tune[tuneCount], toneVolume);
    
//     if (tune[tuneCount] == 0) {
//         tuneScale = tempo - 1;
//     }
    
//     tuneScale++;
//     if (tuneScale == tempo) {
//         tuneScale = 0;
//         tuneCount = (tuneCount + 1) % tune[0];
//     }
// }

/*
 * Initialise
 */
int main(void) {
    spi_init();
    display_wakeup();

    // drawLogo();
    init_game();

    // setup hardware
    enableButtons();
    enableTimer2(31250, 0x1B, 0x111, 1);

    enableMultiVectorMode();
    enable_interrupt();

    for(;;) ;
    return 0;
}

int counter = GAME_SPEED;

// /**
//  * Linear mapping from [0,1023] to valid paddle position
//  */
// int translateToScreen(int val) {
//     return val > 0 ? ((MAX_Y - PADDLE_HEIGHT) * val) / 1024 : 0;
// }



// /**
//  * Maps the master potentiometer position to
//  * to one of the three menu items
//  */
// void updateMenu() {
//     int ADCValueP1;

//     // start sampling and wait to complete
//     IFSCLR(1) = 0x0002;
//     AD1CON1SET = 0x0004;
//     while (!IFS(1) & 0x0002);
    
//     // check which buffer to read from
//     if (AD1CON2 & 0x0080) {
//         ADCValueP1 = ADC1BUF0;
//     } else {
//         ADCValueP1 = ADC1BUF8;
//     }

//     menuState = ((3 * ADCValueP1) / 1024);
// }

// int direction = 0;
// int targetCoord = 0;



int getbtns(void)
{
    //The buttons BTN4, BTN3, and BTN2, are connected to bits 7, 6 and 5 of Port D.
    // int btnStatus = PORTD >> 5;
    //mask all other bits ...0111 = 0x7
    return (PORTD >> 5) & 0x7;
}


/**
 * ISR Interrupt handler for timer 2
 */
void timer2_interrupt_handler(void) {
    IFSCLR(0) = 0x100;
    counter--;

    if (counter != 0) { return; }
    counter = GAME_SPEED;
    // updatePaddles();
    // updateVolume();

    switch (gameState) {
        // case STATE_MENU:
        //     updateMenu();
        //     renderMenu(menuState);
        //     playTune(FF7prelude, 2, volume);
        //     if (isButtonPressed(4)) {
        //         init_game();
        //         gameState = STATE_PONG;
        //         resetMusic();
        //         draw(p1, p2, ball);
        //     }
        //     break;



        case STATE_PONG:
            // // cpu player movement
            // if (menuState == MENU_CPUBAS || menuState == MENU_CPUADV) {
            //     updateCpuPlayer();
            // }

            // check for game abort
            // if (isButtonPressed(4)) {
            //     gameState = STATE_MENU;
            //     resetMusic();
            //     renderMenu(menuState);
            // }

            //draw new letter

            PORTE = 0x1;
            delay(500000);

            ////check letter status
            if (letterO.enabled == 1)
            {
                PORTE = 0x20; //0010 0000
                delay(500000);
            }
            if (letterI.enabled == 1)
            {
                PORTE = 0x10; //0001 0000
                delay(500000);
            }
            ////

            PORTE = 0x2;
            delay(500000);
            draw(letterO, letterI);


            ////check letter status
            if (letterO.enabled == 1)
            {
                PORTE = 0x20; //0010 0000
                delay(500000);
            }
            if (letterI.enabled == 1)
            {
                PORTE = 0x10; //0001 0000
                delay(500000);
            }
            ////

            PORTE = 0x7;
            delay(500000);
            advance();

            // letterI.enabled = 1;
            // draw(letterO, letterI);
            // advance();














            PORTE = 0xf;
            // game end?
            // if (p1.score >= GAME_WIN_SCORE || p2.score >= GAME_WIN_SCORE) {
            //     gameState = STATE_END;
            //     drawEnding(p1, p2);
            // }
            break;


        // case STATE_START:
        //     playTune(FF7prelude, 2, volume);
        //     if (isButtonPressed(4)) {
        //         gameState = STATE_MENU;
        //         renderMenu(menuState);
        //     }
        //     break;

        // case STATE_END:
        //     playTune(FF7fanfare, 2, volume);
        //     if (isButtonPressed(4)) {
        //         gameState = STATE_START;
        //         resetMusic();
        //         drawLogo();
        //     }
        //     break;
    }
}

/**
 * ISR Interrupt handler for timer 3
 */
void timer3_interrupt_handler(void) {}

/**
 * ISR general interrupt handler
 */
void core_interrupt_handler(void) {}
