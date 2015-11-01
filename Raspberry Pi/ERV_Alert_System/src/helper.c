/* helper.c
 * ENB345 Group Project
 *
 * Contains helper functions for project
 * Written by Declan Gilmour n8871566, Justin Kennedy, Lachlan Nicholson
 */

#include "../include/helper.h"

int char_to_int(char* charArray)
{
	int i;
	sscanf(charArray, "%d", &i);
	return i;
}

// char* int_to_char(int i)
// {
// 	char charArray[10];
// 	snprintf(charArray, 10, "%d", i);
// 	return charArray;
// }