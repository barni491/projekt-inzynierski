/*
 * Komputer_rowerowy.c
 *
 * Created: 21.10.2015 00:29:31
 *  Author: Bartosz
 */ 

#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBR (F_CPU/16/BAUD-1)

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/wdt.h>
#include "UART.h"
#include "NMEA.h"

char rmc[] = "$GPGGA";

volatile char bufor[100];						//bufor do obs³ugi komend przez UART
volatile bool buf_ready=false;					//odebranie pelenj komendy
volatile int buf_pos=0;							//pozycja bufora UART 
volatile uint32_t milis = 0;					//licznik milisekund
volatile uint8_t press = 0;
volatile uint8_t pressed = 0;
volatile uint32_t currentBpm = 0;
volatile uint32_t beatTime = 0 ;
volatile uint8_t it  = 0;
bool added = false;
volatile uint8_t kadencja;

char nmea_buffer[100];

typedef struct nmea_b{
	unsigned char tab[10][60];
	bool ready[5] ; 
	uint8_t tail;
	uint8_t head;
	uint8_t size;
	uint8_t i;
	
};

struct nmea_b a;

struct bpm_buffor{
	
	uint8_t  buffor [20];
	uint8_t head;
	uint8_t size;
	
};


struct bpm_buffor bpm = {.head=0,.size=0} ;



void addNMEAChar(unsigned char c, struct nmea_b* pbufor){

	if(c == '\n'){
		pbufor->tab[pbufor->head][pbufor->i] = c;
		pbufor->ready[pbufor->head] = true;
		
		pbufor->head ++;
		pbufor->size ++;
		
		if(pbufor->head >9)
		pbufor->head = 0;
		pbufor->i = 0;
	
		
	}
	
	else{
		
		pbufor->tab[pbufor->head][pbufor->i] = c;
		pbufor->i++;
	}
	
}




void ledOn()
{
	PORTB |= _BV(PB7);
}

void ledOff()
{
	PORTB &= ~(_BV(PB7));
}

void ledToggle(){
	PORTB ^= _BV(PB7);
}

void add(struct bpm_buffor * bpm,uint32_t p_milis){
	
	bpm->buffor[bpm->head] = p_milis;
	bpm->head = (++bpm->head) % 20;
	if(bpm->size<20){
		bpm->size++;
		
	}
	
}


/************************************
* Obs³uga przerwania UART 
*************************************/
ISR(USART0_RX_vect){
	
	
	volatile char c;
	c = UDR0;
	
	bufor[buf_pos]=c;
	buf_pos++;
	if(c=='\n')
	{	
		
		bufor[buf_pos-1]=0;
		buf_ready = true;
	}
}


ISR(USART1_RX_vect){
	
	
	char c;
	c = UDR1;
	//UART_Transmit(c);
	if(it<6){
		if(c == rmc[it] ){
			
			nmea_buffer[it] = c;
			it++;
		}
		else
			it = 0;
		
	}
	else{
		
		nmea_buffer[it] = c;
		it++;
		if(c == '\r'){
		//	ledOn();
			nmea_ready = true;
		    nmea_buffer[--it]='\0';
			it = 0 ;
		}
		
	} 
	
	
}

/************************************
* Obs³uga przerwania zewnêtrznego
*************************************/
ISR (INT0_vect)
{
	//if(added = true){
	//	press = 0 ;
	//	added = false;
		
	//}
	//ledOff(); 
	press++;
//	bpm.buffor[bpm.head]  ++ ;
//	UART_SendString("hall\n");
	pressed = 1;
	beatTime = milis ;
	milis = 0 ;
}

/************************************
* Obs³uga przerwania timera
*************************************/
ISR(TIMER0_OVF_vect)
{
	TCNT0 += 6 ;
	milis = milis + 1;
}

ISR(TIMER1_COMPA_vect){
	
	//add(&bpm,press);
	static j = 0 ; 
	bpm.buffor[bpm.head] = press;
	bpm.head = (++bpm.head) % 20;
	if(bpm.size<20){
		bpm.size++;
		
	}
	//bpm.buffor[bpm.head] = 0;
	press = 0 ;
	
//	j++;
	
}

/************************************
* Inicjalizacja timera 
*************************************/
void initTimer()
{
	TCCR0B |= (_BV(CS01) | _BV(CS00));		
	TIMSK0 |= (_BV(TOIE0) );
	TCNT0 = 0x00;	
	
	OCR1A = 15624;
	TCCR1B |= _BV(WGM12);
	TCCR1B |= (_BV(CS10) | _BV(CS12));
	TIMSK1 |= (_BV(OCIE1A) );
	TCNT1 = 0x00;
							
}

