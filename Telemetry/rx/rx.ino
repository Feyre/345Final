#include <VirtualWire.h>
//#include <gps.h>
#include <stdio.h>
#include <stdlib.h>

//FUNCTIONS
void UART_Init(void);
void UART_TransmitChar(char data);
char UART_Receive(void);
void UART_Flush_Data(void);
void UART_Smart_Flush(void);

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

// SETUP for Teensy
const int led_pin = 11;
const int transmit_pin = 18;
const int receive_pin = 19;
const int transmit_en_pin = 20;

void print_GPS(struct GPS_info_struct GPS_data);
void GPS_empty(uint8_t *buf, uint8_t buflen);
bool rx();

uint8_t extra[2];

struct GPS_info_struct GPS_data_ERV;
struct GPS_info_struct GPS_data_CAR;

void setup()
{
    delay(1000);
    Serial.begin(9600);	// Debugging only
    Serial.println("setup");

    // Initialise the IO and ISR
    vw_set_tx_pin(transmit_pin);
    vw_set_rx_pin(receive_pin);
    vw_set_ptt_pin(transmit_en_pin);
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);  // Bits per sec

    vw_rx_start();       // Start the receiver PLL running

    pinMode(led_pin, OUTPUT);
    
    extra[0] = 'A';
    extra[1] = false;
    
    GPS_init(&GPS_data_CAR);
    GPS_init(&GPS_data_ERV);
}

void loop()
{
    
    
  bool flush_needed = false;
  unsigned long start_parse = millis();
  unsigned long time_tx = 300; // starting guess, need to change this if tx changes
  
    while(1) {

      //if at start of gps code, collect and fill
      if(Serial1.available()) {
//        Serial.println();
//        Serial.println("CAR -- (GPS)");
//        print_GPS(GPS_data_CAR);
//        Serial.println();
//        Serial.println("ERV -- (Rx)");
//        print_GPS(GPS_data_ERV);
//        Serial.println();
//        Serial.println();
        start_parse = millis();
        GPS_parse(&GPS_data_CAR);
        unsigned long stop_parse = millis();
//        Serial.print("GPS_parse() : ");
//        Serial.println(stop_parse - start_parse);
        
        flush_needed = true;
      }

      // if we need to flush the buffer, tx until we are over half a second running, THEN flush
      // that way we don't was the 400mseconds needed for buffer to fill
      if(flush_needed) {
        while((millis() - start_parse) <= 500) {
          unsigned long start_tx = millis();
          if(!rx()) {continue;}
          unsigned long stop_tx = millis();
          time_tx = (time_tx + (stop_tx - start_tx)) / 2;
//          Serial.print("rx(1)       : ");
//          Serial.print(time_tx);
//          Serial.print(" : ");
//          Serial.print(stop_tx - start_tx);
//          Serial.print(" : ");
//          Serial.println(stop_tx - start_parse);
//          break;
        }
        
        unsigned long start_flush = millis();
        UART_Smart_Flush();
        unsigned long stop_flush = millis();
//        Serial.print("UART_Smart_Flush() : ");
//        Serial.print(stop_flush - start_flush);
//        Serial.print(" : ");
//        Serial.println(stop_flush - start_parse);
//        flush_needed = false;
      }
    
      // if theres any time left, do this again and again
      while((millis() - start_parse) < (1000-time_tx-50)) {
        unsigned long start_tx = millis();
        if(!rx()) {continue;}
        unsigned long stop_tx = millis();
        time_tx = (time_tx + (stop_tx - start_tx)) / 2;
//        Serial.print("rx(2)       : ");
//        Serial.print(time_tx);
//        Serial.print(" : ");
//        Serial.print(stop_tx - start_tx);
//        Serial.print(" : ");
//        Serial.println(stop_tx - start_parse);
      }
      
      //must be less than 50ms
      if(flush_needed) {
        int bearing = (int)GPS_bearing(&GPS_data_CAR.pos, &GPS_data_ERV.pos);
        int distance = (int)GPS_distance(&GPS_data_CAR.pos, &GPS_data_ERV.pos);
//        char type = 'A';
        
        // bearing -180 -> 180
        if(bearing < 0) {
          bearing += 360;
        }
        
        // inverted and 90 deg out
        bearing = 360-bearing;
        bearing += 90;
        if(bearing > 360) {
          bearing -= 360; 
        }
        
        //bearing 
        
        if(extra[1]) {
          Serial.println("9999999Z");
        }
        else {
          char buff[80];
          sprintf(buff, "%03d%04d%c", bearing, distance, extra[0]);
          Serial.println(buff);
        }
//        Serial.print("$");
//        Serial.print(bearing);
//        Serial.print(distance);
//        Serial.print(type);
        
        
//        Serial.println();
//        Serial.println(bearing,25);
//        Serial.println(distance,25);
        
        flush_needed = false;
      }
   }
}


