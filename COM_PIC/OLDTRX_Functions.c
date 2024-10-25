// PIN_D3 = GMSK ON/OFF
// PIN_D2 = CW ON/OFF
// PIN_D1 = CW_KEY

void OLD_TRX_RX_MODE()
{
   Output_Low(PIN_D3);            // 0
   delay_ms(10);
   Output_Low(PIN_D2);            // 0
   delay_ms(10);
   fprintf(PC,"OLDTRX RX Mode\n\r");
}

void OLD_TRX_TX_MODE()
{
   Output_High(PIN_D3);             // TX on pin (1)
   delay_ms(10);
   Output_Low(PIN_D2);              // 0
   delay_ms(10);
   fprintf(PC,"OLDTRX TX Mode\n\r");
}  
   
void OLD_TRX_CW_MODE()
{
   Output_Low(PIN_D3);             // 0
   delay_ms(10);
   Output_High(PIN_D2);            // CW on pin (1)
   delay_ms(10);
   fprintf(PC,"OLDTRX CW Mode\n\r");
}
   
// this functions will check OLD tranceiver UART port incoming_________________
//!void CHECK_UART_INCOMING_FROM_OLD_TRX()
//!{
//!   if( OLDTRX_Available() )
//!   {
//!      delay_ms(30);
//!      for(int i=0; i<=50; i++)
//!      {
//!         OLDTRX_CMD_BUFFER[i] = OLDTRX_Read();
//!      }
//!   }
//!}

void CHECK_UART_INCOMING_FROM_OLD_TRX(char header)
{
   if( OLDTRX_Available() )
   {
      Delay_ms(50);   
      
      for(int i = 0; i<10; i++)
      {
         if( OLDTRX_Read() == 0x42 ) 
         {
            OLDTRX_CMD_BUFFER[0] = header;
            break;
         }
      }
      
      for(int i=1; i<=50; i++)
      {
         OLDTRX_CMD_BUFFER[i] = OLDTRX_Read();
      }
   }
}

void EXTRACTING_OLD_TRANCEIVER_COMMAND(unsigned int8 sync_byte)
{
   if( OLDTRX_CMD_BUFFER[0] == sync_byte )    // 0x42
   {
      CLEAR_DATA_ARRAY(OLDTRX_TO_CPIC_ARRAY,50);
      for( int i = 0; i<50; i++ )
      {
         OLDTRX_TO_CPIC_ARRAY[i] = OLDTRX_CMD_BUFFER[i] ;
      }
      
      if( ( OLDTRX_CMD_BUFFER[2] == 0xA1 ) || ( OLDTRX_CMD_BUFFER[2] == 0xA2 ) ) OLDTRX_RPL = 23 ;
      
      else OLDTRX_RPL = 14;                             // PL packet length
   
      
      unsigned int16 CRC     = CALCULATE_CRC(OLDTRX_TO_CPIC_ARRAY,OLDTRX_RPL-2);
      unsigned int16 CMD_CRC = make16( OLDTRX_TO_CPIC_ARRAY[OLDTRX_RPL-1],OLDTRX_TO_CPIC_ARRAY[OLDTRX_RPL-2] ) ;
      
      printline();

      
      if( CRC == CMD_CRC)
      {
         printline(); 
         fprintf( PC,"CRC correct, Command Length %u bytes\n\r", OLDTRX_RPL);
         fprintf( PC,"Received GS CMD to OLDTRX --> ");
         OLDTRX_TO_CPIC_ARRAY[OLDTRX_RPL] = 0xBB;   // creating a footer
         for(int i=0; i<OLDTRX_RPL+1; i++)
         {
            fprintf(PC,"%X ",  OLDTRX_TO_CPIC_ARRAY[i]);      
         }
         printline();  
      }

//!      else
//!      {
//!         printline(); 
//!         fprintf( PC,"CRC wrong, Command Length %u bytes\n\r", OLDTRX_RPL);
//!         fprintf( PC,"Received GS CMD to OLDTRX --> ");
//!         for(int i=0; i<OLDTRX_RPL+1; i++)
//!         {
//!            fprintf(PC,"%X ",  OLDTRX_TO_CPIC_ARRAY[i]);      
//!         }          
//!         CLEAR_DATA_ARRAY(OLDTRX_TO_CPIC_ARRAY,50);
//!         printline();
//!         printline(); 
//!      }
      
      CLEAR_DATA_ARRAY(OLDTRX_CMD_BUFFER,50);     
      break;
   }
   
}

