/*
 * boardsupport.c
 *
 * Created: 20/03/2013 12:14:18
 *  Author: sfx
 */ 

#include "boardsupport.h"
#include "sysclk.h"
#include "sleepmgr.h"
#include "time_keeper.h"
#include "delay.h"


boardsupport_init(central_data_t *central_data){
		
		irq_initialize_vectors();
		cpu_irq_enable();
		Disable_global_interrupt();
			
		// Initialize the sleep manager
		sleepmgr_init();
		sysclk_init();

		board_init();
		delay_init(sysclk_get_cpu_hz());
		init_time_keeper();
			
		INTC_init_interrupts();

		/*
		if (init_i2c(1)!=STATUS_OK) {
			//print_util_putstring(STDOUT, "Error initialising I2C\n");
			while (1==1);
			} else {
			//print_util_putstring(STDOUT, "initialised I2C.\n");
		};
		i2c_slave_interface_init(1);
		*/
		
		init_UART_int(4);

		
		
		
		register_write_stream(get_UART_handle(4), &central_data->wired_out_stream);

		buffer_make_buffered_stream(&(central_data->wired_in_buffer), &(central_data->wired_in_stream));
		register_read_stream(get_UART_handle(4), &(central_data->wired_in_stream));

		central_data->telemetry_down_stream=&(central_data->wired_out_stream);
		central_data->telemetry_up_stream  =&(central_data->wired_in_stream);
		
		central_data->debug_out_stream=&central_data->wired_out_stream;

		// init mavlink
		mavlink_stream_init(central_data->telemetry_down_stream, central_data->telemetry_up_stream, 100);
		print_util_dbg_print_init(central_data->debug_out_stream);
		
		Init_DAC(0);
		DAC_set_value(0);
		Enable_global_interrupt();
}




