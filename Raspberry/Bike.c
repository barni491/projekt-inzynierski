#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <json/json.h>
#include <curl/curl.h>
#include <wiringPi.h>
#include <signal.h>
long bytesWritten;
int pid[3];
int run ;

struct gps_point {

	double latitude;
	double longitude;
	double altitude;
	float hdop;
	double sec;
};

struct training{

	double distance;
	float avg_bpm;
	float avg_speed;
	int  duration;
	float climb;
	float downhill ;
	char date[40];
	int id;
	float bpm;

};


struct training_point {
	struct gps_point point;
	int temp;
	int humidity;
	int bpm;
	float speed;
	char time[40];
};

void interrupt_handler(){
	int i = 0;
	digitalWrite(27,LOW);

	printf("Koniec treningu\n");
	 for( i = 0; i < 3; i++)
        {
                kill(pid[i], SIGTERM);
        }
}

strtok_single (char * str, char const * delims)
{
  static char  * src = NULL;
  char  *  p,  * ret = 0;

  if (str != NULL)
    src = str;

  if (src == NULL)
    return NULL;

  if ((p = strpbrk (src, delims)) != NULL) {
    *p  = 0;
    ret = src;
    src = ++p;

  } else if (*src) {
    ret = src;
    src = NULL;
  }

  return ret;
}

int  init_terminal(char* port){
	int fd ;
	struct termios ttyACM;
	fd =  open(port,O_RDWR | O_NOCTTY | O_SYNC);
	if(fd<0){
		perror("Unable to open terminal");
		exit(-1);
	}

	tcgetattr(fd,&ttyACM);
	cfsetospeed (&ttyACM, B9600);
        cfsetispeed (&ttyACM, B9600);

	ttyACM.c_cflag |=CSIZE;
	ttyACM.c_cflag |=CS8;

	ttyACM.c_iflag &= ~IGNBRK;         // disable break processing
        ttyACM.c_lflag = 0;

	ttyACM.c_oflag = 0;                // no remapping, no delays
        ttyACM.c_cc[VMIN]  = 0;            // read doesn't block
        ttyACM.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        ttyACM.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        ttyACM.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        ttyACM.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        ttyACM.c_cflag |= 0;
        ttyACM.c_cflag &= ~CSTOPB;
        ttyACM.c_cflag &= ~CRTSCTS;
 	tcsetattr (fd, TCSANOW, &ttyACM);
	sleep(2);
	return fd ;

}

double distance(double latitude0 ,double longitude0,double latitude1, double longitude1  ){
		const int radius = 6371;
                double dLatitude = (latitude1 - latitude0) * M_PI / 180;
                double dLongitude = (longitude1 - longitude0) * M_PI / 180;

                double lat1 = latitude0 *  M_PI / 180;
                double lat2 = latitude1 *  M_PI / 180;
		double a =  sin(dLatitude/2) * sin(dLatitude/2) + sin(dLongitude/2) * sin(dLongitude/2) * cos(lat1) * cos(lat2);

                double c = 2 * atan2(sqrt(a),sqrt(1-a));
                double distance = radius * c * 1000;
                return distance;

}

void  arduino(char* port){
	char bpm_buf[5];
	char weather_buf [10];
	char nmea_buf[100];
	char write_buf[150];
	int fd ;
	int fifo_id;
	time_t start_time;
	time_t current_time;

	double sec ;

	fd = init_terminal(port);
	fifo_id = open("data_fifo",O_WRONLY);
	if(fifo_id<0){
		perror("unable to open fifo");
        }
	tcflush(fd,TCIOFLUSH);
//	read(fd,bpm_buf, sizeof bpm_buf);

	time(&start_time);
	while(run){
//    		read(fd,bpm_buf, sizeof bpm_buf);
		memset(bpm_buf,'\0',sizeof bpm_buf);
	//	printf("k1\n");
		write(fd,"k1\n",3);
		usleep(20000);
		read(fd,bpm_buf, sizeof bpm_buf);
	//	 printf("%s\n",read_buf);
	//	write(fifo_id,_buf,100);

		memset(nmea_buf,'\0',sizeof nmea_buf);
		write(fd,"k2\n",3);
		usleep(200000);
		read(fd,nmea_buf,sizeof nmea_buf);
	//	write(fifo_id,read_buf,100);
	//	printf("my %s\n",read_buf);
		memset(weather_buf,'\0',sizeof weather_buf);
 		write(fd,"k3\n",3);
		usleep(150000);
               	read(fd,weather_buf, sizeof weather_buf);
       //       write(fifo_id,read_buf,100);
		time(&current_time);
		sec = difftime(current_time,start_time);
		sprintf(write_buf,"%s:%s:%s:%f\0",bpm_buf,nmea_buf,weather_buf,sec);
	//	printf("Dane z arduino: %s\n",write_buf);
		write(fifo_id,write_buf,150);
	//	printf("Przekazane do przetwarzania\n");
		sleep(5);
	}
	printf("Koniec odbioru danych\n");
}

