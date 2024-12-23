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

//!// using this function external ADC Voltage channels can be read
//!unsigned int8 READ_EXT_ADC_VOLTAGE_CHANELS( unsigned int32 channel )
//!{
//!   unsigned int32 CNL_ADD = 0B1000001100110000 | (channel<<10) ;      // 0B1000001100110000 = 0x8330         
//!   spi_xfer(SPIPORT, CNL_ADD); 
//!   unsigned int16 ADC_READING = spi_xfer(SPIPORT) & 0x0FFF;       // reading ADC values and removing first address bits                            
//!   return ADC_READING/16;                                                // converting to 8 bit
//!}

// suing this function analog inputs connected directly to FAB pic can be read
unsigned int8 READ_FAB_PIC_ADC(int port_name)
{
   SET_ADC_CHANNEL(port_name); //  routing nth channel to adc
   delay_us(20);      
   return READ_ADC()/8;        // converting to 8bit
}

// this function will check command coming from amin pic_______________________
void CHECK_UART_INCOMING_FROM_MPIC()
{
   if( MPic_Available() )
   {
      Delay_ms(10);
      for(int i = 0; i<=10; i++)
      {
         MPIC_TO_FAB_ARRAY[i] = MPic_Read();
      }
   }
}

// this function will print command came from main pic_________________________
void PRINT_MAIN_PIC_COMMAND()
{
   printline();
   fprintf(PC, "OBC COMMAND >> ");
   for(int i=0; i<3; i++)
   {
      fprintf(PC, "%X ", MPIC_TO_FAB_ARRAY[i]);
   }
   printline();
}

//unsigned int16 batery_temp = 0;
//unsigned int16 batery_volt = 0;

void OPERATE_BATTERY_HEATER()
{
   
}

void SEND_FAB_DATA_TO_MAIN_PIC()
{
   if( MPIC_TO_FAB_ARRAY[1] == 0xA0 )
   {
      fprintf(PC,"FAB data request command from main pic\n\r");
      
      CLEAR_DATA_ARRAY(FAB_TO_MPIC_ARRAY,32);
      // preparing data
      FAB_TO_MPIC_ARRAY[0]  = 0xFA  ;        // header
      
      FAB_TO_MPIC_ARRAY[1]  = 0xA0  ;        // command identifier  
      
      FAB_TO_MPIC_ARRAY[2]  = READ_FAB_PIC_ADC( RAW_VOLTAGE )         ;
      FAB_TO_MPIC_ARRAY[3]  = READ_FAB_PIC_ADC( RAW_CURENT )          ;
      FAB_TO_MPIC_ARRAY[4]  = READ_FAB_PIC_ADC( BATTERY_VOLTAGE )     ;
      FAB_TO_MPIC_ARRAY[5]  = READ_FAB_PIC_ADC( BATTERY_CURRENT )     ;
      FAB_TO_MPIC_ARRAY[6]  = READ_FAB_PIC_ADC( BATTERY_TEMP )        ;
      FAB_TO_MPIC_ARRAY[7]  = READ_FAB_PIC_ADC( SP_BUS_VOLTAGE )      ;
      FAB_TO_MPIC_ARRAY[8]  = READ_FAB_PIC_ADC( SP_BUS_CURRENT )      ;
            
      FAB_TO_MPIC_ARRAY[9]  = READ_EXT_ADC_CHANELS( PLUS_X_VOLTAGE )  ;
      FAB_TO_MPIC_ARRAY[10] = READ_FAB_PIC_ADC( PLUS_X_SP_CURRENT )   ;
      FAB_TO_MPIC_ARRAY[11] = READ_EXT_ADC_CHANELS( PLUS_X_TEMP )     ;
      
      FAB_TO_MPIC_ARRAY[12] = READ_EXT_ADC_CHANELS( PLUS_Y_VOLTAGE )  ;
      FAB_TO_MPIC_ARRAY[13] = READ_FAB_PIC_ADC( PLUS_Y_SP_CURRENT )   ;
      FAB_TO_MPIC_ARRAY[14] = READ_EXT_ADC_CHANELS( PLUS_Y_TEMP )     ;
      
      FAB_TO_MPIC_ARRAY[15] = READ_EXT_ADC_CHANELS( PLUS_Z_VOLTAGE )  ;
      FAB_TO_MPIC_ARRAY[16] = READ_FAB_PIC_ADC( PLUS_Z_SP_CURRENT )   ;
      FAB_TO_MPIC_ARRAY[17] = READ_EXT_ADC_CHANELS( PLUS_Z_TEMP )     ;
      
      FAB_TO_MPIC_ARRAY[18] = READ_EXT_ADC_CHANELS( MINUS_X_VOLTAGE ) ;
      FAB_TO_MPIC_ARRAY[19] = READ_FAB_PIC_ADC( MINUS_X_SP_CURRENT )  ;
      FAB_TO_MPIC_ARRAY[20] = READ_EXT_ADC_CHANELS( MINUS_X_TEMP )    ;
      
      FAB_TO_MPIC_ARRAY[21] = READ_EXT_ADC_CHANELS( MINUS_Y_VOLTAGE ) ;
      FAB_TO_MPIC_ARRAY[22] = READ_FAB_PIC_ADC( MINUS_Y_SP_CURRENT )  ;
      FAB_TO_MPIC_ARRAY[23] = READ_EXT_ADC_CHANELS( MINUS_Y_TEMP )    ;
      
      FAB_TO_MPIC_ARRAY[24] = READ_EXT_ADC_CHANELS( MINUS_Z_VOLTAGE ) ;
      FAB_TO_MPIC_ARRAY[25] = READ_FAB_PIC_ADC( MINUS_Z_SP_CURRENT )  ;
      FAB_TO_MPIC_ARRAY[26] = READ_EXT_ADC_CHANELS( MINUS_Z_TEMP )    ;
      
      FAB_TO_MPIC_ARRAY[27] = READ_EXT_ADC_CHANELS( TEMP6 )           ;
      FAB_TO_MPIC_ARRAY[28] = READ_EXT_ADC_CHANELS( TEMP8 )           ;
      FAB_TO_MPIC_ARRAY[29] = input(FAB_KILL_SWITCH_STATUS_PIN)       ;
      FAB_TO_MPIC_ARRAY[30] = input(OBC_KILL_STATUS_STATUS_PIN)       ;
      
      FAB_TO_MPIC_ARRAY[31] = 0xFB                                    ;   // footer
      
      // sending data to main pic
      for( int i = 0; i<33; i++)
      {
         fputc( FAB_TO_MPIC_ARRAY[i], MPIC);
      }
   }
}

