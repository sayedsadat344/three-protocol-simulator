
/* header files for the program */

#include<stdio.h>
#include<math.h>
#include<string.h>
#include<stdlib.h>
#include "lcgrand.h"	
#define Q_LIMIT 10000000	
#define BUSY 1
#define IDLE 0

/* global declarations for the program */

int next_event_type,num_delays_required,num_events,server_status,state_position;
int num_custs_delayed1,num_in_q1,num1=0,num11=	0,packet_tracker1=0;
int num_custs_delayed2,num_in_q2,num2=0,num22 = 0,packet_tracker2=0;
int num_custs_delayed3,num_in_q3,num3=0,num33 = 0,packet_tracker3=0;

float area_server_status,sim_time,time_last_event,time_next_event[12],delay_cust[Q_LIMIT + 1];
float avg_area_queue_one,mean_interarrival1,mean_service1,time_arrival1[Q_LIMIT + 1],time_dep1[Q_LIMIT + 1],total_of_delays1;
float avg_area_queue_two,mean_interarrival2,mean_service2,time_arrival2[Q_LIMIT + 1],time_dep2[Q_LIMIT + 1],total_of_delays2;
float avg_area_queue_three,mean_interarrival3,mean_service3,time_arrival3[Q_LIMIT + 1],time_dep3[Q_LIMIT + 1],total_of_delays3;

float mean_state_change12,new_state_change21,mean_state_change21,mean_state_change23,mean_state_change13,mean_state_change31,mean_state_change32;

FILE *arrival1,*depart1,*delay1,*custsInQ1;
FILE *arrival2,*depart2,*delay2,*custsInQ2;
FILE *arrival3,*depart3,*delay3,*custsInQ3;
FILE *errorFile,*infile11,*infile22,*infile33,*infile123,*outfile,*combineReport,*individualStat,*graph;

/* function declarations for the program */

void initialize(void);
void timing(void);
void update_time_avg_stats(void);
void calculateStat(int count);
void showfile(char filename[100]);
float expon(float mean);

void arrive1Fun(int count_custs);
void depart1Fun(int count);
void arrive2Fun(int count_custs);
void depart2Fun(int count);
void arrive3Fun(int count_custs);
void depart3Fun(int count);


void stateChange12(void);
void stateChange13(void);
void stateChange21(void);
void stateChange23(void);
void stateChange31(void);
void stateChange32(void);

void state1Function(void);
void state2Function(void);
void state3Function(void);

/* main controller */

