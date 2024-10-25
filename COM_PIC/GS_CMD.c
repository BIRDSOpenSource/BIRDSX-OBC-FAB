// Followings are already writen in the compic
//_______________________________________________________________________________________________________________________________________________________
//!   PACKETS_DOWNLOAD_WITHOUT_FM_ACCESS()       ;       // 54 77 00 10 __ __ __ __ 00 00 00 __
//!   PACKETS_DOWNLOAD_WITH_FM_FULL_ACCESS()     ;       // 54 77 00 11 __ __ __ __ 00 00 00 __
//!   DATA_DOWNLOAD_FROM_MAIN_FM_OR_MSN_FM()     ;       // 54 77 00 PP __ __ __ __ 00 00 __ __    PP = 12 (MSNFM), PP = 13 (MAINFM)
//!   FLASH_MEMORY_SECTOR_ERASE()                ;       // 54 77 00 10 __ __ __ __ 00 00 00 FM    FM = F1 (com FM), FM = F2 (main FM), FM = F3 (msn FM),
//!   
//!   UPDATE_RST_PIC_TIME_BY_GS_CMD()            ;       // 54 77 00 EA __ __ __ __ __ __ 00 00
//!   SEND_CUURENT_TIME_OF_THE_SATELLITE()       ;       // 54 77 00 EB 00 00 00 00 00 00 00 00
//!   
//!   CONTROL_CW_TX_MODE()                       ;       // 54 77 00 CA MM 00 00 00 00 00 00 00    MM = AA newtrx cw, MM = BB oldtrx CW, else = no cw
//!   CONTROL_NEW_TRX_POWER()                    ;       // 54 77 00 CB MM 00 00 00 00 00 00 00    MM = AA newtrx board on, MM = BB newtrx board off
//!   FAB_KILL_SWITCH_CONTROL()                  ;       // 54 77 00 FF MM __ 00 00 00 00 00 00    MM = F1 fab kill, MM = F2 obc kill
//!   
//!   SENDING_APRS_MISSION_CMD_TO_MAINPIC()      ;       // 54 77 __ BX
//______________________________________________________________________________________________________________________________________________________



// this function is used to download data from flash memory without getting full acces of flash memry
// there can be a intant Main Pic get fm acces to write HSK data. com pic will not be able to get data 
// at that momment

//=========================================================================================================================================
Void PACKETS_DOWNLOAD_WITHOUT_FM_ACCESS()
{
   if( NEWTRX_TO_CPIC_ARRAY[2] == 0x00 && NEWTRX_TO_CPIC_ARRAY[3] == 0x10 )                 
   {
      Fprintf(PC,"NEW_TRX_PACKETS_DOWNLOAD_WITHOUT_FM_ACCESS\n\r");
      
      unsigned int32 START_ADDRESS = make32( NEWTRX_TO_CPIC_ARRAY[4] , NEWTRX_TO_CPIC_ARRAY[5], NEWTRX_TO_CPIC_ARRAY[6], NEWTRX_TO_CPIC_ARRAY[7] );
      unsigned int16 NUM_OF_PCKT   = NEWTRX_TO_CPIC_ARRAY[11];
      
      NEW_TRX_SEND_DATA_PACKETS_TO_GS(START_ADDRESS, NUM_OF_PCKT); 
   }
}

Void _PACKETS_DOWNLOAD_WITHOUT_FM_ACCESS()
{
   if( OLDTRX_TO_CPIC_ARRAY[2] == 0x00 && OLDTRX_TO_CPIC_ARRAY[3] == 0x10 )                 
   {
      Fprintf(PC,"OLD_TRX_PACKETS_DOWNLOAD_WITHOUT_FM_ACCESS\n\r");
      
      unsigned int32 START_ADDRESS = make32( OLDTRX_TO_CPIC_ARRAY[4] , OLDTRX_TO_CPIC_ARRAY[5], OLDTRX_TO_CPIC_ARRAY[6], OLDTRX_TO_CPIC_ARRAY[7] );
      unsigned int16 NUM_OF_PCKT   = OLDTRX_TO_CPIC_ARRAY[11];
      
      OLD_TRX_SEND_DATA_PACKETS_TO_GS(START_ADDRESS, NUM_OF_PCKT); 
   }
}
//=========================================================================================================================================


//=========================================================================================================================================
// Download data from FM with full access to Flash memory
Void PACKETS_DOWNLOAD_WITH_FM_FULL_ACCESS()
{
   if( NEWTRX_TO_CPIC_ARRAY[2] == 0x00 && NEWTRX_TO_CPIC_ARRAY[3] == 0x11 )                 
   {
      Fprintf(PC,"NEW_TRX_PACKETS_DOWNLOAD_WITH_FM_ACCESS\n\r");
      
      unsigned int32 START_ADDRESS = make32( NEWTRX_TO_CPIC_ARRAY[4] , NEWTRX_TO_CPIC_ARRAY[5], NEWTRX_TO_CPIC_ARRAY[6], NEWTRX_TO_CPIC_ARRAY[7] );
      unsigned int16 NUM_OF_PCKT   = NEWTRX_TO_CPIC_ARRAY[11];
      unsigned int8 acces_time = 0;
      
      // calculating acces time
      if( ( 0<NUM_OF_PCKT ) && (NUM_OF_PCKT<=100) ) acces_time = 50  ;    // adjust this values after checking
      if( (100<NUM_OF_PCKT) && (NUM_OF_PCKT<=200) ) acces_time = 100 ;
      if( (200<NUM_OF_PCKT) && (NUM_OF_PCKT<=255) ) acces_time = 150 ;
      
      GET_FM_ACCESS_FROM_MAIN_PIC(acces_time);
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 && MPIC_TO_CPIC_ARRAY[1] == 0xFA && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         Fprintf(PC,"FM access request succeful\n\r");
         NEW_TRX_SEND_DATA_PACKETS_TO_GS(START_ADDRESS, NUM_OF_PCKT);    // send packets
      }
      
      else
      {
         Fprintf(PC,"FM access request not-succeful\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(0);
      }
      
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 50);
   }
}

