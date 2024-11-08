#include <18F67J94.h>
#FUSES NOWDT,NOBROWNOUT,SOSC_DIG
#use delay(crystal = 16MHz, clock = 16MHz)
#include <PIC18F67J94_REGISTERS.h>
#include <stdio.h>

#include <MainPIC_Settings.c>
#include <RTC_functions.c>
#include <Flash_Memory.c>
#include <RESERVE_fun.c>
#include <MPIC_RPIC.c>
#include <MPIC_FAB.c>
#include <MPIC_MBOSS.c>
#include <MPIC_CPIC.c>
#include <Debug.c>

#PRIORITY INT_RDA4, INT_RDA2, INT_RDA3, INT_RDA 

Void setting()
{
   enable_interrupts(INT_RDA);                 // DEBUG  UART interupt
   enable_interrupts(INT_RDA2);                // COMPIC UART interupt
   enable_interrupts(INT_RDA3);                // FABPIC UART interupt
   enable_interrupts(INT_RDA4);                // RSTPIC UART interupt
   enable_interrupts(GLOBAL);
   
   Output_Low(PIN_A4);                         // OBC kill switch enable
   
   SETUP_RTC(RTC_ENABLE | RTC_CLOCK_SOSC,0);   // enabling internal RTC of main pic
   Write_OBC_RTC(23,07,28,00,00,01);           // setting RTC time value
   
   OUTPUT_HIGH(PIN_C5);                        // start COM flash memoy acces to com PIC
   OUTPUT_HIGH(PIN_A5);                        // start MSN flash memoy acces to mboss
   
   output_HIGH(MBOSS_EN);                      // For DIO of Mission Boss, it must be removed in real operation 
}

