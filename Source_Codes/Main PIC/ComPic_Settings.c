#define TXMODE_PIN    PIN_F6     // PIN 42 in connector -> CP2NTRX_DIO0 
#define TXDONE_PIN    PIN_F5     // PIN 43 in connector -> CP2NTRX_DIO1 
#define TRXPWR_PIN    PIN_A3     // PIN 38 in connector -> CP2NTRX_DIO2  

#define RSTPIC_RESTART_PIN    PIN_F7

// mainlop counter
   unsigned int16 MLC = 0;
   
// cw data array
   unsigned int CW_DATA[16];
   unsigned int CW_DATA_TO_NEWTRX[55];

// CW transmission related variables
   unsigned int8 CW_FLAG   = 0x00;
   
   unsigned int8 NEWTRX_CW     = 0xAA ;
   unsigned int8 OLDTRX_CW     = 0xBB ;
   unsigned int8 NEWTRX_1PCKT  = 0xCC ;
   unsigned int8 OLDTRX_1PCKT  = 0xDD ;
   
   unsigned int32 TLE_ADDRESS  = 0x07D00000;    // 2000 th sector start adress
   
// Transceiver operation
   unsigned int8 NEW_TRX_STATUS = 0x00;
   unsigned int8 ON_  = 0xAA;
   unsigned int8 OFF_ = 0xBB;

   
// these arrays are used to comunicate with reset pic
   unsigned int8 RPIC_TO_CPIC_ARRAY[55];  // only 20 bytes are used
   unsigned int8 CPIC_TO_RPIC_ARRAY[55];  // only 20 bytes are used

// these arrays are used to comunicate with new tranceiver
   unsigned int8 NEWTRX_CMD_BUFFER[60];   
   unsigned int8 NEWTRX_TO_CPIC_ARRAY[60];
   unsigned int8 CPIC_TO_NEWTRX_ARRAY[110];

// these arrays are used to comunicate with old tranceiver
   unsigned int8 OLDTRX_CMD_BUFFER[60];
   unsigned int8 OLDTRX_TO_CPIC_ARRAY[60];
   unsigned int8 CPIC_TO_OLDTRX_ARRAY[110];

// these arrays are used to comunicate with main pic
   unsigned int8 MPIC_TO_CPIC_ARRAY[55];  // only 32 bytes are used
   unsigned int8 CPIC_TO_MPIC_ARRAY[55];  // only 40 bytes are used
 

// call signs
   // GS call sign
   unsigned int8 GS_callsign_leter_1 = 0x4A ;   // J
   unsigned int8 GS_callsign_leter_2 = 0x47 ;   // G
   unsigned int8 GS_callsign_leter_3 = 0x36 ;   // 6
   unsigned int8 GS_callsign_leter_4 = 0x59 ;   // Y
   unsigned int8 GS_callsign_leter_5 = 0x42 ;   // B
   unsigned int8 GS_callsign_leter_6 = 0x57 ;   // W
   
   //Satellite call sign 
   unsigned int8 ST_callsign_leter_1 = 0x6A ;   // J
   unsigned int8 ST_callsign_leter_2 = 0x67 ;   // G
   unsigned int8 ST_callsign_leter_3 = 0x36 ;   // 6
   unsigned int8 ST_callsign_leter_4 = 0x79 ;   // Y
   unsigned int8 ST_callsign_leter_5 = 0x6F ;   // O
   unsigned int8 ST_callsign_leter_6 = 0x77 ;   // W
 
   Void OLD_TRX_SEND_1PCKT_BEACON() ;
   Void NEW_TRX_SEND_1PCKT_BEACON() ;
   void COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(int16 numof_times, int16 time_delay, int16 wait_time = 200);  
   


// debug software uart_________________________________________________________
#use rs232(baud=19200, parity=N, xmit=PIN_B7,  bits=8, stream = PC, errors, force_sw )  //Debug PGD line
//#use rs232(baud=19200, parity=N, xmit=PIN_D7,  bits=8, stream = PC, errors, force_sw )  //dummy port

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//=============================================================================
//This section handle the UART port connected to main pic______________________
#define MP_BFR_SIZE 50
#pin_select TX2=PIN_G1  
#pin_select RX2=PIN_G2  
#use rs232(UART2, baud=38400, parity=N, bits=8, stream=MPic, errors) 

unsigned int8  MP_Buffer[MP_BFR_SIZE];
unsigned int16 MP_Byte_Counter = 0;
unsigned int8  MP_Overflow = 0;
unsigned int16 MP_Read_Byte_counter = 0;
unsigned int8  MP_Temp_byte = 0;