Void OLDTRX_SEND_ACKNOWLEDGEMENT(unsigned int8 mode)
{     
   
   //----Ground stationCall sign---------------[ 0-----5 ]
   CPIC_TO_OLDTRX_ARRAY[0] = GS_callsign_leter_1 ;     
   CPIC_TO_OLDTRX_ARRAY[1] = GS_callsign_leter_2 ;                    
   CPIC_TO_OLDTRX_ARRAY[2] = GS_callsign_leter_3 ;     
   CPIC_TO_OLDTRX_ARRAY[3] = GS_callsign_leter_4 ;     
   CPIC_TO_OLDTRX_ARRAY[4] = GS_callsign_leter_5 ;     
   CPIC_TO_OLDTRX_ARRAY[5] = GS_callsign_leter_6 ;     
      
   //----Dest SSID-----------------------------[ 6 ]
   CPIC_TO_OLDTRX_ARRAY[6] = 0x30 ;     //char 0

   //----Satellite Call Sign-------------------[ 7 - 12 ]
   CPIC_TO_OLDTRX_ARRAY[7]  = ST_callsign_leter_1 ;     
   CPIC_TO_OLDTRX_ARRAY[8]  = ST_callsign_leter_2 ;                    
   CPIC_TO_OLDTRX_ARRAY[9]  = ST_callsign_leter_3 ;     
   CPIC_TO_OLDTRX_ARRAY[10] = ST_callsign_leter_4 ;     
   CPIC_TO_OLDTRX_ARRAY[11] = ST_callsign_leter_5 ;     
   CPIC_TO_OLDTRX_ARRAY[12] = ST_callsign_leter_6 ; 
   
   //----Dest SSID-----------------------------[ 13 ]
   CPIC_TO_OLDTRX_ARRAY[13] = 0x30 ;    //char 0
   
   //----Control-------------------------------[ 14 ]
   CPIC_TO_OLDTRX_ARRAY[14] = 0x3E ;    
   
   //----PID-----------------------------------[ 15 ]
   CPIC_TO_OLDTRX_ARRAY[15] = 0xF0 ;    

   //----Packet specified data-----------------[ 16 - 21 ]
   CPIC_TO_OLDTRX_ARRAY[16] = 0xFF ;
   CPIC_TO_OLDTRX_ARRAY[17] = 0xF0 ;
   CPIC_TO_OLDTRX_ARRAY[18] = 0xFf ;
   CPIC_TO_OLDTRX_ARRAY[19] = 0x00 ;
   CPIC_TO_OLDTRX_ARRAY[20] = 0x00 ;
   CPIC_TO_OLDTRX_ARRAY[21] = 0x01 ;      // Packet number
   
   //----Pay Load------------------------------[ 22 - 102 ] (81 bytes)
   
   if( mode == 0x00)
   {
      for (int i = 22; i <= 102 ; i++)
      {      
         CPIC_TO_OLDTRX_ARRAY[i] = 0x69 ;
      }
   }
   
   if( mode == 0x01)
   {
      for(int i = 22; i < OLDTRX_RPL+22 ; i++)
      {      
         CPIC_TO_OLDTRX_ARRAY[i] = OLDTRX_TO_CPIC_ARRAY[i-22] ;
      }
      
      for(int i = OLDTRX_RPL+22; i <= 102; i++)
      {      
         CPIC_TO_OLDTRX_ARRAY[i] = 0x00 ;
      }
   }

   //----CRC values----------------------------[ 103, 104 ] 
   unsigned int16 CRC_VALUE  = CALCULATE_CRC(CPIC_TO_OLDTRX_ARRAY, 103);  
   
   CPIC_TO_OLDTRX_ARRAY[103] = (unsigned int8) (  CRC_VALUE       & 0x00FF );  // LOW byte
   CPIC_TO_OLDTRX_ARRAY[104] = (unsigned int8) ( (CRC_VALUE >> 8) & 0x00FF );  // HIGH byte
   
   // trransfering data array to new tranceiver
   OLD_TRX_TX_MODE();
   Delay_ms(1000);
   for(int i = 0; i<105; i++)
   {
      fputc(CPIC_TO_OLDTRX_ARRAY[i],OLD_TRX);
   } 
   Delay_ms(1000);
   OLD_TRX_RX_MODE();
}

