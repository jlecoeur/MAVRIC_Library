/*
 * compass_hmc5883.c
 *
 * Created: 12/03/2013 20:51:33
 *  Author: sfx
 */ 
#include "compass_hmc5883l.h"
//#include "twim.h"

static volatile compass_data compass_outputs;




void init_hmc5883_slow() {
	print_util_dbg_print("not implemented\n");
}


compass_data* get_compass_data_slow() {
	return &compass_outputs;

}