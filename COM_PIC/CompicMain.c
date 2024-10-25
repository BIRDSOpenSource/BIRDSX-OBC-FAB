#include <18F67J94.h>
#FUSES WDT,NOBROWNOUT
#use delay(crystal = 16MHz, clock = 16MHz)
#include <PIC18F67J94_REGISTERS.h>

char SAT_ID    = 0x77 ;     // this is the satellite id
int OLDTRX_RPL = 0    ;     // old tranceiver received packet lenngth
int NEWTRX_RPL = 0    ;     // old tranceiver received packet lenngth

int16 CW_INTERVAL = 100 ;

#include <ComPic_Settings.c>
#include <CPic_RPic.c>
#include <CPic_MPic.c>
#include <NEWTRX_Functions.c>
#include <OLDTRX_Functions.c>
#include <CW_Morse.c>

#include <GS_CMD.c>

#priority INT_RDA, INT_RDA4, INT_RDA3, INT_RDA2

char B_6, B_7 ;

void Settings()
{
   B_6 = input(PIN_B6);
   B_7 = input(PIN_B7);
   
   OUtput_Low(RSTPIC_RESTART_PIN) ;                  // reset pic Restart pin low
   OLD_TRX_RX_MODE();
   OUtput_LOW(TRXPWR_PIN);                           // New TRX off start
   NEW_TRX_RX_MODE();
     
   enable_interrupts(INT_RDA)  ;                      // enabling OLDTRX UART interupt
   enable_interrupts(INT_RDA2) ;                      // enabling MAIN PIC UART interupt
   enable_interrupts(INT_RDA3) ;                      // enabling RESET PIC UART interupt
   enable_interrupts(INT_RDA4) ;                      // enabling NEWTRX UART interupt
  
   enable_interrupts(GLOBAL);                        // start interupt procesing
   
   LOAD_CW_MODE_AND_NEW_TRX_STATUS();                // check cw tranmitter and new trx status
   ENABLE_DISABLE_NEW_TRX_POWER();                   // check new trx should be on or not
      
   SETUP_WDT(WDT_128S);

   Delay_ms(2000);
   fprintf(PC, "Compic booting..........\n\r");
   
   //output_high(TRXPWR_PIN);
}

