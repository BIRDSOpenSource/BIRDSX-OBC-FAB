// this functions will check debug port incoming
void CHECK_UART_INCOMING_FROM_DEBUG_PORT()
{
   if( PC_Available() )
   {
      Delay_ms(100);
      for( int i = 0; i<10; i++)
      {
         if( PC_Read() == 0xD0 )
         {
             DEBUG_TO_MPIC_ARRAY[0] = 0xD0;
            break;
         }
      }

      for(int i=1; i<=50; i++)
      {
         DEBUG_TO_MPIC_ARRAY[i] = PC_Read();
      }
    
   }
}

// this functions will print received comand prom bebug port
void PRINT_RECIVED_COMMAND_FROM_DEBUG_PORT()
{
   printline();
   Fprintf(PC,"Received command from debug port >> ");
   for(int i = 0; i<12; i++)
   {
      Fprintf(PC,"%X ",DEBUG_TO_MPIC_ARRAY[i]);
   }
   printline();
}

//PIN_B1 & PIN_B0 is collect pin

void DEPLOY_ANTENNA_INSTANTLY()    // 0xDA
{
   if( DEBUG_TO_MPIC_ARRAY[1] == 0xDA )
   {
      CLEAR_DATA_ARRAY( MPIC_TO_RPIC_ARRAY, 10 );
      
      MPIC_TO_RPIC_ARRAY[0] = 0xA0;   // header
      MPIC_TO_RPIC_ARRAY[1] = 0xDA;   // comand
      MPIC_TO_RPIC_ARRAY[9] = 0xA1;   // footer
            
      // sending command to reset pic wait for respone we will try 10 times
      // then we start antena deployment  
      COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(5, 1000,500);  
      
      if( MPIC_TO_RPIC_ARRAY[1] == 0xDA && RPIC_TO_MPIC_ARRAY[0] == 0xA0 && RPIC_TO_MPIC_ARRAY[31] == 0xA1 )
      {
         if( DEBUG_TO_MPIC_ARRAY[2] == 0x01 )
         {
            Fprintf(PC,"Antenna set-1 deployment started\n\r");
            output_high(PIN_B0);
            //OUTPUT_HIGH(PIN_G1); // put correct pin
         }
         if( DEBUG_TO_MPIC_ARRAY[2] == 0x02 )
         {
            Fprintf(PC,"Antenna set-2 deployment started\n\r");
            output_high(PIN_B1);
            //OUTPUT_HIGH(PIN_G1); // put corect pin
         }
         
         
         for( int i = 0; i<25; i++)
         {
            Delay_ms(1000);
            Fprintf(PC,"Counting deployment time %02d Sec\n\r", i+1);
         }
         
         if( DEBUG_TO_MPIC_ARRAY[2] == 0x01 ) OUTPUT_LOW(PIN_B0);    // put the correct pin
         if( DEBUG_TO_MPIC_ARRAY[2] == 0x02 ) OUTPUT_LOW(PIN_B1);    // put the correct pin
          
         Fprintf(PC,"Antenna deployment finished\n\r"); 
         CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32 );
         CLEAR_DATA_ARRAY( DEBUG_TO_MPIC_ARRAY, 10 );
      }
   }
}

void SET_UP_ANTENNA_DEPLOYMENT_VALUES()    // 0xDB
{
   if( DEBUG_TO_MPIC_ARRAY[1] == 0xDB )
   {  
      printline();
      Fprintf(PC,"Antenna deployment related values update command received\n\r");
      if( (DEBUG_TO_MPIC_ARRAY[3] < 5) && (DEBUG_TO_MPIC_ARRAY[5] < 5) )
      {
         ERASE_PROGRAM_EEPROM(0x18000);
         WRITE_PROGRAM_EEPROM(0x18000, (unsigned int16)DEBUG_TO_MPIC_ARRAY[2]);
         WRITE_PROGRAM_EEPROM(0x18002, (unsigned int16)DEBUG_TO_MPIC_ARRAY[3]);
         WRITE_PROGRAM_EEPROM(0x18004, (unsigned int16)DEBUG_TO_MPIC_ARRAY[4]);
         WRITE_PROGRAM_EEPROM(0x18006, (unsigned int16)DEBUG_TO_MPIC_ARRAY[5]);
         
         ANT_SET_1_DEP_FLAG = READ_PROGRAM_EEPROM(0x18000);
         ANTSET_1_COUNT     = READ_PROGRAM_EEPROM(0x18002);
         ANT_SET_2_DEP_FLAG = READ_PROGRAM_EEPROM(0x18004); 
         ANTSET_2_COUNT     = READ_PROGRAM_EEPROM(0x18006);
      
         // printing antenna related variables 
         Fprintf(PC,"Updated antenna deployment values\n\r");
         Fprintf(PC,"ANT_SET_1_DEP_FLAG = %LX\n\r",ANT_SET_1_DEP_FLAG);
         Fprintf(PC,"ANTSET_1_COUNT = %LX\n\r",ANTSET_1_COUNT);
         Fprintf(PC,"ANT_SET_2_DEP_FLAG = %LX\n\r",ANT_SET_2_DEP_FLAG);
         Fprintf(PC,"ANTSET_2_COUNT = %LX\n\r",ANTSET_2_COUNT);
         printline();
      }
      
      else
      {
         Fprintf(PC,"Wron command ,check command again \n\r");
         printline();
      }
   }
}

