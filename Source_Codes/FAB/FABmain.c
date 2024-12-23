#include <16F1789.h>
#device ADC = 12;
#fuses WDT,MCLR                    // internal watch dog timer will be used.
#use delay(internal=16MHz)

#include <Definitions.c>
#include <FAB_Settings.c>
#include <FAB_OBC.c>

char B_6, B_7 ;

void Settings()
{
   B_6 = input(PIN_B6);
   B_7 = input(PIN_B7);
   
   Setup_WDT(WDT_8S);                // setting up internal WDT 64 seconds
   
   SETUP_ADC(ADC_CLOCK_INTERNAL);
   SETUP_ADC_PORTS(ALL_ANALOG)  ;    // setting all analog ports
   
   enable_interrupts(INT_RDA);
   enable_interrupts(global) ;       // enabling uart interupt
}

void main()
{
   Settings();
   Delay_ms(1000);
   fprintf(PC, "Starting FAB pic___\n\r");
   
   PRINT_KILL_SWITCHES_CURRENT_STATE();
   PRINT_KILL_SWITCHES_EEPROM_VLAUES();
   
   while(true)
   {       
      CHECK_UART_INCOMING_FROM_MPIC();
      
      if( (MPIC_TO_FAB_ARRAY[0] == 0xFA) && (MPIC_TO_FAB_ARRAY[2] == 0xFB) )
      {
         PRINT_MAIN_PIC_COMMAND();
         Delay_ms(5);
         //________________________________________
         
         SEND_FAB_DATA_TO_MAIN_PIC() ;      // 0xA0
         
         FAB_KILL_SWITCH_CLOSE()     ;      // 0xF1
         OBC_KILL_SWITCH_CLOSE()     ;      // 0xF2
         
         // GS must send 10 comands to open the FAB kill switch
         FAB_KILL_SWITCH_OPEN()      ;      // 0xF3
         // GS must send 10 comands to open the OBC kill switch
         OBC_KILL_SWITCH_OPEN()      ;      // 0xF4 
         
         //________________________________________
         CLEAR_DATA_ARRAY(MPIC_TO_FAB_ARRAY, 10);
      }
      
      Restart_WDT();
   }
}
