
void NEW_TRX_RX_MODE()
{
   Output_Low(TXMODE_PIN);                     // Output_low(PIN_F5);    //PA0 = F6, PA1 = B5  --- 00
   fprintf(PC,"NewTRX RX Mode\n\r");
}

void NEW_TRX_TX_MODE()
{
   Output_High(TXMODE_PIN);                     // Output_high(PIN_F5);    //PA0 = F6, PA1 = B5  --- 00
   Delay_ms(500);
   fprintf(PC,"NewTRX TX Mode\n\r");
}

//!void NEW_TRX_CW_MODE()
//!{
//!   Output_High(PIN_F6); Output_low(PIN_F5);    //PA0 = F6, PA1 = B5  --- 00
//!   delay_ms(250);
//!   fprintf(PC,"NewTRX CW Mode\n\r");
//!}

//!// CW transmission related variables
//!   unsigned int8 CW_FLAG   = 0x00;
//!   unsigned int8 NEWTRX_CW = 0xAA;
//!   unsigned int8 OLDTRX_CW = 0xBB;
//!   
//!// Transceiver operation
//!   unsigned int8 NEW_TRX_STATUS = 0x00;
//!   unsigned int8 ON_  = 0xAA;
//!   unsigned int8 OFF_ = 0xBB;


Void LOAD_CW_MODE_AND_NEW_TRX_STATUS()
{
   CW_FLAG        = (unsigned int8)( READ_PROGRAM_EEPROM(0x18000) & 0x00FF);
   NEW_TRX_STATUS = (unsigned int8)( READ_PROGRAM_EEPROM(0x18002) & 0x00FF);
   
   fprintf(PC, "CW Flag flag value  = %X\n\r", CW_FLAG);
   fprintf(PC, "New TRX Status Flag = %X\n\r", NEW_TRX_STATUS);
   
        if( CW_FLAG == NEWTRX_CW    ) Fprintf(PC,"CW using NEW TRX  \n\r");
   else if( CW_FLAG == OLDTRX_CW    ) Fprintf(PC,"CW using OLD TRX  \n\r");
   else if( CW_FLAG == NEWTRX_1PCKT ) Fprintf(PC,"NEWTRX GMSK Beacon\n\r");
   else if( CW_FLAG == OLDTRX_1PCKT ) Fprintf(PC,"OLDTRX GMSK Beacon\n\r");
   else                               Fprintf(PC,"No CW transmission\n\r");
   
        if( NEW_TRX_STATUS == ON_   ) Fprintf(PC,"New TRX is in on mode\n\r");
   else if( NEW_TRX_STATUS == OFF_  ) Fprintf(PC,"New TRX is in off mode\n\r");
}

void ENABLE_DISABLE_NEW_TRX_POWER()
{
   if( NEW_TRX_STATUS == ON_  )
   {
      fprintf(PC,"NewTRX Board turned on\n\r");
      OUtput_High(PIN_A3);
   }
   
   if( NEW_TRX_STATUS == OFF_ )
   {
      fprintf(PC,"NewTRX Board turned OFF\n\r");
      OUtput_LOW(PIN_A3); 
   } 
}


// this functions will check NEW tranceiver UART port incoming_________________
void CHECK_UART_INCOMING_FROM_NEW_TRX(char header)
{
   if( NEWTRX_Available() )
   {
      Delay_ms(100);  
      
      for(int i = 0; i<10; i++)
      {
         if( NEWTRX_Read() == 0x54 ) 
         {
            NEWTRX_CMD_BUFFER[0] = 0x54;
            break;
         }
      }
      
      for(int i=1; i<=50; i++)
      {
         NEWTRX_CMD_BUFFER[i] = NEWTRX_Read();
      }
   }
}