int main(){
	
	int sim_count = 1, itteration_sim,i,j;
		
	num_events = 12;
		
	// accept numer of time to run the simulator
	printf("\nHow many number of times ? \t");
	scanf("%d",&itteration_sim);
		
	//open input files to read input parameters
	infile11 = fopen("input11.txt","r");
	infile22 = fopen("input22.txt","r");
	infile33 = fopen("input33.txt","r");
	infile123 = fopen("input123.txt","r");
	
	errorFile = fopen("error11.txt","w");
	individualStat = fopen("individualStat.txt","w");
	
	// read input file content and store in variables
	fscanf(infile11,"%f %f %f %f", &mean_interarrival1,&mean_service1,&new_state_change21,&mean_state_change13);
	fscanf(infile22,"%f %f %f %f", &mean_interarrival2,&mean_service2,&mean_state_change21,&mean_state_change23);
	fscanf(infile33,"%f %f %f %f", &mean_interarrival3,&mean_service3,&mean_state_change31,&mean_state_change32);
	fscanf(infile123,"%d",&num_delays_required);
	
	//open different files to stores different values
	arrival1 = fopen("arrival11.txt","w");
	arrival2 = fopen("arrival22.txt","w");
	arrival3 = fopen("arrival33.txt","w");
	
	depart1 = fopen("depart11.txt","w");
	depart2 = fopen("depart22.txt","w");
	depart3 = fopen("depart33.txt","w");
	
	delay1 = fopen("delay11.txt","w");
	delay2 = fopen("delay22.txt","w");
	delay3 = fopen("delay33.txt","w");
	
	custsInQ1 = fopen("custsInQ11.txt","w");
	custsInQ2 = fopen("custsInQ22.txt","w");
	custsInQ3 = fopen("custsInQ33.txt","w");
		
	initialize();
	while((num_custs_delayed1+num_custs_delayed2+num_custs_delayed3) < num_delays_required)
	{	
		timing();		
		update_time_avg_stats();
			
		switch(state_position)
		{	
			case 1:
				state1Function();
				break;
			case 2:
				state2Function();
				break;
			case 3:
				state3Function();
				break;
			}
	}
	
	fprintf(custsInQ1,"\nTotal # custs in Q1 throughout simulation  %f\n",avg_area_queue_one);
	fprintf(custsInQ1,"Total # time  %f\n",sim_time);
	fprintf(custsInQ1,"(Avg # custs in Q1)/T  =  %f\n",avg_area_queue_one/sim_time);
	fclose(custsInQ1);
	fprintf(individualStat,"\n\t AVERAGE NUMBER OF CUSTOMERS IN Q1\t= \t%f\n",avg_area_queue_one/sim_time);	
		
	fprintf(custsInQ2,"\nTotal # custs in Q2 throughout simulation  %f\n",avg_area_queue_two);
	fprintf(custsInQ2,"Total # time  %f\n",sim_time);
	fprintf(custsInQ2,"(Avg # custs in Q2)/T  =  %f\n",avg_area_queue_two/sim_time);
	fclose(custsInQ2);
	fprintf(individualStat,"\n\t AVERAGE NUMBER OF CUSTOMERS IN Q2\t= \t%f\n",avg_area_queue_two/sim_time);
	
	fprintf(custsInQ3,"\nTotal # custs in Q3 throughout simulation  %f\n",avg_area_queue_three);
	fprintf(custsInQ3,"Total # time  %f\n",sim_time);
	fprintf(custsInQ3,"(Avg # custs in Q3)/T  =  %f\n",avg_area_queue_three/sim_time);
	fclose(custsInQ3);
	fprintf(individualStat,"\n\t AVERAGE NUMBER OF CUSTOMERS IN Q3\t= \t%f\n",avg_area_queue_three/sim_time);	
	
	fprintf(delay1,"\nTotal # delay1  %f\n",total_of_delays1);
	fprintf(delay1,"(Avg # delay1)/N  =  %f\n",total_of_delays1/num_custs_delayed1);
	fclose(delay1);
	fprintf(individualStat,"\n\t AVERAGE DELAY IN QUEUE 1\t =  \t\t%f\n",total_of_delays1/num_custs_delayed1);
	
	fprintf(delay2,"\nTotal # delay2  %f\n",total_of_delays2);
	fprintf(delay2,"(Avg # delay2)/N  =  %f\n",total_of_delays2/num_custs_delayed2);
	fclose(delay2);
	fprintf(individualStat,"\n\t AVERAGE DELAY IN QUEUE 2\t =  \t\t%f\n",total_of_delays2/num_custs_delayed2);
	
	fprintf(delay3,"\nTotal # delay3  %f\n",total_of_delays3);
	fprintf(delay3,"(Avg # delay3)/N  =  %f\n",total_of_delays3/num_custs_delayed3);
	fclose(delay3);
	fprintf(individualStat,"\n\t AVERAGE DELAY IN QUEUE 3\t =  \t\t%f\n",total_of_delays3/num_custs_delayed3);
		
	fclose(arrival1);
	fclose(arrival2);
	fclose(arrival3);
	fclose(depart1);
	fclose(depart2);
	fclose(depart3);
	fclose(individualStat);
	
	while(sim_count<=itteration_sim)
	{
			//output file to store the statistics of the simulation
			outfile = fopen("output11.txt","w");
			
			calculateStat(sim_count);
		
			fclose(outfile);	
			
			printf("\n\t\t Round    \t%d\t ..............",sim_count);
			printf("\n\t\t ....................................................");
			sim_count++;
	}
	
	outfile = fopen("output11.txt","a");
	fprintf(outfile,"\n\tTotal customers served  =  %d\n",itteration_sim*num_delays_required);
	fclose(outfile);	
	
	printf("\n Arrival1 of Custs: \n\n");
	showfile("arrival11.txt");
	printf("\n\n Departure from queue 2 of Custs\n\n");
	showfile("depart22.txt");
		
	printf("\n Arrival22 of Custs\n\n");
	showfile("arrival22.txt");
	printf("\n\n Departure from queue 3 of Custs \n\n");
	showfile("depart33.txt");

	printf("\n Arrival33 of Custs:\n\n");
	showfile("arrival33.txt");
	printf("\n\n Departure from queue 1 of Cust\n\n");
	showfile("depart11.txt");
		
	printf("\n\n # custs * time in Q1: Round\n\n");
	showfile("custsInQ11.txt");
	printf("\n\n # custs * time in Q2: Round \n\n");
	showfile("custsInQ22.txt");
	printf("\n\n # custs * time in Q3: Round \n\n");
	showfile("custsInQ33.txt");
		
	printf("\n\n # delay in queue 1\n\n");
	showfile("delay11.txt");
	printf("\n\n # delay in queue 2:\n\n");
	showfile("delay22.txt");
	printf("\n\n # delay in queue 3\n\n");
	showfile("delay33.txt");
	
	printf("\n\n Simulation OutPut file content \n\n");
	showfile("output11.txt");
		
	fclose(infile11);
	fclose(infile22);
	fclose(infile33);
	fclose(infile123);
	
	return 0;
}