#INT_RDA2
Void SERIAL_ISR2()         // MAIN PIC uart interupt loop
{
   if( kbhit(MPic) )
   {
      if( MP_Byte_Counter < MP_BFR_SIZE )
      {
         MP_Buffer[MP_Byte_Counter] = fgetc(MPic);
         MP_Byte_Counter++;
      }
      else MP_Overflow = fgetc(MPic);
   }
}

unsigned int8 MPic_Available()
{
   if (MP_Byte_Counter > 0) return MP_Byte_Counter ;
   else return 0;
}

unsigned int8 MPic_Read()
{
   if (MP_Byte_Counter>0)
   {    
      MP_Temp_byte = MP_Buffer[MP_Read_Byte_counter];
      
      MP_Byte_Counter--;
      MP_Read_Byte_counter++;
      if(MP_Byte_Counter == 0) MP_Read_Byte_counter = 0;
      return MP_Temp_byte; 
   }
   
   if (MP_Byte_Counter == 0)
   { 
      MP_Read_Byte_counter = 0;
      MP_Temp_byte = 0x00;
      return MP_Temp_byte; 
   }
 
}

void MPic_flush()
{
   while( MPic_Available() ) MPic_Read() ;
}
//=============================================================================



//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//=============================================================================
//This section handle the UART port connected to RESET pic_______________________
#define RP_BFR_SIZE 50
#pin_select TX3=PIN_E3
#pin_select RX3=PIN_E4  
#use rs232(UART3, baud=38400, parity=N, bits=8, stream=RPic, errors) 

unsigned int8  RP_Buffer[RP_BFR_SIZE];
unsigned int16 RP_Byte_Counter = 0;
unsigned int8  RP_Overflow = 0;
unsigned int16 RP_Read_Byte_counter = 0;
unsigned int8  RP_Temp_byte = 0;

#INT_RDA3
Void SERIAL_ISR3()         // MAIN PIC uart interupt loop
{
   if( kbhit(RPic) )
   {
      if( RP_Byte_Counter < RP_BFR_SIZE )
      {
         RP_Buffer[RP_Byte_Counter] = fgetc(RPic);
         RP_Byte_Counter++;
      }
      
      else RP_Overflow = fgetc(RPic);
   }
}

unsigned int8 RPic_Available()
{
   if (RP_Byte_Counter > 0) return RP_Byte_Counter ;
   else return 0;
}

unsigned int8 RPic_Read()
{
   if (RP_Byte_Counter>0)
   {    
      RP_Temp_byte = RP_Buffer[RP_Read_Byte_counter];
      
      RP_Byte_Counter--;
      RP_Read_Byte_counter++;
      if(RP_Byte_Counter == 0) RP_Read_Byte_counter = 0;
      return RP_Temp_byte; 
   }
   
   if (RP_Byte_Counter == 0)
   { 
      RP_Read_Byte_counter = 0;
      RP_Temp_byte = 0x00;
      return RP_Temp_byte; 
   }
 
}

void RPic_flush()
{
   while( RPic_Available() ) RPic_Read() ;
}
//=============================================================================

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//=============================================================================
//This section handle the UART port connected to OLD TRANCEIVER________________
#define OT_BFR_SIZE 50
#pin_select TX1=PIN_C6
#pin_select RX1=PIN_C7  
#use rs232(UART1, baud=115200, parity=N, bits=8, stream=OLD_TRX, errors) 


unsigned int8  OT_Buffer[OT_BFR_SIZE];
unsigned int16 OT_Byte_Counter = 0;
unsigned int8  OT_Overflow = 0;
unsigned int16 OT_Read_Byte_counter = 0;
unsigned int8  OT_Temp_byte = 0;

#INT_RDA
Void SERIAL_ISR1()         // MAIN PIC uart interupt loop
{
   if( OT_Byte_Counter < OT_BFR_SIZE )
   {
      OT_Buffer[OT_Byte_Counter] = fgetc(OLD_TRX);
      OT_Byte_Counter++;
   }
   
   else OT_Overflow = fgetc(OLD_TRX);
}

unsigned int8 OLDTRX_Available()
{
   return OT_Byte_Counter ;
}

unsigned int8 OLDTRX_Read()
{
   if (OT_Byte_Counter>0)
   {    
      OT_Temp_byte = OT_Buffer[OT_Read_Byte_counter];
      
      OT_Byte_Counter--;
      OT_Read_Byte_counter++;
      if(OT_Byte_Counter == 0) OT_Read_Byte_counter = 0;
      return OT_Temp_byte; 
   }
   
   if (OT_Byte_Counter == 0)
   { 
      OT_Read_Byte_counter = 0;
      OT_Temp_byte = 0x00;
      return OT_Temp_byte; 
   }
 
}

