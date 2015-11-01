/* usbSerialRx.c
 * ENB345 Group Project
 *
 * Description
 * Written by Declan Gilmour n8871566, Justin Kennedy, Lachlan Nicholson
 */

//compile with the command: gcc demo_rx.c rs232.c -Wall -Wextra -o2 -o test_rx


// #include <stdlib.h>
// #include <stdio.h>

// #ifdef _WIN32
// #include <Windows.h>
// #else
// #include <unistd.h>
// #endif

// #include "../include/rs232.h"


// int char_to_int(char* charArray);
#include "../include/usbSerialRx.h"
#include "../include/helper.h"

int serialTest(void)
{  
  int ii,n,
      cport_nr=24,        /* /dev/ttyACM0 */
      bdrate=9600;       /* 9600 baud */

  unsigned char buf[4096];

  char mode[]={'8','N','1',0};

  int buffSize = 12;
  int integerBuffer[buffSize];
  int count = 0;



  if(RS232_OpenComport(cport_nr, bdrate, mode))
  {
    printf("Can not open comport\n");

    return(0);
  }


  while(count < buffSize)
  {
    n = RS232_PollComport(cport_nr, buf, 4095);

    if(n > 0)
    {
      buf[n] = 0;   /* always put a "null" at the end of a string! */  
        
      integerBuffer[count] = char_to_int((char *)buf);
        
      printf((char *)buf);      
      count++;
    }

#ifdef _WIN32
    Sleep(100);
#else
    usleep(100000);  /* sleep for 100 milliSeconds */
#endif
  }

  printf("\n\n");
  for (ii = 0; ii < buffSize; ii++)
  {
    printf("%d\n",integerBuffer[ii]);
  }

  return(0);
}