void state1Function()
{	
	switch(next_event_type)
	{			
		case 1:
			arrive1Fun(num1);
			num1++;
			break;
		case 2:
			state_position = 2;
			break;
		case 3:
			state_position = 3;
			break;
		case 4:
			depart1Fun(num11);
			num11++;
			break;
		case 5:
			state_position = 2;
			break;
		case 6:
			state_position = 3;
			break;
		case 7:
			stateChange12();
			state_position = 2;
			break;
		case 8:
			stateChange13();
			state_position = 3;
			break;
		case 9:
			state_position = 2;
			break;
		case 10:
			state_position = 2;
			break;
		case 11:
			state_position = 3;
			break;
		case 12:
			state_position = 3;
			break;		
	}
}

void state2Function()
{	
	switch(next_event_type)
	{			
		case 1:
			state_position = 1;
			break;
		case 2:
			arrive2Fun(num2);
			num2++;
			break;	
		case 3:
			state_position = 3;
			break;
		case 4:
			state_position = 1;
			break;
		case 5:
			depart2Fun(num22);
			num22++;
			break;
		case 6:
			state_position = 3;
			break;
		case 7:
			state_position = 1;
			break;
		case 8:
			state_position = 1;
			break;
		case 9:
			stateChange21();
			state_position = 1;
			break;
		case 10:
			stateChange23();
			state_position = 3;
			break;
		case 11:
			state_position = 3;
			break;
		case 12:
			state_position = 3;
			break;		
	}	
}

void state3Function()
{	
	switch(next_event_type)
	{			
		case 1:
			state_position = 1;
			break;	
		case 2:
			state_position = 2;
			break;
		case 3:
			arrive3Fun(num3);
			num3++;
			break;
		case 4:
			state_position = 1;
			break;
		case 5:
			state_position = 2;
			break;
		case 6:
			depart3Fun(num33);
			num33++;
			break;
		case 7:
			state_position = 1;
			break;
		case 8:
			state_position = 1;
			break;
		case 9:
			state_position = 2;
			break;
		case 10:
			state_position = 2;
			break;
		case 11:
			stateChange31();
			state_position = 1;
			break;
		case 12:
			stateChange32();
			state_position = 2;
			break;
	}	
}

