
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h> // microprocessor operations
#include "UART.h"
#include "math.h"


struct datetime
{
	int time_hour;
	int time_minute;
	int time_sec;
	int date_day;
	int date_month;
	int date_year;
};

struct position_data
{
	int lat_deg;
	float lat_min;
	char hemi_lat;
	// in format lat_deg, lat_min hemi_lat
	int long_deg;
	float long_min;
	char hemi_long;
	// in format long_deg, long_min hemi_long
};


struct GPS_info_struct
{
	// RMC Data
	struct datetime current_datetime;
	char check_1;
	struct position_data pos;
	char check_2;
	char check_3;
	// VTG Data
	int cog_T;
	char check_T;
	int cog_M;
	char check_M;
	int knot_spd; // there are always 3 decimal places i.e. 1.234 will be represented as 1234
	char check_N;
	int kmh_spd; // there are always 3 decimal places i.e. 1.234 will be represented as 1234
	char check_K;
};

float convert_to_rad(float degree);
float convert_to_deg(float rad);
float convert_to_arcdec(int deg, float min, char hemi);
void GPS_init(struct GPS_info_struct *GPS_data);
void GPS_print();
void GPS_parse(struct GPS_info_struct *GPS_data);
int8_t GPS_RMC(struct GPS_info_struct *GPS_data);
int8_t GPS_VTG(struct GPS_info_struct *GPS_data);
uint8_t GPS_checksum(uint8_t checksum);
float GPS_bearing(struct position_data *pos1, struct position_data *pos2);
float GPS_distance(struct position_data *pos1, struct position_data *pos2);