void EXTRACTING_NEW_TRANCEIVER_COMMAND(unsigned int8 sync_byte)
{
   if( NEWTRX_CMD_BUFFER[0] ==  sync_byte)  // 54
   {
      for(int j = 11; j<50 ; j++)
      {
         if( NEWTRX_CMD_BUFFER[j-1] == 0x7E && NEWTRX_CMD_BUFFER[j] == 0x7E )
         {
            printline();
            CLEAR_DATA_ARRAY(NEWTRX_TO_CPIC_ARRAY,50);
            for( int i = 0; i<50; i++ )
            {
               NEWTRX_TO_CPIC_ARRAY[i] = NEWTRX_CMD_BUFFER[i] ;
            }
            
            NEWTRX_RPL = j-1 ;
            NEWTRX_TO_CPIC_ARRAY[NEWTRX_RPL] = 0xAA;
            
            fprintf( PC,"CMD Size = %u bytes\n\r", NEWTRX_RPL);
            fprintf( PC,"Received GS CMD NEWTRX --> ");
            for(int i=0; i< NEWTRX_RPL+1 ; i++)
            {
               fprintf(PC,"%X ",  NEWTRX_TO_CPIC_ARRAY[i]);      
            }
            printline();    
            
            unsigned int16 CRC     = CALCULATE_CRC(NEWTRX_TO_CPIC_ARRAY, NEWTRX_RPL-2);
            unsigned int16 CMD_CRC = make16( NEWTRX_TO_CPIC_ARRAY[NEWTRX_RPL-1],NEWTRX_TO_CPIC_ARRAY[NEWTRX_RPL-2] ) ;
            
            if( CRC == CMD_CRC )
            {
               fputc(0x7E, NEW_TRX);
               fputc(0xC1, NEW_TRX);
               fputc(0x7E, NEW_TRX);
               fprintf( PC,"CRC is correct\n\r");
            }
            
            else
            {
               fprintf( PC,"CRC is wrong\n\r");  
               CLEAR_DATA_ARRAY(NEWTRX_TO_CPIC_ARRAY,50);
            }
            fprintf( PC,"CMD CRC = %LX\n\r", CMD_CRC);
            fprintf( PC,"CAL CRC = %LX\n\r", CRC    );
            
            CLEAR_DATA_ARRAY(NEWTRX_CMD_BUFFER,55); // delete
            delay_ms(250);
            break;
         }
      }
   }
}


Void DELAY_UNTILL_TRX_FINISH_TRANSMITTING_A_PACKET()
{
   Delay_ms(75);
   unsigned int16 ci = 0;
   while(input(TXDONE_PIN) == 1)
   {
      delay_ms(1); 
      ci++; 
      if(ci>=2000)
      {
         break;
      }
   }
}