//For debug only
void SEND_FAB_DATA_TO_DEBUG()
{
   
      //fprintf(PC,"FAB data request command for debug\n\r");
      
      CLEAR_DATA_ARRAY(FAB_TO_MPIC_ARRAY,32);
      // preparing data
      FAB_TO_MPIC_ARRAY[0]  = 0xFA  ;        // header
      
      FAB_TO_MPIC_ARRAY[1]  = 0xA0  ;        // command identifier  
      
      FAB_TO_MPIC_ARRAY[2]  = READ_FAB_PIC_ADC( RAW_VOLTAGE )         ;
      FAB_TO_MPIC_ARRAY[3]  = READ_FAB_PIC_ADC( RAW_CURENT )          ;
      FAB_TO_MPIC_ARRAY[4]  = AVERAGE_PICADC_READING(BATTERY_VOLTAGE, 10, 100)     ;
      FAB_TO_MPIC_ARRAY[5]  = AVERAGE_PICADC_READING(BATTERY_CURENT, 10, 100)     ;
      FAB_TO_MPIC_ARRAY[6]  = READ_FAB_PIC_ADC( BATTERY_TEMP )        ;
      FAB_TO_MPIC_ARRAY[7]  = READ_FAB_PIC_ADC( SP_BUS_VOLTAGE )      ;
      FAB_TO_MPIC_ARRAY[8]  = READ_FAB_PIC_ADC( SP_BUS_CURRENT )      ;
            
      FAB_TO_MPIC_ARRAY[9]  = READ_EXT_ADC_CHANELS( PLUS_X_VOLTAGE )  ;
      FAB_TO_MPIC_ARRAY[10] = READ_FAB_PIC_ADC( PLUS_X_SP_CURRENT )   ;
      FAB_TO_MPIC_ARRAY[11] = READ_EXT_ADC_CHANELS( PLUS_X_TEMP )     ;
      
      FAB_TO_MPIC_ARRAY[12] = READ_EXT_ADC_CHANELS( PLUS_Y_VOLTAGE )  ;
      FAB_TO_MPIC_ARRAY[13] = READ_FAB_PIC_ADC( PLUS_Y_SP_CURRENT )   ;
      FAB_TO_MPIC_ARRAY[14] = READ_EXT_ADC_CHANELS( PLUS_Y_TEMP )     ;
      
      FAB_TO_MPIC_ARRAY[15] = READ_EXT_ADC_CHANELS( PLUS_Z_VOLTAGE )  ;
      FAB_TO_MPIC_ARRAY[16] = READ_FAB_PIC_ADC( PLUS_Z_SP_CURRENT )   ;
      FAB_TO_MPIC_ARRAY[17] = READ_EXT_ADC_CHANELS( PLUS_Z_TEMP )     ;
      
      FAB_TO_MPIC_ARRAY[18] = READ_EXT_ADC_CHANELS( MINUS_X_VOLTAGE ) ;
      FAB_TO_MPIC_ARRAY[19] = READ_FAB_PIC_ADC( MINUS_X_SP_CURRENT )  ;
      FAB_TO_MPIC_ARRAY[20] = READ_EXT_ADC_CHANELS( MINUS_X_TEMP )    ;
      
      FAB_TO_MPIC_ARRAY[21] = READ_EXT_ADC_CHANELS( MINUS_Y_VOLTAGE ) ;
      FAB_TO_MPIC_ARRAY[22] = READ_FAB_PIC_ADC( MINUS_Y_SP_CURRENT )  ;
      FAB_TO_MPIC_ARRAY[23] = READ_EXT_ADC_CHANELS( MINUS_Y_TEMP )    ;
      
      FAB_TO_MPIC_ARRAY[24] = READ_EXT_ADC_CHANELS( MINUS_Z_VOLTAGE ) ;
      FAB_TO_MPIC_ARRAY[25] = READ_FAB_PIC_ADC( MINUS_Z_SP_CURRENT )  ;
      FAB_TO_MPIC_ARRAY[26] = READ_EXT_ADC_CHANELS( MINUS_Z_TEMP )    ;
      
      FAB_TO_MPIC_ARRAY[27] = READ_EXT_ADC_CHANELS( TEMP6 )           ;
      FAB_TO_MPIC_ARRAY[28] = READ_EXT_ADC_CHANELS( TEMP8 )           ;
      FAB_TO_MPIC_ARRAY[29] = input(FAB_KILL_SWITCH_STATUS_PIN)       ;
      FAB_TO_MPIC_ARRAY[30] = input(OBC_KILL_STATUS_STATUS_PIN)       ;
      
      FAB_TO_MPIC_ARRAY[31] = 0xFB                                    ;   // footer
      
         //fputc(FAB_TO_MPIC_ARRAY[i], PC );
     fprintf(PC, "%x\n\r", FAB_TO_MPIC_ARRAY[4]);
     fprintf(PC, "%x\n\r", FAB_TO_MPIC_ARRAY[5]);
}







