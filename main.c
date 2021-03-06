#include <pic32mx.h>
#include "types.h"
#include "helpers.h"
#include "assets.h"
#include "display.h"

#define CONTROLLER_SPEED    40

#define STATE_GAMESTART    0
#define STATE_GAMEOVER      1

int gameState = STATE_GAMESTART;
int gameSpeed = 650;
int delayMain = 100000;
int score = 0;



Letter letterO;
Letter letterI;

/*
 * One frame of the game
 */
void inGame() {

    if (letterO.enabled == 1)
    {
        PORTE = 0x20;
        delay(delayMain);
    }

    if (letterI.enabled == 1)
    {
        // PORTE = 0xAA;
        delay(delayMain);
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
        delay(delayMain);
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
        delay(delayMain);
        draw(letterO, letterI);

    }

    // checks if the letter falls to the bottom
    if (letterO.enabled == 1 && isBottomYet(letterO))
    {
        PORTE = 0x9; //1001
        saveGame();
        gameSpeed = clearScreenRow(gameSpeed);
        //score!
        score += 100;
        //change game speed
        if (delayMain >= 0)
        {
            delayMain = delayMain - 1000000;
        }

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
        gameSpeed = clearScreenRow(gameSpeed);
        //score!
        score += 100;
        //change game speed
        if (delayMain >= 0)
        {
            delayMain = delayMain - 1000000;
        }

        letterI.x = 80;
        letterI.y =  16;
        letterI.enabled = 0;
        letterO.enabled = 1;

        PORTE = 0xB; //1011
    }

        PORTE = 0xA;
        delay(delayMain);
        draw(letterO, letterI);
        delay(delayMain);

    //BTN4 pressed, speedy down
    if (getbtns() & 4) //0100, BTN4 pressed
    {
        
        delay(delayMain);
        draw(letterO, letterI);        
        if (letterO.enabled)
        {
            letterO = dropFastly(letterO, letterI);
            draw(letterO, letterI);
            PORTE = 0xAA;
            delay(delayMain);

        } else if (letterI.enabled)
        {
            letterI = dropFastly(letterI, letterO);
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
    letterI.width = 16;
    letterI.speedX = 8;
    letterI.speedY = 8;
    letterI.enabled = 0;
}

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

int counter = 800;


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
    counter = gameSpeed;
   
    switch (gameState) {

        case STATE_GAMESTART:
            //draw new letter

            PORTE = 0x1;
            delay(delayMain);

            ////check letter status
            if (letterO.enabled == 1)
            {
                PORTE = 0x20; //0010 0000
                delay(delayMain);
            }
            if (letterI.enabled == 1)
            {
                PORTE = 0x10; //0001 0000
                delay(delayMain);
            }
            ////end

            PORTE = 0x2;
            delay(delayMain);
            draw(letterO, letterI);


            ////check letter status
            if (letterO.enabled == 1)
            {
                PORTE = 0x20; //0010 0000
                delay(delayMain);
            }
            if (letterI.enabled == 1)
            {
                PORTE = 0x10; //0001 0000
                delay(delayMain);
            }
            ////end

            PORTE = 0x7;
            delay(delayMain);
            inGame();


            PORTE = 0xf;


            if (isGameOver(letterO, letterI) == 1) {
                gameState = STATE_GAMEOVER;
                PORTE = 0;
            }
            break;


        case STATE_GAMEOVER:
            clearGame();
            renderScreen();
            display_string( 2, "HIGH SCORE!");
            display_update();
            PORTE = 0xAA;
            delay(200000);
            PORTE = 0x55; 
            delay(200000);
            PORTE = 0;
            break;
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