Void NEWTRX_SEND_ACKNOWLEDGEMENT(unsigned int8 mode)
{       
   //----Ground stationCall sign---------------[ 0-----5 ]
   CPIC_TO_NEWTRX_ARRAY[0] = GS_callsign_leter_1 ;     
   CPIC_TO_NEWTRX_ARRAY[1] = GS_callsign_leter_2 ;                    
   CPIC_TO_NEWTRX_ARRAY[2] = GS_callsign_leter_3 ;     
   CPIC_TO_NEWTRX_ARRAY[3] = GS_callsign_leter_4 ;     
   CPIC_TO_NEWTRX_ARRAY[4] = GS_callsign_leter_5 ;     
   CPIC_TO_NEWTRX_ARRAY[5] = GS_callsign_leter_6 ;     
      
   //----Dest SSID-----------------------------[ 6 ]
   CPIC_TO_NEWTRX_ARRAY[6] = 0x30 ;     //char 0

   //----Satellite Call Sign-------------------[ 7 - 12 ]
   CPIC_TO_NEWTRX_ARRAY[7]  = ST_callsign_leter_1 ;     
   CPIC_TO_NEWTRX_ARRAY[8]  = ST_callsign_leter_2 ;                    
   CPIC_TO_NEWTRX_ARRAY[9]  = ST_callsign_leter_3 ;     
   CPIC_TO_NEWTRX_ARRAY[10] = ST_callsign_leter_4 ;     
   CPIC_TO_NEWTRX_ARRAY[11] = ST_callsign_leter_5 ;     
   CPIC_TO_NEWTRX_ARRAY[12] = ST_callsign_leter_6 ; 
   
   //----Dest SSID-----------------------------[ 13 ]
   CPIC_TO_NEWTRX_ARRAY[13] = 0x30 ;    //char 0
   
   //----Control-------------------------------[ 14 ]
   CPIC_TO_NEWTRX_ARRAY[14] = 0x3E ;    
   
   //----PID-----------------------------------[ 15 ]
   CPIC_TO_NEWTRX_ARRAY[15] = 0xF0 ;    

   //----Packet specified data-----------------[ 16 - 21 ]
   CPIC_TO_NEWTRX_ARRAY[16] = 0xFF ;
   CPIC_TO_NEWTRX_ARRAY[17] = 0xF0 ;
   CPIC_TO_NEWTRX_ARRAY[18] = 0xFf ;
   CPIC_TO_NEWTRX_ARRAY[19] = 0x00 ;
   CPIC_TO_NEWTRX_ARRAY[20] = 0x00 ;
   CPIC_TO_NEWTRX_ARRAY[21] = 0x01 ;      // Packet number
   
   //----Pay Load------------------------------[ 22 - 102 ] (81 bytes)
   
   if( mode == 0x00)
   {
      for (int i = 22; i <= 102 ; i++)
      {      
         CPIC_TO_NEWTRX_ARRAY[i] = 0x69 ;
      }
   }
   
   if( mode == 0x01)
   {
      for(int i = 22; i < NEWTRX_RPL+22; i++)
      {      
         CPIC_TO_NEWTRX_ARRAY[i] = NEWTRX_TO_CPIC_ARRAY[i-22] ;
      }
      
      for(int i = NEWTRX_RPL+22; i <= 102; i++)
      {      
         CPIC_TO_NEWTRX_ARRAY[i] = 0x00 ;
      }
   }
   
   NEW_TRX_TX_MODE(); // turning on trnaceiver transmit mode
   unsigned int16 CRC_VALUE  = CALCULATE_CRC(CPIC_TO_NEWTRX_ARRAY, 103);  
   
   CPIC_TO_NEWTRX_ARRAY[103] = (unsigned int8) (  CRC_VALUE       & 0x00FF );  // LOW byte
   CPIC_TO_NEWTRX_ARRAY[104] = (unsigned int8) ( (CRC_VALUE >> 8) & 0x00FF );  // HIGH byte
   
   
   fputc(0x7E, NEW_TRX );   // header
   // trransfering data array to new tranceiver________________
   for(int i = 0; i<=104; i++)
   {
      fputc(CPIC_TO_NEWTRX_ARRAY[i],NEW_TRX);
   } 
   fputc(0x7E, NEW_TRX );  // footer
   DELAY_UNTILL_TRX_FINISH_TRANSMITTING_A_PACKET();
   NEW_TRX_RX_MODE(); // turning on trnaceiver receive mode
}


