#include <pic32mx.h>
#include "types.h"
#include "helpers.h"
#include "assets.h"
#include "display.h"
#include "sound.h"
#include "freqmap.h"

#define GAME_SPEED          800
#define GAME_WIN_SCORE      3
#define MAX_X               128
#define MAX_Y               32
#define PADDLE_HEIGHT       8
#define CONTROLLER_SPEED    40

#define STATE_START     0
#define STATE_PONG      1
#define STATE_END       2
#define STATE_MENU      3

#define MENU_MULTI      1
#define MENU_CPUBAS     0
#define MENU_CPUADV     2

int gameState = STATE_PONG;
int menuState = MENU_MULTI;

Letter letterO;
Letter letterI;

/*
 * One frame of the game
 */
void advance() {

    //letter is moving
    if (letterO.x >= 0)
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


        draw(letterO);

    } else return;

    if (isBottomYet(letterO))
    {
        saveGame();
        PORTE = 0xff;
        letterO.x = 80;
        letterO.y =  16;
        draw(letterO);
        delayAssembly(3000);
    }

    // ball.x = (ball.x + ball.speedX);
    // ball.y = (ball.y + ball.speedY);

    // // vertical collision detection
    // if (ball.y <= 0) {
    //     ball.y = 0;
    //     ball.speedY *= (-1);
    // }else if (ball.y >= MAX_Y - 1) {
    //     ball.y = MAX_Y - 1;
    //     ball.speedY *= (-1);
    // }

    // // horizontal collision detection
    // if (ball.x <= 0) {
    //     // score for p1?
    //     if (ball.y < p1.y || ball.y > p1.y + PADDLE_HEIGHT - 1) { p2.score += 1; }

    //     ball.x = 0;
    //     ball.speedX *= (-1);
    // }else if (ball.x >= MAX_X - 1) {
    //     // score for p1?
    //     if (ball.y < p2.y || ball.y > p2.y + PADDLE_HEIGHT - 1) { p1.score += 1; }

    //     ball.x = MAX_X - 1;
    //     ball.speedX *= (-1);
    // }


}

/*
 * Set up game configuration
 */
void init_game() {
    letterO.x = 80;
    letterO.y = 16;
    letterO.speedX = 8;
    letterO.speedY = 8;    
}

int tuneCount = 1;
int tuneScale = 0;
/*
 * Plays a single note. Call repetitively for
 * music sequences.
 */
void playTune(int tune[], int tempo, int toneVolume) {
    tone(tune[tuneCount], toneVolume);
    
    if (tune[tuneCount] == 0) {
        tuneScale = tempo - 1;
    }
    
    tuneScale++;
    if (tuneScale == tempo) {
        tuneScale = 0;
        tuneCount = (tuneCount + 1) % tune[0];
    }
}

/*
 * Initialise
 */
int main(void) {
    spi_init();
    display_wakeup();

    drawLogo();
    init_game();

    // setup hardware
    enableButtons();
    enableTimer2(31250, 0x1B, 0x111, 1);
    enableTimer3PWM();
    setupPotentiometers();

    enableMultiVectorMode();
    enable_interrupt();

    for(;;) ;
    return 0;
}

int counter = GAME_SPEED;

/**
 * Linear mapping from [0,1023] to valid paddle position
 */
int translateToScreen(int val) {
    return val > 0 ? ((MAX_Y - PADDLE_HEIGHT) * val) / 1024 : 0;
}

// void updatePaddles() {
//     int ADCValueP1, ADCValueP2;

//     // start sampling and wait to complete
//     IFSCLR(1) = 0x0002;
//     AD1CON1SET = 0x0004;
//     while (!IFS(1) & 0x0002);
    
//     // check which buffer to read from
//     if (AD1CON2 & 0x0080) {
//         ADCValueP1 = ADC1BUF0;
//         ADCValueP2 = ADC1BUF1;
//     } else {
//         ADCValueP1 = ADC1BUF8;
//         ADCValueP2 = ADC1BUF9;
//     }

//     p1.y = translateToScreen(ADCValueP1);
//     if (menuState == MENU_MULTI) {
//         p2.y = translateToScreen(ADCValueP2);
//     }
// }

/**
 * Increases (BTN3) or decreases (BTN2) music volume
 */
// void updateVolume() {
//     if (isButtonPressed(3) && (volume + 100 <= 800)) {
//         volume += 100;
//     } else if (isButtonPressed(2) && (volume - 100 >= 2)) {
//         volume -= 100;
//     }
// }

// /**
//  * Resets music 
//  */
// void resetMusic() {
//     tuneCount = 1;
//     mute();
// }

/**
 * Maps the master potentiometer position to
 * to one of the three menu items
 */
void updateMenu() {
    int ADCValueP1;

    // start sampling and wait to complete
    IFSCLR(1) = 0x0002;
    AD1CON1SET = 0x0004;
    while (!IFS(1) & 0x0002);
    
    // check which buffer to read from
    if (AD1CON2 & 0x0080) {
        ADCValueP1 = ADC1BUF0;
    } else {
        ADCValueP1 = ADC1BUF8;
    }

    menuState = ((3 * ADCValueP1) / 1024);
}

int direction = 0;
int targetCoord = 0;
/**
 *  Generate a CPU player at two levels: basic and advanced (always correct)
 */
// void updateCpuPlayer() {
//     int offset, side, estimate, max;
//     max = MAX_Y - PADDLE_HEIGHT;
//     switch(menuState) {
//         case MENU_CPUBAS:
//             if (ball.y == 0) {
//                 direction = 1;
//             } else if (ball.y == MAX_Y - 1) {
//                 direction = -1;
//             }

//             if (direction == 1 && p2.y < 23 && ball.x > 100) {
//                 p2.y++;
//             } else if (direction == -1 && p2.y > 0 && ball.x > 100) {
//                 p2.y--;
//             }
//             break;

//         case MENU_CPUADV:
//             if (ball.x == 63 && ball.speedX > 0) {
//                 targetCoord = MAX_Y - 1 - ball.y;
//             } else if (ball.x == 64 && ball.speedX > 0) {
//                 targetCoord = MAX_Y - 1 - (ball.y - (direction));
//             }
            
//             if (p2.y < 23 && p2.y + 4 < targetCoord) {
//                 p2.y++;
//             } else if (p2.y > 0 && p2.y + 4 > targetCoord) {
//                 p2.y--;
//             }
//             break;
//         default:
//             // predict y position
//             offset = ball.y - (ball.speedX / ball.speedY) * ball.x;
//             side = ball.speedX > 0 ? MAX_X : 0;
//             estimate = (ball.speedX / ball.speedY) * side + offset;

//             // move paddle
//             if (estimate > ball.y && p2.y < max) {
//                 p2.y++;
//             }else if (estimate < ball.y && p2.y > 0) {
//                 p2.y--;
//             }
//     }
// }


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

            draw(letterO);

            advance();

            PORTE ++;














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