void main()
{
   Delay_ms(1000);
   setting();
   fprintf(PC,"MainPIC booting...........\n\r");
   
   LOAD_ANTENNA_DEPLOYMENT_VALUES();
   LOAD_RESERVATION_COMMANDS_ON_OFF_TIME_AT_BOOT();
   
   LOAD_30DAY_COUNTER();
   INCREACE_30DAY_COUNTER_ONCE_PER_DAY_AND_CHECK_30DAY_CW_STATUS();
      
   while(true)
   {     
      // this will print main pic RTC__________________________________________
      READ_MP_RTC_AND_PRINT();
      
      DEPLOY_ANTENNAS_SET_1(31,5);              // antenna dep min = 31, number of days = 5
      DEPLOY_ANTENNAS_SET_2(40,5);              // antenna dep min = 40, number of days = 5
      
      COMUNICATION_WITH_RST_PIC_90SEC(90)   ;   // 90 means every 90 seconds main pic will comunicate with reset pic
      
      CHECK_RESERVATION_COMMAND_ON_TIME_AND_OFF_TIME();   
      RESTARTING_MISSIONS_AFTER_SYSTEM_RESET()        ;
      
      CHECK_UART_INCOMING_FROM_DEBUG_PORT() ;
      CHECK_UART_INCOMING_FROM_COM_PIC()    ;
                    
      
      /////////////////////////////////////////////////////////////////////////
      // this section will execute com pic commands____________________________
      if( CPIC_TO_MPIC_ARRAY[0] == 0xB0 && CPIC_TO_MPIC_ARRAY[39] == 0xB1 )
      {
         PRINT_RECIVED_COMMAND_FROM_COM_PIC();    
         Delay_ms(5);    // just delay 5ms before respond
         
         // perform your task here
         GIVE_COMFM_ACCESS_TO_COMPIC_FOR_DATA_DOWNLOAD();       // 0x00, 0xFA
         DATA_COPY_FROM_MSN_FM_TO_CFM_USING_GS_CMD()  ;         // 0x00, 0x12
         DATA_COPY_FROM_MAIN_FM_TO_CFM_USING_GS_CMD() ;         // 0x00, 0x13
         SECTOR_ERASE_USING_GS_COMMAND();                       // 0x00, 0x14
         GIVE_CW_DATA_TO_COM_PIC();                             // 0x00, 0xCA
         CONTROL_KILL_SWITCHES();                               // 0x00, 0xFF, 0xF1 for FAB and 0cF2 for OBC, 0x01 close 0x00 open
         
         WRITE_GS_CMD_IN_RESERVATION_LOCATION()          ;      // 0x21 to 2A, 0xXX
         
         SEND_APRS_COMMAND_TO_MISSIONBOSS_THROUGH_MAIN() ;       // 0x00, 0xB? ?==> Board number
         
         DEPLOY_ANTENNA_USING_GS_COMMAND()               ;       // 0xDA 
         SET_ANTENNA_VALUES_USING_GS_COMMAND()           ;       // 0xDB
         
         RESET_30DAY_CONTER();
         //_____________________________________________________
         // finaly clear the data array received from com pic
         CLEAR_DATA_ARRAY( CPIC_TO_MPIC_ARRAY, 32 );
      }
      
      
      /////////////////////////////////////////////////////////////////////////
      // this section will execute debug port commands_________________________
      if( DEBUG_TO_MPIC_ARRAY[0] == 0xD0 && DEBUG_TO_MPIC_ARRAY[12] == 0xD1 )
      {
         PRINT_RECIVED_COMMAND_FROM_DEBUG_PORT(); 
         Delay_ms(5);    // just delay 5ms before respond
         
         
         // perform your task here
         READ_FM_DATA_THROUGH_PC();                  // 0x1F
         DEPLOY_ANTENNA_INSTANTLY();                 // 0xDA
         SET_UP_ANTENNA_DEPLOYMENT_VALUES();         // 0xDB
         GET_ANTENNA_DEPLOYMENT_VALUES();            // 0xDC
         UPDATE_RESET_PIC_TIME();                    // 0x7B
         
         CLOSE_FAB_KILL_SWITCH_DB() ;                // 0xF1
         CLOSE_OBC_KILL_SWITCH_DB() ;                // 0xF2 
         OPEN_FAB_KILL_SWITCH_DB()  ;                // 0xF3
         OPEN_OBC_KILL_SWITCH_DB()  ;                // 0xF4
         POWER_LINE_CONTROL_USING_DEBUG_COMMAND_TO_RST_PIC(); // 0xF5
         RESET_SATELLITE_CMD()      ;                // 0x7D
         
         SEND_COM_CMD_THROUGH_PC()  ;                // 0x77
            
         //_____________________________________________________
         // finaly clear the data array received from debug port
         CLEAR_DATA_ARRAY( DEBUG_TO_MPIC_ARRAY, 20 );
      }
      CHECK_COMFM_ACCES_RELEASING();
      MLC++;  
   } 
}
















      /////////////////////////////////////////////////////////////////////////
      // this section will execute Mission Boss commands____________________________
     /* if( MBOSS_TO_MPIC_ARRAY[0] == 0x90 && MBOSS_TO_MPIC_ARRAY[12] == 0x91 )
      {
         PRINT_RECIVED_COMMAND_FROM_MISSION_BOSS();
         Delay_ms(5);    // just delay 5ms before respond
         
         // perform your task here
         APRS_PAYLOAD_FLAG_OPERATION();              // if ACK from MBOSS == 0xB?
         TRANSFER_N_PACKETS_TO_FLASH();              // 0x13
         ERASE_SFWD_FLASH();                         // 0x14
         
         // finaly clear the data array received from com port
         CLEAR_DATA_ARRAY( MBOSS_TO_MPIC_ARRAY, 40 );     
      }*/













//!      /////////////////////////////////////////////////////////////////////////
//!      // this section will execute FAB pic commands____________________________
//!      if( FAB_TO_MPIC_ARRAY[0] == 0xF0 && FAB_TO_MPIC_ARRAY[31] == 0xF1 )
//!      {
//!         PRINT_RECIVED_COMMAND_FROM_FAB_PIC();
//!         Delay_ms(5);    // just delay 5ms before respond
//!         
//!         // perform your task here
//!         
//!         
//!         //___________________________________________________
//!         // finaly clear the data array received from com port
//!         CLEAR_DATA_ARRAY( FAB_TO_MPIC_ARRAY, 32 );     
//!      }
//!      
//!      /////////////////////////////////////////////////////////////////////////
//!      // this section will execute reset pic commands__________________________
//!      if( RPIC_TO_MPIC_ARRAY[0] == 0xA0 && RPIC_TO_MPIC_ARRAY[31] == 0xA1 )
//!      {
//!         PRINT_RECIVED_COMMAND_FROM_RESET_PIC();
//!         //RESET_TO_MAIN_FLAG = 1;
//!         Delay_ms(5);    // just delay 5ms before respond
//!         
//!         // perform your task here
//!         
//!         
//!         //_____________________________________________________
//!         // finaly clear the data array received from reset port
//!         CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32 );     
//!      }