double speed_mps(double distance, double sec){
	//printf("%f ## %f\n",distance,sec);
	if(sec!=0)
		return distance / sec;
	else
		return 0;
}

double nmea_to_dec(double value){
	double dec ;
	double min ;
	int deg ;
	printf("value %f",value);
	deg = (int)(value/100);
	min = (((value/100.0)-(double)deg)*100.)/60.0;
	dec = (double)deg+min;
	return dec;

}

struct gps_point nmea_parse(char * nmea){

	struct gps_point point;
	int i = 0;
	char* word ;
	int lat;

	const char delimiter[2] = ",";

	word = strtok_single(nmea,delimiter);

	while(word!=NULL){

		i++;
		word = strtok_single(NULL,delimiter);

		if(i==2){
			if(!strcmp(word,"")){
				point.latitude = NAN;
                        }
                        else
			point.latitude = nmea_to_dec(strtod(word,NULL));
		}
		else

		if(i==4){
			printf("Word: %s\n",word);
				if(!strcmp(word,"")){
					point.longitude = NAN;
				}
				else
                       		 point.longitude = nmea_to_dec(strtod(word,NULL));
		//	else
		//		point.longitude  = NULL;
		      }

		else
		if(i==8){
                        point.hdop = (atof(word));
                }
		else
                if(i==9){
                        point.altitude = (atof(word));
                }
	}
	return point;
}

void delta_altitude(float* climb , float* downhill , double  altitude0, double altitude1){

	if(altitude1 > altitude0){
		*climb +=  (altitude1  - altitude0);
	}else
		*downhill += (altitude0 - altitude1);

}

double avg_bpm(int bpm){

	static int i = 0;
	static unsigned int sum = 0 ;
	i++;
	sum+=bpm;
	return sum / i ;

}


char* create_json(struct training_point  c_training_point, struct training c_training){

	json_object  * j_training = json_object_new_object();
	json_object  * j_training_point = json_object_new_object();
	 json_object * j_altitude = NULL;
	 json_object * j_longitude = NULL ;
	json_object * j_latitude = NULL;

	if(!isnan(c_training_point.point.altitude) )
		j_altitude = json_object_new_double(c_training_point.point.altitude);
	if(!isnan(c_training_point.point.longitude) ){
		printf("In JSON %f\n",c_training_point.point.longitude);
 		j_longitude = json_object_new_double(c_training_point.point.longitude);
	}
	if(!isnan(c_training_point.point.latitude))
		j_latitude= json_object_new_double(c_training_point.point.latitude);

	json_object * j_bpm = json_object_new_int(c_training_point.bpm);
	json_object * j_temperature = json_object_new_int(c_training_point.temp);
	json_object * j_humidity = json_object_new_int(c_training_point.humidity);
	json_object * j_speed = json_object_new_double(c_training_point.speed);
	json_object * j_time = json_object_new_string(c_training_point.time);

	json_object * j_abpm = json_object_new_double(c_training.avg_bpm);
	json_object * j_aspeed = json_object_new_double(c_training.avg_speed);
	json_object * j_climb = json_object_new_double(c_training.climb);
	json_object * j_downhill = json_object_new_double(c_training.downhill);
	json_object * j_duration = json_object_new_int(c_training.duration);


	json_object * j_date= json_object_new_string(c_training.date);
	json_object * j_distance= json_object_new_double(c_training.distance);
	json_object * j_id= json_object_new_int(c_training.id);

	json_object_object_add(j_training,"avgBpm",j_abpm);
	json_object_object_add(j_training,"avgSpeed",j_aspeed);
	json_object_object_add(j_training,"climb",j_climb);
	json_object_object_add(j_training,"date",j_date);
	json_object_object_add(j_training,"downhill",j_downhill);
	json_object_object_add(j_training,"duration",j_duration);
	json_object_object_add(j_training,"id",j_id);
	json_object_object_add(j_training,"totalDistance",j_distance);

	json_object_object_add(j_training_point,"training",j_training);
	if(j_altitude != NULL)
		json_object_object_add(j_training_point,"altitude",j_altitude);	
	if(j_longitude != NULL)
		json_object_object_add(j_training_point,"longitude",j_longitude);
	if(j_latitude != NULL)
		json_object_object_add(j_training_point,"latitude",j_latitude);

	json_object_object_add(j_training_point,"bpm",j_bpm);
	json_object_object_add(j_training_point,"temperature",j_temperature);
	json_object_object_add(j_training_point,"humidity",j_humidity);
	json_object_object_add(j_training_point,"speed",j_speed);

	json_object_object_add(j_training_point,"time",j_time);
	return json_object_to_json_string(j_training_point);
}

