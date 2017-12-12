#ifndef DISPLAY_H_   /* Include guard */
#define DISPLAY_H_

#include <stdint.h>
#include "types.h"

void delay(int cyc);
uint8_t spi_send_recv(uint8_t data);
void spi_init();
void display_wakeup();
void lightUpPixel(int x, int y);


void drawLetterO(Letter myletter);
void drawLetterI(Letter myletter);
Letter dropFastly(Letter myletter, Letter myOtherLetter);

void clearGame();
void saveGame();
int  isGameOver(Letter myletter, Letter myOtherLetter);

void draw(Letter myletterO, Letter myletterI);

#endif // DISPLAY_H_