Void _PACKETS_DOWNLOAD_WITH_FM_FULL_ACCESS()
{
   if( OLDTRX_TO_CPIC_ARRAY[2] == 0x00 && OLDTRX_TO_CPIC_ARRAY[3] == 0x11 )                 
   {
      Fprintf(PC,"OLD_TRX_PACKETS_DOWNLOAD_WITH_FM_ACCESS\n\r");
      
      unsigned int32 START_ADDRESS = make32( OLDTRX_TO_CPIC_ARRAY[4] , OLDTRX_TO_CPIC_ARRAY[5], OLDTRX_TO_CPIC_ARRAY[6], OLDTRX_TO_CPIC_ARRAY[7] );
      unsigned int16 NUM_OF_PCKT   = OLDTRX_TO_CPIC_ARRAY[11];
      unsigned int8 acces_time = 0;
      
      // calculating acces time
      if( ( 0<NUM_OF_PCKT ) && (NUM_OF_PCKT<=100) ) acces_time = 50  ;    // adjust this values after checking
      if( (100<NUM_OF_PCKT) && (NUM_OF_PCKT<=200) ) acces_time = 100 ;
      if( (200<NUM_OF_PCKT) && (NUM_OF_PCKT<=255) ) acces_time = 150 ;
      
      GET_FM_ACCESS_FROM_MAIN_PIC(acces_time);
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 && MPIC_TO_CPIC_ARRAY[1] == 0xFA && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         Fprintf(PC,"FM access request succeful\n\r");
         OLD_TRX_SEND_DATA_PACKETS_TO_GS(START_ADDRESS, NUM_OF_PCKT);    // send packets
      }
      
      else
      {
         Fprintf(PC,"FM access request not-succeful\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(0);
      }
      
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 50);
   }
}
//=========================================================================================================================================





//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//=========================================================================================================================================
void DATA_DOWNLOAD_FROM_MAIN_FM_OR_MSN_FM()
{
   if( NEWTRX_TO_CPIC_ARRAY[2] == 0x00 && ( NEWTRX_TO_CPIC_ARRAY[3] == 0x12 || NEWTRX_TO_CPIC_ARRAY[3] == 0x13 ) )
   {
      unsigned int8 _FM = NEWTRX_TO_CPIC_ARRAY[3];
      
      if( _FM == 0x12 ) Fprintf(PC,"DATA_DOWNLOAD_FROM_MSN_FM  NEWTRX\n\r");
      if( _FM == 0x13 ) Fprintf(PC,"DATA_DOWNLOAD_FROM_MAIN_FM NEWTRX\n\r");  
      
      //_________________________________________________________________
      CLEAR_DATA_ARRAY(CPIC_TO_MPIC_ARRAY, 40);
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 40);
      CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;                       // header
      for(int i = 1; i<=10; i++)
      {
         CPIC_TO_MPIC_ARRAY[i] = NEWTRX_TO_CPIC_ARRAY[i+1];
      }
      CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;                       // footer
      //_________________________________________________________________
      
      COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(2, 1000,200);
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 &&  MPIC_TO_CPIC_ARRAY[1] == _FM  && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 40);
         Fprintf(PC,"Main PIC responded copying started \n\r"); 
         int16 count = 0;
         while( !(MPIC_TO_CPIC_ARRAY[0] == 0xB0 &&  MPIC_TO_CPIC_ARRAY[1] == _FM  && MPIC_TO_CPIC_ARRAY[31] == 0xB1) )
         {
            CHECK_UART_INCOMING_FROM_MAIN_PIC() ; 
            Delay_ms(1);
            count++;
            if( count >= 10000) break;
         }
         
         if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 &&  MPIC_TO_CPIC_ARRAY[1] == _FM  && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
         {
            Fprintf(PC,"Main PIC responded copying finish sendinf data to GS\n\r");
            unsigned int32 START_ADDRESS = make32( NEWTRX_TO_CPIC_ARRAY[4]  , NEWTRX_TO_CPIC_ARRAY[5], NEWTRX_TO_CPIC_ARRAY[6], NEWTRX_TO_CPIC_ARRAY[7] );
            unsigned int16 NUM_OF_PCKT   = make16( NEWTRX_TO_CPIC_ARRAY[10] , NEWTRX_TO_CPIC_ARRAY[11] );
            
            NEW_TRX_SEND_DATA_PACKETS_TO_GS(START_ADDRESS, NUM_OF_PCKT);
         }   
         else
         {
            Fprintf(PC,"Copying timeout \n\r");
            NEWTRX_SEND_ACKNOWLEDGEMENT(0);
         }  
      }
      
      else
      {
         Fprintf(PC,"Main PIC not responded\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(0);
      }
   }
   

}