void main()
{
   Settings();
   
   while(true)
   {
      CHECK_UART_INCOMING_FROM_NEW_TRX(0x54)  ;
      CHECK_UART_INCOMING_FROM_OLD_TRX(0x42)  ;
       
      COMUNICATE_WITH_RSTPIC_EVERY90SEC_AND_ASK_CW_DATA_FROM_MAINPIC(CW_INTERVAL);

      // =====================================================================================================================================
      // Processing command coming from New UHF Tranceiver_____________________
      EXTRACTING_NEW_TRANCEIVER_COMMAND(0x54);
      
      if( NEWTRX_TO_CPIC_ARRAY[0] == 0x54 && NEWTRX_TO_CPIC_ARRAY[1] == SAT_ID && NEWTRX_TO_CPIC_ARRAY[NEWTRX_RPL] == 0xAA )     // 0x54 = "T"
      {
         fprintf(PC, "Command from Tharindu's new Transceiver\n\r");
         
         PACKETS_DOWNLOAD_WITHOUT_FM_ACCESS()         ;       // 54 77 00 10 __ __ __ __ 00 00 00 __
         PACKETS_DOWNLOAD_WITH_FM_FULL_ACCESS()       ;       // 54 77 00 11 __ __ __ __ 00 00 00 __
         DATA_DOWNLOAD_FROM_MAIN_FM_OR_MSN_FM()       ;       // 54 77 00 PP __ __ __ __ 00 00 __ __    PP = 12 (MSNFM), PP = 13 (MAINFM)
         FLASH_MEMORY_SECTOR_ERASE()                  ;       // 54 77 00 10 __ __ __ __ 00 00 00 FM    FM = F1 (com FM), FM = F2 (main FM), FM = F3 (msn FM),
         
         UPDATE_RST_PIC_TIME_BY_GS_CMD()              ;       // 54 77 00 EA __ __ __ __ __ __ 00 00
         SEND_CUURENT_TIME_OF_THE_SATELLITE()         ;       // 54 77 00 EB 00 00 00 00 00 00 00 00
         RESTART_RESET_PIC_BY_GS_CMD()                ;       // 54 77 00 EC 00 00 00 00 00 00 00 00
         
         CONTROL_CW_TX_MODE()                         ;       // 54 77 00 CA MM 00 00 00 00 00 00 00    MM = AA newtrx cw, MM = BB oldtrx CW, else = no cw
         CONTROL_NEW_TRX_POWER()                      ;       // 54 77 00 CB MM 00 00 00 00 00 00 00    MM = AA newtrx board on, MM = BB newtrx board off
         TLE_DOWNLINK_MISSION()                       ;       // 54 77 AA CE 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
         
         FAB_KILL_SWITCH_CONTROL()                    ;       // 54 77 00 FF MM __ 00 00 00 00 00 00    MM = F1 fab kill, MM = F2 obc kill
         
         SENDING_APRS_MISSION_CMD_TO_MAINPIC()        ;       // 54 77 __ BX
         TRANSFER_RESERVATION_CMD_TO_MAINPIC()        ;       // 54 77 2X YY __ __ DD HH MM DD HH MM    X = 1 to A
         
         TRANSFERING_ANTENNA_RELATED_CMD_TO_MAINPIC() ;       // 54 77 00 DX 00 00 00 00 00 00 00 00 
         
         // finally crear the buffer_______________
         CLEAR_DATA_ARRAY(NEWTRX_TO_CPIC_ARRAY,50);
         NEWTRX_RPL  = 0;      // clearing new tranceiver receive packet length
      }
      // ____________________________________________________________________________________________________________________________________

      
     
      // =====================================================================================================================================
      // Processing command coming from ADDNICS Tranceiver_____________________
      EXTRACTING_OLD_TRANCEIVER_COMMAND(0x42);
      
      if( OLDTRX_TO_CPIC_ARRAY[0] == 0x42 && OLDTRX_TO_CPIC_ARRAY[1] == SAT_ID && OLDTRX_TO_CPIC_ARRAY[OLDTRX_RPL] == 0xBB )     // 0x4E = "B"
      {
         fprintf(PC, "Command from ADDNICS Transceiver\n\r");
         
         _PACKETS_DOWNLOAD_WITHOUT_FM_ACCESS()         ;       // 42 77 00 10 __ __ __ __ 00 00 00 __
         _PACKETS_DOWNLOAD_WITH_FM_FULL_ACCESS()       ;       // 42 77 00 11 __ __ __ __ 00 00 00 __
         _DATA_DOWNLOAD_FROM_MAIN_FM_OR_MSN_FM()       ;       // 42 77 00 PP __ __ __ __ 00 00 __ __    PP = 12 (MSNFM), PP = 13 (MAINFM)
         _FLASH_MEMORY_SECTOR_ERASE()                  ;       // 42 77 00 10 __ __ __ __ 00 00 00 FM    FM = F1 (com FM), FM = F2 (main FM), FM = F3 (msn FM),
         
         _UPDATE_RST_PIC_TIME_BY_GS_CMD()              ;       // 42 77 00 EA __ __ __ __ __ __ 00 00
         _SEND_CUURENT_TIME_OF_THE_SATELLITE()         ;       // 42 77 00 EB 00 00 00 00 00 00 00 00
         _RESTART_RESET_PIC_BY_GS_CMD()                ;       // 42 77 00 EC 00 00 00 00 00 00 00 00
         
         _CONTROL_CW_TX_MODE()                         ;       // 42 77 00 CA MM 00 00 00 00 00 00 00    MM = AA newtrx cw, MM = BB oldtrx CW, else = no cw
         _CONTROL_NEW_TRX_POWER()                      ;       // 42 77 00 CB MM 00 00 00 00 00 00 00    MM = AA newtrx board on, MM = BB newtrx board off
         _TURN_ON_CW_FOR_ONE_MINUTE()                  ;       // 42 77 00 CC MM 00 00 00 00 00 00 00
         _TLE_DOWNLINK_MISSION()                       ;       // 42 77 AA CE 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
         
         _FAB_KILL_SWITCH_CONTROL()                    ;       // 42 77 00 FF __ __ 00 00 00 00 00 00    MM = F1 fab kill, MM = F2 obc kill
         
         _SENDING_APRS_MISSION_CMD_TO_MAINPIC()        ;       // 42 77 __ BX
         _TRANSFER_RESERVATION_CMD_TO_MAINPIC()        ;       // 42 77 2X YY __ __ DD HH MM DD HH MM    X = 1 to A
         
         _TRANSFERING_ANTENNA_RELATED_CMD_TO_MAINPIC() ;       // 42 77 00 DX 00 00 00 00 00 00 00 00 
         
         _UPDATING_NEWTRX_SETTINGS_BY_GS_CMD()         ;       // 42 77 00 D1
         
         // finally crear the buffer_______________
         CLEAR_DATA_ARRAY(OLDTRX_TO_CPIC_ARRAY,50);
         OLDTRX_RPL = 0;     // clearing old tranceiver receive packet length
      }
      // _____________________________________________________________________________________________________________________________________
      
      delay_ms(10);
      
      RESTART_WDT();
      MLC++;
   }
}
