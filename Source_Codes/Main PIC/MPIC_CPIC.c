// this functions will check com pic UART port incoming
void CHECK_UART_INCOMING_FROM_COM_PIC()
{
   if( CPic_available() )
   {
      Delay_ms(100);
      
      for( int i = 0; i<5; i++)
      {
         if( CPic_Read() == 0xB0 )
         {
            CPIC_TO_MPIC_ARRAY[0] = 0xB0;
            break;
         }
      }
      
      for(int gi = 1; gi<=50; gi++)
      {
        CPIC_TO_MPIC_ARRAY[gi] = CPIC_Read();         
      }
   }
}

// this functions will print received comand prom com pic
void PRINT_RECIVED_COMMAND_FROM_COM_PIC()
{
   printline();
   Fprintf(PC,"RCVD CMD FORM COM PIC >> ");
   for(int i = 0; i<40; i++)
   {
      Fprintf(PC,"%X ",CPIC_TO_MPIC_ARRAY[i]);
   }
   printline();
   
}
void COMUNICATION_WITH_COM_PIC_AND_WAIT_FOR_RESPONE(int16 numof_times, int16 time_delay, int16 wait_time = 70)
{
   printline();
   for(int j=0; j<numof_times; j++)
   {
      CPic_flush();
      Fprintf(PC,"Num of comuniction tries to COM PIC = %d\n\r", j+1)                ;
      
      for( int i = 0; i<31; i++)
      {
        fputc(MPIC_TO_RPIC_ARRAY[i], RPic);
      }
      Delay_ms(wait_time);
      CHECK_UART_INCOMING_FROM_COM_PIC() ;
      
      if( CPIC_TO_MPIC_ARRAY[0] == 0xB0 && CPIC_TO_MPIC_ARRAY[39] == 0xB1 )
      {
         // it this is the case we have recived response from com pic we can
         // stop tying
         Fprintf(PC,"Received response from com pic >> ");
         for(int i = 0; i<39; i++)
         {
            Fprintf(PC,"%X ",CPIC_TO_MPIC_ARRAY[i]);
         }
            printline();  
            
         break;
      } 
      else
      {  
         Delay_ms(time_delay);
         Fprintf(PC,"Received wrong response from com pic >> ");
         for(int i = 0; i<39; i++)
         {
            Fprintf(PC,"%X ",CPIC_TO_MPIC_ARRAY[i]);
         }
         printline();  
      }
   }
}

// this function is related to flash memory access+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void GIVE_COMFM_ACCESS_TO_COMPIC_FOR_DATA_DOWNLOAD()
{
   if( CPIC_TO_MPIC_ARRAY[1] == 0x00 && CPIC_TO_MPIC_ARRAY[2] == 0xFA)
   {
      // Acknoleging to com pic
      CLEAR_DATA_ARRAY( MPIC_TO_CPIC_ARRAY, 32 );
      MPIC_TO_CPIC_ARRAY[0]  = 0xB0;   // header
      MPIC_TO_CPIC_ARRAY[1]  = 0xFA;   // comand
      MPIC_TO_CPIC_ARRAY[31] = 0xB1;   // footer
      
      for(int i = 0; i<32; i++)
      {
         fputc(MPIC_TO_CPIC_ARRAY[i], CPic);
      }
      
      CFM_ACCESS_FLAG = 1;
      CFM_ACCESS_TIME = 50; //CPIC_TO_MPIC_ARRAY[2];
      OUTPUT_HIGH(PIN_C5);           // giving access
      Fprintf(PC,"Com flash memory access gave to com pic for = %03u Seconds\n\r", CFM_ACCESS_TIME);
      printline();
   }
}

                                              // 0:Generate CW 1:Stopping CW

