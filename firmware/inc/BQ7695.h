#ifndef BQ7695
#define BQ7695
#include <chip.h>

#define I2C_ADDRESS_DEVICE		0x04

//-------------------------------------------------------------
// I2C registers and masks
// Includes descriptions of them all, for your convenience
#define I2C_ADDRESS_STATUS 		0x00
// Various Status indications

#define I2C_MASK_STATUS_ALERT		0x04
// Becomes a "1" if over-current is detected
#define I2C_MASK_STATUS_CRC_ERR		0x02
// Updates on write when checksums are enabled
// Becomes a "1" if there is a CRC error
#define I2C_MASK_STATUS_POR		0x01		
// Sets to a "1" upon power-up, reset, or wake from sleep
// can be written to 0 to clear the bit

#define I2C_ADDRESS_CELL_CTL		0x01
//	Controls the internal Muxes
#define I2C_MASK_CELL_CTL_CELL_SEL	0X08
//	Selects the cell to read
#define I2C_MASK_CELL_CTL_VCOUT_SEL	0X30
//	Selects the output (ex. Vss, Vcell, Vref)

#define I2C_ADDRESS_BAL_CTL		0x02
//	Enables balance transistors

#define I2C_ADDRESS_CONFIG_1		0x03
//	Many configurations relating to current
#define I2C_MASK_CONFIG_1_I_GAIN	0X01
//	Sets the current gain: 0 gives a gain of 4, 1 gives a gain of 8

#define I2C_MASK_CONFIG_1_I_AMP_CAL	0X04
// Current amplifier calibration 0 reports SENSEN, 1 reports SENSEP
// I don't fully understand this one

#define I2C_MASK_CONFIG_1_I_COMP_POL	0X08
// Sets the polarity fo the current comparator
// 0 triggers with discharging, 1 triggers when charging

#define I2C_MASK_CONFIG_1_I_THRESH	0XF0
// Sets the current comparators threshhold
// the threshold is (val+1)*25mV


#define I2C_ADDRESS_CONFIG_2		0x04
// A couple additional config params

#define I2C_MASK_CONFIG_2_REF_SEL	0x01
// Selects reference voltage, which controls
// VREF output, VCOUT gain, and VIOUT Output Range
// Default 0: 1.5V, gain 0.3, 0.25-1.25V
// with a 1:  3.0V, gain 0.6, 0.5-2.5V

#define I2C_MASK_CONFIG_2_CRC_EN	0x80
// CRC Comparison enable
// CRC is a checksum

#define I2C_ADDRESS_POWER_CTL		0x05

#define I2C_MASK_POWER_CTL_SLEEP	0x80
// Writing a "1" puts the device to sleep
#define I2C_MASK_POWER_CTL_SLEEP_DIS	0x40
// Writing a "1" disables sleep mode
#define I2C_MASK_POWER_CTL_I_COMP_EN	
// Writing a "1" enables the current comparator
#define I2C_MASK_POWER_CTL_I_AMP_EN
// Writing a "1" enables the current amplifier
#define I2C_MASK_POWER_CTL_VC_AMP_EN
// Writing a "1" enables the cell amplifer
#define I2C_MASK_POWER_CTL_VTB_EN
// Writing a "1" enables the thermistor bias
#define I2C_MASK_POWER_CTL_REF_EN
// Writing a "1" enables the voltage reference

#define I2C_ADDRESS_CHIP_ID		0x07
// Contains the Chip ID
#define I2C_MASK_CHIP_ID		0xFF
// The chip ID

#define I2C_ADDRESS_VREF_CAL		0x10
#define I2C_ADDRESS_VC1_CAL		0x11
#define I2C_ADDRESS_VC2_CAL		0x12
#define I2C_ADDRESS_VC3_CAL		0x13
#define I2C_ADDRESS_VC4_CAL		0x14
#define I2C_ADDRESS_VC5_CAL		0x15
#define I2C_ADDRESS_VC6_CAL		0x16

#define I2C_MASK_VC_CAL_VC_OFFSET_CORR `0xF0
// The offset correction factor, lower four bits
// Full value is 5 bits, signed, 6 bits for VREF
#define I2C_MASK_VC_CAL_VC_GAIN_CORR	0x0F
// The gain correction factor, lower four bits
// Full value is 5 bits, signed

#define I2C_ADDRESS_VC_CAL_EXT_1	0x17
// Most significant bits for for the offset correction factors
// These correspond to the signs of the values

#define I2C_MASK_VC_CAL_EXT_1_VC1_OC_4	0x80
#define I2C_MASK_VC_CAL_EXT_1_VC1_GC_4	0x40
#define I2C_MASK_VC_CAL_EXT_1_VC2_OC_4	0x20
#define I2C_MASK_VC_CAL_EXT_1_VC2_GC_4	0x10


#define I2C_ADDRESS_VC_CAL_EXT_2	0x18

#define I2C_MASK_VC_CAL_EXT_2_VC3_OC_4	0x80
#define I2C_MASK_VC_CAL_EXT_2_VC3_GC_4	0x40
#define I2C_MASK_VC_CAL_EXT_2_VC4_OC_4	0x20
#define I2C_MASK_VC_CAL_EXT_2_VC4_GC_4	0x10
#define I2C_MASK_VC_CAL_EXT_2_VC5_OC_4	0x08
#define I2C_MASK_VC_CAL_EXT_2_VC5_GC_4	0x04
#define I2C_MASK_VC_CAL_EXT_2_VC6_OC_4	0x02
#define I2C_MASK_VC_CAL_EXT_2_VC6_GC_4	0x01


#define I2C_ADDRESS_VREF_CAL_EXT	0x1B
// Most significant bits for the vref correction factors
#define I2C_MASK_VREF_CAL_EXT_VREF_GC_5	0x04
#define I2C_MASK_VREF_CAL_EXT_VREF_OC_4	0x02
#define I2C_MASK_VREF_CAL_EXT_VREF_GC_4	0x01

//---------------------------------------------------
// Helper macros for I2C with this chip
#define BQ7695_Read(addr, buff)		Chip_I2C_MasterRead(I2C0, ((I2C_ADDRESS_DEVICE<<3)|(addr)),buff,1)
#define BQ7695_Write(addr, buff)	Chip_I2C_MasterSend(I2C0, ((I2C_ADDRESS_DEVICE<<3)|(addr)),buff,1) 

//----------------------------------------------------
// Helpful project relevent functions

// Sets the cell output
// @param cell Accepts a number between 0 and 7
// Selects the cell who's output will be read by the ADC
uint8_t BQ7695_set_cell(uint8_t cell);

// Disables all of the unecessary features of the chip
// Examples include sleep mode
// Please adjust the function if you need more functionality
uint8_t BQ7695_disable_unecessary(void);

// Reads the offset correction value for a given Vc
// if vc is 0, reads the offset of VREF
int8_t BQ7695_read_offset_corr(uint8_t vc);

// Reads the gain correction value for a given vc
// if vc is 0, reads the offset of VREF
int8_t BQ7695_read_gain_corr(uint8_t vc);
 
// Returns the status register and returns it
uint8_t BQ7695_read_status(void);

// Checks if the device detected an overcurrent 
bool BQ7695_over_current(void);

//Reads all of the cell voltages and stores it in a provided array
void BQ7695_read_cells(uint16_t *cells);

// Reads the current and stores it
void BQ7695_read_current(uint16_t *current);

// Record all of the correction factors
void BQ7695_setup_corrections(int8_t *gc, int8_t *oc);

#endif
