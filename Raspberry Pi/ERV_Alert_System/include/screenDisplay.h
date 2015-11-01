/* screenDisplay.h
 * ENB345 Group Project
 *
 * Description
 * Written by Declan Gilmour n8871566, Justin Kennedy, Lachlan Nicholson
 */

#ifndef _ScreenDisplay_H_
#define _ScreenDisplay_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"
#include <math.h>
 
int drawScreen();
void drawLine(int width, int height);
double getAngle(int width, int height);

void coordpoint(VGfloat x, VGfloat y, VGfloat size, VGfloat pcolor[4]);



#endif