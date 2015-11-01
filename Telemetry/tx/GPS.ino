#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h> // microprocessor operations
#include <math.h>




void GPS_init(struct GPS_info_struct *GPS_data)
{
	UART_Init();
	// Initialise the UART protocol to the teensy

	DDRB = 0x0C;	// Same as 0b00001100 - declare Register B for use with LEDs
	PORTB = 0x00;	// Turn off all LEDs on Register B

	GPS_data->current_datetime.time_hour = 0, GPS_data->current_datetime.time_minute = 0, GPS_data->current_datetime.time_sec = 0;
	GPS_data->pos.lat_deg = 0, GPS_data->pos.lat_min = 0;
	GPS_data->pos.long_deg = 0, GPS_data->pos.long_min = 0;
	GPS_data->knot_spd = 0;
	GPS_data->current_datetime.date_day = 0, GPS_data->current_datetime.date_month = 0, GPS_data->current_datetime.date_year = 0;
	GPS_data->knot_spd = 0; // only reset speed
	GPS_data->kmh_spd = 0;
	GPS_data->cog_T = 0;
	GPS_data->cog_M = 0;
}

void GPS_print()
{
	// Print the UART output straight to the terminal screen
	char receive;

	while(1){
		// For all time
		receive = UART_Receive();

		if (receive > 0){
			// usb_serial_putchar(receive);
			// Send it back to the computer to print in the console
		}

	}
}


void GPS_parse(struct GPS_info_struct *GPS_data)
{
//	//declare variables
//	char receive, rec_G, rec_P;
//	char rec_1, rec_2, rec_3;
//	
//	int8_t RMC_check = -1;
//	int8_t VTG_check = -1;
//
//	while(1) // we want to continue to receive data until we are happy with what we have
//	{
////                Serial.println("LOOPING");
//		// switch LED on to indicate we are in loop
//		PORTB |= (1 << 2);
//
//		// continue to receive data until we are at the start of a NMEA sentence (with $)
//		receive = UART_Receive();
////                Serial.print(receive);
//		if((receive > 0) && (receive == '$'))
//		{
//			// The first two characters should be GP, if this is not true continue to cycle data
//			rec_G = UART_Receive();
//			rec_P = UART_Receive();
//			if((rec_G == 'G') && (rec_P == 'P'))
//			{
//				// Now receive the next three characters - this will tell us what data we are now receiving
//				rec_1 = UART_Receive();
//				rec_2 = UART_Receive();
//				rec_3 = UART_Receive();
//
//				if((rec_1 == 'R') && (rec_2 == 'M') && (rec_3 == 'C'))
//					RMC_check = GPS_RMC(GPS_data); // This contains recommended core minimum data
//				else if((rec_1 == 'V') && (rec_2 == 'T') && (rec_3 == 'G'))
//					VTG_check = GPS_VTG(GPS_data); // This contains speed and course information
//				// else if((rec_1 == 'G') && (rec_2 == 'G') && (rec_3 == 'A')) // This contains altitude as well as RMC information - not required for this project
//				// else if((rec_1 == 'G') && (rec_2 == 'S') && (rec_3 == 'A')) // This contains sattelite information - not required for project 
//				// else if((rec_1 == 'G') && (rec_2 == 'S') && (rec_3 == 'V')) // More sattelite information, again note required for project 
//				// else if((rec_1 == 'G') && (rec_2 == 'L') && (rec_3 == 'L')) // Repeat of long/lat/time - already aquired with RMC
//				// else // not recognised loop
//
//				if(RMC_check == 0 && VTG_check == 0) // we are happy with the data to send out
//				{
//					PORTB &= ~(1 << 2);
////                                        delay();
////                                        Serial.println("LEAVING");
////					UART_Flush_Data();
//					break;
//				}
//			}
//		}
//	}


  PORTB |= (1 << 2);
  int8_t RMC_check = -1;
  int8_t VTG_check = -1;

  if(Serial1.find("GPRMC")) {
    RMC_check = GPS_RMC(GPS_data);
  }
  if(Serial1.find("GPVTG")) {
    VTG_check = GPS_VTG(GPS_data);
  }

//  UART_Smart_Flush();

  PORTB &= ~(1 << 2);



}

