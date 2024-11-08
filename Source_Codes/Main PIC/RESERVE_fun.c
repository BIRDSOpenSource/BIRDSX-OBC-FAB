// we will use sector 1 of the main flash memry to save rservation commands

unsigned int8 Location_1_Array[12] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,    0x00, 0x01, 0x10, 0x00};
unsigned int8 Location_2_Array[12] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,    0x00, 0x01, 0x20, 0x00};
unsigned int8 Location_3_Array[12] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,    0x00, 0x01, 0x30, 0x00};
unsigned int8 Location_4_Array[12] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,    0x00, 0x01, 0x40, 0x00};
unsigned int8 Location_5_Array[12] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,    0x00, 0x01, 0x50, 0x00};
unsigned int8 Location_6_Array[12] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,    0x00, 0x01, 0x60, 0x00};
unsigned int8 Location_7_Array[12] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,    0x00, 0x01, 0x70, 0x00};
unsigned int8 Location_8_Array[12] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,    0x00, 0x01, 0x80, 0x00};
unsigned int8 Location_9_Array[12] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,    0x00, 0x01, 0x90, 0x00};
unsigned int8 Location_A_Array[12] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,    0x00, 0x01, 0xA0, 0x00};
                                   //  0     1     2     3     4     5     6     7     8     9     10    11
//  flash memroy location details
// |  0    |  1   |  2    |  3   |    4   |   5   |   6    |   7   | 8  | 9  | 10 | 11 |    12     |
// |ON flag|ON Day|ON Hour|ON Min|OFF flag|OFF Day|OFF Hour|OFF Min|0XFF|0xFF|0xFF|0xFF|Payload Num|

// |   13   |   14   |   15   ||   16   |   17   |   18   |   19   ||   20   |   21   |   22    | 
// |  0x00  |CPtoMP_2|CPtoMP_3||CPtoMP_4|CPtoMP_5|CPtoMP_6|CPtoMP_7||CPtoMP_8|CPtoMP_9|CPtoMP_10|


// Array[0] = On flag    = 0th byte
// Array[1] = ON Day     = 1st byte
// Array[2] = ON Hour    = 2nd byte
// Array[3] = ON Minute  = 3rd byte

// Array[4] = OFF flag   = 4th byte
// Array[5] = OFF Day    = 5th byte
// Array[6] = OFF Hour   = 6th byte
// Array[7] = OFF Minute = 7th byte

// Array[8], Array[9], Array[10], Array[11] = location ADD

// 12th byte = Mision board number

// 13th byte to 30th byte command

unsigned int8 Reservation_CMD_Buffer[30];


void _LOAD_RESERVATION_COMMANDS_ON_OFF_TIME_AT_BOOT(unsigned int8 array[])
{
   unsigned int32 Location_ADD = make32( Array[8], Array[9], Array[10], Array[11] );
   
   fprintf( PC, "RSVSN Location - %X >> ", Array[10]>>4);
   for(int i = 0; i<30; i++) fprintf( PC, "%X ", MAIN_FM_BYTE_READ(location_ADD+i) );
   printline();
   
   if( MAIN_FM_BYTE_READ(location_ADD) == 0x01 )    // on flag
   {
      Array[0]  = 0x01 ;          // turn on time check flag
      // loading time details
      Array[1]  = MAIN_FM_BYTE_READ(Location_ADD + 1);
      Array[2]  = MAIN_FM_BYTE_READ(Location_ADD + 2);
      Array[3]  = MAIN_FM_BYTE_READ(Location_ADD + 3);
      fprintf(PC, "RSVSN CMD on time loaded, Location = %X | %X-%X:%X\n\r",Array[10]>>4, Array[1], Array[2], Array[3]);
   }
   else Array[0] = 0xFF;
   
   if( MAIN_FM_BYTE_READ(location_ADD+4) == 0x01 )
   {
      Array[4]  = 0x01 ;          // turn on time check flag
      // loading time details
      Array[5]  = MAIN_FM_BYTE_READ(Location_ADD + 5);
      Array[6]  = MAIN_FM_BYTE_READ(Location_ADD + 6);
      Array[7]  = MAIN_FM_BYTE_READ(Location_ADD + 7);
      fprintf(PC, "RSVSN CMD off time loaded, Location = %X | %X-%X:%X\n\r",Array[10]>>4, Array[5], Array[6], Array[7]);
   }
   else Array[4] = 0xFF;
}