void _DATA_DOWNLOAD_FROM_MAIN_FM_OR_MSN_FM()
{
   if( OLDTRX_TO_CPIC_ARRAY[2] == 0x00 && ( OLDTRX_TO_CPIC_ARRAY[3] == 0x12 || OLDTRX_TO_CPIC_ARRAY[3] == 0x13 ) )
   {
      unsigned int8 _FM = OLDTRX_TO_CPIC_ARRAY[3];
      
      if( _FM == 0x12 ) Fprintf(PC,"DATA_DOWNLOAD_FROM_MSN_FM  OLDTRX\n\r");
      if( _FM == 0x13 ) Fprintf(PC,"DATA_DOWNLOAD_FROM_MAIN_FM OLDTRX\n\r");  
      
      //_________________________________________________________________
      CLEAR_DATA_ARRAY(CPIC_TO_MPIC_ARRAY, 40);
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 40);
      CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;                       // header
      for(int i = 1; i<=10; i++)
      {
         CPIC_TO_MPIC_ARRAY[i] = OLDTRX_TO_CPIC_ARRAY[i+1];
      }
      CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;                       // footer
      //_________________________________________________________________
      
      COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(2, 1000,200);
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 &&  MPIC_TO_CPIC_ARRAY[1] == _FM  && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 40);
         Fprintf(PC,"Main PIC responded copying started \n\r"); 
         int16 count = 0;
         while( !(MPIC_TO_CPIC_ARRAY[0] == 0xB0 &&  MPIC_TO_CPIC_ARRAY[1] == _FM  && MPIC_TO_CPIC_ARRAY[31] == 0xB1) )
         {
            CHECK_UART_INCOMING_FROM_MAIN_PIC() ; 
            Delay_ms(1);
            count++;
            if( count >= 10000) break;
         }
         
         if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 &&  MPIC_TO_CPIC_ARRAY[1] == _FM  && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
         {
            Fprintf(PC,"Main PIC responded copying finish sendinf data to GS\n\r");
            unsigned int32 START_ADDRESS = make32( OLDTRX_TO_CPIC_ARRAY[4]  , OLDTRX_TO_CPIC_ARRAY[5], OLDTRX_TO_CPIC_ARRAY[6], OLDTRX_TO_CPIC_ARRAY[7] );
            unsigned int16 NUM_OF_PCKT   = make16( OLDTRX_TO_CPIC_ARRAY[10] , OLDTRX_TO_CPIC_ARRAY[11] );
            
            OLD_TRX_SEND_DATA_PACKETS_TO_GS(START_ADDRESS, NUM_OF_PCKT);
         }   
         else
         {
            Fprintf(PC,"Copying timeout \n\r");
            OLDTRX_SEND_ACKNOWLEDGEMENT(0);
         }  
      }
      
      else
      {
         Fprintf(PC,"Main PIC not responded\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(0);
      }
   }
   

}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//===============================================================================================================================================
void FLASH_MEMORY_SECTOR_ERASE()
{
   if( NEWTRX_TO_CPIC_ARRAY[2] == 0x00 && NEWTRX_TO_CPIC_ARRAY[3] == 0x14  )
   {
      Fprintf(PC,"FLASH_MEMORY_SECTOR_ERASE_NEWTRX\n\r");
      
      //_________________________________________________________________
      CLEAR_DATA_ARRAY(CPIC_TO_MPIC_ARRAY, 40);
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 40);
      CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;                       // header
      for(int i = 1; i<=10; i++)
      {
         CPIC_TO_MPIC_ARRAY[i] = NEWTRX_TO_CPIC_ARRAY[i+1];
      }
      CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;                       // footer
      //_________________________________________________________________
      COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(2, 200,170);
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 && MPIC_TO_CPIC_ARRAY[1] == 0x14 && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         Fprintf(PC,"Main pic responded\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(1);
      }
      
      else
      {
         Fprintf(PC,"Main pic did not respond\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(0);
      }
   }
}

void _FLASH_MEMORY_SECTOR_ERASE()
{
   if( OLDTRX_TO_CPIC_ARRAY[2] == 0x00 && OLDTRX_TO_CPIC_ARRAY[3] == 0x14  )
   {
      Fprintf(PC,"FLASH_MEMORY_SECTOR_ERASE_OLDTRX\n\r");
      
      //_________________________________________________________________
      CLEAR_DATA_ARRAY(CPIC_TO_MPIC_ARRAY, 40);
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 40);
      CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;                       // header
      for(int i = 1; i<=10; i++)
      {
         CPIC_TO_MPIC_ARRAY[i] = OLDTRX_TO_CPIC_ARRAY[i+1];
      }
      CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;                       // footer
      //_________________________________________________________________
      COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(2, 200,170);
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 && MPIC_TO_CPIC_ARRAY[1] == 0x14 && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         Fprintf(PC,"Main pic responded\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(1);
      }
      
      else
      {
         Fprintf(PC,"Main pic did not respond\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(0);
      }
   }
}
//=========================================================================================================================================



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//=========================================================================================================================================
void TRANSFERING_ANTENNA_RELATED_CMD_TO_MAINPIC()
{
   if( (NEWTRX_TO_CPIC_ARRAY[2] == 0x00) && ( (NEWTRX_TO_CPIC_ARRAY[3] & 0xF0) == 0xD0 ) )                 
   {
      Fprintf(PC,"TRANSFERING_ANTENNA_RELATED_CMD_TO_MAINPIC_ADDTRX\n\r");
      
      //_________________________________________________________________
      CLEAR_DATA_ARRAY(CPIC_TO_MPIC_ARRAY, 40);
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 40);
      CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;                       // header
      for(int i = 1; i<=10; i++)
      {
         CPIC_TO_MPIC_ARRAY[i] = NEWTRX_TO_CPIC_ARRAY[i+1];
      }
      CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;                       // footer
      //_________________________________________________________________
      COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(2, 1000,200);
      
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 &&  (MPIC_TO_CPIC_ARRAY[1]&0xF0) == 0xD0  && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         Fprintf(PC,"Main pic responded\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(1);
      }
      
      else
      {
         Fprintf(PC,"Main pic did not respond\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(0);
      }
   }
}


