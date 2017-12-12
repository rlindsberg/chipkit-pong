#ifndef DISPLAY_H_   /* Include guard */
#define DISPLAY_H_

#include <stdint.h>
#include "types.h"

void delay(int cyc);
uint8_t spi_send_recv(uint8_t data);
void spi_init();
void display_wakeup();
void lightUpPixel(int x, int y);
void drawPaddle(Paddle p);
void drawBall(Ball b);

void drawLetterO(Letter myletter);
void drawLetterI(Letter myletter);
void letterSpeedy(Letter myletter, Letter myOtherLetter);

void clearGame();
void saveGame();

void drawScore(Paddle p1, Paddle p2);
void draw(Letter myletterO, Letter myletterI);

#endif // DISPLAY_H_