void LOAD_RESERVATION_COMMANDS_ON_OFF_TIME_AT_BOOT()
{
   _LOAD_RESERVATION_COMMANDS_ON_OFF_TIME_AT_BOOT(Location_1_Array);
   _LOAD_RESERVATION_COMMANDS_ON_OFF_TIME_AT_BOOT(Location_2_Array);
   _LOAD_RESERVATION_COMMANDS_ON_OFF_TIME_AT_BOOT(Location_3_Array);
   _LOAD_RESERVATION_COMMANDS_ON_OFF_TIME_AT_BOOT(Location_4_Array);
   _LOAD_RESERVATION_COMMANDS_ON_OFF_TIME_AT_BOOT(Location_5_Array);
   _LOAD_RESERVATION_COMMANDS_ON_OFF_TIME_AT_BOOT(Location_6_Array);
   _LOAD_RESERVATION_COMMANDS_ON_OFF_TIME_AT_BOOT(Location_7_Array);
   _LOAD_RESERVATION_COMMANDS_ON_OFF_TIME_AT_BOOT(Location_8_Array);
   _LOAD_RESERVATION_COMMANDS_ON_OFF_TIME_AT_BOOT(Location_9_Array);
   _LOAD_RESERVATION_COMMANDS_ON_OFF_TIME_AT_BOOT(Location_A_Array);
}


void _CHECK_RESERVATION_COMMAND_ON_TIME_AND_OFF_TIME(unsigned int8 Array[])
{
   if( Array[0] == 0x01 )
   {
      if( (MP_RTC_DAY == Array[1]) && (MP_RTC_HOUR == Array[2]) && (MP_RTC_MIN == Array[3]) )
      {
         unsigned int32 Location_ADD = make32( Array[8], Array[9], Array[10], Array[11] );
         fprintf( PC, "Executing location - %X reservation command \n\r", Array[10]>>4);
         
         // loding command to compic array
         CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;
         for(unsigned int32 i = 1 ; i<30; i++)
         {
            CPIC_TO_MPIC_ARRAY[i] = MAIN_FM_BYTE_READ(Location_ADD+i+12) ;
         }      
         CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;
        
         Array[0]  = 0xFF ;    // on  flag
         Array[4]  = 0x01 ;    // off flag
         
         for( int i = 0; i<30; i++) Reservation_CMD_Buffer[i] = MAIN_FM_BYTE_READ(location_ADD+i) ;
         Reservation_CMD_Buffer[0] = 0xFF     ;
         Reservation_CMD_Buffer[1] = 0xFF     ;
         Reservation_CMD_Buffer[2] = 0xFF     ;
         Reservation_CMD_Buffer[3] = 0xFF     ;
         Reservation_CMD_Buffer[4] = Array[4] ;
         MAIN_FM_SECTOR_ERASE(location_ADD, 4, 100);
         for( int i = 0; i<30; i++)  MAIN_FM_BYTE_WRITE(location_ADD+i, Reservation_CMD_Buffer[i]) ;
      }
   }
   
   if( Array[4] == 0x01 )
   {
      if( (MP_RTC_DAY == Array[5]) && (MP_RTC_HOUR == Array[6]) && (MP_RTC_MIN == Array[7]) )
      {
         unsigned int32 Location_ADD = make32( Array[8], Array[9], Array[10], Array[11] );
         fprintf( PC, "Location - %X reservation cleared \n\r", Array[10]>>4);
         
         // we can set the off things here__________________
         
         output_LOW(MBOSS_EN); 
         delay_ms(10000);
         output_high(MBOSS_EN); 
         
         //_________________________________________________
         Array[0]  = 0xFF ;    // on  flag
         Array[4]  = 0xFF ;    // off flag
         MAIN_FM_SECTOR_ERASE(location_ADD, 4, 100);      // clear the reservation location
      }
   }  
}

void CHECK_RESERVATION_COMMAND_ON_TIME_AND_OFF_TIME()
{
   _CHECK_RESERVATION_COMMAND_ON_TIME_AND_OFF_TIME(Location_1_Array);
   _CHECK_RESERVATION_COMMAND_ON_TIME_AND_OFF_TIME(Location_2_Array);
   _CHECK_RESERVATION_COMMAND_ON_TIME_AND_OFF_TIME(Location_3_Array);
   _CHECK_RESERVATION_COMMAND_ON_TIME_AND_OFF_TIME(Location_4_Array);
   _CHECK_RESERVATION_COMMAND_ON_TIME_AND_OFF_TIME(Location_5_Array);
   _CHECK_RESERVATION_COMMAND_ON_TIME_AND_OFF_TIME(Location_6_Array);
   _CHECK_RESERVATION_COMMAND_ON_TIME_AND_OFF_TIME(Location_7_Array);
   _CHECK_RESERVATION_COMMAND_ON_TIME_AND_OFF_TIME(Location_8_Array);
   _CHECK_RESERVATION_COMMAND_ON_TIME_AND_OFF_TIME(Location_9_Array);
   _CHECK_RESERVATION_COMMAND_ON_TIME_AND_OFF_TIME(Location_A_Array);
}