//This function will be used to send data packets using New Tranceiver_________
Void OLD_TRX_SEND_DATA_PACKETS_TO_GS(unsigned int32 START_ADDRESS,  unsigned int32 NUM_OF_PACKETS )
{
   // not changin values assigning____________________________
 
   //----Ground stationCall sign---------------[ 0-----5 ]
   CPIC_TO_OLDTRX_ARRAY[0] = GS_callsign_leter_1 ;     
   CPIC_TO_OLDTRX_ARRAY[1] = GS_callsign_leter_2 ;                    
   CPIC_TO_OLDTRX_ARRAY[2] = GS_callsign_leter_3 ;     
   CPIC_TO_OLDTRX_ARRAY[3] = GS_callsign_leter_4 ;     
   CPIC_TO_OLDTRX_ARRAY[4] = GS_callsign_leter_5 ;     
   CPIC_TO_OLDTRX_ARRAY[5] = GS_callsign_leter_6 ;     
      
   //----Dest SSID-----------------------------[ 6 ]
   CPIC_TO_OLDTRX_ARRAY[6] = 0x30 ;     //char 0

   //----Satellite Call Sign-------------------[ 7 - 12 ]
   CPIC_TO_OLDTRX_ARRAY[7]  = ST_callsign_leter_1 ;     
   CPIC_TO_OLDTRX_ARRAY[8]  = ST_callsign_leter_2 ;                    
   CPIC_TO_OLDTRX_ARRAY[9]  = ST_callsign_leter_3 ;     
   CPIC_TO_OLDTRX_ARRAY[10] = ST_callsign_leter_4 ;     
   CPIC_TO_OLDTRX_ARRAY[11] = ST_callsign_leter_5 ;     
   CPIC_TO_OLDTRX_ARRAY[12] = ST_callsign_leter_6 ; 
   
   //----Dest SSID-----------------------------[ 13 ]
   CPIC_TO_OLDTRX_ARRAY[13] = 0x30 ;    //char 0
   
   //----Control-------------------------------[ 14 ]
   CPIC_TO_OLDTRX_ARRAY[14] = 0x3E ;    
   
   //----PID-----------------------------------[ 15 ]
   CPIC_TO_OLDTRX_ARRAY[15] = 0xF0 ;    

   //----Packet specified data-----------------[ 16 - 21 ]
   CPIC_TO_OLDTRX_ARRAY[16] = 0xFF ;
   CPIC_TO_OLDTRX_ARRAY[17] = 0xF0 ;
   CPIC_TO_OLDTRX_ARRAY[18] = 0xFf ;
   //________________________________________________________________
   
   OLD_TRX_TX_MODE(); // turning on trnaceiver transmit mode
   Delay_ms(1000);
   
   for(unsigned int32 PN = 1; PN <= NUM_OF_PACKETS ; PN++)    // PN packet number
   { 
      CPIC_TO_OLDTRX_ARRAY[19] = 0x00 ;
      CPIC_TO_OLDTRX_ARRAY[20] = (unsigned int8) ( (PN >> 8 ) & 0x000000FF );
      CPIC_TO_OLDTRX_ARRAY[21] = (unsigned int8) ( (PN      ) & 0x000000FF );
      
      //----Pay Load------------------------------[ 22 - 102 ] (81 bytes)______
      // Get data from com flash memory 
      for (unsigned int32 i = 22; i <= 102; i++)
      {      
         CPIC_TO_OLDTRX_ARRAY[i] = FM_BYTE_READ_( START_ADDRESS + 81*( PN - 1 ) + (i-22) );
      }
      
      //----CRC values----------------------------[ 103, 104 ] 
      unsigned int16 CRC_VALUE = CALCULATE_CRC(CPIC_TO_OLDTRX_ARRAY, 103);  
      CPIC_TO_OLDTRX_ARRAY[103] = (unsigned int8) (  CRC_VALUE       & 0x00FF );  // LOW byte
      CPIC_TO_OLDTRX_ARRAY[104] = (unsigned int8) ( (CRC_VALUE >> 8) & 0x00FF );  // HIGH byte
      
      // transfering data array to new tranceiver________________
      for(int i = 0; i<105; i++)
      {
         fputc(CPIC_TO_OLDTRX_ARRAY[i],OLD_TRX);
      } 
      
      // Delay untill packet transmission
      Delay_ms(230);    
   }
   
   Delay_ms(1000);
   OLD_TRX_RX_MODE(); // turning on trnaceiver receive mode
   
} 


