#define sel_0 PIN_C6    //CPLD(D)
#define sel_1 PIN_C7    //CPLD(E)
#define sel_2 PIN_C2    //CPLD(F)
//#define sel_3 PIN_C3    //CPLD(G)


//////////////////////////////////  Software UART for debugging using PGC and PGD  ///////////////////////////////////////////////////////////////

#use rs232(baud=19200, parity=N, xmit=pin_C0,  bits=8, stream=DEBUG, ERRORS, force_sw) //Pin_B7 = PGD, Pin_B6 = PGC


////////////////////////////////////////////  UART APRS  ////////////////////////////////////////////////////////////////
unsigned int8 APRS_RESPONSE[210];

#define APRS_BFR_SIZE 200
// UART PC functions___________________________________________________________
#pin_select TX2 = PIN_G1   //MB1, CPLD(84)
#pin_select RX2 = PIN_G0   //MB1, CPLD(83)                                                      
#use rs232(baud=9600, parity=N, UART2, bits=8, stream=APRS, ERRORS)                     


unsigned int8  APRS_Buffer[APRS_BFR_SIZE];
unsigned int16 APRS_Byte_Counter = 0;
unsigned int8  APRS_Overflow = 0;
unsigned int16 APRS_Read_Byte_counter = 0;
unsigned int8  APRS_Temp_byte = 0;
#INT_RDA2
Void SERIAL_ISR2()                                                             // MAIN PIC uart interupt loop
{
   if( APRS_Byte_Counter < APRS_BFR_SIZE )
   {
      APRS_Buffer[APRS_Byte_Counter] = fgetc(APRS);
      APRS_Byte_Counter++;
   }
   
   else APRS_Overflow = fgetc(APRS);
}

unsigned int8 APRS_Available()
{
   return APRS_Byte_Counter ;
}

unsigned int8 APRS_Read()
{
   if (APRS_Byte_Counter>0)
   {    
      APRS_Temp_byte = APRS_Buffer[APRS_Read_Byte_counter];
      
      APRS_Byte_Counter--;
      APRS_Read_Byte_counter++;
      if(APRS_Byte_Counter == 0) APRS_Read_Byte_counter = 0;
      return APRS_Temp_byte; 
   }
   
   if (APRS_Byte_Counter == 0)
   { 
      APRS_Read_Byte_counter = 0;
      APRS_Temp_byte = 0x00;
      return APRS_Temp_byte; 
   }
 
}

void APRS_flush()
{
   while( APRS_Available() ) APRS_Read() ;
}


void GET_DATA_OR_ACK_FROM_APRS()
{
   if( APRS_Available() )
   {
      delay_ms(300);
      for(int i=0; i<200; i++)
      {
         APRS_RESPONSE[i] = APRS_READ();
      }
   }
}





///////////////////////////////////////////////////  UART MainPIC  /////////////////////////////////////////////////////////
char command_data[39];

#define OBC_BFR_SIZE 20
// UART PC functions___________________________________________________________
#pin_select TX3 = PIN_E1   //RAB, CPLD(81)
#pin_select RX3 = PIN_E0   //RAB, CPLD(82)
#use rs232(baud=9600, parity=N, UART3, bits=8, stream=OBC, ERRORS)                  


unsigned int8  OBC_Buffer[OBC_BFR_SIZE];
unsigned int16 OBC_Byte_Counter = 0;
unsigned int8  OBC_Overflow = 0;
unsigned int16 OBC_Read_Byte_counter = 0;
unsigned int8  OBC_Temp_byte = 0;

int8 Flag_OBC = 0;

#INT_RDA3
Void SERIAL_ISR3()                                                             // MAIN PIC uart interupt loop
{
   if( OBC_Byte_Counter < OBC_BFR_SIZE )
   {
      OBC_Buffer[OBC_Byte_Counter] = fgetc(OBC);
      OBC_Byte_Counter++;
   }
   
   else OBC_Overflow = fgetc(OBC);
}

unsigned int8 OBC_Available()
{
   return OBC_Byte_Counter ;
}

unsigned int8 OBC_Read()
{
   if (OBC_Byte_Counter>0)
   {    
      OBC_Temp_byte = OBC_Buffer[OBC_Read_Byte_counter];
      
      OBC_Byte_Counter--;
      OBC_Read_Byte_counter++;
      if(OBC_Byte_Counter == 0) OBC_Read_Byte_counter = 0;
      return OBC_Temp_byte; 
   }
   
   if (OBC_Byte_Counter == 0)
   { 
      OBC_Read_Byte_counter = 0;
      OBC_Temp_byte = 0x00;
      return OBC_Temp_byte; 
   }
 
}

void OBC_flush()
{
   while( OBC_Available() ) OBC_Read() ;
}

char header ;

void GET_COMMANDS_FROM_OBC()
{
   if( OBC_Available() )
   {
      header = OBC_READ() ;
      
      if( (0xB0 <= header) && (header <= 0xB6) )
      {    
         delay_ms(100);
         
         command_data[0] = header ;
         
         for(int i=1; i<20; i++)
         {
            command_data[i] = OBC_READ();
         }
         Flag_OBC = 1;
      }
   }
}