void PRINT_KILL_SWITCHES_CURRENT_STATE()
{
   if( input(FAB_KILL_SWITCH_STATUS_PIN) ) fprintf(PC, "FAB KILL SWITCH PIN STATUS = CLOSE\n\r");
   else                                    fprintf(PC, "FAB KILL SWITCH PIN STATUS = OPEN\n\r" );
   if( input(OBC_KILL_STATUS_STATUS_PIN) ) fprintf(PC, "OBC KILL SWITCH PIN STATUS = CLOSE\n\r");
   else                                    fprintf(PC, "OBC KILL SWITCH PIN STATUS = OPEN\n\r" );
}

void PRINT_KILL_SWITCHES_EEPROM_VLAUES()
{
   fprintf(PC, "FAB KILL SWITCH EEPROM FLAG VALUE = %X\n\r", READ_EEPROM(FAB_KILL_SWITCH_FLAG) );
   fprintf(PC, "OBC KILL SWITCH EEPROM FLAG VALUE = %X\n\r", READ_EEPROM(OBC_KILL_SWITCH_FLAG) );
}

void FAB_KILL_SWITCH_CLOSE()
{
   if( MPIC_TO_FAB_ARRAY[1] == 0xF1 )
   {
      fprintf(PC,"FAB KILL SWITCH close command\n\r");
      
      // acknowleging to main pic
      FAB_TO_MPIC_ARRAY[0] = 0xFA;        // header
      FAB_TO_MPIC_ARRAY[1] = 0xF1;
      FAB_TO_MPIC_ARRAY[2] = 0xFB;        // footer
      
      // sending ack to main pic
      for( int i = 0; i<3; i++)
      {
         fputc( FAB_TO_MPIC_ARRAY[i], MPIC);
      }
      
      FAB_KILL_SWITCH(CLOSE);
      Delay_ms(1000);
      WRITE_EEPROM(FAB_KILL_SWITCH_FLAG, 0xFF);
      Delay_ms(5);
      PRINT_KILL_SWITCHES_EEPROM_VLAUES();
      PRINT_KILL_SWITCHES_CURRENT_STATE();
   }
}