void _TRANSFERING_ANTENNA_RELATED_CMD_TO_MAINPIC()
{
   if( (OLDTRX_TO_CPIC_ARRAY[2] == 0x00) && ( (OLDTRX_TO_CPIC_ARRAY[3] & 0xF0) == 0xD0 ) )                 
   {
      Fprintf(PC,"TRANSFERING_ANTENNA_RELATED_CMD_TO_MAINPIC_ADDTRX\n\r");
      
      //_________________________________________________________________
      CLEAR_DATA_ARRAY(CPIC_TO_MPIC_ARRAY, 40);
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 40);
      CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;                       // header
      for(int i = 1; i<=10; i++)
      {
         CPIC_TO_MPIC_ARRAY[i] = OLDTRX_TO_CPIC_ARRAY[i+1];
      }
      CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;                       // footer
      //_________________________________________________________________
      COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(2, 1000,200);
      
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 &&  (MPIC_TO_CPIC_ARRAY[1]&0xF0) == 0xD0  && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         Fprintf(PC,"Main pic responded\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(1);
      }
      
      else
      {
         Fprintf(PC,"Main pic did not respond\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(0);
      }
   }
}



// MISION_COMMANDS MISION_COMMANDS MISION_COMMANDS MISION_COMMANDS MISION_COMMANDS MISION_COMMANDS MISION_COMMANDS MISION_COMMANDS MISION_COMMANDS
// MISION_COMMANDS MISION_COMMANDS MISION_COMMANDS MISION_COMMANDS MISION_COMMANDS MISION_COMMANDS MISION_COMMANDS MISION_COMMANDS MISION_COMMANDS 
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//=========================================================================================================================================
void SENDING_APRS_MISSION_CMD_TO_MAINPIC()
{
   if( (NEWTRX_TO_CPIC_ARRAY[2] == 0x00) && ( (NEWTRX_TO_CPIC_ARRAY[3] & 0xF0) == 0xB0 ) )                 
   {
      Fprintf(PC,"SENDING_APRS_MISSION_CMD_TO_MAINPIC_NEWTRX\n\r");
      
      //_________________________________________________________________
      CLEAR_DATA_ARRAY(CPIC_TO_MPIC_ARRAY, 40);
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 40);
      CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;                       // header
      for(int i = 1; i<=10; i++)
      {
         CPIC_TO_MPIC_ARRAY[i] = NEWTRX_TO_CPIC_ARRAY[i+1];
      }
      CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;                       // footer
      //_________________________________________________________________
      COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(2, 1000,200);
      
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 && (MPIC_TO_CPIC_ARRAY[1]&0xF0) == 0xB0 && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         Fprintf(PC,"Main pic responded\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(1);
      }
      
      else
      {
         Fprintf(PC,"Main pic did not respond\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(0);
      }
   }
}

void _SENDING_APRS_MISSION_CMD_TO_MAINPIC()
{
   if( (OLDTRX_TO_CPIC_ARRAY[2] == 0x00) && ( (OLDTRX_TO_CPIC_ARRAY[3] & 0xF0) == 0xB0 ) )                 
   {
      Fprintf(PC,"SENDING_APRS_MISSION_CMD_TO_MAINPIC_OLDTRX\n\r");
      
      //_________________________________________________________________
      CLEAR_DATA_ARRAY(CPIC_TO_MPIC_ARRAY, 40);
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 40);
      CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;                       // header
      for(int i = 1; i<=10; i++)
      {
         CPIC_TO_MPIC_ARRAY[i] = OLDTRX_TO_CPIC_ARRAY[i+1];
      }
      CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;                       // footer
      //_________________________________________________________________
      COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(2, 1000,200);
      
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 && (MPIC_TO_CPIC_ARRAY[1]&0xF0) == 0xB0 && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         Fprintf(PC,"Main pic responded\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(1);
      }
      
      else
      {
         Fprintf(PC,"Main pic did not respond\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(0);
      }
   }
}
//=========================================================================================================================================



//COMPIC COMMANDS COMPIC COMMANDS COMPIC COMMANDS COMPIC COMMANDS COMPIC COMMANDS COMPIC COMMANDS COMPIC COMMANDS COMPIC COMMANDS COMPIC COMMANDS
//COMPIC COMMANDS COMPIC COMMANDS COMPIC COMMANDS COMPIC COMMANDS COMPIC COMMANDS COMPIC COMMANDS COMPIC COMMANDS COMPIC COMMANDS COMPIC COMMANDS
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//=========================================================================================================================================
void CONTROL_CW_TX_MODE()
{
   if( NEWTRX_TO_CPIC_ARRAY[2] == 0x00 && NEWTRX_TO_CPIC_ARRAY[3] == 0xCA )                 
   {
      Fprintf(PC,"CONTROL_CW_TX_MODE_NEW_TRX\n\r");
      unsigned int16 CWMODE  = (unsigned int16) NEWTRX_TO_CPIC_ARRAY[4];
      unsigned int16 TRXMODE = READ_PROGRAM_EEPROM(0x18002);
      
      ERASE_PROGRAM_EEPROM(0x18000);
      WRITE_PROGRAM_EEPROM(0x18000,CWMODE );
      WRITE_PROGRAM_EEPROM(0x18002,TRXMODE);
      Delay_ms(10);
      LOAD_CW_MODE_AND_NEW_TRX_STATUS();
      
      NEWTRX_SEND_ACKNOWLEDGEMENT(1);
   }
}

