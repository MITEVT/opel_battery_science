#include <chip.h>
#include <BQ7695.h>
#include <board.h>

uint8_t rx_buf[10];
uint8_t tx;

uint8_t BQ7695_set_cell(uint8_t cell){
	if(cell == 7){	// Output the VREF
		tx = (3<<4);
	}
	else{
		tx = ((1<<4)|cell);
	}
	BQ7695_Write(I2C_ADDRESS_CELL_CTL,&tx);
	BQ7695_Read(I2C_ADDRESS_CELL_CTL,rx_buf);
	return rx_buf[0];
}

uint8_t BQ7695_disable_unecessary(){
	uint8_t power = 0x45; // Everything disabled
	BQ7695_Write(I2C_ADDRESS_POWER_CTL, &power);
	BQ7695_Read(I2C_ADDRESS_POWER_CTL, rx_buf);
	return rx_buf[0];
}


int8_t BQ7695_read_offset_corr(uint8_t vc){
	uint8_t offset = 0;
	int8_t sOffset = 0;
	BQ7695_Read(I2C_ADDRESS_VREF_CAL + vc,rx_buf); // lower bits
	offset = rx_buf[0];
	if(vc == 0){
		BQ7695_Read(I2C_ADDRESS_VREF_CAL_EXT, rx_buf);
		offset |= (rx_buf[0] & I2C_MASK_VREF_CAL_EXT_VREF_OC_4)<< 3;
	}
	else if(vc < 3){
		BQ7695_Read(I2C_ADDRESS_VC_CAL_EXT_1,rx_buf);
		if(vc == 1){
			offset |= (rx_buf[0] & I2C_MASK_VC_CAL_EXT_1_VC1_OC_4) >> 3;
		}
		else if(vc == 2){
			offset |= (rx_buf[0] & I2C_MASK_VC_CAL_EXT_1_VC2_OC_4) >> 1;
		}
	}
	else{
		BQ7695_Read(I2C_ADDRESS_VREF_CAL_EXT, rx_buf);
		if(vc == 3){
			offset |= (rx_buf[0] & I2C_MASK_VC_CAL_EXT_2_VC3_OC_4) >> 3;
		}
		else if(vc==4){
			offset |= (rx_buf[0] & I2C_MASK_VC_CAL_EXT_2_VC4_OC_4) >> 1;
		}
		else if(vc==5){
			offset |= (rx_buf[0] & I2C_MASK_VC_CAL_EXT_2_VC5_OC_4) << 1;
		}
		else if(vc==6){
			offset |= (rx_buf[0] & I2C_MASK_VC_CAL_EXT_2_VC6_OC_4) << 3;
		}

	}
	if(vc == 0){
		if(offset & 0x20){	// Convert the 6 bit two's complement
			sOffset = -1*(0x20-(offset & 0x1F));
		}
		else{
			sOffset = offset;
		}
	
	}
	else{
		if(offset & 0x10){	// Convert the 5 bit two's complement
			sOffset = -1*(0x10-(offset & 0x0F));
		}
		else{
			sOffset = offset;
		}
	}
	return sOffset;
}

int8_t BQ7695_read_gain_corr(uint8_t vc){
	uint8_t gain = 0;
	int8_t sGain = 0;
	BQ7695_Read(I2C_ADDRESS_VREF_CAL + vc,rx_buf); // lower bits
	gain = rx_buf[0];
	if(vc == 0){
		BQ7695_Read(I2C_ADDRESS_VREF_CAL_EXT, rx_buf);
		gain |= (rx_buf[0] & I2C_MASK_VREF_CAL_EXT_VREF_GC_4) << 4;
	}
	else if(vc < 3){
		BQ7695_Read(I2C_ADDRESS_VC_CAL_EXT_1,rx_buf);
		if(vc == 1){
			gain |= (rx_buf[0] & I2C_MASK_VC_CAL_EXT_1_VC1_GC_4) >> 2;
		}
		else if(vc == 2){
			gain |= (rx_buf[0] & I2C_MASK_VC_CAL_EXT_1_VC2_GC_4);
		}
	}
	else{
		BQ7695_Read(I2C_ADDRESS_VREF_CAL_EXT, rx_buf);
		if(vc == 3){
			gain |= (rx_buf[0] & I2C_MASK_VC_CAL_EXT_2_VC3_GC_4) >> 2;
		}
		else if(vc==4){
			gain |= (rx_buf[0] & I2C_MASK_VC_CAL_EXT_2_VC4_GC_4);
		}
		else if(vc==5){
			gain |= (rx_buf[0] & I2C_MASK_VC_CAL_EXT_2_VC5_GC_4) << 2;
		}
		else if(vc==6){
			gain |= (rx_buf[0] & I2C_MASK_VC_CAL_EXT_2_VC6_GC_4) << 4;
		}
	}
	if(gain & 0x10){	// Convert the 5 bit two's complement
		sGain = -1*(0x10-(gain & 0x0F));
	}
	else{
		sGain = gain;
	}
	return sGain;
}

uint8_t BQ7695_read_status(){
	BQ7695_Read(I2C_ADDRESS_STATUS, rx_buf);
	return rx_buf[0];
}

bool BQ7695_over_current(){
	BQ7695_Read(I2C_ADDRESS_STATUS, rx_buf);
	return ((rx_buf[0] & I2C_MASK_STATUS_ALERT)!= 0);
}

static inline void readCell(uint16_t *cell){
	
	Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
	while (!Chip_ADC_ReadStatus(LPC_ADC, ADC_CH5, ADC_DR_DONE_STAT));	
	Chip_ADC_ReadValue(LPC_ADC, ADC_CH5, cell);
}

void BQ7695_read_cells(uint16_t *cells){
	uint8_t i = 0;
	while(i<7){
		Board_UART_PrintNum(BQ7695_set_cell(++i),2,true);
		readCell(&(cells[i]));
	}
}

void BQ7695_read_current(uint16_t *current){
	Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
	while (!Chip_ADC_ReadStatus(LPC_ADC, ADC_CH4, ADC_DR_DONE_STAT));	
	Chip_ADC_ReadValue(LPC_ADC, ADC_CH4, current);
}

void BQ7695_setup_corrections(int8_t *gc, int8_t *oc){
	uint8_t i =0;
	while(i<7){
		gc[i] = BQ7695_read_gain_corr(i);
		oc[i] = BQ7695_read_offset_corr(i);
		i++;
	}
}