//This function will be used to send data packets using New Tranceiver_________
Void NEW_TRX_SEND_DATA_PACKETS_TO_GS(unsigned int32 START_ADDRESS,  unsigned int32 NUM_OF_PACKETS )
{
   // not changin values assigning____________________________
   // packet header to new tranceiver
   //----Ground stationCall sign---------------[ 0-----5 ]
   CPIC_TO_NEWTRX_ARRAY[0] = GS_callsign_leter_1 ;     
   CPIC_TO_NEWTRX_ARRAY[1] = GS_callsign_leter_2 ;                    
   CPIC_TO_NEWTRX_ARRAY[2] = GS_callsign_leter_3 ;     
   CPIC_TO_NEWTRX_ARRAY[3] = GS_callsign_leter_4 ;     
   CPIC_TO_NEWTRX_ARRAY[4] = GS_callsign_leter_5 ;     
   CPIC_TO_NEWTRX_ARRAY[5] = GS_callsign_leter_6 ;     
      
   //----Dest SSID-----------------------------[ 6 ]
   CPIC_TO_NEWTRX_ARRAY[6] = 0x30 ;     //char 0

   //----Satellite Call Sign-------------------[ 7 - 12 ]
   CPIC_TO_NEWTRX_ARRAY[7]  = ST_callsign_leter_1 ;     
   CPIC_TO_NEWTRX_ARRAY[8]  = ST_callsign_leter_2 ;                    
   CPIC_TO_NEWTRX_ARRAY[9] = ST_callsign_leter_3 ;     
   CPIC_TO_NEWTRX_ARRAY[10] = ST_callsign_leter_4 ;     
   CPIC_TO_NEWTRX_ARRAY[11] = ST_callsign_leter_5 ;     
   CPIC_TO_NEWTRX_ARRAY[12] = ST_callsign_leter_6 ; 
   
   //----Dest SSID-----------------------------[ 13 ]
   CPIC_TO_NEWTRX_ARRAY[13] = 0x30 ;    //char 0
   
   //----Control-------------------------------[ 14 ]
   CPIC_TO_NEWTRX_ARRAY[14] = 0x3E ;    
   
   //----PID-----------------------------------[ 15 ]
   CPIC_TO_NEWTRX_ARRAY[15] = 0xF0 ;    

   //----Packet specified data-----------------[ 16 - 21 ]
   CPIC_TO_NEWTRX_ARRAY[16] = 0xFF ;
   CPIC_TO_NEWTRX_ARRAY[17] = 0xF0 ;
   CPIC_TO_NEWTRX_ARRAY[18] = 0xFf ;
   //________________________________________________________________
   
   NEW_TRX_TX_MODE(); // turning on trnaceiver transmit mode
   for(unsigned int32 PN = 1; PN <= NUM_OF_PACKETS ; PN++)    // PN packet number
   { 
      CPIC_TO_NEWTRX_ARRAY[19] = 0x00 ;
      CPIC_TO_NEWTRX_ARRAY[20] = (unsigned int8) ( (PN >> 8 ) & 0x000000FF );
      CPIC_TO_NEWTRX_ARRAY[21] = (unsigned int8) ( (PN      ) & 0x000000FF );
      
      //----Pay Load------------------------------[ 22 - 102 ] (81 bytes)______
      // Get data from com flash memory 
      for (unsigned int32 i = 22; i <= 102; i++)
      {      
         CPIC_TO_NEWTRX_ARRAY[i] = FM_BYTE_READ_( START_ADDRESS + 81*( PN - 1 ) + (i-22) );
      }
          
      unsigned int16 CRC_VALUE = CALCULATE_CRC(CPIC_TO_NEWTRX_ARRAY, 103);  
      CPIC_TO_NEWTRX_ARRAY[103] = (unsigned int8) (  CRC_VALUE       & 0x00FF );  // LOW byte
      CPIC_TO_NEWTRX_ARRAY[104] = (unsigned int8) ( (CRC_VALUE >> 8) & 0x00FF );  // HIGH byte
      
      fputc(0x7E, NEW_TRX );   // header
      // trransfering data array to new tranceiver________________
      for(int i = 0; i<=104; i++)
      {
         fputc(CPIC_TO_NEWTRX_ARRAY[i],NEW_TRX);
      } 
      fputc(0x7E, NEW_TRX );  // footer
       
      //delay untill packet finish transmitting___________________
      DELAY_UNTILL_TRX_FINISH_TRANSMITTING_A_PACKET();        
   }
   
   NEW_TRX_RX_MODE(); // turning on trnaceiver receive mode
} 


