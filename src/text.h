#ifndef INCLUDE_TEXT
//Returns the number of digits in a number
int getDigits(int num);
//Draws a number with OpenGL
/*
 * Notes:
 * Assumes that icons.png is the loaded texture
 * and that the numbers are in the second row of the texture
 * in the first 10 slots
 *
 * */
void drawNumber(float x, float y, float scale, int value);

#define INCLUDE_TEXT
#endif
