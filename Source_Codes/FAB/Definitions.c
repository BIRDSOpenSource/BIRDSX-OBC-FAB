#define ON    1
#define OFF   0
#define CLOSE 1
#define OPEN  0

// flags memory locations of kill switches in EEROM
#define FAB_KILL_SWITCH_FLAG            5
#define OBC_KILL_SWITCH_FLAG            10

// kill switch status read pin
#define FAB_KILL_SWITCH_STATUS_PIN      PIN_D3
#define OBC_KILL_STATUS_STATUS_PIN      PIN_C1

// definitions related external adc channel address
unsigned int8 CNL_ADD = 0B1000001100110000 ;              // 0x8330

#define PLUS_X_TEMP         0       //  V0 channel TEMP1
#define MINUS_Y_TEMP        1       //  V1 channel TEMP2
#define MINUS_Z_TEMP        2       //  V2 channel TEMP3
#define PLUS_Y_TEMP         3       //  V3 channel TEMP4
#define PLUS_Z_TEMP         4       //  V4 channel TEMP5
#define BACKPLANE_TEMP      5       //  V5 channel TEMP6  // rename
#define MINUS_X_TEMP        6       //  V6 channel TEMP7
#define TEMP8               7       //  V7 channel TEMP8  // rename

#define PLUS_X_VOLTAGE      8       //  V_1 V8 channel voltage
#define PLUS_Y_VOLTAGE      9       //  V_2 V9 channel voltage
#define MINUS_Z_VOLTAGE    10       //  V_3 V10 channel voltage
#define MINUS_Y_VOLTAGE    11       //  V_4 V11 channel voltage
#define PLUS_Z_VOLTAGE     12       //  V_5 V12 channel voltage
#define MINUS_X_VOLTAGE    13       //  V_6 V13 channel voltage

// PIC ADC lines
#define MINUS_X_SP_CURRENT 5
#define MINUS_Y_SP_CURRENT 3
#define MINUS_Z_SP_CURRENT 2
#define PLUS_Y_SP_CURRENT  1
#define PLUS_Z_SP_CURRENT  4
#define PLUS_X_SP_CURRENT  0  //

#define RAW_VOLTAGE        9
#define RAW_CURRENT         6
#define BATTERY_VOLTAGE    10
#define BATTERY_CURRENT    12
#define BATTERY_TEMP       13
#define SP_BUS_VOLTAGE     8
#define SP_BUS_CURRENT     11

// external adc spi port definition
#use spi(MASTER, CLK = PIN_C3, DI = PIN_C4, DO = PIN_C5,ENABLE = PIN_C2, BAUD = 100000, BITS = 16, STREAM = SPIPORT, MODE = 2 )


// using this function external ADC temperature channels can be read
unsigned int8 READ_EXT_ADC_CHANELS( unsigned int32 channel )
{
   unsigned int32 CNL_ADD = 0B1000001100110000 | (channel<<10) ;      // 0B1000001100110000 = 0x8330         
   spi_xfer(SPIPORT, CNL_ADD); 
   unsigned int16 ADC_READING = spi_xfer(SPIPORT) & 0x0FFF;           // reading ADC values and removing first address bits  
   
   return ADC_READING/16;                                             // converting to 8 bit
}

// suing this function analog inputs connected directly to FAB pic can be read
unsigned int16 READ_FAB_PIC_ADC(int port_name)
{
   SET_ADC_CHANNEL(port_name); //  routing nth channel to adc
   delay_us(40);      

   return READ_ADC();       
}

unsigned int8 AVERAGE_PICADC_READING(int channel, int16 num_of_readings, int16 timedelay)
{
   unsigned int16 val = 0;
   
   for(int16 i=0; i<num_of_readings; i++)  
   {
      val = val + READ_FAB_PIC_ADC( channel ) ;
      delay_us(timedelay);
   }
   
   val = (val/num_of_readings);
   
   return (char)((val>>4)&0xFF);
}

unsigned int8 AVERAGE_EXTADC_READING(int channel, int16 num_of_readings, int16 timedelay)
{
   unsigned int32 val = 0;
   
   for(int16 i=0; i<num_of_readings; i++)  
   {
      val = val + READ_EXT_ADC_CHANELS( channel ) ;
      delay_us(timedelay);
   }
   
   val = val /num_of_readings;
   
   return (char)val;
}


unsigned int16 AVERAGE_HECS_READING(int channel, int16 num_of_readings, int16 timedelay )
{
   unsigned int32 VAL = 0;
   
   for(int16 i=0; i<num_of_readings; i++)  
   {
      val = val + READ_FAB_PIC_ADC( channel ) ;
      delay_us(timedelay);
   }
   val = (val/num_of_readings); 
   
   return (unsigned int16)val;
}