// this function will be used to send one packet intead of CW morse code
Void NEW_TRX_SEND_1PCKT_BEACON()
{
   // not changin values assigning____________________________

   //----Ground stationCall sign---------------[ 0-----5 ]
   CPIC_TO_NEWTRX_ARRAY[0] = GS_callsign_leter_1 ;     
   CPIC_TO_NEWTRX_ARRAY[1] = GS_callsign_leter_2 ;                    
   CPIC_TO_NEWTRX_ARRAY[2] = GS_callsign_leter_3 ;     
   CPIC_TO_NEWTRX_ARRAY[3] = GS_callsign_leter_4 ;     
   CPIC_TO_NEWTRX_ARRAY[4] = GS_callsign_leter_5 ;     
   CPIC_TO_NEWTRX_ARRAY[5] = GS_callsign_leter_6 ;     
      
   //----Dest SSID-----------------------------[ 6 ]
   CPIC_TO_NEWTRX_ARRAY[6] = 0x30 ;     //char 0

   //----Satellite Call Sign-------------------[ 7 - 12 ]
   CPIC_TO_NEWTRX_ARRAY[7]  = ST_callsign_leter_1 ;     
   CPIC_TO_NEWTRX_ARRAY[8]  = ST_callsign_leter_2 ;                    
   CPIC_TO_NEWTRX_ARRAY[9]  = ST_callsign_leter_3 ;     
   CPIC_TO_NEWTRX_ARRAY[10] = ST_callsign_leter_4 ;     
   CPIC_TO_NEWTRX_ARRAY[11] = ST_callsign_leter_5 ;     
   CPIC_TO_NEWTRX_ARRAY[12] = ST_callsign_leter_6 ; 
   
   //----Dest SSID-----------------------------[ 13 ]
   CPIC_TO_NEWTRX_ARRAY[13] = 0x30 ;    //char 0
   
   //----Control-------------------------------[ 14 ]
   CPIC_TO_NEWTRX_ARRAY[14] = 0x3E ;    
   
   //----PID-----------------------------------[ 15 ]
   CPIC_TO_NEWTRX_ARRAY[15] = 0xF0 ;    

   //----Packet specified data-----------------[ 16 - 21 ]
   CPIC_TO_NEWTRX_ARRAY[16] = 0xFF ;
   CPIC_TO_NEWTRX_ARRAY[17] = 0xF0 ;
   CPIC_TO_NEWTRX_ARRAY[18] = 0xFf ;
   CPIC_TO_NEWTRX_ARRAY[19] = 0x00 ;
   CPIC_TO_NEWTRX_ARRAY[20] = 0x00 ;
   CPIC_TO_NEWTRX_ARRAY[21] = 0x01 ;
   
   
   //----Pay Load------------------------------[ 22 - 102 ] (81 bytes)______
   // we put CW HSK data here from 22 element to 37 th element 
   for (int i = 22; i < 38 ; i++)
   {      
      CPIC_TO_NEWTRX_ARRAY[i] = CW_DATA[i-22];
   }
   
   // just a separator
   CPIC_TO_NEWTRX_ARRAY[38] = 0xAA ;
   CPIC_TO_NEWTRX_ARRAY[39] = 0xBB ;
   CPIC_TO_NEWTRX_ARRAY[40] = 0xCC ;
   
   
   CPIC_TO_NEWTRX_ARRAY[41] = RPIC_TO_CPIC_ARRAY[2] ;
   CPIC_TO_NEWTRX_ARRAY[42] = RPIC_TO_CPIC_ARRAY[3];
   CPIC_TO_NEWTRX_ARRAY[43] = RPIC_TO_CPIC_ARRAY[4]  ;
   CPIC_TO_NEWTRX_ARRAY[44] = RPIC_TO_CPIC_ARRAY[5] ;
   CPIC_TO_NEWTRX_ARRAY[45] = RPIC_TO_CPIC_ARRAY[6]  ;
   CPIC_TO_NEWTRX_ARRAY[46] = RPIC_TO_CPIC_ARRAY[7]  ;
   
   
   for (unsigned int32 i = 47; i <= 103; i++)
   {      
      CPIC_TO_NEWTRX_ARRAY[i] = FM_BYTE_READ_( TLE_ADDRESS + i-47 );
   }
   
   
   NEW_TRX_TX_MODE(); // turning on trnaceiver transmit mode
   unsigned int16 CRC_VALUE = CALCULATE_CRC(CPIC_TO_NEWTRX_ARRAY, 103);  
   CPIC_TO_NEWTRX_ARRAY[103] = (unsigned int8) (  CRC_VALUE       & 0x00FF );  // LOW byte
   CPIC_TO_NEWTRX_ARRAY[104] = (unsigned int8) ( (CRC_VALUE >> 8) & 0x00FF );  // HIGH byte
   
   fputc(0x7E, NEW_TRX );   // header
   // trransfering data array to new tranceiver________________
   for(int i = 0; i<=104; i++)
   {
      fputc(CPIC_TO_NEWTRX_ARRAY[i],NEW_TRX);
   } 
   fputc(0x7E, NEW_TRX );  // footer
   
   DELAY_UNTILL_TRX_FINISH_TRANSMITTING_A_PACKET();
   NEW_TRX_RX_MODE(); // turning on trnaceiver receive mode
}