// Give CW data to compic+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void GIVE_CW_DATA_TO_COM_PIC()
{
   if( CPIC_TO_MPIC_ARRAY[1] == 0x00 && CPIC_TO_MPIC_ARRAY[2] == 0xCA )
   {
      // Acknoleging to com pic
      Fprintf(PC,"GIVE_CW_DATA_TO_COM_PIC\n\r");      
      
      CLEAR_DATA_ARRAY( MPIC_TO_CPIC_ARRAY, 32 );
      MPIC_TO_CPIC_ARRAY[0]  = 0xB0;   // header
      MPIC_TO_CPIC_ARRAY[1]  = 0xCA;   // comand
      for( int i = 0; i< 16; i++)
      {
         MPIC_TO_CPIC_ARRAY[i+2] = CW_DATA_ARRAY[i] ;
      }
      MPIC_TO_CPIC_ARRAY[31] = 0xB1;   // footer
      
      // sending 
      if( (_CW_STATUS_30MIN == Enabled) && (CW30DAY_FLAG == 1) )
      {
         Fprintf(PC,"CW Data transfered to compic\n\r"); 
         for(int i = 0; i<32; i++)
         {
            fputc(MPIC_TO_CPIC_ARRAY[i], CPic);
         }
      }
      else Fprintf(PC,"Sat is not in CW transmit mode\n\r"); 
      
      Fprintf(PC,"Genarated CW > "); 
      for(int i = 0; i<16; i++)
      {
         fprintf(PC, "%X ", CW_DATA_ARRAY[i]);
      }
      printline();
      printline();  
   }
}

void CHECK_COMFM_ACCES_RELEASING()
{
   if( CFM_ACCESS_FLAG == 1 )
   {
      if( CFM_ACCESS_TIME == 0)
      {
         CFM_ACCESS_FLAG = 0;
         Fprintf(PC,"Com flash memory access released\n\r");
         printline();
      }
   }
}

// copying sectors using COM command (GS command)______________________________0X12+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DATA_COPY_FROM_MSN_FM_TO_CFM_USING_GS_CMD()
{
   if( CPIC_TO_MPIC_ARRAY[1] == 0x00 && CPIC_TO_MPIC_ARRAY[2] == 0x12 )
   {
      //printing com data array     
      Fprintf(PC,"DATA_COPY_FROM_MSN_FM_TO_CFM_USING_GS_CMD\n\r");      
      STOP_SFM_ACCESS_TO_MISSIONBOSS();
      // first Acknoleging to com pic 
      CLEAR_DATA_ARRAY( MPIC_TO_CPIC_ARRAY, 32 );
      MPIC_TO_CPIC_ARRAY[0]  = 0xB0;   // header
      MPIC_TO_CPIC_ARRAY[1]  = 0x12;   // comand
      MPIC_TO_CPIC_ARRAY[31] = 0xB1;   // footer
      for(int i = 0; i<32; i++)
      {
         fputc(MPIC_TO_CPIC_ARRAY[i], CPic);
      }
      
      unsigned int32 FM_DATA_START_ADD = make32( CPIC_TO_MPIC_ARRAY[3], CPIC_TO_MPIC_ARRAY[4], CPIC_TO_MPIC_ARRAY[5], CPIC_TO_MPIC_ARRAY[6] );
      unsigned int32 num_of_bytes      = make16(CPIC_TO_MPIC_ARRAY[9], CPIC_TO_MPIC_ARRAY[10] ) *81;
      Fprintf(PC,"num_of_bytes = %Lu\n\r", num_of_bytes);  
      
      Fprintf(PC,"Data copy start from msn fm to com fm\n\r");   
      DATA_COPY_FROM_MSN_FM_TO_CFM(FM_DATA_START_ADD, num_of_bytes);
      Fprintf(PC,"Data copied__________________\n\r");   
      
      // second Acknoleging to com pic 
      CLEAR_DATA_ARRAY( MPIC_TO_CPIC_ARRAY, 32 );
      MPIC_TO_CPIC_ARRAY[0]  = 0xB0;   // header
      MPIC_TO_CPIC_ARRAY[1]  = 0x12;   // comand
      MPIC_TO_CPIC_ARRAY[31] = 0xB1;   // footer
      for(int i = 0; i<32; i++)
      {
         fputc(MPIC_TO_CPIC_ARRAY[i], CPic);
      }
      GIVE_SFM_ACCESS_TO_MISSIONBOSS();
      printline();
   }
}