// this function will be used to send one packet intead of CW morse code
Void OLD_TRX_SEND_1PCKT_BEACON()
{
   CW_INTERVAL = 120;
   // not changin values assigning____________________________
 
   //----Ground stationCall sign---------------[ 0-----5 ]
   CPIC_TO_OLDTRX_ARRAY[0] = GS_callsign_leter_1 ;     
   CPIC_TO_OLDTRX_ARRAY[1] = GS_callsign_leter_2 ;                    
   CPIC_TO_OLDTRX_ARRAY[2] = GS_callsign_leter_3 ;     
   CPIC_TO_OLDTRX_ARRAY[3] = GS_callsign_leter_4 ;     
   CPIC_TO_OLDTRX_ARRAY[4] = GS_callsign_leter_5 ;     
   CPIC_TO_OLDTRX_ARRAY[5] = GS_callsign_leter_6 ;     
      
   //----Dest SSID-----------------------------[ 6 ]
   CPIC_TO_OLDTRX_ARRAY[6] = 0x30 ;     //char 0

   //----Satellite Call Sign-------------------[ 7 - 12 ]
   CPIC_TO_OLDTRX_ARRAY[7]  = ST_callsign_leter_1 ;     
   CPIC_TO_OLDTRX_ARRAY[8]  = ST_callsign_leter_2 ;                    
   CPIC_TO_OLDTRX_ARRAY[9]  = ST_callsign_leter_3 ;     
   CPIC_TO_OLDTRX_ARRAY[10] = ST_callsign_leter_4 ;     
   CPIC_TO_OLDTRX_ARRAY[11] = ST_callsign_leter_5 ;     
   CPIC_TO_OLDTRX_ARRAY[12] = ST_callsign_leter_6 ; 
   
   //----Dest SSID-----------------------------[ 13 ]
   CPIC_TO_OLDTRX_ARRAY[13] = 0x30 ;    //char 0
   
   //----Control-------------------------------[ 14 ]
   CPIC_TO_OLDTRX_ARRAY[14] = 0x3E ;    
   
   //----PID-----------------------------------[ 15 ]
   CPIC_TO_OLDTRX_ARRAY[15] = 0xF0 ;    

   //----Packet specified data-----------------[ 16 - 21 ]
   CPIC_TO_OLDTRX_ARRAY[16] = 0xFF ;
   CPIC_TO_OLDTRX_ARRAY[17] = 0xF0 ;
   CPIC_TO_OLDTRX_ARRAY[18] = 0xFf ;
   //________________________________________________________________
   
   OLD_TRX_TX_MODE(); // turning on trnaceiver transmit mode
   Delay_ms(500);
   
   CPIC_TO_OLDTRX_ARRAY[19] = 0x00 ;
   CPIC_TO_OLDTRX_ARRAY[20] = 0x00 ;
   CPIC_TO_OLDTRX_ARRAY[21] = 0x01 ;
   
   //----Pay Load------------------------------[ 22 - 102 ] (81 bytes)______
   // we put CW HSK data here from 22 element to 37 th element 
   for (int i = 22; i < 38 ; i++)
   {      
      CPIC_TO_OLDTRX_ARRAY[i] = CW_DATA[i-22];
   }
   
   // just a separator
   CPIC_TO_OLDTRX_ARRAY[38] = 0xAA ;
   CPIC_TO_OLDTRX_ARRAY[39] = 0xBB ;
   CPIC_TO_OLDTRX_ARRAY[40] = 0xCC ;
   
   //Adding time
   
   CPIC_TO_OLDTRX_ARRAY[41] = RPIC_TO_CPIC_ARRAY[2] ;
   CPIC_TO_OLDTRX_ARRAY[42] = RPIC_TO_CPIC_ARRAY[3];
   CPIC_TO_OLDTRX_ARRAY[43] = RPIC_TO_CPIC_ARRAY[4]  ;
   CPIC_TO_OLDTRX_ARRAY[44] = RPIC_TO_CPIC_ARRAY[5] ;
   CPIC_TO_OLDTRX_ARRAY[45] = RPIC_TO_CPIC_ARRAY[6]  ;
   CPIC_TO_OLDTRX_ARRAY[46] = RPIC_TO_CPIC_ARRAY[7]  ;
   
   // read FM data
   for (unsigned int32 i = 47; i <= 102; i++)
   {      
      CPIC_TO_OLDTRX_ARRAY[i] = FM_BYTE_READ_( TLE_ADDRESS + i-47 );
   }
   
   //----CRC values----------------------------[ 103, 104 ] 
   unsigned int16 CRC_VALUE = CALCULATE_CRC(CPIC_TO_OLDTRX_ARRAY, 103);  
   CPIC_TO_OLDTRX_ARRAY[103] = (unsigned int8) (  CRC_VALUE       & 0x00FF );  // LOW byte
   CPIC_TO_OLDTRX_ARRAY[104] = (unsigned int8) ( (CRC_VALUE >> 8) & 0x00FF );  // HIGH byte
   
   // transfering data array to old tranceiver________________
   for(int i = 0; i<105; i++)
   {
      fputc(CPIC_TO_OLDTRX_ARRAY[i],OLD_TRX);
   } 
   
   // Delay untill packet transmission   
   Delay_ms(300);
   OLD_TRX_RX_MODE(); // turning on trnaceiver receive mode 
} 