/////////////////////////////////////////////  SPI for shared flash memory  /////////////////////////////////////////

#use spi(MASTER, CLK = PIN_A0, DI = PIN_A1, DO = PIN_A2,  BAUD = 1000000, BITS = 8, STREAM = SHARED_FM, MODE = 0)
// CS pin = PIN_A3
unsigned int8 address[4];
unsigned int8 _data;


void SHARED_FM_WRITE_ENABLE()
{
  Output_low(Pin_A3);
  spi_xfer(SHARED_FM,0x06);                
  Output_high(Pin_A3);
  return;
}

void SHARED_FM_SECTOR_ERASE(unsigned int32 sector_address,char sector_size, unsigned int16 delay = 1000)
{
   
   address[0]  = (unsigned int8)((sector_address>>24) & 0xFF);   // 0x __ 00 00 00
   address[1]  = (unsigned int8)((sector_address>>16) & 0xFF);   // 0x 00 __ 00 00
   address[2]  = (unsigned int8)((sector_address>>8)  & 0xFF);   // 0x 00 00 __ 00
   address[3]  = (unsigned int8)((sector_address)     & 0xFF);   // 0x 00 00 00 __
   
   SHARED_FM_WRITE_ENABLE();
   Output_low(Pin_A3);             //lower the CS PIN

   ///////////////////////////////////////////////////////////////////

   if( Sector_size == 4  ) spi_xfer(SHARED_FM,0x21);                    // 4KB Sector erase
   if( Sector_size == 32 ) spi_xfer(SHARED_FM,0x5C);                    // 32KB Sector erase
   if( Sector_size == 64 ) spi_xfer(SHARED_FM,0xDC);                    // 64KB Sector erase
   
   spi_xfer(SHARED_FM,address[0]);   
   spi_xfer(SHARED_FM,address[1]);    
   spi_xfer(SHARED_FM,address[2]);    
   spi_xfer(SHARED_FM,address[3]);
   //////////////////////////////////////////////////////////////////
   Output_high(Pin_A3);           //take CS PIN higher back

   delay_ms(delay); 
   return;
}

void SHARED_FM_BYTE_WRITE(unsigned int32 byte_address, unsigned int8 data)
{
   
   //Byte extraction
   address[0]  = (unsigned int8)((byte_address>>24) & 0xFF);   // 0x __ 00 00 00
   address[1]  = (unsigned int8)((byte_address>>16) & 0xFF);   // 0x 00 __ 00 00
   address[2]  = (unsigned int8)((byte_address>>8)  & 0xFF);   // 0x 00 00 __ 00
   address[3]  = (unsigned int8)((byte_address)     & 0xFF);   // 0x 00 00 00 __
 
   SHARED_FM_WRITE_ENABLE();
   Output_low(Pin_A3);             //lower the CS PIN
   ///////////////////////////////////////////////////////////////////
   spi_xfer(SHARED_FM,0x12);         //Byte WRITE COMAND  (0x12)
   spi_xfer(SHARED_FM,address[0]);    
   spi_xfer(SHARED_FM,address[1]);    
   spi_xfer(SHARED_FM,address[2]);    
   spi_xfer(SHARED_FM,address[3]);

   spi_xfer(SHARED_FM,data); 
   //////////////////////////////////////////////////////////////////
   Output_high(Pin_A3);           //take CS PIN higher back 
   
   return;
}

//!unsigned int8 SHARED_FM_BYTE_READ(unsigned INT32 ADDRESS)
//!{
//!   //Byte extraction
//!   
//!   address[0]  = (unsigned int8)((ADDRESS>>24) & 0xFF);   // 0x __ 00 00 00
//!   address[1]  = (unsigned int8)((ADDRESS>>16) & 0xFF);   // 0x 00 __ 00 00
//!   address[2]  = (unsigned int8)((ADDRESS>>8)  & 0xFF);   // 0x 00 00 __ 00
//!   address[3]  = (unsigned int8)((ADDRESS)     & 0xFF);   // 0x 00 00 00 __
//!   
//!
//!   output_low(PIN_A3);             //lower the CS PIN
//!   ///////////////////////////////////////////////////////////////////
//!   spi_xfer(SHARED_FM,0X13);  //READ DATA COMAND   (0x13)
//!   spi_xfer(SHARED_FM,address[0]);
//!   spi_xfer(SHARED_FM,address[1]);
//!   spi_xfer(SHARED_FM,address[2]);
//!   spi_xfer(SHARED_FM,address[3]);
//!
//!   _data = spi_xfer(SHARED_FM); 
//!   //////////////////////////////////////////////////////////////////
//!   output_high(PIN_A3);            //take CS PIN higher back 
//!
//!   return _data;
//!}





////////////////////////////////////////////  CPLD config and input definition  /////////////////////////////
/*
CPLD connection configuration
board   sel_0   sel_1   sel_2
#R1     0       0       1
#R2     0       1       0
#P1     0       1       1
#P2     1       0       0
#P3     1       0       1
#P4     1       1       0
#P5     1       1       1
*/





