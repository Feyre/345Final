/* screenDisplay.c
 * ENB345 Group Project
 *
 * Description
 * Written by Declan Gilmour n8871566, Justin Kennedy, Lachlan Nicholson
 */

#include "../include/screenDisplay.h"

int drawScreen() {
	int width, height;
	char s[3];
	
	init(&width, &height);                  // Graphics initialization
	width = height;
	Start(width, height);                   // Start the picture
	Background(255, 255, 255);                    // Black background
	
	Fill(44, 77, 232, 1);
	
	//Circle(x,y,r) 
	Circle(width/2, height/2, height/10);	// Circle in middle
	
	//Rect(x1,y1,x2,y2)
	Fill(44, 77, 232, 1);
	Rect(0,0,width,height/50);	// Bottom Boarder
	Rect(0,height-height/50,width,height);	// Top Boarder
	Rect(0,0,width/50,height);	// Left Boarder
	Rect(width-width/50,0,width,height);	// Right Boarder

	drawLine(width, height);

	End();                                  // End the picture
	
	fgets(s, 2, stdin);                     // look at the pic, end with [RETURN]
	finish();                               // Graphics cleanup
	exit(0);
}

void drawLine(int width, int height) {
	VGfloat shapecolor[4];
	RGB(202, 225, 255, shapecolor);
	
	VGfloat dotsize = 20;	
	setfill(shapecolor);

	double angle = getAngle(width, height);

	double hyp1 = height/1.3;	// inner radius
	double hyp2 = height/2;	// outer radius

	VGfloat x1, y1, x2, y2;
	x1 = hyp1 * cos(angle);
	y1 = hyp1 * sin(angle);
	x2 = hyp2 * cos(angle);
	y2 = hyp2 * sin(angle);


	StrokeWidth(5);
	Stroke(43, 43, 43, 1);
	Line(x1, y1, x2, y2);
	coordpoint(x2, y2, dotsize, shapecolor);

}

double getAngle(int width, int height)
{	
	double pi = 3.1415927;

	// Get angle and convert to radians
	double angle = 60 / 360.0 * 2 * pi;

	return angle;
}

void coordpoint(VGfloat x, VGfloat y, VGfloat size, VGfloat pcolor[4]) {
	Fill(128, 0, 0, 1);
	Circle(x, y, size);
	setfill(pcolor);
}