void OBC_KILL_SWITCH_CLOSE()
{
   if( MPIC_TO_FAB_ARRAY[1] == 0xF2 )
   {
      fprintf(PC,"OBC KILL SWITCH close command\n\r");
      
      // acknowleging to main pic
      FAB_TO_MPIC_ARRAY[0] = 0xFA;        // header
      FAB_TO_MPIC_ARRAY[1] = 0xF2;
      FAB_TO_MPIC_ARRAY[2] = 0xFB;        // footer
      
      // sending ack to main pic
      for( int i = 0; i<3; i++)
      {
         fputc( FAB_TO_MPIC_ARRAY[i], MPIC);
      }
      
      OBC_KILL_SWITCH(CLOSE);
      Delay_ms(1000);
      WRITE_EEPROM(OBC_KILL_SWITCH_FLAG, 0xFF);
      Delay_ms(5);
      PRINT_KILL_SWITCHES_EEPROM_VLAUES();
      PRINT_KILL_SWITCHES_CURRENT_STATE();
   }
}



void FAB_KILL_SWITCH_OPEN()
{
   if( MPIC_TO_FAB_ARRAY[1] == 0xF3 )
   {
      fprintf(PC,"FAB KILL SWITCH open command\n\r");
      
      // acknowleging to main pic
      FAB_TO_MPIC_ARRAY[0] = 0xFA;        // header
      FAB_TO_MPIC_ARRAY[1] = 0xF3;
      FAB_TO_MPIC_ARRAY[2] = 0xFB;        // footer
      
      // sending ack to main pic
      for( int i = 0; i<3; i++)
      {
         fputc( FAB_TO_MPIC_ARRAY[i], MPIC);
      }     
      
      unsigned int8 FABKILLFLAG = READ_EEPROM(FAB_KILL_SWITCH_FLAG) ;
      
      if( (0< FABKILLFLAG) && (FABKILLFLAG < 10) )
      {
         FABKILLFLAG++;
         WRITE_EEPROM(FAB_KILL_SWITCH_FLAG, FABKILLFLAG);         
      }
      
      if( FABKILLFLAG == 0xFF )
      {
         WRITE_EEPROM(FAB_KILL_SWITCH_FLAG, 0x01);  // first command recived
      }
      
      Delay_ms(5);
      FABKILLFLAG = READ_EEPROM(FAB_KILL_SWITCH_FLAG) ;
      fprintf( PC, "FAB kill switch open command number = %X\n\r", FABKILLFLAG);
      
      if( FABKILLFLAG == 10 )
      {
         fprintf(PC, "Since 10 FAB kill open commands received, FAB kill switch is opened\n\r");
         FAB_KILL_SWITCH(OPEN) ;
         Delay_ms(1000);
      }
      
      PRINT_KILL_SWITCHES_EEPROM_VLAUES();
      PRINT_KILL_SWITCHES_CURRENT_STATE();
   }
}



void OBC_KILL_SWITCH_OPEN()
{
   if( MPIC_TO_FAB_ARRAY[1] == 0xF4 )
   {
      fprintf(PC,"OBC KILL SWITCH open command\n\r");
      
      unsigned int8 OBCKILLFLAG = READ_EEPROM(OBC_KILL_SWITCH_FLAG) ;
            
      // acknowleging to main pic
      FAB_TO_MPIC_ARRAY[0] = 0xFA;                       // header
      if( OBCKILLFLAG == 9) FAB_TO_MPIC_ARRAY[1] = 0xF4;
      else                  FAB_TO_MPIC_ARRAY[1] = 0x00;
      FAB_TO_MPIC_ARRAY[2] = 0xFB;                       // footer
      
      // sending ack to main pic
      for( int i = 0; i<3; i++)
      {
         fputc( FAB_TO_MPIC_ARRAY[i], MPIC);
      }     

      if( (0< OBCKILLFLAG) && (OBCKILLFLAG < 10) )
      {
         OBCKILLFLAG++;
         WRITE_EEPROM(OBC_KILL_SWITCH_FLAG, OBCKILLFLAG);         
      }
      
      if( OBCKILLFLAG == 0xFF )
      {
         WRITE_EEPROM(OBC_KILL_SWITCH_FLAG, 0x01);  // first command recived
      }
      
      Delay_ms(5);
      OBCKILLFLAG = READ_EEPROM(OBC_KILL_SWITCH_FLAG) ;
      fprintf( PC, "OBC kill switch open command number = %X\n\r", OBCKILLFLAG);
      
      if( OBCKILLFLAG == 10 )
      {
         fprintf(PC, "Since 10 OBC kill open commands received, OBC kill switch is opened\n\r");
         OBC_KILL_SWITCH(OPEN) ;
         Delay_ms(1000);
      }
      
      PRINT_KILL_SWITCHES_EEPROM_VLAUES();
      PRINT_KILL_SWITCHES_CURRENT_STATE();
   }
}
