#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include "NMEA.h"



char* GPS_get(char* prefix, char* rec,uint8_t size)
{	
	uint8_t s ;
	
	uint8_t i = 1;
	nmea_ready = false;
	
	while(nmea_ready == false)
	{
		
			i = 1;
			memset(rec,0,size);
			while(UART1_Recive() != '$');
			rec[0] = '$';
		do{
			s = UART1_Recive() ;
			rec[i] = s;
			i++;
			if(s == '\r'){
				rec[i]='\n';
				UART_SendString(rec);
				UART_Transmit('\n');
				//if(strstr(rec,prefix)){
					nmea_ready = true;
					
			//	}
				break;
			}
		
		}while(1);
	}
	
	return rec;
}

bool NMEA_chcecksum(char* nmea){
	uint8_t dec_checksum ;
	uint8_t i = 2;
	char  checksum[2];
	uint8_t result = 0;
	char* asterisk = strstr(nmea,"*");
	dec_checksum = 0 ;
	strncpy( checksum,asterisk+1,2);
	dec_checksum = strtol(checksum,NULL,16);
	if (nmea[0] == '$'){
		result = nmea[1];
		while(nmea[i]!='*'){
			result = result ^ nmea[i];
			i++;
		};
	}
	if(result == dec_checksum)
		return true;
	else
		return false;
}

double NMEA_toDec(double value){
	double dec ;
	double min ;
	uint8_t deg;
	
	deg = (uint8_t)(value / 100) ; // stopnie
	
	min = (((value/ 100) - deg )*100)/60;
	dec = (double)deg + min ;
	
	return dec;
}

struct nmea NMEA_parse(char* nmea){
	uint8_t i = 0;
	uint8_t deg;
	struct nmea my_nmea;
	float min ;
	char* word;	
	const char* delimiter = ",";
	word = strtok(nmea,delimiter);
	
	while(word != NULL){
		i++;
		word = strtok(NULL,",");
		
		if(i == 3 ){
		
			my_nmea.latitiude =	atof(word);
			my_nmea.latitiude =  NMEA_toDec(my_nmea.latitiude);			
			
		}
		
		if (i == 5){
			my_nmea.longitiude = atof(word);
			my_nmea.longitiude = NMEA_toDec(my_nmea.longitiude);	
		}
	
	}
	/*UART_StopReceiver();
	dtostrf(latitiude, 10, 6,bufor);
	UART_SendString(bufor);
	UART_SendString("\n");
	
	dtostrf(longitude, 10, 6,bufor);
	UART_SendString(bufor);
	UART_SendString("\n");
	UART_StartReceiver();
	*/
	return my_nmea;
}