int8_t GPS_RMC(struct GPS_info_struct *GPS_data)
{
	// RMC is the most useful NMEA sentence to us to get out basic information from the GPS unit
	
	// RMC Data - reset the struct values to 0
	int hour = 0, min = 0, sec = 0;
	int lat_deg = 0, long_deg = 0;
	float lat_min = 0, long_min = 0;
	int knot_spd = 0;
	int day = 0, month = 0, year = 0;
	int cog_T = 0;

	// initialise counters for use in the switch cases
	int time_counter = 0, lat_counter = 0, long_counter = 0, date_counter = 0, counter = 0;

	// declare other variables
	char receive;
	int checksum = 'G' ^ 'P' ^ 'R' ^ 'M' ^ 'C'; // start the checksum - if we are in this function then this is the data that has already been received
	uint8_t checksum_output;
	uint8_t cog_check = 0;

	receive = UART_Receive(); // get the first item

	while(receive != '*') // i.e. not end of statement
	{
		checksum = checksum ^ receive; // compute the checksum
		if(receive == ',')
			++counter; // increase the counter - go to the next part of the sentence
		else // otherwise we continue on dealing with things
		{
			switch(counter)
			{
				case  1 : // time
					// we should receive 6 items, a '.' then 2 more items
					++time_counter;
					switch(time_counter)
					{
						case 1 : // hour 10
							hour = (receive - '0') * 10;
							break;
						case 2 : // hour 0
							hour += receive - '0';
							break;
						case 3 : // minute 10
							min = (receive - '0') * 10;
							break;
						case 4 : // minutes 0
							min += receive - '0';
							break;
						case 5 : // second 10
							sec = (receive - '0') * 10;
							break;
						case 6 : // second 0
							sec += receive - '0';
							break;
						// We aren't actually interested in the decimal places of seconds, and it was noted in testing that these are generally 0
					}
					break;
				case  2 : // check
					if(receive == 'A') // data good
						GPS_data->check_1 = receive;
					// else // this 'should' be 'V' - indicates that the data is invalid - but we wont write this
					// {
					// 	GPS_data->check_1 = receive;
					// 	return -1;
					// }
					break;
				case  3 : // Latitude
					++lat_counter;
					switch(lat_counter)
					{
						case  1 : // deg 10s
							lat_deg = (receive - '0') * 10;
							break;
						case  2 : // deg 0s
							lat_deg += receive - '0';
							break;
						case  3 : // minute 10s
							lat_min = (receive - '0') * 10;
							break;
						case  4 : // minutes 0s
							lat_min += (receive - '0');
							break;
						// case  5 : // '.' - we do nothing in this case - not interested but we need to mark that it occurs
						case  6 : // minute 0.1s
							lat_min += (receive - '0') * 0.1;
							break;
						case  7 : // minute 0.01s
							lat_min += (receive - '0') * 0.01;
							break;
						case  8 : // minute 0.001s
							lat_min += (receive - '0') * 0.001;
							break;
						case  9 : // minute 0.0001s
							lat_min += (receive - '0') * 0.0001;
							break;
						case 10 : // minute 0.00001s
							lat_min += (receive - '0') * 0.00001;
							break;
						// There should be no further decimal points for the 
					}
					break;
				case  4 : // North or South hemisphere
					if((receive == 'N') || (receive == 'S'))
						GPS_data->pos.hemi_lat = receive;
					// else
					// 	return -1;
					break;
				case  5 : // Longitude
					++long_counter;
					switch(long_counter)
					{
						case  1 : // deg 100s
							long_deg = (receive - '0') * 100;
							break;
						case  2 : // deg 10s
							long_deg += (receive - '0') * 10;
							break;
						case  3 : // deg 0s
							long_deg += receive - '0';
							break;
						case  4 : // minute 10s
							long_min = (receive - '0') * 10;
							break;
						case  5 : // minutes 0s
							long_min += receive - '0';
							break;
						// case  6 : // '.' - we do nothing in this case - not interested but we need to mark that it occurs
						case  7 : // minute 0.1s
							long_min += (receive - '0') * 0.1;
							break;
						case  8 : // minute 0.01s
							long_min += (receive - '0') * 0.01;
							break;
						case  9 : // minute 0.001s
							long_min += (receive - '0') * 0.001;
							break;
						case 10 : // minute 0.0001s
							long_min += (receive - '0') * 0.0001;
							break;
						case 11 : // minute 0.00001s
							long_min += (receive - '0') * 0.00001;
							break;
						// There should be no further decimal points for the 
					}
					break;
				case  6 : // East/West
					if((receive == 'E') || (receive == 'W'))
						GPS_data->pos.hemi_long = receive;
					// else
					// 	return -1;
					break;
				case  7 : // speed - always has 3 decimals
					knot_spd *= 10;
					if(receive != '.')
						knot_spd += receive - '0';
					break;
				case  8 : // course over ground
					if(cog_check == 0) // on the first run through reset to 0
					{
						cog_T = 0;
						++cog_check;
					}
						// else we wish to maintain the last value of course read
					cog_T *= 10;
					cog_T += receive;
					break;
				case  9 : // date
					++date_counter;
					switch(date_counter)
					{
						case 1 : // day 10s
							day = (receive - '0') * 10;
							break;
						case 2 : // day 0s
							day += receive - '0';
							break;
						case 3 : // month 10s
							month = (receive - '0') * 10;
							break;
						case 4 : // month 0s
							month += receive - '0';
							break;
						case 5 : // year 10s
							year = (receive - '0') *10;
							break;
						case 6 : // year 0s
							year += receive - '0';
							break;
					}
					break;
				// case 10 : // magnetic variation - no value shown in general testing
				// case 11 : // magnetic variation indicator - no value shown in general testing
				case 12 : // mode indicator
					if(receive == 'A') // data good
						GPS_data->check_2 = receive;
					// else // indicates that the data is invalid - however we again continue
					// {
					// 	GPS_data->check_2 = receive;
					// 	return -1;
					// }
					break;
			}

		}
		receive = UART_Receive(); // get the next item
	}

	checksum_output = GPS_checksum(checksum);
	if (checksum_output == 0)
		return 0; // this is to continue on with the old data
	// 	return -1;
	else //(checksum_output == 1) // else we are good and can rewrite all of the data
	{
		GPS_data->current_datetime.time_hour = hour, GPS_data->current_datetime.time_minute = min, GPS_data->current_datetime.time_sec = sec;
		GPS_data->pos.lat_deg = lat_deg, GPS_data->pos.lat_min = lat_min;
		GPS_data->pos.long_deg = long_deg, GPS_data->pos.long_min = long_min;
		GPS_data->knot_spd = knot_spd;
		GPS_data->current_datetime.date_day = day, GPS_data->current_datetime.date_month = month, GPS_data->current_datetime.date_year = year;
		GPS_data->cog_T = cog_T;
		return 0;
	}
}