void procissing(int t_id){
	struct training_point c_training_point = {.speed = 0};
	bool has_st_point = false;
	struct gps_point last_point;
	struct gps_point current_point;
	struct training c_training = {.id=t_id,.distance = 0,.climb = 0 ,.downhill = 0};
	char* token;
	char* json;
	int  bpm;
	double speed;
	char* nmea;
	time_t point_time;
	time_t training_time;
	struct tm *local;
	int temp;
	double sec;
	double d_distance;                      // przyrost drogi 
	int fifo_id;
	int json_fifo_id;
	char buf[150];
	char write_buf[400];
	fifo_id = open("data_fifo",O_RDONLY);
	if(fifo_id < 0){
		perror("Unable to open fifo");
        }
	json_fifo_id = open("json_fifo",O_WRONLY);
	
	if(json_fifo_id < 0){
		perror("Unable to open fifo");
        }
	training_time = time(NULL);
        local = localtime(&training_time);
        printf("data\n");
        strftime( c_training.date, sizeof( c_training.date), "%Y-%m-%dT%H:%M:%S.000Z", local);


	while(run){

		memset(buf,0,150);
		read(fifo_id,buf,150);

	//	printf("Odebrano z arduin: %s\n",buf);

		token =strtok_single(buf,":");
		c_training_point.bpm = atoi(token);

		nmea = strtok_single(NULL,":");

		token  = strtok_single(NULL,":");
		c_training_point.temp = atoi(token);

		token  = strtok_single(NULL,":");
		c_training_point.humidity = atoi(token);

		token  = strtok_single(NULL,":");
                sec = atof(token);

		if(strcmp(nmea,"")){

			current_point = nmea_parse(nmea);
			current_point.sec = sec;
			c_training_point .point = current_point;
			if(!has_st_point  && !isnan(current_point.latitude) && !isnan(current_point.longitude))
			{
				last_point = current_point;
				has_st_point = true;
			}
			else
			{
			//printf("%f %f %f %f\n",last_point.latitude,last_point.longitude,current_point.latitude,current_point.longitude);
				if (!isnan(current_point.latitude) && !isnan(current_point.longitude )){       // != NAN && current_point.latitude,current_point.longitude
					d_distance =  distance(last_point.latitude,last_point.longitude,current_point.latitude,current_point.longitude);
					c_training.distance += d_distance;

					c_training_point.speed = speed_mps(d_distance,current_point.sec - last_point.sec);

					delta_altitude(&c_training.climb,&c_training.downhill,last_point.altitude,current_point.altitude);
	
					last_point = current_point;
				}
			}
			//printf("Dis: %f Spe: %f Cli: %f Dow: %f \n",c_training.distance,c_training_point.speed,c_training.climb,c_training.downhill);
		}
		c_training.avg_bpm = avg_bpm(c_training_point.bpm);
		c_training.duration =sec;
		point_time = time(NULL);
		local = localtime(&point_time);

		strftime( c_training_point.time, sizeof( c_training_point.time), "%Y-%m-%dT%H:%M:%S.000Z", local); 
		json = create_json(c_training_point,c_training);
		sprintf(write_buf,"%s\n",json);
		printf(write_buf);
		write(json_fifo_id,write_buf,400);
//		printf("\nPrzetworzono dane\n");
	}
}