void _CONTROL_CW_TX_MODE()
{
   if( OLDTRX_TO_CPIC_ARRAY[2] == 0x00 && OLDTRX_TO_CPIC_ARRAY[3] == 0xCA )                 
   {
      Fprintf(PC,"CONTROL_CW_TX_MODE_OLD_TRX\n\r");
      unsigned int16 CWMODE  = (unsigned int16) OLDTRX_TO_CPIC_ARRAY[4];
      unsigned int16 TRXMODE = READ_PROGRAM_EEPROM(0x18002);
      
      ERASE_PROGRAM_EEPROM(0x18000);
      WRITE_PROGRAM_EEPROM(0x18000,CWMODE );
      WRITE_PROGRAM_EEPROM(0x18002,TRXMODE);
      Delay_ms(10);
      LOAD_CW_MODE_AND_NEW_TRX_STATUS();
      
      OLDTRX_SEND_ACKNOWLEDGEMENT(1);
   }
}
//=========================================================================================================================================


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//=========================================================================================================================================
void CONTROL_NEW_TRX_POWER()
{
   if( NEWTRX_TO_CPIC_ARRAY[2] == 0x00 && NEWTRX_TO_CPIC_ARRAY[3] == 0xCB )                 
   {
      Fprintf(PC,"CONTROL_NEW_TRX_POWER__NEWTRX\n\r");
      unsigned int16 CWMODE  = READ_PROGRAM_EEPROM(0x18000); 
      unsigned int16 TRXMODE = (unsigned int16) NEWTRX_TO_CPIC_ARRAY[4];
      
      ERASE_PROGRAM_EEPROM(0x18000);
      WRITE_PROGRAM_EEPROM(0x18000,CWMODE );
      WRITE_PROGRAM_EEPROM(0x18002,TRXMODE);
      Delay_ms(10);
      NEWTRX_SEND_ACKNOWLEDGEMENT(1);
            
      LOAD_CW_MODE_AND_NEW_TRX_STATUS();
      ENABLE_DISABLE_NEW_TRX_POWER();                   // check new trx should be on or not
   }
}