// copying sectors using COM command (GS command)______________________________0X13+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DATA_COPY_FROM_MAIN_FM_TO_CFM_USING_GS_CMD()
{
   if( CPIC_TO_MPIC_ARRAY[1] == 0x00 && CPIC_TO_MPIC_ARRAY[2] == 0x13 )
   {
      //printing com data array     
      Fprintf(PC,"DATA_COPY_FROM_MAIN_FM_TO_CFM_USING_GS_CMD\n\r");      
      
      // first Acknoleging to com pic 
      CLEAR_DATA_ARRAY( MPIC_TO_CPIC_ARRAY, 32 );
      MPIC_TO_CPIC_ARRAY[0]  = 0xB0;   // header
      MPIC_TO_CPIC_ARRAY[1]  = 0x13;   // comand
      MPIC_TO_CPIC_ARRAY[31] = 0xB1;   // footer
      for(int i = 0; i<32; i++)
      {
         fputc(MPIC_TO_CPIC_ARRAY[i], CPic);
      }
      
      unsigned int32 FM_DATA_START_ADD = make32( CPIC_TO_MPIC_ARRAY[3], CPIC_TO_MPIC_ARRAY[4], CPIC_TO_MPIC_ARRAY[5], CPIC_TO_MPIC_ARRAY[6] );
      unsigned int32 num_of_bytes      = make16(CPIC_TO_MPIC_ARRAY[9], CPIC_TO_MPIC_ARRAY[10] ) *81;
      Fprintf(PC,"num_of_bytes = %Lu\n\r", num_of_bytes);  
      
      Fprintf(PC,"Data copy start from Main fm to com fm\n\r");   
      DATA_COPY_FROM_MAIN_FM_TO_CFM(FM_DATA_START_ADD, num_of_bytes);
      Fprintf(PC,"Data copied____________________\n\r");   
      
      // second Acknoleging to com pic 
      CLEAR_DATA_ARRAY( MPIC_TO_CPIC_ARRAY, 32 );
      MPIC_TO_CPIC_ARRAY[0]  = 0xB0;   // header
      MPIC_TO_CPIC_ARRAY[1]  = 0x13;   // comand
      MPIC_TO_CPIC_ARRAY[31] = 0xB1;   // footer
      for(int i = 0; i<32; i++)
      {
         fputc(MPIC_TO_CPIC_ARRAY[i], CPic);
      }
      printline();
   }
}

// sector erasing+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void SECTOR_ERASE_USING_GS_COMMAND()
{
   if( CPIC_TO_MPIC_ARRAY[1] == 0x00 && CPIC_TO_MPIC_ARRAY[2] == 0x14 )
   {
      Fprintf(PC,"SECTOR_ERASE_USING_GS_COMMAND\n\r"); 
      
      // Acknoleging to com pic 
      CLEAR_DATA_ARRAY( MPIC_TO_CPIC_ARRAY, 32 );
      MPIC_TO_CPIC_ARRAY[0]  = 0xB0;   // header
      MPIC_TO_CPIC_ARRAY[1]  = 0x14;   // comand
      MPIC_TO_CPIC_ARRAY[31] = 0xB1;   // footer
      for(int i = 0; i<32; i++)
      {
         fputc(MPIC_TO_CPIC_ARRAY[i], CPic);
      }
      
      unsigned int32 ADD_ = make32( CPIC_TO_MPIC_ARRAY[3], CPIC_TO_MPIC_ARRAY[4], CPIC_TO_MPIC_ARRAY[5], CPIC_TO_MPIC_ARRAY[6] );
      unsigned int8  FM_  = CPIC_TO_MPIC_ARRAY[10];
      
      if( FM_ == 0xF1 ) 
      {
         Fprintf(PC,"COM FM sector erase\n\r");
         Output_Low(PIN_C5);
         COM_FM_SECTOR_ERASE(ADD_, 64);
         Output_HIGH(PIN_C5);
      }
      
      else if( FM_ == 0xF2 ) 
      {
         Fprintf(PC,"MAIN FM sector erase\n\r");
         MAIN_FM_SECTOR_ERASE(ADD_, 64);
      }
      
      else if( FM_ == 0xF3 ) 
      {
         Fprintf(PC,"MSN FM sector erase\n\r");
         Output_Low(PIN_A5);
         MSN_FM_SECTOR_ERASE(ADD_, 64);
         Output_HIGH(PIN_A5);
      }
   }
}


