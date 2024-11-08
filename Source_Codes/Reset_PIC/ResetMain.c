#include <18F67J94.h>
#FUSES NOWDT,NOBROWNOUT,SOSC_DIG
#device ADC = 12
#use delay(crystal = 16MHz, clock = 16MHz)
#include <PIC18F67J94_REGISTERS.h>

#include <ResetPIC_Functions.c>
#include <ADC_Power_Lines_Control.c>
#include <RPIC_MPIC.c>
#include <RPIC_CPIC.c>
#include <RPIC_STARTPIC.c>
#include <RTC_fun.c>

unsigned int16 MLC = 0;

// we set interupt priority here , timer 1 is the highest priority_____________
#PRIORITY INT_TIMER1,INT_RDA,INT_RDA3           

// this is the timer 1 interupt loop___________________________________________
#INT_TIMER1
Void TIMER1_ISR()                                                              
{ 
  set_timer1(0x8000);                                // Timer-1 preload
  RTC_Function();                                    // updating RTC
  
  MPIC_TIME_COUNTER++;                               // this is main pic comunication time counter
  CPIC_TIME_COUNTER++;                               // this is com  pic comunication time counter
}

// here we set the reset pic initial setups____________________________________
void settings()
{
   //output_low(PIN_C6);
   MP_CP_BuckBoost(ON)              ;      // enable MP CP buck boost conveter
   MainPic_Power(ON)                ;      // turn on main pic power
   ComPic_Power(ON)                 ;      // turn on com pic power
   _3V3Power_Line1(BB_ON_OCP_ON)    ;      // both obc and buck boost converters are ON
   _3V3Power_Line2(BB_ON_OCP_ON)    ;      // both obc and buck boost converter are OFF
   _5V0Power_Line(BB_ON_OCP_ON)     ;      // both obc and buck boost converters are OFF BB_ON_OCP_ON
   
   Unreg1_Line(ON)                  ;      // turn on unreg line 1
   Unreg2_Line(ON)                  ;      // turn off unreg line 2

   setup_timer_1(T1_EXTERNAL | T1_DIV_BY_1);         // timer-1 clock source ans prescaler value         
   SOSCEN1 = 1;                                      // enabling timer 1
   set_timer1(0x8000);                               // timer 1 preload
   
   enable_interrupts(INT_RDA2);                      // enabling com pic UART interupt
   enable_interrupts(INT_RDA3);                      // enabling main pic UART interupt
   enable_interrupts(INT_TIMER1);                    // enabling timer - 1 interupt
   enable_interrupts(GLOBAL);                        // start interupt procesing
   
   SETUP_ADC(ADC_CLOCK_INTERNAL);
   SETUP_ADC_PORTS(sAN2|sAN1|sAN0|sAN4|sAN6|sAN9);   // setting all analog ports
   
   Set_RTC(24,01,01, 00,00,06);
   MPic_flush() ;
   fprintf( PC, "Reset pic is booting.......\n\r");
}

void main()
{
   settings();
   while(true)
   {  
      // printing rtc__________________________________________________________
      Print_RTC();  
      
      // this funtion will reset the system if time is 00:00:00________________
      SYSTEM_RESET_24H();
      
      // check uart incoming from main pic and compic__________________________
      CHECK_UART_INCOMING_FROM_MAIN_PIC()  ;
      CHECK_UART_INCOMING_FROM_COM_PIC()   ; 
      
      // monitoring 90 second comunication of compic and mainpic_______________
      MONITOR_MAIN_PIC_90SEC_COMUNICATION(10)  ;   // 10 means how much turn off time before restart 
      MONITOR_COM_PIC_90SEC_COMUNICATION(10)   ;   // 10 means how much turn off time before restart 
  
      
      // executing main pic comands____________________________________________
      if( MPIC_TO_RPIC_ARRAY[0] == 0xA0 && MPIC_TO_RPIC_ARRAY[9] == 0xA1 )
      {
         PRINT_RECIVED_COMMAND_FROM_MAIN_PIC();
         Delay_ms(5);   // just delay 5ms before execute
         RST_EXT_WDT();
         
         RESEPOND_TO_MPIC_90SEC_CMD();                // 0x7A
         UPDATE_RTC_BY_MAIN_PIC_CMD();                // 0x7B
         POWER_LINE_CONTROL_USING_MAIN_PIC_CMD();     // 0x7C
         RESET_SATELLITE_CMD();                       // 0x7D
         TURN_ON_UNREG_2_LINE_FOR_ANT_DEPLOYMENT();   // 0xDA
         
         // finaly clear the data array received from main pic
         CLEAR_DATA_ARRAY( MPIC_TO_RPIC_ARRAY, 10 );
      }
      
      // executing com pic comands____________________________________________
      if( CPIC_TO_RPIC_ARRAY[0] == 0xC0 && CPIC_TO_RPIC_ARRAY[19] == 0xC1 )
      {
         PRINT_RECIVED_COMMAND_FROM_COM_PIC();
         Delay_ms(5);   // just delay 5ms before execute
         RST_EXT_WDT();
         
         RESEPOND_TO_CPIC_90SEC_CMD()     ;           // 0xE0
         UPDATE_RTC_BY_COM_PIC_CMD()      ;           // 0xEA
         SENDING_TIME_TO_COMPIC()         ;           // 0xEB
         
         // finaly clear the data array received from main pic
         CLEAR_DATA_ARRAY( CPIC_TO_RPIC_ARRAY, 20 );
      }
      
          
      if( (MLC%25) == 0 )  // every half sec
      {        
         Output_Toggle(PIN_F2); 
         if(MLC>=500)
         {
            INFORM_WORKING_TO_START_PIC();
            MLC = 0;
         }
      }

      Delay_ms(10);
      MLC++;
   }
}
