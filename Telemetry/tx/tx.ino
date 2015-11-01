#include <VirtualWire.h>
//#include <gps.h>

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

// SETUP for CAB202
const int led_pin = 11;
const int transmit_pin = 18;
const int receive_pin = 19;
const int transmit_en_pin = 20;

struct GPS_info_struct GPS_data;
uint8_t datetime[6];
uint8_t position[12];
uint8_t info[11];
uint8_t extra[2];

char *modes = "APFRS";
uint8_t mode = 0;
uint8_t mode_length = 5;

unsigned long last_data_t;
unsigned long last_mode_t;

void print_all(void);
void dump_buffer();
void GPS_fill();
void tx();
void print_data(uint8_t *time, uint8_t *pos, uint8_t *info);

void setup()
{
  delay(1000);
  Serial.begin(9600); // Debugging only

  // Initialise the IO and ISR
  vw_set_tx_pin(transmit_pin);
  vw_set_rx_pin(receive_pin);
  vw_set_ptt_pin(transmit_en_pin);
  vw_set_ptt_inverted(true);  // Required for DR3100
  vw_setup(2000);             // Bits per sec
  pinMode(led_pin, OUTPUT);
  
  pinMode(5, INPUT);	   // Pin 5 is input to which a switch is connected = INT0
  pinMode(6, INPUT);	   // Pin 6 is input to which a switch is connected = INT1
  attachInterrupt(0, toggle_data, RISING);
  attachInterrupt(1, toggle_mode, RISING);

  extra[0] = 'A';    //type
  extra[1] = false;  //dummy
  
  last_data_t = millis();
  last_mode_t = last_data_t;

  GPS_init(&GPS_data);
  GPS_parse(&GPS_data);
  GPS_fill();
}





void loop()
{
  
//  uint8_t datetime[] = {0, 1, 2, 3, 4, 5};
//  uint8_t position[] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
//  uint8_t info[] = {18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28};

//  while(1) {
//    if(Serial1.available()) {
//      Serial.print((char)Serial1.read());
//    }
//  }


  bool flush_needed = false;
  unsigned long start_parse = millis();
  unsigned long time_tx = 300; // starting guess, need to change this if tx changes
  
    while(1) {

      //if at start of gps code, collect and fill
      if(Serial1.available()) {
        print_all();
        Serial.println();
        Serial.print((char)extra[0]);
        Serial.print(" ");
        Serial.println(extra[1]);
        start_parse = millis();
        GPS_parse(&GPS_data);
        unsigned long stop_parse = millis();
        Serial.print("GPS_parse() : ");
        Serial.println(stop_parse - start_parse);
        
        unsigned long start_fill = millis();
        GPS_fill();
        unsigned long stop_fill = millis();
        Serial.print("GPS_fill()  : ");
        Serial.print(stop_fill - start_fill);
        Serial.print(" : ");
        Serial.println(stop_fill - start_parse);
        
        flush_needed = true;
      }

      // if we need to flush the buffer, tx until we are over half a second running, THEN flush
      // that way we don't was the 400mseconds needed for buffer to fill
      if(flush_needed) {
        while((millis() - start_parse) <= 500) {
          unsigned long start_tx = millis();
          tx();
          unsigned long stop_tx = millis();
          time_tx = (time_tx + (stop_tx - start_tx)) / 2;
          Serial.print("tx(1)       : ");
          Serial.print(time_tx);
          Serial.print(" : ");
          Serial.print(stop_tx - start_tx);
          Serial.print(" : ");
          Serial.println(stop_tx - start_parse);
//          break;
        }
        
        unsigned long start_flush = millis();
        UART_Smart_Flush();
        unsigned long stop_flush = millis();
        Serial.print("UART_Smart_Flush() : ");
        Serial.print(stop_flush - start_flush);
        Serial.print(" : ");
        Serial.println(stop_flush - start_parse);
        flush_needed = false;
      }
    
      // if theres any time left, tx again
      while((millis() - start_parse) < (1000-time_tx-50)) {
        unsigned long start_tx = millis();
        tx();
        unsigned long stop_tx = millis();
        time_tx = (time_tx + (stop_tx - start_tx)) / 2;
        Serial.print("tx(2)       : ");
        Serial.print(time_tx);
        Serial.print(" : ");
        Serial.print(stop_tx - start_tx);
        Serial.print(" : ");
        Serial.println(stop_tx - start_parse);
      }
      
//      print_all();
    }
    
}