bool rx() {
    // RECEIVE GPS DATA
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    if (vw_get_message(buf, &buflen)) { // Non-blocking
        digitalWrite(led_pin, HIGH); // Flash a light to show received good message

        GPS_empty(buf, buflen);
//        print_GPS(GPS_data_ERV);

//        int ii;
//        for(ii = 0; ii < buflen; ii++) {
//          Serial.print(buf[ii]);
//          Serial.print(" ");
//        }     
//        Serial.println();

        digitalWrite(led_pin, LOW);
        return true;
    }
    return false;
}



void GPS_empty(uint8_t *buf, uint8_t buflen) {
        if(buflen == 12) {
            GPS_data_ERV.pos.lat_deg    = (int)  buf[0];
            GPS_data_ERV.pos.lat_min    = buildUp(1,buf);
            GPS_data_ERV.pos.hemi_lat   = (char) buf[5];
            GPS_data_ERV.pos.long_deg   = (int)  buf[6];
            GPS_data_ERV.pos.long_min   = buildUp(7,buf);
            GPS_data_ERV.pos.hemi_long  = (char) buf[11];
        }
        else if(buflen == 2) {
            extra[0] = buf[0];
            extra[1] = buf[1];
        }
//        else if(buflen == 6) {
//            GPS_data_ERV.current_datetime.time_hour     = (int)buf[0];
//            GPS_data_ERV.current_datetime.time_minute   = (int)buf[1];
//            GPS_data_ERV.current_datetime.time_sec      = (int)buf[2];
//            GPS_data_ERV.current_datetime.date_day      = (int)buf[3];
//            GPS_data_ERV.current_datetime.date_month    = (int)buf[4];
//            GPS_data_ERV.current_datetime.date_year     = (int)buf[5];
//        }
//        else if(buflen == 11) {
//            GPS_data_ERV.check_1  = (char)buf[0];
//            GPS_data_ERV.check_2  = (char)buf[1];
//            GPS_data_ERV.check_3  = (char)buf[2];
//            GPS_data_ERV.cog_T    = (int) buf[3];
//            GPS_data_ERV.check_T  = (char)buf[4];
//            GPS_data_ERV.cog_M    = (int) buf[5];
//            GPS_data_ERV.check_M  = (char)buf[6];
//            GPS_data_ERV.knot_spd = (int) buf[7];
//            GPS_data_ERV.check_N  = (char)buf[8];
//            GPS_data_ERV.kmh_spd  = (int) buf[9];
//            GPS_data_ERV.check_K  = (char)buf[10];
//        }
}







int breakDown(int index, unsigned char outbox[], float member)
{
  unsigned long d = *(unsigned long *)&member;

  outbox[index] = d & 0x00FF;
  index++;

  outbox[index] = (d & 0xFF00) >> 8;
  index++;

  outbox[index] = (d & 0xFF0000) >> 16;
  index++;

  outbox[index] = (d & 0xFF000000) >> 24;
  index++;
  return index;
}


float buildUp(int index, unsigned char outbox[])
{
  unsigned long d;

  d =  ((unsigned long int)outbox[index+3] << 24) | ((unsigned long int)outbox[index+2] << 16)
    | ((unsigned long int)outbox[index+1] << 8) | ((unsigned long int)outbox[index]);
  float member = *(float *)&d;
  return member;
}





void print_GPS(struct GPS_info_struct GPS_data) {
  Serial.print(GPS_data.current_datetime.time_hour);
  Serial.print(" ");
  Serial.print(GPS_data.current_datetime.time_minute);
  Serial.print(" ");
  Serial.print(GPS_data.current_datetime.time_sec);
  Serial.print(" ");
  Serial.print(GPS_data.current_datetime.date_day);
  Serial.print(" ");
  Serial.print(GPS_data.current_datetime.date_month);
  Serial.print(" ");
  Serial.println(GPS_data.current_datetime.date_year);

  Serial.print(GPS_data.pos.lat_deg);
  Serial.print(" ");
  Serial.print(GPS_data.pos.lat_min, 25);
  Serial.print(" ");
  Serial.print(GPS_data.pos.hemi_lat);
  Serial.print(" ");
  Serial.print(GPS_data.pos.long_deg);
  Serial.print(" ");
  Serial.print(GPS_data.pos.long_min, 25);
  Serial.print(" ");
  Serial.println(GPS_data.pos.hemi_long);

  Serial.print(GPS_data.check_1);
  Serial.print(" ");
  Serial.print(GPS_data.check_2);
  Serial.print(" ");
  Serial.print(GPS_data.check_3);
  Serial.print(" ");
  Serial.print(GPS_data.cog_T);
  Serial.print(" ");
  Serial.print(GPS_data.check_T);
  Serial.print(" ");
  Serial.print(GPS_data.cog_M);
  Serial.print(" ");
  Serial.print(GPS_data.check_M);
  Serial.print(" ");
  Serial.print(GPS_data.knot_spd);
  Serial.print(" ");
  Serial.print(GPS_data.check_N);
  Serial.print(" ");
  Serial.print(GPS_data.kmh_spd);
  Serial.print(" ");
  Serial.println(GPS_data.check_K);
}