void initialize(){

	state_position = 1;
	sim_time= 0.0;
	server_status = IDLE;
	area_server_status = 0.0;
	time_last_event = 0.0;
	
	num_in_q1 = 0;
	num_in_q2 = 0;
	num_in_q3 = 0;	
		
	num_custs_delayed1 = 0;
	num_custs_delayed2 = 0;
	num_custs_delayed3 = 0;
	
	total_of_delays1 = 0.0;
	total_of_delays2 = 0.0;
	total_of_delays3 = 0.0;
		
	avg_area_queue_one = 0.0;
	avg_area_queue_two = 0.0;
	avg_area_queue_three = 0.0;
	
	//since there are no customers , departure cant happen only switching and arrivals are possible
	// therefore departures are given long time
	time_next_event[1] = sim_time + expon(mean_interarrival1);
	time_next_event[2] = sim_time + expon(mean_interarrival2);
	time_next_event[3] = sim_time + expon(mean_interarrival3);
	
	time_next_event[4] = 1.0e+30;
	time_next_event[5] = 1.0e+31;
	time_next_event[6] = 1.0e+32;
	
	time_next_event[7] = sim_time + expon(new_state_change21);
	time_next_event[8] = sim_time + expon(mean_state_change13);
	time_next_event[9] = sim_time + expon(mean_state_change21);
	time_next_event[10] = sim_time + expon(mean_state_change23);
	time_next_event[11] = sim_time + expon(mean_state_change31);
	time_next_event[12] = sim_time + expon(mean_state_change32);	
 }
 
 
 void timing(){

 	int i;
 	float min_time_next_event = 1.0e+29;
 	
 	next_event_type = 0;
 	
 	for(i=1;i<= num_events;++i){
 		
 		if(time_next_event[i] < min_time_next_event){
 			min_time_next_event = time_next_event[i];
 			next_event_type = i;	
		 }
	 }
 	
 	if(next_event_type == 0){
 		
 		fprintf(errorFile,"\n event list empty at time %f",sim_time);
 		exit(1);
	 }
	 
	 sim_time = min_time_next_event;
 }
 
 void update_time_avg_stats(){

 	float time_since_last_event;
 	
 	time_since_last_event = sim_time - time_last_event;
 	time_last_event = sim_time;
 	
	fprintf(custsInQ1,"%d    *    %f    =   %f\n",num_in_q1,time_since_last_event,num_in_q1*time_since_last_event);
	fprintf(custsInQ2,"%d    *    %f    =   %f\n",num_in_q2,time_since_last_event,num_in_q2*time_since_last_event); 
 	fprintf(custsInQ3,"%d    *    %f    =   %f\n",num_in_q3,time_since_last_event,num_in_q3*time_since_last_event);
 	
 	avg_area_queue_one += num_in_q1 * time_since_last_event;
 	avg_area_queue_two += num_in_q2 * time_since_last_event;
 	avg_area_queue_three += num_in_q3 * time_since_last_event;
 
 	area_server_status += server_status*time_since_last_event;
 }
 
 
 void arrive1Fun(int count_custs){
 	
 	float delay;
	
 	time_next_event[1] = sim_time + expon(mean_interarrival1);
 	
 	if(server_status == BUSY)
	 {		
 		++num_in_q2;
 		
 		if(num_in_q2 > Q_LIMIT)
		 {		
 				fprintf(errorFile,"\n overflow of the array time_arrival at time %f",sim_time);	
		 }
	 }
	 
	 else
	 {	
	 	delay = 0.0;
	    fprintf(delay2,"\n delay2 -> %f",delay);
	    packet_tracker2++;
	 	total_of_delays2 += delay;
	 	
	 	time_dep2[num_custs_delayed2] = sim_time;
	  	fprintf(depart2,"%f\n",time_dep2[num_custs_delayed2]);
	 	
	 	++num_custs_delayed2;
	 	server_status = BUSY;
	 	
		time_next_event[2] = sim_time + expon(mean_interarrival2);
		time_next_event[3] = sim_time + expon(mean_interarrival3);
		time_next_event[4] = sim_time + expon(mean_service1);
		time_next_event[5] = sim_time + expon(mean_service2);
		time_next_event[6] = sim_time + expon(mean_service3);
		time_next_event[7] = sim_time + expon(new_state_change21);
		time_next_event[8] = sim_time + expon(mean_state_change13);
		time_next_event[9] = sim_time + expon(mean_state_change21);
		time_next_event[10] = sim_time + expon(mean_state_change23);
		time_next_event[11] = sim_time + expon(mean_state_change31);
		time_next_event[12] = sim_time + expon(mean_state_change32);
	 }
	 
	 time_arrival1[count_custs] = sim_time;
	 fprintf(arrival1,"%f\n",time_arrival1[count_custs]);
 }
 
 
  void arrive2Fun(int count_custs){
 	
 	float delay;
 	
 	time_next_event[2] = sim_time + expon(mean_interarrival2);
 	
 	if(server_status == BUSY)
	 {
 		++num_in_q3;
 		
 		if(num_in_q3 > Q_LIMIT)
		 {		
 			fprintf(outfile,"\n overflow of the array time_arrival at time %f",sim_time);			
		 }
	 }
	 
	 else
	 {	
	 	delay = 0.0;
	 	fprintf(delay3,"\n delay -> %f",delay);
	 	packet_tracker3++;
	 	total_of_delays3 += delay;

	 	time_dep3[num_custs_delayed3] = sim_time;
	    fprintf(depart3,"%f\n",time_dep3[num_custs_delayed3]);
	 	
	 	++num_custs_delayed3;
	 	server_status = BUSY;
	 	
		time_next_event[1] = sim_time + expon(mean_interarrival1);
		time_next_event[3] = sim_time + expon(mean_interarrival3);
		time_next_event[4] = sim_time + expon(mean_service1);
		time_next_event[5] = sim_time + expon(mean_service2);
		time_next_event[6] = sim_time + expon(mean_service3);	
		time_next_event[7] = sim_time + expon(new_state_change21);
		time_next_event[8] = sim_time + expon(mean_state_change13);	
		time_next_event[9] = sim_time + expon(mean_state_change21);
		time_next_event[10] = sim_time + expon(mean_state_change23);
		time_next_event[11] = sim_time + expon(mean_state_change31);
		time_next_event[12] = sim_time + expon(mean_state_change32);
	 }
	 
	time_arrival2[count_custs] = sim_time;
	fprintf(arrival2,"%f\n",time_arrival2[count_custs]); 
 }
 
