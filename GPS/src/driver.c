#include "usb_serial.h"
// This is required for spitting data to the screen

#include "interrupt_def.h"
// This is required in setting up the Teensy

#include "gps.h"
// This contains the GPS code

void print_stuff(struct GPS_info_struct *GPS_data);
void bearing_distance(void);
void print_data(struct GPS_info_struct *GPS_data);

int main(){
	CPU_PRESCALE(CPU_8MHz);
	// Initiliaze Teensy

	usb_init();
	// Initialise the usb serial interface (only in driver) - this is not necessary for the running of the GPS code

	struct GPS_info_struct GPS_data;
	// initialise the GPS data struct
	GPS_init(&GPS_data);
	// Initialise GPS properties

	print_data(&GPS_data);

	// bearing_distance();

	// // Print everything to the screen in a continuous stream
	// GPS_print();

	return 0;
}



void print_stuff(struct GPS_info_struct *GPS_data)
{
	usb_serial_putint(GPS_data->current_datetime.time_hour);
	usb_serial_putchar(':');
	usb_serial_putint(GPS_data->current_datetime.time_minute);
	usb_serial_putchar(':');
	usb_serial_putint(GPS_data->current_datetime.time_sec);
	usb_serial_putchar(GPS_data->check_1);
	usb_serial_putchar(10); // newline
	usb_serial_putchar(13);

	usb_serial_putint(GPS_data->pos.lat_deg);
	usb_serial_putchar('-');
	usb_serial_putfloat(GPS_data->pos.lat_min);
	usb_serial_putchar(GPS_data->pos.hemi_lat);
	usb_serial_putchar(10); // newline
	usb_serial_putchar(13);

	usb_serial_putint(GPS_data->pos.long_deg);
	usb_serial_putchar('-');
	usb_serial_putfloat(GPS_data->pos.long_min);
	usb_serial_putchar(GPS_data->pos.hemi_long);
	usb_serial_putchar(10); // newline
	usb_serial_putchar(13);

	usb_serial_putint(GPS_data->knot_spd);
	usb_serial_putchar(10); // newline
	usb_serial_putchar(13);

	usb_serial_putint(GPS_data->current_datetime.date_day);
	usb_serial_putchar('/');
	usb_serial_putint(GPS_data->current_datetime.date_month);
	usb_serial_putchar('/');
	usb_serial_putint(GPS_data->current_datetime.date_year);
	usb_serial_putchar(GPS_data->check_2);
	usb_serial_putchar(10); // newline
	usb_serial_putchar(10); // newline
	usb_serial_putchar(13);

	usb_serial_putint(GPS_data->cog_T);
	usb_serial_putchar(GPS_data->check_T);
	usb_serial_putchar(10); // newline
	usb_serial_putchar(13);

	usb_serial_putint(GPS_data->cog_M);
	usb_serial_putchar(GPS_data->check_M);
	usb_serial_putchar(10); // newline
	usb_serial_putchar(13);			

	usb_serial_putint(GPS_data->knot_spd);
	usb_serial_putchar(GPS_data->check_N);
	usb_serial_putchar(10); // newline
	usb_serial_putchar(13);
	
	usb_serial_putint(GPS_data->kmh_spd);
	usb_serial_putchar(GPS_data->check_K);
	usb_serial_putchar(10); // newline
	usb_serial_putchar(13);

	usb_serial_putchar(GPS_data->check_3);
	usb_serial_putchar(10); // newline
	usb_serial_putchar(10); // newline
	usb_serial_putchar(10); // newline
	usb_serial_putchar(13);
}

void bearing_distance(void)
{
	// Show the use of the course finder
	struct position_data pos1;
	struct position_data pos2;

	pos1.lat_deg = 27;
	pos1.lat_min = 23.17455;
	pos1.hemi_lat = 'S';
	pos1.long_deg = 153;
	pos1.long_min = 00.04114;
	pos1.hemi_long = 'E';

	pos2.lat_deg = 27;
	pos2.lat_min = 23.2576;
	pos2.hemi_lat = 'S';
	pos2.long_deg = 153;
	pos2.long_min = 00.07895;
	pos2.hemi_long = 'E';

	// Press any key then spit important data to the screen
	while(1)
	{
		char receive = usb_serial_getchar(); // once putty/terminal is open - press any key
		if(receive > 0)
		{	

			float degree = GPS_bearing(&pos1, &pos2);
			usb_serial_putfloat(degree);
			usb_serial_putchar(10);
			usb_serial_putchar(10);
			usb_serial_putchar(13);

			float dist = GPS_distance(&pos1, &pos2);
			usb_serial_putfloat(dist);
			usb_serial_putchar(10);
			usb_serial_putchar(10);
			usb_serial_putchar(13);
		}
	}
}

void print_data(struct GPS_info_struct *GPS_data)
{
	
	// Press any key then spit important data to the screen
	while(1)
	{
		char receive = usb_serial_getchar(); // once putty/terminal is open - press any key
		if(receive > 0)
		{	
			GPS_parse(GPS_data); // parse incoming GPS data
			// print some stuff to the screen

			print_stuff(GPS_data);
		}
	}
}