void WRITING_CMD(unsigned int8 Array[])
{
   unsigned int32 ADD_ = make32( Array[8], Array[9], Array[10], Array[11] ) ;
   MAIN_FM_SECTOR_ERASE(ADD_, 4, 100);
   Array[0]  = 0x01 ; 
   Array[1]  = CPIC_TO_MPIC_ARRAY[5]  ; 
   Array[2]  = CPIC_TO_MPIC_ARRAY[6]  ;
   Array[3]  = CPIC_TO_MPIC_ARRAY[7]  ;
   Array[4]  = 0xFF ;
   Array[5]  = CPIC_TO_MPIC_ARRAY[8]  ; 
   Array[6]  = CPIC_TO_MPIC_ARRAY[9]  ;
   Array[7]  = CPIC_TO_MPIC_ARRAY[10] ;
   
   for( int i=0; i<=7; i++)
   {
      MAIN_FM_BYTE_WRITE( ADD_+i  , Array[i] );
   }
   
   MAIN_FM_BYTE_WRITE( ADD_+13, 0x00);
   
   for( int i = 14; i<=22; i++)
   {
      MAIN_FM_BYTE_WRITE( ADD_+i  , CPIC_TO_MPIC_ARRAY[i-12] );
   }
   
   fprintf( PC, "RSVSN Location - %X >> ", Array[10]>>4);
   for(int i = 0; i<30; i++) fprintf( PC, "%X ", MAIN_FM_BYTE_READ(ADD_+i) );
   printline();
}

void WRITE_GS_CMD_IN_RESERVATION_LOCATION()
{
   if( (CPIC_TO_MPIC_ARRAY[1] & 0xF0) == 0x20 )
   {
      unsigned int Location = CPIC_TO_MPIC_ARRAY[1] & 0x0F ;
      
      fprintf( PC, "Reservation command Received\n\r");
      fprintf( PC, "Reservation Location = %X\n\r", Location );

      if( Location == 0x01 ) WRITING_CMD( Location_1_Array );
      if( Location == 0x02 ) WRITING_CMD( Location_2_Array );
      if( Location == 0x03 ) WRITING_CMD( Location_3_Array );
      if( Location == 0x04 ) WRITING_CMD( Location_4_Array );
      if( Location == 0x05 ) WRITING_CMD( Location_5_Array );
      if( Location == 0x06 ) WRITING_CMD( Location_6_Array );
      if( Location == 0x07 ) WRITING_CMD( Location_7_Array );
      if( Location == 0x08 ) WRITING_CMD( Location_8_Array );
      if( Location == 0x09 ) WRITING_CMD( Location_9_Array );
      if( Location == 0x0A ) WRITING_CMD( Location_A_Array );

   }
   printline();
}

VOId _RESTARTING_MISSIONS_AFTER_SYSTEM_RESET(unsigned int32 Location , int16 check_time)
{
   if( SECONDS_COUNTER == check_time )
   {
      unsigned int32 ADD = 0x00010004 + (Location<<12);
      if( MAIN_FM_BYTE_READ(ADD) == 0x01 )
      {
         fprintf(PC, "Rebooing a reservation command mission after a system reset, Location = %X | 0x%LX\n\r", Location, ADD);
         // loding command to compic array
         CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;
         for(unsigned int32 i = 1 ; i<30; i++)
         {
            CPIC_TO_MPIC_ARRAY[i] = MAIN_FM_BYTE_READ(ADD+i+12) ;
         }      
         CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;
      } 
      else( fprintf(PC, "Nothing to, reboot at reservation Location = %X\n\r", Location) ); 
      SECONDS_COUNTER++;
   }
}


VOID RESTARTING_MISSIONS_AFTER_SYSTEM_RESET()
{ 
   if( SECONDS_COUNTER < 600 )
   {
      _RESTARTING_MISSIONS_AFTER_SYSTEM_RESET(1, 105);
      _RESTARTING_MISSIONS_AFTER_SYSTEM_RESET(2, 125);
      _RESTARTING_MISSIONS_AFTER_SYSTEM_RESET(3, 145);
      _RESTARTING_MISSIONS_AFTER_SYSTEM_RESET(4, 165);
      _RESTARTING_MISSIONS_AFTER_SYSTEM_RESET(5, 185);
      _RESTARTING_MISSIONS_AFTER_SYSTEM_RESET(6, 205);
      _RESTARTING_MISSIONS_AFTER_SYSTEM_RESET(7, 225);
      _RESTARTING_MISSIONS_AFTER_SYSTEM_RESET(8, 245);
      _RESTARTING_MISSIONS_AFTER_SYSTEM_RESET(9, 265);
      _RESTARTING_MISSIONS_AFTER_SYSTEM_RESET(0x0A, 285);
   }
}

