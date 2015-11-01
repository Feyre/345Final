// first OpenVG program
// Anthony Starks (ajstarks@gmail.com)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"
#include <math.h>

int main() {
	int width, height;
	char s[3];
	
	init(&width, &height);                  // Graphics initialization
	
	Start(width, height);                   // Start the picture
	Background(0, 0, 0);                    // Black background
	
	Fill(44, 77, 232, 1);
	
	//Circle(x,y,r) 
	Circle(width/2, height/2, height/10);	// Circle in middle
	
	//Rect(x1,y1,x2,y2)
	Fill(44, 77, 232, 1);
	Rect(0,0,width,height/50);	// Bottom Boarder
	Rect(0,height-height/50,width,height);	// Top Boarder
	Rect(0,0,width/50,height);	// Left Boarder
	Rect(width-width/50,0,width,height);	// Left Boarder
	
	//Arrow
	int xArray[6];
	char character = width;
	
	//Text
	Fill(201,29,24,1);
	TextMid(width / 2, height / 5, "ERV Alert System", SerifTypeface, width / 30);
	
	//int integer = 5;
	//int numDigits = floor(log10(abs(integer))) + 1; 

	
    // Fill(44, 77, 232, 1);                   // Big blue marble
    // Circle(width / 2, 0, width);            // The "world"
    // Fill(255, 255, 255, 1);                 // White text
    // TextMid(width / 2, height / 2, "hello, world", SerifTypeface, width / 10);  // Greetings 
	End();                                  // End the picture
	
	fgets(s, 2, stdin);                     // look at the pic, end with [RETURN]
	finish();                               // Graphics cleanup
	exit(0);
}