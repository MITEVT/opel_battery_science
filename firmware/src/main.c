#include "board.h"
#include "BQ7695.h"

// -------------------------------------------------------------
// Macro Definitions
#define UART_BAUD_RATE 57600 					// Desired UART Baud Rate

#define BUFFER_SIZE 8

// -------------------------------------------------------------
// Static Variable Declaration

extern volatile uint32_t msTicks;

//static CCAN_MSG_OBJ_T msg_obj; 					// Message Object data structure for manipulating CAN messages
//static RINGBUFF_T can_rx_buffer;				// Ring Buffer for storing received CAN messages
//static CCAN_MSG_OBJ_T _rx_buffer[BUFFER_SIZE]; 	// Underlying array used in ring buffer

//static char str[100];							// Used for composing UART messages
static uint8_t uart_rx_buffer[BUFFER_SIZE]; 	// UART received message buffer

//static bool can_error_flag;
//static uint32_t can_error_info;

static uint32_t lastPrint;
static uint16_t cellVolts[8];
static uint8_t data[10];
static int8_t gc[7];
static int8_t oc[7];

// -------------------------------------------------------------
// Helper Functions

/**
 * Delay the processor for a given number of milliseconds
 * @param ms Number of milliseconds to delay
 */
void _delay(uint32_t ms) {
	uint32_t curTicks = msTicks;
	while ((msTicks - curTicks) < ms);
}

static inline void print_cells(void){
	uint8_t i = 1;
	while(i<7){
		Board_UART_Print("[Cell Voltage ");
		Board_UART_PrintNum(i,10,false);
		Board_UART_Print(": ");

//		Board_UART_PrintNum(((((3.3*cellVolts[i++]*1000)/1023)+oc[i])*(1+gc[i]))/gc[i],10,false);
		Board_UART_PrintNum((3.3*cellVolts[i++]*1000)/1023, 10, false);
		Board_UART_Println("]");
	}
}
// -------------------------------------------------------------
// Interrupt Service Routines

void I2C_IRQHandler(void){
	if(Chip_I2C_IsMasterActive(I2C0)){
		Chip_I2C_MasterStateHandler(I2C0);
	}
	else{
		Chip_I2C_SlaveStateHandler(I2C0);
	}
}


// -------------------------------------------------------------
// Main Program Loop

int main(void)
{

	//---------------
	// Initialize UART Communication
	Board_UART_Init(UART_BAUD_RATE);
	Board_UART_Println("Started up");
//	Board_UART_PrintNum(&msTicks,16,true);
//	int i=0;	
//	while(i<8){
//		Board_UART_PrintNum(&(cellVolts[i++]),16,true);
//	}
	//---------------
	// Initialize SysTick Timer to generate millisecond count
	if (Board_SysTick_Init()) {
		Board_UART_Println("Failed to Initialize SysTick. ");
		// Unrecoverable Error. Hang.
		while(1);
	}

	//---------------
	// Initialize GPIO and LED as output
	Board_LEDs_Init();
	Board_LED_On(LED0);


	//---------------
	// Initialize Things relevant to taking data
	Board_I2C_Init();
	Board_ADC_Init();
	
	BQ7695_disable_unecessary();	
	BQ7695_setup_corrections(gc,oc);

	int i = 0;
	while(i<7){
		Board_UART_PrintNum(gc[i],10,true);
		Board_UART_PrintNum(oc[i++],10,true);
	}

	msTicks = 1000;
	lastPrint = msTicks;

	while (1) {

		if(lastPrint < msTicks-1000){
			lastPrint = msTicks;
			BQ7695_read_cells(cellVolts);	
			Board_UART_PrintNum(msTicks,10,true);
			print_cells();
		}
		uint8_t count;
		if ((count = Board_UART_Read(uart_rx_buffer, BUFFER_SIZE)) != 0) {
			Board_UART_SendBlocking(uart_rx_buffer, count); // Echo user input
			switch (uart_rx_buffer[0]) {
				case 'a':


					break;
				default:
					//Board_UART_Println("Invalid Command");
					break;
			}
		}
	}
}