void OLDTRX_flush()
{
   while( OLDTRX_Available() ) OLDTRX_Read() ;
}
//=============================================================================





//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//=============================================================================
//This section handle the UART port connected to OLD TRANCEIVER________________
#define NT_BFR_SIZE 100
#pin_select TX4=PIN_B1
#pin_select RX4=PIN_B0  
#use rs232(UART4, baud=115200, parity=N, bits=8, stream=NEW_TRX, errors) 


unsigned int8  NT_Buffer[NT_BFR_SIZE];
unsigned int16 NT_Byte_Counter = 0;
unsigned int8  NT_Overflow = 0;
unsigned int16 NT_Read_Byte_counter = 0;
unsigned int8  NT_Temp_byte = 0;

#INT_RDA4
Void SERIAL_ISR4()         // MAIN PIC uart interupt loop
{
   if( NT_Byte_Counter < NT_BFR_SIZE )
   {
      NT_Buffer[NT_Byte_Counter] = fgetc(NEW_TRX);
      NT_Byte_Counter++;
   }
   
   else NT_Overflow = fgetc(NEW_TRX);
}

unsigned int8 NEWTRX_Available()
{
  return NT_Byte_Counter ;
}

unsigned int8 NEWTRX_Read()
{
   if (NT_Byte_Counter>0)
   {    
      NT_Temp_byte = NT_Buffer[NT_Read_Byte_counter];
      
      NT_Byte_Counter--;
      NT_Read_Byte_counter++;
      if(NT_Byte_Counter == 0) NT_Read_Byte_counter = 0;
      return NT_Temp_byte; 
   }
   
   else
   { 
      NT_Read_Byte_counter = 0;
      NT_Temp_byte = 0x00;
      return NT_Temp_byte; 
   }
 
}

void NEWTRX_flush()
{
   while( NEWTRX_Available() ) NEWTRX_Read() ;
}
//=============================================================================


// just printing a line________________________________________________________
void printline()
{
   fprintf(PC,"\n\r");
}

// using this function we can make any data array clear________________________
void CLEAR_DATA_ARRAY(unsigned int8 array[], int array_size)
{   
   for(int i = 0; i < array_size; i++)
   {
      array[i] = 0 ;
   }
}

// crc calculating function____________________________________________________
#define _DATA_SIZE 160
unsigned int8 cal_data[_DATA_SIZE];

unsigned int16 CALCULATE_CRC(unsigned int8 data[], unsigned int8 data_number)
{
   unsigned int32 crcReg = 0xFFFF;   
   unsigned int32 calc   = 0x8408;     
   unsigned int8 w;            
   int32 i;                    
   int32 k;                     
   
   for(k = 0; k < data_number; k++)
   {
      cal_data[k] = data[k];   

      for(i = 0; i < 8; i++)
      {
         w = (crcReg ^ cal_data[k]) & 0x0001;
         crcReg = crcReg >> 1;

         if(w == 1)
         {
            crcReg = crcReg ^ calc;
         }

         cal_data[k] = cal_data[k] >> 1;
      }
   }

   crcReg = crcReg ^ 0xFFFF;   
   return crcReg;
}