void initInternlInterrupt()
{
	EICRA |= (1 << ISC01) ;    
	EIMSK |= (1 << INT0);     
}


void initInput()
{
	DDRD &= ~_BV(PD0);
	PORTD |= _BV(PD0);
}

void ledInit()
{
	DDRB=_BV(PB7);							// ustawienie pinów jako wyjsciowych
}


int getTemp(char* temp, char* hum){
	uint8_t result_bytes[5];
	uint8_t result ;
	uint8_t j,i ;
	DDRB |= _BV(PB2);
	PORTB |= _BV(PB2);
	_delay_ms(100);
	//
	PORTB &= ~ _BV(PB2);
	_delay_ms(18);// start
	
	PORTB |= _BV(PB2);
	DDRB &= ~ _BV(PB2);
	_delay_us(40); // wait for response
	
	if  (bit_is_set(PINB,2)){
		return -1;
	}
	_delay_us(80);
	
	if (bit_is_clear(PINB,2)){
		
		return -1;
		
	}
	_delay_us(80);
	
	
	for (i = 0 ; i < 5 ; i ++){
		result = 0 ;
		for (j = 0 ; j < 8 ; j++)
		{
			while(bit_is_clear(PINB,2));
			_delay_us(30);
			if (bit_is_set(PINB,2)){
				result |= _BV(7-j) ;
			}
			while (bit_is_set(PINB,2));
		}
		result_bytes[i]= result;
	}
	
	itoa (result_bytes[2],temp,10);
	itoa (result_bytes[0],hum,10);

	return 1;
	
}


int main(void)
{	
	uint8_t sum = 0;
	uint8_t i = 0;
	char humidity[10];
	char temperatur [10];
	char write_buffer[100]  ;
	char t[100];
	char t2[100];
	memset(bpm.buffor,0,20);
	UART_Init();
	ledInit();
	initInput();
	initInternlInterrupt();
	initTimer();
	ledOff();
	_delay_ms(1000);
	nmea_ready = false;
	wdt_enable(WDTO_8S);
	sei();
    while(1){
	
		
		/*if(a.ready[a.tail]==true){
			UART_SendString(a.tab[a.tail]);
			a.ready[a.tail] == false;
			tail
		}*/
		wdt_reset();
        if(pressed == 1){
	            
	    //    add(&bpm, beatTime);
    
	        ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
//		        milis -= milis ;
		        TCNT0 = 6;
		        pressed = 0 ;
	        }
	      //  if(bpm.size == 1){
		   //  currentBpm = 0;
		     currentBpm = (60000/ beatTime );   
			
	       // }
        }
		
		if(nmea_ready == true){
			
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		        strcpy((char *)t2,(const char *)nmea_buffer);
	        }
			
			//UART_SendString(t2);
			memset(nmea_buffer,0,100);
			nmea_ready = false;
			wdt_reset();
		}
		
		
		
		
		
		
        
        if(buf_ready == true){
			wdt_reset();
	        strcpy((char *)t,(const char *)bufor);
	        buf_ready= false;
	        buf_pos = 0;
	        
	        if (strcmp((char *)t,"k1")==0)
	        {	
			//	UART_SendString("KADENCJA\n");
		        sprintf(write_buffer,"%d",currentBpm);
		        UART_SendString(write_buffer);
		        
	        }
			
			if (strcmp((char *)t,"k2")==0)
			{
				//UART_SendString("GETTING GPS\n");
			//	GPS_get("GPRMC",nmea_buffer,100);
				if(t2[0] == '$' && NMEA_chcecksum(t2)==true )
					UART_SendString(t2);
				
				
				
			}
			
			if (strcmp((char *)t,"k3")==0)
			{
				getTemp(temperatur,humidity);				
				//GPS_get("GPRMC",nmea_buffer,100);
				sprintf(write_buffer,"%s:%s",temperatur,humidity);
				UART_SendString(write_buffer);
				//UART_SendString(":");
				//UART_SendString(humidity);
				//UART_SendString("\n");
				
			}
			
			if (strcmp((char *)t,"k4")==0)
			{	
				sum = 0;
				for (i = 0 ; i < bpm.size;i++)
				{	
					
					sum+=bpm.buffor[i];
					//sprintf(write_buffer,"%d\n",bpm.buffor[i]);
					//UART_SendString(write_buffer);
				}
				sum*=3;
				sprintf(write_buffer,"%d",sum);
				UART_SendString(write_buffer);
			}
			
			
        }

    }
}