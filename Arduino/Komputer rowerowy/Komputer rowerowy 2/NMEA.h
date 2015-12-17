
#include <stdbool.h>

volatile bool nmea_ready ; 

 struct nmea {
	
	char * type;
	double latitiude;
	double longitiude;
	
	
};

char* GPS_get(char* prefix,char* rec,uint8_t size);

bool NMEA_chcecksum(char* nmea);

double NMEA_toDec(double value);

struct nmea NMEA_parse(char* nmea);