void _CONTROL_NEW_TRX_POWER()
{
   if( OLDTRX_TO_CPIC_ARRAY[2] == 0x00 && OLDTRX_TO_CPIC_ARRAY[3] == 0xCB )                 
   {
      Fprintf(PC,"CONTROL_NEW_TRX_POWER__OLDTRX\n\r");
      unsigned int16 CWMODE  = READ_PROGRAM_EEPROM(0x18000); 
      unsigned int16 TRXMODE = (unsigned int16) OLDTRX_TO_CPIC_ARRAY[4];
      
      ERASE_PROGRAM_EEPROM(0x18000);
      WRITE_PROGRAM_EEPROM(0x18000,CWMODE );
      WRITE_PROGRAM_EEPROM(0x18002,TRXMODE);
      Delay_ms(10);
      OLDTRX_SEND_ACKNOWLEDGEMENT(1);
      
      LOAD_CW_MODE_AND_NEW_TRX_STATUS();
      ENABLE_DISABLE_NEW_TRX_POWER();                   // check new trx should be on or not
   }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//=========================================================================================================================================
void _TURN_ON_CW_FOR_ONE_MINUTE()
{
   if( OLDTRX_TO_CPIC_ARRAY[2] == 0x00 && OLDTRX_TO_CPIC_ARRAY[3] == 0xCC )                 
   {
      Fprintf(PC,"TURN_ON_CW_FOR_ONE_MINUTE__OLDTRX\n\r");
      
      OLDTRX_SEND_ACKNOWLEDGEMENT(1);
      OLD_TRX_CW_MODE();
      Output_high(CW_PIN);
      for(int i = 0; i<60; i++)
      {
         Delay_ms(1000);
      }
      Output_low(CW_PIN);
      OLD_TRX_RX_MODE();
   }
}

// new t
void TLE_DOWNLINK_MISSION()
{
   if( NEWTRX_TO_CPIC_ARRAY[2] == 0xA1 && NEWTRX_TO_CPIC_ARRAY[3] == 0xCE )                 
   {
      Fprintf(PC,"TLE_DOWNLINK_MISSION__NEWTRX\n\r");
      NEWTRX_SEND_ACKNOWLEDGEMENT(1);
      
      FM_SECTOR_ERASE( TLE_ADDRESS, 4 );  // we earse only first 4KB
      
      for( int i = 0; i<34 ; i++)
      {
         FM_BYTE_WRITE( (TLE_ADDRESS + i) , NEWTRX_TO_CPIC_ARRAY[i+4] ) ;
      }
   }
}

void _TLE_DOWNLINK_MISSION()
{
   if( (OLDTRX_TO_CPIC_ARRAY[2] & 0xF0) == 0xA0  && OLDTRX_TO_CPIC_ARRAY[3] == 0xCE )                 
   {
      Fprintf(PC,"TLE_DOWNLINK_MISSION__OLDTRX\n\r");
      
      if( OLDTRX_TO_CPIC_ARRAY[2] == 0xA1 )
      {
         FM_SECTOR_ERASE( TLE_ADDRESS, 4 );  // we earse only first 4KB
      
         for( int i = 0; i<17 ; i++)
         {
            FM_BYTE_WRITE( (TLE_ADDRESS + i) , OLDTRX_TO_CPIC_ARRAY[i+4] ) ;
         }
         
         printline();
         for( int i = 0; i<34 ; i++)
         {
            fprintf(PC , "%X ", FM_BYTE_READ_( TLE_ADDRESS + i ) );
         }
         printline();
         OLD_TRX_SEND_DATA_PACKETS_TO_GS(TLE_ADDRESS, 1);
      }
      
      else if( OLDTRX_TO_CPIC_ARRAY[2] == 0xA2 )
      {
         //FM_SECTOR_ERASE( TLE_ADDRESS, 4 );  // we earse only first 4KB
      
         for( int i = 17; i<34 ; i++)
         {
            FM_BYTE_WRITE( (TLE_ADDRESS + i) , OLDTRX_TO_CPIC_ARRAY[i-13] ) ;
         }
         
         printline();
         for( int i = 0; i<34 ; i++)
         {
            fprintf(PC , "%X ", FM_BYTE_READ_( TLE_ADDRESS + i ) );
         }
         printline();
         OLD_TRX_SEND_DATA_PACKETS_TO_GS(TLE_ADDRESS, 1);
      }
      
      
      //14 byte length format
      else
      {
         unsigned int8 CMD_NUM = OLDTRX_TO_CPIC_ARRAY[2] - 0xA3 ;              // A3-A3 = 1
         if( CMD_NUM == 0) FM_SECTOR_ERASE( TLE_ADDRESS, 4 );                  // we earse only first 4KB      
         
         for( int i = 0 ; i<8 ; i++)
         {
            FM_BYTE_WRITE( (TLE_ADDRESS + i + (CMD_NUM*8) ) , OLDTRX_TO_CPIC_ARRAY[i+4] ) ;
         }
         
         printline();
         for( int i = 0; i<34 ; i++)
         {
            fprintf(PC , "%X ", FM_BYTE_READ_( TLE_ADDRESS + i ) );
         }
         printline();
         OLD_TRX_SEND_DATA_PACKETS_TO_GS(TLE_ADDRESS, 1);
      }

   }
}



//RESETPIC_COMMANDS RESETPIC_COMMANDS RESETPIC_COMMANDS RESETPIC_COMMANDS RESETPIC_COMMANDS RESETPIC_COMMANDS RESETPIC_COMMANDS RESETPIC_COMMANDS
//RESETPIC_COMMANDS RESETPIC_COMMANDS RESETPIC_COMMANDS RESETPIC_COMMANDS RESETPIC_COMMANDS RESETPIC_COMMANDS RESETPIC_COMMANDS RESETPIC_COMMANDS
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//=========================================================================================================================================
// updating rest pic time using GS command new trx
Void UPDATE_RST_PIC_TIME_BY_GS_CMD()
{
   if( NEWTRX_TO_CPIC_ARRAY[2] == 0x00 && NEWTRX_TO_CPIC_ARRAY[3] == 0xEA )                 
   {
      Fprintf(PC,"NEW_TRX_UPDATE_RST_PIC_TIME_BY_GS_CMD\n\r");
      
      UPDATE_RESET_PIC_TIME_BY_COM_PIC(NEWTRX_TO_CPIC_ARRAY[4],NEWTRX_TO_CPIC_ARRAY[5], NEWTRX_TO_CPIC_ARRAY[6], NEWTRX_TO_CPIC_ARRAY[7], NEWTRX_TO_CPIC_ARRAY[8], NEWTRX_TO_CPIC_ARRAY[9] ) ;
            
      if( RPIC_TO_CPIC_ARRAY[0] == 0xC0 && RPIC_TO_CPIC_ARRAY[1] == 0xEA && RPIC_TO_CPIC_ARRAY[19] == 0xC1 )
      {
         Fprintf(PC,"Reset pic time updated\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(1);                       // send succes ack
      }
      
      else
      {
         Fprintf(PC,"Reset pic did not respond\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(0);
      }
      
      CLEAR_DATA_ARRAY(RPIC_TO_CPIC_ARRAY, 20);
   }
}

// updating rest pic time using GS command old trx
Void _UPDATE_RST_PIC_TIME_BY_GS_CMD()
{
   if( OLDTRX_TO_CPIC_ARRAY[2] == 0x00 && OLDTRX_TO_CPIC_ARRAY[3] == 0xEA )                 
   {
      Fprintf(PC,"OLD_TRX_UPDATE_RST_PIC_TIME_BY_GS_CMD\n\r");
      
      UPDATE_RESET_PIC_TIME_BY_COM_PIC(OLDTRX_TO_CPIC_ARRAY[4],OLDTRX_TO_CPIC_ARRAY[5], OLDTRX_TO_CPIC_ARRAY[6], OLDTRX_TO_CPIC_ARRAY[7], OLDTRX_TO_CPIC_ARRAY[8], OLDTRX_TO_CPIC_ARRAY[9] ) ;
      
      if( RPIC_TO_CPIC_ARRAY[0] == 0xC0 && RPIC_TO_CPIC_ARRAY[1] == 0xEA && RPIC_TO_CPIC_ARRAY[19] == 0xC1 )
      {
         Fprintf(PC,"Reset pic time updated\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(1);                       // send succes ack
      }
      
      else
      {
         Fprintf(PC,"Reset pic did not respond\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(0);
      }
      
      CLEAR_DATA_ARRAY(RPIC_TO_CPIC_ARRAY, 20);
   }
}
//===============================================================================================================================================


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//=========================================================================================================================================
// updating rest pic time using GS command new trx
Void SEND_CUURENT_TIME_OF_THE_SATELLITE()
{
   if( NEWTRX_TO_CPIC_ARRAY[2] == 0x00 && NEWTRX_TO_CPIC_ARRAY[3] == 0xEB )                 
   {
      Fprintf(PC,"SEND_CUURENT_TIME_OF_THE_SATELLITE\n\r");
      
      CLEAR_DATA_ARRAY(CPIC_TO_RPIC_ARRAY,20);
      CLEAR_DATA_ARRAY(RPIC_TO_CPIC_ARRAY,20);
      CPIC_TO_RPIC_ARRAY[0]  = 0xC0;
      CPIC_TO_RPIC_ARRAY[1]  = 0xEB;
      CPIC_TO_RPIC_ARRAY[19] = 0xC1;
      COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(2, 500, 400);
      PRINT_RECIVED_COMMAND_FROM_RESET_PIC();
      
      
      if( RPIC_TO_CPIC_ARRAY[0] == 0xC0 && RPIC_TO_CPIC_ARRAY[1] == 0xEB && RPIC_TO_CPIC_ARRAY[19] == 0xC1 )
      {
         for( int i = 4; i<=10; i++) NEWTRX_TO_CPIC_ARRAY[i] = RPIC_TO_CPIC_ARRAY[i-2];
         
         Fprintf(PC,"Reset pic responded\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(1);                       // send succes ack
      }
      
      else 
      {
         Fprintf(PC,"Reset pic did not responded\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(0);                       // send succes ack
      }
   }
}

Void _SEND_CUURENT_TIME_OF_THE_SATELLITE()
{
   if( OLDTRX_TO_CPIC_ARRAY[2] == 0x00 && OLDTRX_TO_CPIC_ARRAY[3] == 0xEB )                 
   {
      Fprintf(PC,"SEND_CUURENT_TIME_OF_THE_SATELLITE\n\r");
      
      CLEAR_DATA_ARRAY(CPIC_TO_RPIC_ARRAY,20);
      CLEAR_DATA_ARRAY(RPIC_TO_CPIC_ARRAY,20);
      CPIC_TO_RPIC_ARRAY[0]  = 0xC0;
      CPIC_TO_RPIC_ARRAY[1]  = 0xEB;
      CPIC_TO_RPIC_ARRAY[19] = 0xC1;
      COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(2, 500, 500);
      PRINT_RECIVED_COMMAND_FROM_RESET_PIC();
      
      if( RPIC_TO_CPIC_ARRAY[0] == 0xC0 && RPIC_TO_CPIC_ARRAY[1] == 0xEB && RPIC_TO_CPIC_ARRAY[19] == 0xC1 )
      {
         for( int i = 4; i<=10; i++) OLDTRX_TO_CPIC_ARRAY[i] = RPIC_TO_CPIC_ARRAY[i-2];
         
         Fprintf(PC,"Reset pic responded\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(1);                       // send succes ack
      }
      
      else 
      {
         Fprintf(PC,"Reset pic did not responded\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(0);                       // send succes ack
      }
   }
}




//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//=========================================================================================================================================
// Restart Reset pic using gs command
Void RESTART_RESET_PIC_BY_GS_CMD()
{
   if( NEWTRX_TO_CPIC_ARRAY[2] == 0x00 && NEWTRX_TO_CPIC_ARRAY[3] == 0xEC )                 
   {
      Fprintf(PC,"RESTART_RESET_PIC_BY_GS_CMD_NEWTRX\n\r");
      NEWTRX_SEND_ACKNOWLEDGEMENT(1);                       // send succes ack
      
      OUtput_HIGH(RSTPIC_RESTART_PIN) ;
   }
}

Void _RESTART_RESET_PIC_BY_GS_CMD()
{
   if( OLDTRX_TO_CPIC_ARRAY[2] == 0x00 && OLDTRX_TO_CPIC_ARRAY[3] == 0xEC )                 
   {
      Fprintf(PC,"RESTART_RESET_PIC_BY_GS_CMD_ADDTRX\n\r");
      OLDTRX_SEND_ACKNOWLEDGEMENT(1);                       // send succes ack
      
      OUtput_HIGH(RSTPIC_RESTART_PIN) ;
      
   }
}


//FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS  
//FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS FAB_COMMANDS  
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//===============================================================================================================================================
//control FAB kill switch
Void FAB_KILL_SWITCH_CONTROL()
{
   if( NEWTRX_TO_CPIC_ARRAY[2] == 0x00 && NEWTRX_TO_CPIC_ARRAY[3] == 0xFF )                 
   {
      Fprintf(PC,"FAB_KILL_SWITCH_CONTROL_NEWTRX\n\r");
      
      CLEAR_DATA_ARRAY(CPIC_TO_MPIC_ARRAY, 40);
      CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;                              // header
      CPIC_TO_MPIC_ARRAY[1]  = NEWTRX_TO_CPIC_ARRAY[2];            // 
      CPIC_TO_MPIC_ARRAY[2]  = NEWTRX_TO_CPIC_ARRAY[3];            // command
      CPIC_TO_MPIC_ARRAY[3]  = NEWTRX_TO_CPIC_ARRAY[4];            // FAB select Obc kill = 0xF1, Fab kill = 0xF2
      CPIC_TO_MPIC_ARRAY[4]  = NEWTRX_TO_CPIC_ARRAY[5];            // Close = 0x01, Open = 0x00
      CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;                              // footer
      COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(1, 1,1000);  
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 && MPIC_TO_CPIC_ARRAY[1] == 0xFF && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         Fprintf(PC,"Kill switch operation succesfull\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(1);
      }
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 && MPIC_TO_CPIC_ARRAY[1] == 0xF0 && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         Fprintf(PC,"Kill switch operation not succesfull\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(0);
      }
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 50);
   }
}

Void _FAB_KILL_SWITCH_CONTROL()
{
   if( OLDTRX_TO_CPIC_ARRAY[2] == 0x00 && OLDTRX_TO_CPIC_ARRAY[3] == 0xFF )                 
   {
      Fprintf(PC,"FAB_KILL_SWITCH_CONTROL_OLDTRX\n\r");
      
      CLEAR_DATA_ARRAY(CPIC_TO_MPIC_ARRAY, 40);
      CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;                              // header
      CPIC_TO_MPIC_ARRAY[1]  = OLDTRX_TO_CPIC_ARRAY[2];            // 
      CPIC_TO_MPIC_ARRAY[2]  = OLDTRX_TO_CPIC_ARRAY[3];            // command
      CPIC_TO_MPIC_ARRAY[3]  = OLDTRX_TO_CPIC_ARRAY[4];            // FAB select Obc kill = 0xF1, Fab kill = 0xF2
      CPIC_TO_MPIC_ARRAY[4]  = OLDTRX_TO_CPIC_ARRAY[5];            // Close = 0x01, Open = 0x00
      CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;                              // footer
      COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(1, 1,1000);  
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 && MPIC_TO_CPIC_ARRAY[1] == 0xFF && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         Fprintf(PC,"Kill switch operation succesfull\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(1);
      }
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 && MPIC_TO_CPIC_ARRAY[1] == 0xF0 && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         Fprintf(PC,"Kill switch operation not succesfull\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(0);
      }
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 50);
   }
}
//===============================================================================================================================================



// NEWTRX setting commands NEWTRX setting commands NEWTRX setting commands NEWTRX setting commands NEWTRX setting commands NEWTRX setting commands NEWTRX setting commands
// NEWTRX setting commands NEWTRX setting commands NEWTRX setting commands NEWTRX setting commands NEWTRX setting commands NEWTRX setting commands NEWTRX setting commands
// NEWTRX setting commands NEWTRX setting commands NEWTRX setting commands NEWTRX setting commands NEWTRX setting commands NEWTRX setting commands NEWTRX setting commands


void _UPDATING_NEWTRX_SETTINGS_BY_GS_CMD()
{
   if( OLDTRX_TO_CPIC_ARRAY[2] == 0x00 && ( (OLDTRX_TO_CPIC_ARRAY[3] == 0xD1) ||  (OLDTRX_TO_CPIC_ARRAY[3] == 0xD2) || (OLDTRX_TO_CPIC_ARRAY[3] == 0xD3) ) )                 
   {
      Fprintf(PC,"_UPDATING_NEWTRX_SETTINGS_BY_GS_CMD\n\r");
      
      CLEAR_DATA_ARRAY(CPIC_TO_NEWTRX_ARRAY, 60);
      CPIC_TO_NEWTRX_ARRAY[0]  = 0x7E ;                              // header
      for(int i = 3; i<=11; i++) 
      {
         CPIC_TO_NEWTRX_ARRAY[i-2] = OLDTRX_TO_CPIC_ARRAY[i] ;
      }
      CPIC_TO_NEWTRX_ARRAY[50] = 0x7E ;                              // footer
      
      NEWTRX_flush();
      // sending command to new trx
      for(int i = 0; i<=50; i++)
      {
         fputc(CPIC_TO_NEWTRX_ARRAY[i],NEW_TRX);
      } 
      delay_ms(500);
      
      char D1 = NEWTRX_READ();
      char D2 = NEWTRX_READ();
      char D3 = NEWTRX_READ();

      
      if( (D1 == 0x7E) && (D3 == 0x7E))
      {
         fprintf(PC, "Respons %X %X %X \n\r", D1, D2, D3);
         if(D2 == 0xD1) 
         {
            Fprintf(PC,"New TRX setting updated correctly \n\r");
            OLDTRX_SEND_ACKNOWLEDGEMENT(1);
         }
         
         else if(D2 == 0xD2)
         {
            Fprintf(PC,"New TRX Receive frequency updated correctly \n\r");
            OLDTRX_SEND_ACKNOWLEDGEMENT(1);
         }
         
         else if(D2 == 0xD3)
         {
            Fprintf(PC,"New TRX transmit frequency updated correctly \n\r");
            OLDTRX_SEND_ACKNOWLEDGEMENT(1);            
         }
         
         else Fprintf(PC,"different response \n\r");
      }
      
      else
      {
         Fprintf(PC,"New TRX did not respond\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(0);
      }
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 50);
   }
}



// Reservation Commands Reservation Commands Reservation Commands Reservation Commands Reservation Commands Reservation Commands Reservation Commands Reservation Commands
// Reservation Commands Reservation Commands Reservation Commands Reservation Commands Reservation Commands Reservation Commands Reservation Commands Reservation Commands
// Reservation Commands Reservation Commands Reservation Commands Reservation Commands Reservation Commands Reservation Commands Reservation Commands Reservation Commands
void TRANSFER_RESERVATION_CMD_TO_MAINPIC()
{
   if( (NEWTRX_TO_CPIC_ARRAY[2] & 0xF0) == 0x20 )                 
   {
      Fprintf(PC,"TRANSFER_RESERVATION_CMD_TO_MAINPIC_NEWTRX\n\r");
      
      //_________________________________________________________________
      CLEAR_DATA_ARRAY(CPIC_TO_MPIC_ARRAY, 40);
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 40);
      CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;                       // header
      for(int i = 1; i<=10; i++)
      {
         CPIC_TO_MPIC_ARRAY[i] = NEWTRX_TO_CPIC_ARRAY[i+1];
      }
      CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;                       // footer
      //_________________________________________________________________
      COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(2, 1000,200);
      
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 && (MPIC_TO_CPIC_ARRAY[1]&0xF0) == 0x20 && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         Fprintf(PC,"Main pic responded\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(1);
      }
      
      else
      {
         Fprintf(PC,"Main pic did not respond\n\r");
         NEWTRX_SEND_ACKNOWLEDGEMENT(0);
      }
   }
}


void _TRANSFER_RESERVATION_CMD_TO_MAINPIC()
{
   if( (OLDTRX_TO_CPIC_ARRAY[2] & 0xF0) == 0x20 )                 
   {
      Fprintf(PC,"TRANSFER_RESERVATION_CMD_TO_MAINPIC_OLDTRX\n\r");
      
      //_________________________________________________________________
      CLEAR_DATA_ARRAY(CPIC_TO_MPIC_ARRAY, 40);
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 40);
      CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;                       // header
      for(int i = 1; i<=10; i++)
      {
         CPIC_TO_MPIC_ARRAY[i] = OLDTRX_TO_CPIC_ARRAY[i+1];
      }
      CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;                       // footer
      //_________________________________________________________________
      COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(2, 1000,200);
      
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 && (MPIC_TO_CPIC_ARRAY[1]&0xF0) == 0x20 && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         Fprintf(PC,"Main pic responded\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(1);
      }
      
      else
      {
         Fprintf(PC,"Main pic did not respond\n\r");
         OLDTRX_SEND_ACKNOWLEDGEMENT(0);
      }
   }
}