// controlling compic+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CONTROL_KILL_SWITCHES()
{
   if( CPIC_TO_MPIC_ARRAY[1] == 0x00 && CPIC_TO_MPIC_ARRAY[2] == 0xFF )
   {    
      char switch_       = CPIC_TO_MPIC_ARRAY[3] ;
      char switch_status = CPIC_TO_MPIC_ARRAY[4] ;
      Fprintf(PC,"CONTROL_FAB_KILL_SWITCH \n\r");

      if( switch_ == 0xF1 )
      {
         Fprintf(PC,"Controlling FAB kill switch = %X\n\r", switch_status);
         if( switch_status == 0x01 ) _CLOSE_FAB_KILL_SWITCH();
         if( switch_status == 0x00 ) _OPEN_FAB_KILL_SWITCH();
      }
      
      if( switch_ == 0xF2 )
      {
         Fprintf(PC,"Controlling OBC kill switch = %X\n\r", switch_status);
         if( switch_status == 0x01 ) _CLOSE_OBC_KILL_SWITCH();
         if( switch_status == 0x00 ) _OPEN_OBC_KILL_SWITCH();
      }
      
      // Acknoleging to com pic________________________________________________
      if( FAB_TO_MPIC_ARRAY[0] == 0xFA && FAB_TO_MPIC_ARRAY[2] == 0xFB )
      {
         CLEAR_DATA_ARRAY( MPIC_TO_CPIC_ARRAY, 32 );
         MPIC_TO_CPIC_ARRAY[0]  = 0xB0;   // header
         MPIC_TO_CPIC_ARRAY[1]  = 0xFF;   // comand
         MPIC_TO_CPIC_ARRAY[31] = 0xB1;   // footer
         for(int i = 0; i<32; i++)
         {
            fputc(MPIC_TO_CPIC_ARRAY[i], CPic);
         }
      }
         
      else 
      {
         CLEAR_DATA_ARRAY( MPIC_TO_CPIC_ARRAY, 32 );
         MPIC_TO_CPIC_ARRAY[0]  = 0xB0;   // header
         MPIC_TO_CPIC_ARRAY[1]  = 0xF0;   // comand
         MPIC_TO_CPIC_ARRAY[31] = 0xB1;   // footer
         for(int i = 0; i<32; i++)
         {
            fputc(MPIC_TO_CPIC_ARRAY[i], CPic);
         }
      }
      CLEAR_DATA_ARRAY(FAB_TO_MPIC_ARRAY, 10);
      //_______________________________________________________________________
   }

}


// DEPLOY_ANTENNA_USING_GS_COMMAND______________________________________________0XDA+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DEPLOY_ANTENNA_USING_GS_COMMAND()
{
   if( CPIC_TO_MPIC_ARRAY[1] == 0x00 && CPIC_TO_MPIC_ARRAY[2] == 0xDA )
   {   
      Fprintf(PC,"DEPLOY_ANTENNA_USING_GS_COMMAND\n\r");      

      // first Acknoleging to com pic 
      CLEAR_DATA_ARRAY( MPIC_TO_CPIC_ARRAY, 32 );
      MPIC_TO_CPIC_ARRAY[0]  = 0xB0;   // header
      MPIC_TO_CPIC_ARRAY[1]  = 0xDA;   // comand
      MPIC_TO_CPIC_ARRAY[31] = 0xB1;   // footer
      for(int i = 0; i<32; i++)
      {
         fputc(MPIC_TO_CPIC_ARRAY[i], CPic);
      }
      
      //_______________________________________________
      // sending command to reset pic wait for respone we will try 5 times
      // then we start antena deployment 
      CLEAR_DATA_ARRAY( MPIC_TO_RPIC_ARRAY, 10 );
      MPIC_TO_RPIC_ARRAY[0] = 0xA0;   // header
      MPIC_TO_RPIC_ARRAY[1] = 0xDA;   // comand
      MPIC_TO_RPIC_ARRAY[9] = 0xA1;   // footer
          
      COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(5, 1000,500);     

      if( CPIC_TO_MPIC_ARRAY[3] == 0x01 )
      {
         Fprintf(PC,"Antenna set-1 deployment started\n\r");
         output_high(PIN_B0);
      }
      
      else if( CPIC_TO_MPIC_ARRAY[3] == 0x02 )
      {
         Fprintf(PC,"Antenna set-2 deployment started\n\r");
         output_high(PIN_B1);
      }
      
      for( int i = 0; i<25; i++)
      {
         Delay_ms(1000);
         Fprintf(PC,"Counting deployment time %02d Sec\n\r", i+1);
      }
      
           if( CPIC_TO_MPIC_ARRAY[2] == 0x01 ) OUTPUT_LOW(PIN_B0);    
      else if( CPIC_TO_MPIC_ARRAY[2] == 0x02 ) OUTPUT_LOW(PIN_B1); 
       
      Fprintf(PC,"Antenna deployment finished\n\r"); 
      CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32 );
   }
}