void arrive3Fun(int count_custs){
 	
 	float delay;
 	
 	time_next_event[3] = sim_time + expon(mean_interarrival3);
 	
 	if(server_status == BUSY)
	 {
 		++num_in_q1;
 		
 		if(num_in_q1 > Q_LIMIT)
		 {		
 			fprintf(outfile,"\n overflow of the array time_arrival at time %f",sim_time);
		 }
	 }
	 else
	 {	
	 	delay = 0.0;
	 	fprintf(delay1,"\n delay -> %f",delay);
	 	printf("\n\t D1 -> %f   ",total_of_delays1);
	 	
	 	packet_tracker1++;
	 	total_of_delays1 += delay;
	 	
	 	time_dep1[num_custs_delayed1] = sim_time;
	    fprintf(depart1,"%f\n",time_dep1[num_custs_delayed1]);
	 	
	 	++num_custs_delayed1;
	 	server_status = BUSY;
	 	
	 	time_next_event[1] = sim_time + expon(mean_interarrival1);
		time_next_event[2] = sim_time + expon(mean_interarrival2);
		time_next_event[4] = sim_time + expon(mean_service1);
		time_next_event[5] = sim_time + expon(mean_service2);
		time_next_event[6] = sim_time + expon(mean_service3);
		time_next_event[7] = sim_time + expon(new_state_change21);
		time_next_event[8] = sim_time + expon(mean_state_change13);
		time_next_event[9] = sim_time + expon(mean_state_change21);
		time_next_event[10] = sim_time + expon(mean_state_change23);
		time_next_event[11] = sim_time + expon(mean_state_change31);
		time_next_event[12] = sim_time + expon(mean_state_change32);
	 }

	time_arrival3[count_custs] = sim_time;
	fprintf(arrival3,"%f\n",time_arrival3[count_custs]); 
 } 
 
 void depart1Fun(int count_custs){
 	
 	int i;
 	float delay;
 	
 	if(num_in_q2 == 0){
 		
 		server_status = IDLE;
 		time_next_event[4] = 1.0e+30;
	 }
	 else{
	 	
	 	--num_in_q2;
	 	delay = sim_time - time_arrival1[packet_tracker2];
	 	packet_tracker2++;
	 	total_of_delays2 += delay;

	 	fprintf(delay2,"\n delay -> %f",delay);
	 	time_dep2[num_custs_delayed2] = sim_time;

		fprintf(depart2,"%f\n",time_dep2[num_custs_delayed2]);
	 	++num_custs_delayed2;
	 	
	 	time_next_event[4] = sim_time + expon(mean_service1);
	
	 	for(i=0; i<num_in_q2; i++){
	 		
	 		time_arrival1[i] = time_arrival1[++i]; 		
		 }
	 }
 }
 
 void depart2Fun(int count_custs){
 	
 	int i;
 	float delay;
 	
 	if(num_in_q3 == 0){
 		
 		server_status = IDLE;
 		time_next_event[5] = 1.0e+30;	
	 }
	 else{
	 	
	 	--num_in_q3;
	 	delay = sim_time - time_arrival2[packet_tracker3];
	 	total_of_delays3 += delay;
	 	packet_tracker3++;
	 	fprintf(delay3,"\n delay -> %f",delay);
	 	
	 	time_dep3[num_custs_delayed3] = sim_time;
		fprintf(depart3,"%f\n",time_dep3[num_custs_delayed3]);
		
	 	++num_custs_delayed3;
	 	time_next_event[5] = sim_time + expon(mean_service2);
	 	
	 	for(i=0; i< num_in_q3; i++){
	 		
	 		time_arrival2[i] = time_arrival2[++i];	
		 }
	 }
 }
 
 
 void depart3Fun(int count_custs){
 	
 	int i;
 	float delay;
 	
 	if(num_in_q1 == 0){
 		
 		server_status = IDLE;
 		time_next_event[6] = 1.0e+30;
	 }
	 else{
	 	
	 	--num_in_q1;
	 	delay = sim_time - time_arrival3[packet_tracker1];
	 	total_of_delays1 += delay;
	 	packet_tracker1++;
	 	fprintf(delay1,"\n delay -> %f",delay);
	 	
	 	time_dep1[num_custs_delayed1] = sim_time;
		fprintf(depart1,"%f\n",time_dep1[num_custs_delayed1]);
		
	 	++num_custs_delayed1;
	 	time_next_event[6] = sim_time + expon(mean_service3);
	 	
	 	for(i=0; i< num_in_q1; i++){
	 		
	 		time_arrival3[i] = time_arrival3[++i];	
		 }
	 }
	 
 }
 
 