unsigned int16 CALCULATE_CRC_(unsigned int8 data[], unsigned int8 data_number)
{
   unsigned int32 crcReg = 0xFFFF;   
   unsigned int32 calc   = 0x8408;     
   unsigned int8 w;            
   int32 i;                    
   int32 k;                     
   
   for(k = 1; k < data_number; k++)
   {
      cal_data[k] = data[k];   

      for(i = 0; i < 8; i++)
      {
         w = (crcReg ^ cal_data[k]) & 0x0001;
         crcReg = crcReg >> 1;

         if(w == 1)
         {
            crcReg = crcReg ^ calc;
         }

         cal_data[k] = cal_data[k] >> 1;
      }
   }

   crcReg = crcReg ^ 0xFFFF;   
   return crcReg;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// flash memory functions/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#use spi(MASTER,CLK=PIN_E1, DI=PIN_E0, DO=PIN_E6, BAUD=1000000, BITS=8, STREAM=FM, MODE=0) //FLASH_M_______COM
//E2 chip select

unsigned int8 adsress[4];
//unsigned int8 _data;

void FM_WRITE_ENABLE()
{
  Output_low(Pin_E2);
  spi_xfer(FM,0x06);                
  Output_high(Pin_E2);
}

void FM_SECTOR_ERASE(unsigned int32 sector_address,char sector_size, unsigned int16 delay = 1000)
{
   
   adsress[0]  = (unsigned int8)((sector_address>>24) & 0xFF);   // 0x __ 00 00 00
   adsress[1]  = (unsigned int8)((sector_address>>16) & 0xFF);   // 0x 00 __ 00 00
   adsress[2]  = (unsigned int8)((sector_address>>8)  & 0xFF);   // 0x 00 00 __ 00
   adsress[3]  = (unsigned int8)((sector_address)     & 0xFF);   // 0x 00 00 00 __
   
   FM_WRITE_ENABLE();
   Output_low(Pin_E2);             //lower the CS PIN

   ///////////////////////////////////////////////////////////////////

   if( Sector_size == 4  ) spi_xfer(FM,0x21);                    // 4KB Sector erase
   if( Sector_size == 32 ) spi_xfer(FM,0x5C);                    // 32KB Sector erase
   if( Sector_size == 64 ) spi_xfer(FM,0xDC);                    // 64KB Sector erase
   
   spi_xfer(FM,adsress[0]);   
   spi_xfer(FM,adsress[1]);    
   spi_xfer(FM,adsress[2]);    
   spi_xfer(FM,adsress[3]);
   //////////////////////////////////////////////////////////////////
   Output_high(Pin_E2);           //take CS PIN higher back
   delay_ms(delay); 
}

void FM_BYTE_WRITE(unsigned int32 byte_address, unsigned int8 data)
{
   
   //Byte extraction
   adsress[0]  = (unsigned int8)((byte_address>>24) & 0xFF);   // 0x __ 00 00 00
   adsress[1]  = (unsigned int8)((byte_address>>16) & 0xFF);   // 0x 00 __ 00 00
   adsress[2]  = (unsigned int8)((byte_address>>8)  & 0xFF);   // 0x 00 00 __ 00
   adsress[3]  = (unsigned int8)((byte_address)     & 0xFF);   // 0x 00 00 00 __
 
   FM_WRITE_ENABLE();
   Output_low(Pin_E2);             //lower the CS PIN
   ///////////////////////////////////////////////////////////////////
   spi_xfer(FM,0x12);         //Byte WRITE COMAND  (0x12)
   spi_xfer(FM,adsress[0]);    
   spi_xfer(FM,adsress[1]);    
   spi_xfer(FM,adsress[2]);    
   spi_xfer(FM,adsress[3]);

   spi_xfer(FM,data); 
   //////////////////////////////////////////////////////////////////
   Output_high(Pin_E2);           //take CS PIN higher back 
   
   return;
}

unsigned int8 FM_BYTE_READ_(unsigned int32 ADDRESS)
{
   //Byte extraction
   adsress[0]  = (unsigned int8)( (ADDRESS & 0xFF000000) >> 24 );   
   adsress[1]  = (unsigned int8)( (ADDRESS & 0x00FF0000) >> 16 );   
   adsress[2]  = (unsigned int8)( (ADDRESS & 0x0000FF00) >>  8 );   
   adsress[3]  = (unsigned int8)( (ADDRESS & 0x000000FF)       );   
   
   Output_Low(PIN_E2);                //lower the CS PIN
   //////////////////////////////////////////////////////////////////
   spi_xfer(FM,0X13);  //READ DATA COMAND   (0x13)
   spi_xfer(FM,adsress[0]);
   spi_xfer(FM,adsress[1]);
   spi_xfer(FM,adsress[2]);
   spi_xfer(FM,adsress[3]);
   char _data = spi_xfer(FM);
   //////////////////////////////////////////////////////////////////
   Output_High(PIN_E2);;               //take CS PIN higher back
   
   return _data;
}

char PRF = 0;

unsigned int8 FM_BYTE_READ(unsigned int32 ADDRESS)
{
   if( PRF == 0x00)
   {
      //Byte extraction
      adsress[0]  = (unsigned int8)( (ADDRESS & 0xFF000000) >> 24 );   
      adsress[1]  = (unsigned int8)( (ADDRESS & 0x00FF0000) >> 16 );   
      adsress[2]  = (unsigned int8)( (ADDRESS & 0x0000FF00) >>  8 );   
      adsress[3]  = (unsigned int8)( (ADDRESS & 0x000000FF)       );   
      Output_LOW(PIN_E2);     
      spi_xfer(FM,0X13);  //READ DATA COMAND   (0x13)
      spi_xfer(FM,adsress[0]);
      spi_xfer(FM,adsress[1]);
      spi_xfer(FM,adsress[2]);
      spi_xfer(FM,adsress[3]);
      PRF = 0xFF;
   }
   
   return spi_xfer(FM);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   