// SET_ANTENNA_VALUES_USING_GS_COMMAND()______________________________________________0XDB+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void SET_ANTENNA_VALUES_USING_GS_COMMAND()
{
   if( CPIC_TO_MPIC_ARRAY[1] == 0x00 && CPIC_TO_MPIC_ARRAY[2] == 0xDB )
   {   
      Fprintf(PC,"SET_ANTENNA_VALUES_USING_GS_COMMAND\n\r");      

      // first Acknoleging to com pic 
      CLEAR_DATA_ARRAY( MPIC_TO_CPIC_ARRAY, 32 );
      MPIC_TO_CPIC_ARRAY[0]  = 0xB0;   // header
      MPIC_TO_CPIC_ARRAY[1]  = 0xDB;   // comand
      MPIC_TO_CPIC_ARRAY[31] = 0xB1;   // footer
      for(int i = 0; i<32; i++)
      {
         fputc(MPIC_TO_CPIC_ARRAY[i], CPic);
      }
      
      ERASE_PROGRAM_EEPROM(0x18000);
      WRITE_PROGRAM_EEPROM(0x18000, (unsigned int16)CPIC_TO_MPIC_ARRAY[3]);
      WRITE_PROGRAM_EEPROM(0x18002, (unsigned int16)CPIC_TO_MPIC_ARRAY[4]);
      WRITE_PROGRAM_EEPROM(0x18004, (unsigned int16)CPIC_TO_MPIC_ARRAY[5]);
      WRITE_PROGRAM_EEPROM(0x18006, (unsigned int16)CPIC_TO_MPIC_ARRAY[6]);
      
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
}

void LOAD_30DAY_COUNTER()
{
   CW30DAY_COUNTER = MAIN_FM_BYTE_READ(0x0018B000);
   fprintf(PC, "CW 30Day Counter = %u \n\r", CW30DAY_COUNTER );  
}

void INCREACE_30DAY_COUNTER_ONCE_PER_DAY_AND_CHECK_30DAY_CW_STATUS()
{
   CW30DAY_COUNTER++;
   MAIN_FM_SECTOR_ERASE(0x0018B000, 4);
   MAIN_FM_BYTE_WRITE(0x0018B000, CW30DAY_COUNTER);

   if(CW30DAY_COUNTER > 30) 
   {
      CW30DAY_FLAG = 0;
      fprintf(PC, "SAT is in 30 day CW disable mode %u\n\r", CW30DAY_FLAG ); 
   }
   else
   {
      CW30DAY_FLAG = 1 ;
      fprintf(PC, "SAT is NOT in 30 day CW disable mode %u\n\r", CW30DAY_FLAG );
   }
}

void RESET_30DAY_CONTER()
{
   if(  CPIC_TO_MPIC_ARRAY[2] != 0xCA )
   {
      MAIN_FM_SECTOR_ERASE(0x0018B000, 4, 250);
      MAIN_FM_BYTE_WRITE(0x0018B000, 0x00);
      LOAD_30DAY_COUNTER();
      
      CW30DAY_FLAG = 1; 
   }
}

