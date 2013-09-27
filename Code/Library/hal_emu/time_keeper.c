/*
 * time_keeper.c
 *
 * Created: 08/06/2012 17:48:13
 *  Author: Felix Schill
 */ 

#include <sys/time.h>
#include "time_keeper.h"


void init_time_keeper() {
//	ast_init_counter(&AVR32_AST, AST_OSC_PB, AST_PRESCALER_SETTING, 0);
//	ast_enable(&AVR32_AST);
}


double get_time(){          // time in seconds since system start
	return ticks_to_seconds(get_time_ticks());
}
uint32_t get_millis() {     //milliseconds since system start
	return get_time_ticks()/1000; /// (TK_AST_FREQUENCY/1000);
}	
uint32_t get_micros() {     // microseconds since system start. Will run over after an hour.
	return get_time_ticks()* (1000000/TK_AST_FREQUENCY);
}	

uint32_t get_time_ticks(){ //raw timer ticks
//	return ast_get_counter_value(&AVR32_AST);
	struct timeval tv;
	
	gettimeofday(&tv,0);
	return tv.tv_sec*1000000+tv.tv_usec;
}

float ticks_to_seconds(uint32_t timer_ticks){
	return ((double)timer_ticks/(double)TK_AST_FREQUENCY);
}