int8_t GPS_VTG(struct GPS_info_struct *GPS_data)
{
	// VTG contains information on course over ground and ground speed data
	
	// VTG Data
	int cog_T = 0, cog_M = 0, knot_spd = 0, kmh_spd = 0;
	
	// initialise counters for use in the switch cases
	int counter = 0;

	// declare other variables
	char receive;
	int checksum = 'G' ^ 'P' ^ 'V' ^ 'T' ^ 'G'; // start the checksum - if we are in this function then this is the data that has already been received
	uint8_t checksum_output;
	uint8_t cog_check_T = 0, cog_check_M = 0;

	receive = UART_Receive(); // get the first item

	while(receive != '*') // i.e. not end of statement
	{
		checksum = checksum ^ receive; // compute the checksum
		if(receive == ',')
			++counter; // increase the counter - go to the next part of the sentence
		else // otherwise we continue on dealing with things
		{
			switch(counter)
			{
				case  1 : // true course over ground
					if(cog_check_T == 0) // on the first run through reset to 0
					{
						cog_T = 0;
						++cog_check_T;
					}
						// else we wish to maintain the last value of course read
					cog_T *= 10;
					cog_T += receive;
					break;
				case  2 : // check
					if(receive == 'T') // data good
						GPS_data->check_T = receive;
					// else // this should be T to indicate true heading
					// {
					// 	GPS_data->check_T = receive;
					// 	return -1;
					// }
					break;
				case  3 : // magnetic course over ground
					if(cog_check_M == 0) // on the first run through reset to 0
					{
						cog_M = 0;
						++cog_check_M;
					}
						// else we wish to maintain the last value of course read
					cog_M *= 10;
					cog_M += receive;
					break;
				case  4 : // check
					if(receive == 'M') // data good
						GPS_data->check_M = receive;
					// else // this should be M to indicate magnetic heading
					// {
					// 	GPS_data->check_M = receive;
					// 	return -1;
					// }
					break;
				case  5 : // speed - always has 3 decimals
					knot_spd *= 10;
					if(receive != '.')
						knot_spd += receive - '0';
					break;
				case  6 : // speed check
					if(receive == 'N') // data good
						GPS_data->check_N = receive;
					// else // this should be N to indicate Knots
					// {
					// 	GPS_data->check_N = receive;
					// 	return -1;
					// }
					break;
				case  7 : //speed (kmh) - always has 3 decimals
					GPS_data->kmh_spd *= 10;
					if(receive != '.')
						GPS_data->kmh_spd += receive - '0';
					break;
				case  8 : // speed check
					if(receive == 'K') // data good
						GPS_data->check_K = receive;
					// else // this should be K to indicate km/h
					// {
					// 	GPS_data->check_K = receive;
					// 	return -1;
					// }
					break;
				case  9 : // flag check
					if(receive == 'A') // data good
						GPS_data->check_3 = receive;
					// else // this should be A to indicate good data
					// {
					// 	GPS_data->check_3 = receive;
					// 	return -1;
					// }
					break;
			}

		}
		receive = UART_Receive(); // get the next item
	}

	checksum_output = GPS_checksum(checksum);
	if (checksum_output == 0)
		return 0; // this is to continue on with the old data
	// 	return -1;
	else //(checksum_output == 1) // else we are good and can rewrite all of the data
	{
		GPS_data->cog_T = cog_T;
		GPS_data->cog_M = cog_M;
		GPS_data->knot_spd = knot_spd;
		GPS_data->kmh_spd = kmh_spd;
		return 0;
	}
	
	return 0;
}