void GET_ANTENNA_DEPLOYMENT_VALUES()
{
   if( DEBUG_TO_MPIC_ARRAY[1] == 0xDC )
   {  
      Fprintf(PC,"Antenna deploymet values\n\r");
   
      // printing antenna related variables 
      
      Fprintf(PC,"ANT_SET_1_DEP_FLAG = %LX\n\r",READ_PROGRAM_EEPROM(0x18000));
      Fprintf(PC,"ANTSET_1_COUNT = %LX\n\r",READ_PROGRAM_EEPROM(0x18002));
      Fprintf(PC,"ANT_SET_2_DEP_FLAG = %LX\n\r",READ_PROGRAM_EEPROM(0x18004));
      Fprintf(PC,"ANTSET_2_COUNT = %LX\n\r",READ_PROGRAM_EEPROM(0x18006));
      printline();  
   }
}

void UPDATE_RESET_PIC_TIME()
{
   if( DEBUG_TO_MPIC_ARRAY[1] == 0x7B )
   {  
      Fprintf(PC,"Reset pic time set commands\n\r");
      SET_RESET_PIC_TIME(DEBUG_TO_MPIC_ARRAY[2],DEBUG_TO_MPIC_ARRAY[3],DEBUG_TO_MPIC_ARRAY[4],DEBUG_TO_MPIC_ARRAY[5],DEBUG_TO_MPIC_ARRAY[6],DEBUG_TO_MPIC_ARRAY[7]); 
   }
}

// kill switch handling--------------------------------------------------------
void CLOSE_FAB_KILL_SWITCH_DB()
{
   if( DEBUG_TO_MPIC_ARRAY[1] == 0xF1 )
   {  
      Fprintf(PC,"close FAB kill switch command from debug\n\r");
      _CLOSE_FAB_KILL_SWITCH();
   }
}

void CLOSE_OBC_KILL_SWITCH_DB()
{
   if( DEBUG_TO_MPIC_ARRAY[1] == 0xF2 )
   {  
      Fprintf(PC,"close OBC kill switch command from debug\n\r");
      _CLOSE_OBC_KILL_SWITCH();
   }
}

void OPEN_FAB_KILL_SWITCH_DB()
{
   if( DEBUG_TO_MPIC_ARRAY[1] == 0xF3 )
   {  
      Fprintf(PC,"Open FAB kill switch command from debug\n\r");
      _OPEN_FAB_KILL_SWITCH();
   }
}

void OPEN_OBC_KILL_SWITCH_DB()
{
   if( DEBUG_TO_MPIC_ARRAY[1] == 0xF4 )
   {  
      Fprintf(PC,"Open OBC kill switch command from debug\n\r");
      _OPEN_OBC_KILL_SWITCH();
   }
}