void tx() {
  digitalWrite(led_pin, HIGH); // Flash a light to show transmitting
//  vw_send(datetime, 6);
//  vw_wait_tx(); // Wait until the whole message is gone
  //delay(15);
  
  vw_send(position, 12);
  vw_wait_tx(); // Wait until the whole message is gone
  //delay(15);
  
//  vw_send(info, 11);
//  vw_wait_tx(); // Wait until the whole message is gone

  vw_send(extra, 2);
  vw_wait_tx(); 

  digitalWrite(led_pin, LOW);
  //delay(15);
}

#define button_delay 250

void toggle_data() {
  if((millis() - last_data_t) < button_delay) {return;}
  
  extra[1] ^= 1;
  last_data_t = millis();
}

void toggle_mode() {
  if((millis() - last_mode_t) < button_delay) {return;}
  
  mode++;
  if(mode >= mode_length) {mode = 0;}
  extra[0] = modes[mode];
  last_mode_t = millis();
}





void dump_buffer() {
    while(Serial1.available()) {
      Serial.print((char)Serial1.read());
    }
}








void GPS_fill() {
  unsigned char lat_min[4];
  unsigned char long_min[4];
  
  breakDown(0, lat_min, GPS_data.pos.lat_min);
  breakDown(0, long_min, GPS_data.pos.long_min);
  
  datetime[0] = (uint8_t)GPS_data.current_datetime.time_hour; 
  datetime[1] =  (uint8_t)GPS_data.current_datetime.time_minute; 
  datetime[2] =  (uint8_t)GPS_data.current_datetime.time_sec; 
  datetime[3] =  (uint8_t)GPS_data.current_datetime.date_day; 
  datetime[4] =  (uint8_t)GPS_data.current_datetime.date_month; 
  datetime[5] =  (uint8_t)GPS_data.current_datetime.date_year;

  position[0] = (uint8_t)GPS_data.pos.lat_deg;
                // (uint8_t)GPS_data.pos.lat_min;     //float
  position[1] = lat_min[0];
  position[2] = lat_min[1];
  position[3] = lat_min[2];
  position[4] = lat_min[3];
  position[5] = (uint8_t)GPS_data.pos.hemi_lat;    //char
  position[6] = (uint8_t)GPS_data.pos.long_deg;  
                // (uint8_t)GPS_data.pos.long_min;    //float
  position[7] = long_min[0];
  position[8] = long_min[1];
  position[9] = long_min[2];
  position[10] = long_min[3];
  position[11] = (uint8_t)GPS_data.pos.hemi_long;  //char

  info[0] = (uint8_t)GPS_data.check_1;   //char
  info[1] = (uint8_t)GPS_data.check_2;   //char
  info[2] = (uint8_t)GPS_data.check_3;   //char
  info[3] = (uint8_t)GPS_data.cog_T;
  info[4] = (uint8_t)GPS_data.check_T;   //char
  info[5] = (uint8_t)GPS_data.cog_M;
  info[6] = (uint8_t)GPS_data.check_M;   //char
  info[7] = (uint8_t)GPS_data.knot_spd;
  info[8] = (uint8_t)GPS_data.check_N;   //char
  info[9] = (uint8_t)GPS_data.kmh_spd;
  info[10] = (uint8_t)GPS_data.check_K;  //char
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






























void print_data(uint8_t *time, uint8_t *pos, uint8_t *info){
 int ii; 
 
 Serial.println();
 Serial.println();
 Serial.println("ARRAYS:");
 
 for(ii = 0; ii < 6; ii++) {
   Serial.print(time[ii]);
   Serial.print(" ");
 }
 Serial.println();
 

 Serial.print(pos[0]);
 Serial.print(" ");

 Serial.print(buildUp(1, pos),25);
 Serial.print(" "); 

 Serial.print((char)pos[5]);
 Serial.print(" ");
 Serial.print(pos[6]);
 Serial.print(" ");

 Serial.print(buildUp(7, pos),25);
 Serial.print(" ");

 Serial.print((char)pos[11]);
 Serial.println();
 
 Serial.print((char)info[0]);
 Serial.print(" ");
 Serial.print((char)info[1]);
 Serial.print(" ");
 Serial.print((char)info[2]);
 Serial.print(" ");
 Serial.print(info[3]);
 Serial.print(" ");
 Serial.print((char)info[4]);
 Serial.print(" ");
 Serial.print(info[5]);
 Serial.print(" ");
 Serial.print((char)info[6]);
 Serial.print(" ");
 Serial.print(info[7]);
 Serial.print(" ");
 Serial.print((char)info[8]);
 Serial.print(" ");
 Serial.print(info[9]);
 Serial.print(" ");
 Serial.print((char)info[10]);
}


void print_all(void) {
  Serial.println();
  Serial.println();
  Serial.println("ACTUAL:");
  
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

