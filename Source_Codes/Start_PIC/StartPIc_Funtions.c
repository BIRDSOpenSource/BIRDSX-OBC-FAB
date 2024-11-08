#byte T1CON = 0x018
#bit    TMR1ON = T1CON.0
#bit    T1SYNC = T1CON.2
#bit    T1OSCEN = T1CON.3
#bit    T1CKPS0 = T1CON.4
#bit    T1CKPS1 = T1CON.5
#bit    TMR1CS0 = T1CON.6
#bit    TMR1CS1 = T1CON.7

unsigned int8 RPIC_TO_SPIC_ARRAY[10];

#define ON   0x69
#define OFF  0x96

char POWER_LINE_STATUS ;

// UART port related functions_________________________________________________
///////////////////////////////////////////////////////////////////////////////
#define RP_BFR_SIZE 10                                                        
#use rs232(UART1, baud=38400, parity=N, bits=8, stream=RPIC, ERRORS)

unsigned int8  RP_Buffer[RP_BFR_SIZE];
unsigned int16 RP_Byte_Counter = 0;
unsigned int8  RP_Overflow = 0;
unsigned int16 RP_Read_Byte_counter = 0;
unsigned int8  RP_Temp_byte = 0;

#INT_RDA
Void SERIAL_ISR1()                                                             // MAIN PIC uart interupt loop
{
   if( RP_Byte_Counter < RP_BFR_SIZE )
   {
      RP_Buffer[RP_Byte_Counter] = fgetc(RPIC);
      RP_Byte_Counter++;
   }
   
   else RP_Overflow = fgetc(RPIC);
}


unsigned int8 RPic_Available()
{
   return RP_Byte_Counter ;
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


void CHECK_UART_INCOMING_FROM_RESET_PIC()
{
   if( Rpic_Available() )
   {
      delay_ms(10);
      for(int i=0; i<8; i++)
      {
         if( RPIC_Read() == 0xAA )
         {
            RPIC_TO_SPIC_ARRAY[0] = 0xAA;
            break;
         }
      }
      
      for(int i=1; i<10; i++)
      {
         RPIC_TO_SPIC_ARRAY[i] = RPIC_Read() ;
      }
      
      for(int i=0; i<3; i++)
      {
         fprintf(PC, "%X ", RPIC_TO_SPIC_ARRAY[i] );
      }
      fprintf(PC, "\n\r");
   }
}


void TURN_OFF_ALL_POWER_LINES()
{
   output_Low(PIN_D6);                                                         // Switch enable for COM PIC
   output_Low(PIN_D7);                                                         // Switch enable for Main PIC   
   output_Low(PIN_C2);                                                         //OCP switch disable for Unreg #1
   output_Low(PIN_C5);                                                         //OCP switch disable for Unreg #2
   
   POWER_LINE_STATUS = OFF ;
   fprintf(PC, "Turned OFF Power lines\n\r");
}

void TURN_ON_ALL_POWER_LINES()
{
   output_High(PIN_D6);                                                         
   Delay_ms(50);
   output_High(PIN_D7);                                                         
   Delay_ms(50);
   output_High(PIN_C2);                                                         
   Delay_ms(50);
   output_High(PIN_C5);                                                        
   Delay_ms(50);
   
   POWER_LINE_STATUS = ON ;
   fprintf(PC, "Turned ON Power lines\n\r");
}

void CLEAR_RPIC_TO_SPIC_ARRAY()
{
   for( int i = 0; i<10; i++ ) RPIC_TO_SPIC_ARRAY[i] = 0;
}