static int writer (char *data,size_t size , size_t nmemb, char* buffer_in){

        size_t realsize = size * nmemb;

        if(realsize > 0){
                memcpy(&((char *)buffer_in)[bytesWritten],data,realsize);
                bytesWritten += realsize;
        }
        return realsize;
}


void http_post(CURL *curl,char* text){
	int res;
	struct curl_slist *headers = NULL;
	printf("hello");
	headers = curl_slist_append(headers,"Content-Type:  application/json");
	

	if(curl){
		printf("ifcurl");
		curl_easy_setopt(curl,CURLOPT_URL,
		"http://78.88.254.200:8080/trainingPoint/add");
                curl_easy_setopt(curl,CURLOPT_POST,1);
                curl_easy_setopt(curl,CURLOPT_POSTFIELDS,text);
                curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);

		res = curl_easy_perform(curl);
                if(res != CURLE_OK){
                        printf("error");
                }
                curl_easy_cleanup(curl);
	}
}


void sending(){
	char json[400];
	CURL *curl ;
	int fifo_id;
	int res;
	struct curl_slist *headers = NULL;
	curl = curl_easy_init();
	fifo_id = open("json_fifo",O_RDONLY);
	headers = curl_slist_append(headers,"Content-Type:  application/json");
	curl_easy_setopt(curl,CURLOPT_URL,
		"http://78.88.254.200:8080/trainingPoint/add");
                curl_easy_setopt(curl,CURLOPT_POST,1);
                curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);
	while(run){
		read(fifo_id,json,400);
		//printf(json);
		if(curl){
			curl_easy_setopt(curl,CURLOPT_POSTFIELDS,json);
			do{
				res = curl_easy_perform(curl);
                        	if(res != CURLE_OK){
                                	printf("Blad wysylania danych:%d\n",res);
					sleep(1);
				} else {
					printf("\nWyslano dane\n");
				}

			}while (res !=CURLE_OK && run ==1);
			//res = curl_easy_perform(curl);
		}
	}
}

int main(int argc , char ** argv){
	time_t  start_time;
	char*  start_time_string;
	int status;
	int  fifo_data_id;
	int res;
	int i ;
	int id; 
	char id_buf[10];
	struct sigaction stop_signal;
	stop_signal.sa_handler =interrupt_handler;
	sigaction(SIGINT,&stop_signal,NULL); 
	run = 1;
	wiringPiSetup ();
	pinMode (27, OUTPUT) ;
	digitalWrite(27,HIGH);
	pinMode(26,INPUT);
	pullUpDnControl(26,PUD_UP);
	if(digitalRead(26) == 1){
		printf("pull up");
	}
	printf ("Uruchomiono program treningowy\n");
	wiringPiISR (26, INT_EDGE_FALLING, &interrupt_handler);
	start_time =  time(NULL);
	printf("%ld\n",start_time);
	start_time_string = ctime(&start_time);
	

	printf("Start o: %s\n",start_time_string);
	
	res = open("idfile",O_CREAT | O_RDWR );
	read(res,id_buf,10);
	id = atoi(id_buf) ;
	id++;
	lseek(res,0,0);
	sprintf(id_buf,"%d",id);
	write(res,id_buf,10);
	printf("ID:%d\n",id);

	res = mkfifo("data_fifo",0666);
	if(res <  0){
                perror("Unable to make data_fifo");
        }

	res = mkfifo("json_fifo",0666);
	if(res <  0){
                perror("Unable to make data_fifo");
        }


	if(pid[0]=fork()==0){
		sleep(2);
		printf("Uruchomiono komunikacje z Arduino\n");
		arduino("/dev/ttyACM0");
		exit(0);
	}

	if(pid[1]=fork()==0){
		procissing(id);
                printf("Uruchomiono przetwarzanie danych\n");
                exit(0);
        }

	if(pid[2]=fork()==0){
                printf("Uruchomiono komunikacje z serwerem\n");
		sending();
                exit(0);
        }
	
	for(i = 0 ;i<3;i++)
		wait(&status);

	return 1;

}




