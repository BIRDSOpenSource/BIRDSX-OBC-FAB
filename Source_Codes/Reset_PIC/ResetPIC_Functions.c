
/* This variable is used to monitor the main pic. It is increased in the
timer-1 interrupt loop every one second. If the main pic communicates 
correctly with the reset pic within 90 seconds, this variable will become 0, 
and it will be increased again. If there is no communication with the main pic,
this variable will go to a higher value. If it reaches 600 
(which equals 10 minutes), the reset pic will restart the main pic.*/
unsigned int16 MPIC_TIME_COUNTER = 0;

// this variable will count how many times main pic was reseted by reset pic
unsigned int8  NUMOF_MPIC_RST = 0;

// similar to above two variable below two variables are used to monitor com pic
unsigned int16 CPIC_TIME_COUNTER = 0;
unsigned int8  NUMOF_CPIC_RST = 0;

// these the UART buffers used to comunicate with Main Pic_____________________
unsigned int8 MPIC_TO_RPIC_ARRAY[35] ;   // only 10 bytes will be used
unsigned int8 RPIC_TO_MPIC_ARRAY[32] ;

// these the UART buffers used to comunicate with com Pic______________________
unsigned int8 CPIC_TO_RPIC_ARRAY[35] ;   // only 20
unsigned int8 RPIC_TO_CPIC_ARRAY[20] ;

// these the UART buffers used to comunicate with com Pic______________________
//unsigned int8 SPIC_TO_RPIC_ARRAY[10] ;   // only 
//unsigned int8 RPIC_TO_SPIC_ARRAY[10] ;

// these are RTC related variables
unsigned int8 year, month, day, hour, minute, second;

// this UART port is used as debug port of reset PIC___________________________
//#use rs232(baud=38400, parity=N, xmit=PIN_B7,  bits=8, stream = PC, errors, force_sw )               // Debuging software UART
#use rs232(baud=38400, parity=N, xmit=PIN_B2,  bits=8, stream = PC, errors, force_sw )  // dummy port
//B2

void RST_EXT_WDT()
{
   Output_Toggle(PIN_F2);
}

//=============================================================================
//This section handle the UART port connected to main pic______________________
#define MP_BFR_SIZE 30
#pin_select TX3=PIN_E1  
#pin_select RX3=PIN_E0  
#use rs232(UART3, baud=38400, parity=N, bits=8, stream=MPic, errors) 

unsigned int8  MP_Buffer[MP_BFR_SIZE];
unsigned int16 MP_Byte_Counter = 0;
unsigned int8  MP_Overflow = 0;
unsigned int16 MP_Read_Byte_counter = 0;
unsigned int8  MP_Temp_byte = 0;

unsigned int16 LAST_RESET_HOUR = 0;
#INT_RDA3
Void SERIAL_ISR3()         // MAIN PIC uart interupt loop
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
   return MP_Byte_Counter ;
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


//=============================================================================
//This section handle the UART port connected to com pic______________________
#define CP_BFR_SIZE 30
#pin_select TX2=PIN_G1  
#pin_select RX2=PIN_G0  
#use rs232(UART2, baud=38400, parity=N, bits=8, stream=CPic, errors) 

unsigned int8  CP_Buffer[CP_BFR_SIZE];
unsigned int16 CP_Byte_Counter = 0;
unsigned int8  CP_Overflow = 0;
unsigned int16 CP_Read_Byte_counter = 0;
unsigned int8  CP_Temp_byte = 0;

#INT_RDA2
Void SERIAL_ISR2()         // MAIN PIC uart interupt loop
{
   if( kbhit(CPic) )
   {
      if( CP_Byte_Counter < CP_BFR_SIZE )
      {
         CP_Buffer[CP_Byte_Counter] = fgetc(CPic);
         CP_Byte_Counter++;
      }
      else CP_Overflow = fgetc(CPic);
   }
}

unsigned int8 CPic_Available()
{
   return CP_Byte_Counter ;
}

unsigned int8 CPic_Read()
{
   if (CP_Byte_Counter>0)
   {    
      CP_Temp_byte = CP_Buffer[CP_Read_Byte_counter];
      
      CP_Byte_Counter--;
      CP_Read_Byte_counter++;
      if(CP_Byte_Counter == 0) CP_Read_Byte_counter = 0;
      return CP_Temp_byte; 
   }
   
   if (CP_Byte_Counter == 0)
   { 
      CP_Read_Byte_counter = 0;
      CP_Temp_byte = 0x00;
      return CP_Temp_byte; 
   }
 
}

void CPic_flush()
{
   while( CPic_Available() ) CPic_Read() ;
}
//=============================================================================


//=============================================================================
//#use rs232(baud=38400, xmit = PIN_C6,  parity=N, bits=8, stream=SPic, force_sw ,errors) 

//!//This section handle the UART port connected to com pic______________________
#define SP_BFR_SIZE 5
#pin_select TX1=PIN_C6  
#pin_select RX1=PIN_C7  
#use rs232(UART1, baud=38400, parity=N, bits=8, stream=SPic, errors) 

unsigned int8  SP_Buffer[SP_BFR_SIZE];
unsigned int16 SP_Byte_Counter = 0;
unsigned int8  SP_Overflow = 0;
unsigned int16 SP_Read_Byte_counter = 0;
unsigned int8  SP_Temp_byte = 0;

#INT_RDA
Void SERIAL_ISR()         // MAIN PIC uart interupt loop
{
   if( SP_Byte_Counter < SP_BFR_SIZE )
   {
      SP_Buffer[SP_Byte_Counter] = fgetc(SPic);
      SP_Byte_Counter++;
   }
   
   else SP_Overflow = fgetc(SPic);
}

unsigned int8 SPic_Available()
{
   return SP_Byte_Counter ;
}

unsigned int8 SPic_Read()
{
   if (SP_Byte_Counter>0)
   {    
      SP_Temp_byte = SP_Buffer[SP_Read_Byte_counter];
      
      SP_Byte_Counter--;
      SP_Read_Byte_counter++;
      if(SP_Byte_Counter == 0) SP_Read_Byte_counter = 0;
      return SP_Temp_byte; 
   }
   
   if (SP_Byte_Counter == 0)
   { 
      SP_Read_Byte_counter = 0;
      SP_Temp_byte = 0x00;
      return SP_Temp_byte; 
   }
 }

void SPic_flush()
{
   while( SPic_Available() ) SPic_Read() ;
}
//=============================================================================


// this function will just print a line in debug port
void printline()
{
   fprintf( PC, "\n\r");
}

// using this function we can make any data array clear
void CLEAR_DATA_ARRAY(unsigned int8 array[], int array_size)
{   
   for(int i = 0; i < array_size; i++)
   {
      array[i] = 0 ;
   }
}


