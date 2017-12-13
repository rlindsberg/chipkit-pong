#ifndef DISPLAY_H_   /* Include guard */
#define DISPLAY_H_

#include <stdint.h>
#include "types.h"

void delay(int cyc);
uint8_t spi_send_recv(uint8_t data);
void spi_init();
void display_wakeup();

/* Declare text buffer for display output */
extern char textbuffer[4][16];
void time2string( char *, int );
void display_string(int line, char *s);
void display_update(void);
// char* itoa(int i, char b[]);


void lightUpPixel(int x, int y);


void drawLetterO(Letter myletter);
void drawLetterI(Letter myletter);
Letter dropFastly(Letter myletter, Letter myOtherLetter);
int clearScreenRow(int gameSpeed);
void clearGame();
void saveGame();
int  isGameOver(Letter myletter, Letter myOtherLetter);

void draw(Letter myletterO, Letter myletterI);

#endif // DISPLAY_H_