void POWER_LINE_CONTROL_USING_DEBUG_COMMAND_TO_RST_PIC()
{
   if(DEBUG_TO_MPIC_ARRAY[1] == 0xF5)
   {
      Fprintf(PC,"Power line control using main pic command to reset pic\n\r");

      CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32);
      MPIC_TO_RPIC_ARRAY[0]  = 0xA0   ;
      MPIC_TO_RPIC_ARRAY[1]  = 0x7C   ;
      MPIC_TO_RPIC_ARRAY[2]  = DEBUG_TO_MPIC_ARRAY[2] ;  //3V3_1
      MPIC_TO_RPIC_ARRAY[3]  = DEBUG_TO_MPIC_ARRAY[3] ;  //3V3_2
      MPIC_TO_RPIC_ARRAY[4]  = DEBUG_TO_MPIC_ARRAY[4] ;  //5V0
      MPIC_TO_RPIC_ARRAY[5]  = DEBUG_TO_MPIC_ARRAY[5] ;  //UNREG2
      MPIC_TO_RPIC_ARRAY[9]  = 0xA1   ;   // footer
   
      // sending command to reset pic__________________________________________
      COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(3, 700);
      if( RPIC_TO_MPIC_ARRAY[1] == 0x7C && RPIC_TO_MPIC_ARRAY[0] == 0xA0 && RPIC_TO_MPIC_ARRAY[31] == 0xA1 )
      { 
         Fprintf(PC,"Reset pic power line controlled\n\r"); 
         printline();
         CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32 ); 
      }
   
      else Fprintf(PC,"Reset pic did not respond\n\r");
      
   CLEAR_DATA_ARRAY( DEBUG_TO_MPIC_ARRAY, 55 ); 
   }
   
}

void RESET_SATELLITE_CMD()
{
   if(DEBUG_TO_MPIC_ARRAY[1] == 0x7D)
   {
      Fprintf(PC, "Reset satellite command from debug \n\r");
      printline();
      
      //STORE_FLAG_INFO();
      
      CLEAR_DATA_ARRAY( MPIC_TO_RPIC_ARRAY, 10 );
      MPIC_TO_RPIC_ARRAY[0] = 0xA0;   // header
      MPIC_TO_RPIC_ARRAY[1] = 0x7D;   // comand
      MPIC_TO_RPIC_ARRAY[9] = 0xA1;   // footer
      
      COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(3, 500);
      
      if( RPIC_TO_MPIC_ARRAY[1] == 0x7D && RPIC_TO_MPIC_ARRAY[0] == 0xA0 && RPIC_TO_MPIC_ARRAY[31] == 0xA1 )
      { 
         Fprintf(PC,"Reset pic reset satellite \n\r"); 
         printline();
         CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32 ); 
      }  
      
   }
}

void SEND_COM_CMD_THROUGH_PC()
{
   if(DEBUG_TO_MPIC_ARRAY[1] == 0x77)
   { 
      Fprintf(PC,"ComPic command received from PC \n\r");
      CPIC_TO_MPIC_ARRAY[0]  = 0xB0;
      for(int i=1; i<11; i++)
      {
         CPIC_TO_MPIC_ARRAY[i] = DEBUG_TO_MPIC_ARRAY[i+1];
      }  
      CPIC_TO_MPIC_ARRAY[39] = 0xB1;
   }
}

void READ_FM_DATA_THROUGH_PC()
{
   if(DEBUG_TO_MPIC_ARRAY[1] == 0x1F)
   { 
      Fprintf(PC,"Read FM data command");
      
      unsigned int8 FM_ = DEBUG_TO_MPIC_ARRAY[11];
      unsigned int32 i;
      
      unsigned int32 address = make32( DEBUG_TO_MPIC_ARRAY[2], DEBUG_TO_MPIC_ARRAY[3], DEBUG_TO_MPIC_ARRAY[4], DEBUG_TO_MPIC_ARRAY[5] );
      unsigned int32 num_of_bytes = (unsigned int32)DEBUG_TO_MPIC_ARRAY[6]*81;
      Fprintf(PC, "Num of Bytes = %lu ",num_of_bytes);
      
      if( FM_ == 0xF1 ) 
      {
         Fprintf(PC,"Read COM FM \n\r");
         Output_Low(PIN_C5);
         for (i = 0; i < num_of_bytes; i++)
         {
            Fprintf(PC, "%X ",COM_FM_BYTE_READ(address + i));
            
         }
         printline();
         Output_HIGH(PIN_C5);
      }
      
      else if( FM_ == 0xF2 ) 
      {
         Fprintf(PC,"Read Main FM \n\r");
         for (i = 0; i < num_of_bytes; i++)
         {
            Fprintf(PC, "%X ",MAIN_FM_BYTE_READ(address + i));
            
         }
         printline();
      }
      
      else if( FM_ == 0xF3 ) 
      {
         printline();
         Fprintf(PC, "Num of Bytes = %lu ",num_of_bytes);
   
         printline();
         Fprintf(PC,"Read MSN FM\n\r");
         Output_Low(PIN_A5);
         for (i = 0; i < num_of_bytes; i++)
         {
            Fprintf(PC, "%X ",MSN_FM_BYTE_READ(address + i));
            
            
         }
         printline();
         Output_HIGH(PIN_A5);
      }
      
   }
}