uint8_t GPS_checksum(uint8_t checksum)
{
	// This function is called to analyse the checksum - generally called once the '*' character has been found in parsing

	// declare variables
	char item1, item2;
	uint8_t received_checksum;
	item1 = UART_Receive();
	item2 = UART_Receive();

	received_checksum = ((item1 -'0') * 16) + (item2 - '0');

	// return received_checksum == checksum;
	return 1;
}

float GPS_bearing(struct position_data *pos1, struct position_data *pos2)
{
	// Convert Lats and Longs to a single unit then convert to radians
	float lat1 = convert_to_rad(convert_to_arcdec(pos1->lat_deg, pos1->lat_min, pos1->hemi_lat));
	float lat2 = convert_to_rad(convert_to_arcdec(pos2->lat_deg, pos2->lat_min, pos2->hemi_lat));
	float long1 = convert_to_rad(convert_to_arcdec(pos1->long_deg, pos1->long_min, pos1->hemi_long));
	float long2 = convert_to_rad(convert_to_arcdec(pos2->long_deg, pos2->long_min, pos2->hemi_long));

	// Calculate x and y coordinates
	float y = sin(long2 - long1)*cos(lat2);
	float x = cos(lat1)*sin(lat2) - sin(lat1)*cos(lat2)*cos(long2 - long1);

	// Calculate the angle of bearing
	return convert_to_deg(atan2(y,x));
}

float GPS_distance(struct position_data *pos1, struct position_data *pos2)
{
	// Convert Lats and Longs to a single unit then convert to radians
	float lat1 = convert_to_rad(convert_to_arcdec(pos1->lat_deg, pos1->lat_min, pos1->hemi_lat));
	float lat2 = convert_to_rad(convert_to_arcdec(pos2->lat_deg, pos2->lat_min, pos2->hemi_lat));
	float long1 = convert_to_rad(convert_to_arcdec(pos1->long_deg, pos1->long_min, pos1->hemi_long));
	float long2 = convert_to_rad(convert_to_arcdec(pos2->long_deg, pos2->long_min, pos2->hemi_long));

	float a = sin((lat2-lat1)/2)*sin((lat2-lat1)/2) + cos(lat1)*cos(lat2)*sin((long2-long1)/2)*sin((long2-long1)/2);
	float c = 2*atan2(sqrt(a),sqrt(1-a));
	return 6371000 * c;
}

float convert_to_rad(float degree)
{
	return degree * M_PI / 180; // convert degree to radian units
}

float convert_to_deg(float rad)
{
	return rad * 180 / M_PI; // convert radian to degree units
}

float convert_to_arcdec(int deg, float min, char hemi)
{
	// converts deg minutes into a single unit of degrees
	if ((hemi == 'W') || (hemi == 'S')) // if in the Western or Southern hemispheres make negative
		return -1*(deg + (min / 60));
	else // else it is positive
		return (deg + (min / 60));
}