void stateChange12(){
		
	time_next_event[7] = sim_time + expon(new_state_change21);
}

void stateChange13(){
		
	time_next_event[8] = sim_time + expon(mean_state_change13);
}

void stateChange21(){
		
	time_next_event[9] = sim_time + expon(mean_state_change21);
}


void stateChange23(){
		
	time_next_event[10] = sim_time + expon(mean_state_change23);
}
void stateChange31(){
		
	time_next_event[11] = sim_time + expon(mean_state_change31);
}

void stateChange32(){
		
	time_next_event[12] = sim_time + expon(mean_state_change32);
}

float expon(float mean){
 	
 	return -mean*log(lcgrand(1));
 }
 
 
 void showfile(char filename[100]){
 	
 	char str[1000];
 	char* pend;
 	
 	FILE *file;
 	int i =0,j=0;
 	float value;
 	file = fopen(filename,"r");
 	
 	while( fgets (str, 100, file)!=NULL){

		printf("%s",str);
		
	}
	
	fclose(file);
 }
 
 void calculateStat(int count){
	
	fprintf(outfile,"\n\tSIMULATION REPORT Round          \t\t%d",count);
	fprintf(outfile,"\n\tMEAN INTERARRIVAL TYPE 1         \t\t%f", mean_interarrival1);
	fprintf(outfile,"\n\tMEAN INTERARRIVAL TYPE 2         \t\t%f", mean_interarrival2);
	fprintf(outfile,"\n\tMEAN INTERARRIVAL TYPE 3         \t\t%f", mean_interarrival3);
	
	fprintf(outfile,"\n\tMEAN SERVICE TIME TYPE 1         \t\t%f",mean_service1);
	fprintf(outfile,"\n\tMEAN SERVICE TIME TYPE 2         \t\t%f",mean_service2);
	fprintf(outfile,"\n\tMEAN SERVICE TIME TYPE 3         \t\t%f",mean_service3);
	
	fprintf(outfile,"\n\tMEAN SWITCHING OF TYPE 1-2       \t\t%f",new_state_change21);
	fprintf(outfile,"\n\tMEAN SWITCHING OF TYPE 1-3       \t\t%f",mean_state_change13);
	fprintf(outfile,"\n\tMEAN SWITCHING OF TYPE 2-1       \t\t%f",mean_state_change21);
	fprintf(outfile,"\n\tMEAN SWITCHING OF TYPE 2-3       \t\t%f",mean_state_change23);
	fprintf(outfile,"\n\tMEAN SWITCHING OF TYPE 3-1       \t\t%f",mean_state_change31);
	fprintf(outfile,"\n\tMEAN SWITCHING OF TYPE 3-2       \t\t%f",mean_state_change32);
	
	fprintf(outfile,"\n\n\t **************** COMBINE QUEUES STATISTICS ***********************\n");
	
	fprintf(outfile,"\n\tAVERAGE DELAY IN ALL QUEUES      \t\t%f",(total_of_delays1 + total_of_delays2+total_of_delays3)/(num_custs_delayed1+num_custs_delayed2+num_custs_delayed3));
	fprintf(outfile,"\n\tAVERAGE DELAY IN SYSTEM      	  \t\t%f",((total_of_delays1+total_of_delays2+total_of_delays3)/(num_custs_delayed1+num_custs_delayed2+num_custs_delayed3))+((mean_service1+mean_service2+mean_service3)/3));
 	fprintf(outfile,"\n\tAVERAGE CUSTOMERS IN ALL QUEUE   \t\t%f",(avg_area_queue_one+avg_area_queue_two+avg_area_queue_three)/sim_time);
 	fprintf(outfile,"\n\tSERVER UTILIZATION (PERCENT) 	  \t\t%f%%",(area_server_status/sim_time)*100);
 	fprintf(outfile,"\n\tTOTAL SIMULATION TIME            \t\t%f",sim_time);
 	
 	fprintf(outfile,"\n\n\t **************** INDIVIDUAL QUEUE QUEUES STATISTICS ***********************\n");
 	
 	fprintf(outfile,"\n\t AVERAGE DELAY IN QUEUE 1\t =  \t\t%f\n",total_of_delays1/num_custs_delayed1);
	fprintf(outfile,"\n\t AVERAGE DELAY IN QUEUE 2\t =  \t\t%f\n",total_of_delays2/num_custs_delayed2);
	fprintf(outfile,"\n\t AVERAGE DELAY IN QUEUE 3\t =  \t\t%f\n",total_of_delays3/num_custs_delayed3);
	fprintf(outfile,"\n\t AVERAGE NUMBER OF CUSTOMERS IN Q1\t= \t%f\n",avg_area_queue_one/sim_time);
	fprintf(outfile,"\n\t AVERAGE NUMBER OF CUSTOMERS IN Q2\t= \t%f\n",avg_area_queue_two/sim_time);
	fprintf(outfile,"\n\t AVERAGE NUMBER OF CUSTOMERS IN Q3\t= \t%f\n",avg_area_queue_three/sim_time);
	
	
 	
 }

 
