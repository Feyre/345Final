/* usbSerialRx.h
 * ENB345 Group Project
 *
 * Description
 * Written by Declan Gilmour n8871566, Justin Kennedy, Lachlan Nicholson
 */

#ifndef _UsbSerialRx_H_
#define _UsbSerialRx_H_

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include "../include/rs232.h"

int serialTest(void);
// int char_to_int(char* charArray); 


#endif