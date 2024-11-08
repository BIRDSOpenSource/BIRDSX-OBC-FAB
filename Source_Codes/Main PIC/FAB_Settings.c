// these data arrays are used to comunicated with OBC
   unsigned int8 MPIC_TO_FAB_ARRAY[12];  // only 3 bytes are used
   unsigned int8 FAB_TO_MPIC_ARRAY[50];

// debug uart port_____________________________________________________________
//#use rs232(baud=19200, xmit = PIN_E2,  parity=N, bits=8, stream=PC, force_sw, ERRORS)
#use rs232(baud=19200, xmit = PIN_B7,  parity=N, bits=8, stream=PC, force_sw, ERRORS)


// UART port related functions_________________________________________________
///////////////////////////////////////////////////////////////////////////////
#define MP_BFR_SIZE 10                                                        
#use rs232(UART1, baud=38400, parity=N, bits=8, stream=MPIC, ERRORS)

unsigned int8  MP_Buffer[MP_BFR_SIZE];
unsigned int16 MP_Byte_Counter = 0;
unsigned int8  MP_Overflow = 0;
unsigned int16 MP_Read_Byte_counter = 0;
unsigned int8  MP_Temp_byte = 0;
#INT_RDA
Void SERIAL_ISR1()                                                             // MAIN PIC uart interupt loop
{
   if( MP_Byte_Counter < MP_BFR_SIZE )
   {
      MP_Buffer[MP_Byte_Counter] = fgetc(MPIC);
      MP_Byte_Counter++;
   }
   
   else MP_Overflow = fgetc(MPIC);
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
//_____________________________________________________________________________



// using this function we can make any data array clear________________________
void CLEAR_DATA_ARRAY(unsigned int8 array[], int array_size)
{   
   for(int i = 0; i < array_size; i++)
   {
      array[i] = 0 ;
   }
}

// just printing line__________________________________________________________
void printline()
{
   fprintf(PC, "\n\r");
}

// FAB kill switch status PIN = D3_____________________________________________
// D3 reading = 1 means FAB kill switch cloase and solar panels charging battery
// D3 reading = 0 means solar panels disconected through FAB kill switch
void FAB_KILL_SWITCH( int status)
{
   Output_High(PIN_D4);    // FAB_Kill_Switch driver enable pin
   Delay_ms(100);
   
   if( status == CLOSE )
   {
      // D5 = 1, D6 = 0
      Output_High(PIN_D5);    
      Output_Low(PIN_D6) ;
      fprintf(PC, "FAB Kill Switch Closing done\n\r");
   }
   
   if( status == OPEN )
   {
      // D5 = 0, D6 = 1
      Output_Low(PIN_D5) ;      
      Output_High(PIN_D6); 
      fprintf(PC, "FAB Kill Switch Opening done\n\r");
   }
   
   Delay_ms(100);
   Output_Low(PIN_D4);    // FAB_Kill_Switch driver enable pin
}

// FAB kill switch status PIN = C1
// C1 reading = 1 means OBC kill switch cloase and solar panels charging battery
// C1 reading = 0 means solar panels disconected through OBC kill switch
void OBC_KILL_SWITCH(int status)
{
   if( status == CLOSE )
   {
      // D2 = 1, D1 = 0
      Output_High(PIN_D2);    
      Output_Low(PIN_D1) ;
      fprintf(PC, "OBC Kill Switch Closing done\n\r");
   }
   
   if( status == OPEN  )
   {
      Output_Low(PIN_D2);     
      Output_High(PIN_D1);
       fprintf(PC, "OBC Kill Switch Opening done\n\r");
   }
}

