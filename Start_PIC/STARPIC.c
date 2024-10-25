#include <16F1789.h>
#fuses NOWDT,MCLR,NOBROWNOUT
#use delay(Crystal = 16MHz, clock = 16MHz)

#use rs232(baud=38400, parity=N, xmit=PIN_D3, bits=8, stream=PC, force_sw)  // dummy port

#include <StartPIc_Funtions.c>
unsigned int16 RPIC_RESPOND_SEC_COUNTER = 0 ;               // main loop counter
unsigned int16 MLC = 0 ;

unsigned int16 SEC_COUNT    = 0;
unsigned int8  ONEHOUR_FLAG = 0x00 ;   

char BB, CC;
void main()
{
   Delay_ms(500);
   fprintf(PC, "Start PIC booting...........\n\r");
   
   enable_interrupts(INT_RDA) ;                                                // enabling timer 1 interupt 
   enable_interrupts(GLOBAL)  ;                                                // start interupt procesing
   
   TURN_ON_ALL_POWER_LINES() ;                                                // beging with power lines off
   
   CC = input(PIN_B6);
   BB = input(PIN_B7);
   
   while(TRUE)
   {
      CHECK_UART_INCOMING_FROM_RESET_PIC();
 
      if( ONEHOUR_FLAG == 0xAA )
      {
         // check response from reset pic
         if( (RPIC_TO_SPIC_ARRAY[0] == 0xAA) && (RPIC_TO_SPIC_ARRAY[1] == 0xBB) && (RPIC_TO_SPIC_ARRAY[2] == 0xCC) )
         {
            RPIC_RESPOND_SEC_COUNTER = 0 ;
            MLC = 0;
            //fprintf(PC, "Reset pic respond\n\r");
            if( POWER_LINE_STATUS == ON ) TURN_OFF_ALL_POWER_LINES();
            
            CLEAR_RPIC_TO_SPIC_ARRAY();
         }
               
         if( MLC >= 50 )
         {
            MLC = 0;
            RPIC_RESPOND_SEC_COUNTER++;
         }
         
         // if response counter reach three minute we have to turn on power lines
         if( RPIC_RESPOND_SEC_COUNTER >= 36 )
         {
            fprintf(PC, "Reset pic did not respond for three minute\n\r");
            RPIC_RESPOND_SEC_COUNTER = 0 ;
            
            TURN_ON_ALL_POWER_LINES();
         }  
         MLC++;
      }
      
      if( ONEHOUR_FLAG == 0x00 )
      {
         SEC_COUNT++;
         if(SEC_COUNT >= 36000)     // one hour 36000
         {
            ONEHOUR_FLAG = 0xAA;
         }
      }
      
      delay_ms(100);
      Output_Toggle(PIN_B2);            // external WDT pin toggle  
   }
}

