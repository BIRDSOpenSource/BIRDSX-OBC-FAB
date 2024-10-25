  // this functions will check reset pic UART port incoming
void CHECK_UART_INCOMING_FROM_RESET_PIC()
{
   if( RPic_available() )
   {
      Delay_ms(100);
      for( int i = 0; i<5; i++)
      {
         if( RPIC_Read() == 0xA0 )
         {
            RPIC_TO_MPIC_ARRAY[0] = 0xA0;
            break;
         }
      }
   
      for(int i=1; i<=50; i++)
      {
         RPIC_TO_MPIC_ARRAY[i] = RPIC_Read();
      }
   }
}

// this functions will print received comand prom reset pic
void PRINT_RECIVED_COMMAND_FROM_RESET_PIC()
{
   Fprintf(PC,"Reset PIC Response >> ");
   for(int i = 0; i<32; i++)
   {
      Fprintf(PC,"%X ",RPIC_TO_MPIC_ARRAY[i]);
   }
   printline();
   //printline();
}

void COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(int16 numof_times, int16 time_delay, int16 wait_time = 200)
{
   printline();
   for(int j=0; j<numof_times; j++)
   {
      RPIC_TO_MPIC_ARRAY[0] = 0;  RPIC_TO_MPIC_ARRAY[31] = 0;
      Fprintf(PC,"Num of comuniction tries to RESET PIC = %d\n\r", j+1)                ;
      
      for( int i = 0; i<10; i++)
      {
        fputc(MPIC_TO_RPIC_ARRAY[i], RPic);
      }
      Delay_ms(500);
      CHECK_UART_INCOMING_FROM_RESET_PIC() ;
      
      if( RPIC_TO_MPIC_ARRAY[0] == 0xA0 && RPIC_TO_MPIC_ARRAY[31] == 0xA1 )
      {
         // it this is the case we have recived response from reset pic we can
         // stop tying
         Fprintf(PC,"Reset PIC responded correctly\n\r");
         break;
      } 
      
      else
      {  
         Delay_ms(time_delay);
         Fprintf(PC,"Received wrong response from reset pic >> ");
         for(int i = 0; i<32; i++)
         {
            Fprintf(PC,"%X ",RPIC_TO_MPIC_ARRAY[i]);
         }
         printline();  
      }
   }
}


// sync reset pic and main pic time
void SYNC_MAIN_PIC_AND_RESET_PIC_TIME()
{
   rtc_time_t  read_clock; 
   rtc_read(&read_clock);
   
   Fprintf(PC,"Reset PIC time = %02u:%02u:%02u %02u:%02u:%02u:\n\r",RPIC_TO_MPIC_ARRAY[2],RPIC_TO_MPIC_ARRAY[3],RPIC_TO_MPIC_ARRAY[4],RPIC_TO_MPIC_ARRAY[5],RPIC_TO_MPIC_ARRAY[6],RPIC_TO_MPIC_ARRAY[7]);
   
   if(   (RPIC_TO_MPIC_ARRAY[2]!= read_clock.tm_year) 
      || (RPIC_TO_MPIC_ARRAY[3]!= read_clock.tm_mon ) 
      || (RPIC_TO_MPIC_ARRAY[4]!= read_clock.tm_mday) 
      || (RPIC_TO_MPIC_ARRAY[5]!= read_clock.tm_hour) 
      || (RPIC_TO_MPIC_ARRAY[6]!= read_clock.tm_min ) )
      
   {
      Fprintf(PC,"Time updated\n\r");
      WRITE_OBC_RTC(RPIC_TO_MPIC_ARRAY[2],RPIC_TO_MPIC_ARRAY[3],RPIC_TO_MPIC_ARRAY[4],RPIC_TO_MPIC_ARRAY[5],RPIC_TO_MPIC_ARRAY[6],RPIC_TO_MPIC_ARRAY[7]);
      FAILED_COMUNICATION_WITH_RST_PIC = 0; //making this zero because new time is running in main pic
   }
}

unsigned int8 CHECK_SUNSHINE(float Voltage, float current ) // set corect threshold 0x34 is a random value 
{
   // voltage > 4.5;
   // current > 50 mA;
   
   float power = voltage * current ;
   
   if( power > 225 ) return 0x01;
   else return 0x00;
}

char SOLP_STATUS_PX, SOLP_STATUS_MX ;
char SOLP_STATUS_PY, SOLP_STATUS_MY ;
char SOLP_STATUS_PZ, SOLP_STATUS_MZ ;

char RP_SAVED_DATA[42];   // only 24 bytes used
char FP_SAVED_DATA[42];   // only 34 bytes used

float V_PX, C_PX ;
float V_MX, C_MX;
float V_PY, C_PY;
float V_MY, C_MY;
float V_PZ, C_PZ;
float V_MZ, C_MZ;

void GENARATE_CW_DATA()                                                            
{
   CLEAR_DATA_ARRAY(CW_DATA_ARRAY, 16);  
   // [A,B,C.D,E,F,G,H,I,J,K,L,M,N,O,P]
   
   // battery voltage [A,B]
   CW_DATA_ARRAY[0] = (FAB_TO_MPIC_ARRAY[4] >> 4) & 0x0F;
   CW_DATA_ARRAY[1] = (FAB_TO_MPIC_ARRAY[4]     ) & 0x0F;
   
   // battery current [C,D]
   unsigned int16 BATC_  = make16(FAB_TO_MPIC_ARRAY[5], FAB_TO_MPIC_ARRAY[32] ) ;
                  BATC_  = (BATC_&0xFF0) >> 4 ;
             char BATC__ = (char)BATC_;
             
   CW_DATA_ARRAY[2] = (BATC__ >> 4) & 0x0F;
   CW_DATA_ARRAY[3] = (BATC__     ) & 0x0F;
   
   // battery temperature [E,F]
   CW_DATA_ARRAY[4] = (FAB_TO_MPIC_ARRAY[6] >> 4) & 0x0F;
   CW_DATA_ARRAY[5] = (FAB_TO_MPIC_ARRAY[6]     ) & 0x0F;
   
   // battery temperature [G,H]
   // obc kill, fab kill, +X sunshine, -X sunshine
   CW_DATA_ARRAY[6] = (FAB_TO_MPIC_ARRAY[29]<<3) | (FAB_TO_MPIC_ARRAY[30]<<2) | (SOLP_STATUS_PX << 1) | SOLP_STATUS_MX  ;
                      
   // +Y sunshine, -Y sunshine, +Z sunshine, -Z sunshine
   CW_DATA_ARRAY[7] = (SOLP_STATUS_PY << 3) | (SOLP_STATUS_MY << 2) | (SOLP_STATUS_PZ << 1) | SOLP_STATUS_MZ ;
                      
   // antenna deployment flags [I,J]                    
   CW_DATA_ARRAY[8] = (unsigned int8)(ANT_SET_1_DEP_FLAG << 3) | (unsigned int8)( ANTSET_1_COUNT & 0x0007) ;
   CW_DATA_ARRAY[9] = (unsigned int8)(ANT_SET_2_DEP_FLAG << 3) | (unsigned int8)( ANTSET_2_COUNT & 0x0007) ;

   // APRS payload status [K,L]
   CW_DATA_ARRAY[10] += APRS_REFERENSE_1_FLAG *8;
   CW_DATA_ARRAY[10] += APRS_REFERENSE_2_FLAG *4;
   CW_DATA_ARRAY[10] += APRS_PAYLOAD_1_FLAG *2;
   CW_DATA_ARRAY[10] += APRS_PAYLOAD_2_FLAG ;
   
                       
   CW_DATA_ARRAY[11] += APRS_PAYLOAD_3_FLAG *8;
   CW_DATA_ARRAY[11] += APRS_PAYLOAD_4_FLAG *4;
   CW_DATA_ARRAY[11] += APRS_PAYLOAD_5_FLAG *2;
   CW_DATA_ARRAY[11] += 0 ;
   
   // Powerline status [M,N]
   CW_DATA_ARRAY[12] = (RPIC_TO_MPIC_ARRAY[22] >> 4) & 0x0F ;
   CW_DATA_ARRAY[13] = (RPIC_TO_MPIC_ARRAY[22]     ) & 0x0F ;
   
   
   // Last reset hour [O,P]
   CW_DATA_ARRAY[14] = (RPIC_TO_MPIC_ARRAY[24] >> 4) & 0x0F ;
   CW_DATA_ARRAY[15] = (RPIC_TO_MPIC_ARRAY[24]    ) & 0x0F ;

   fprintf(PC, "Genaretd CW data = ");
   for (int i = 0; i < 16; i ++) 
   {
     Fprintf(PC,"%01X ", CW_DATA_ARRAY[i]);
   } 
   printline(); 
}

char RP_FLAG = 0;
char FP_FLAG = 0;



// 90 second comuniction with reset pic function_______________________________0x7A
void COMUNICATION_WITH_RST_PIC_90SEC(int sec)
{
   if(SEC90_Count>= sec)
   {
      SEC90_Count = 0;     
      printline();

      CLEAR_DATA_ARRAY( MPIC_TO_RPIC_ARRAY, 10 );
      MPIC_TO_RPIC_ARRAY[0] = 0xA0;   // header
      MPIC_TO_RPIC_ARRAY[1] = 0x7A;   // comand
      MPIC_TO_RPIC_ARRAY[9] = 0xA1;   // footer
      
      // just printing sending comand to reset pic_____________________________     
      Fprintf(PC,"Sent comand to Reset PIC >> ");
      for(int i = 0; i<10; i++)
      {
         Fprintf(PC,"%X ",MPIC_TO_RPIC_ARRAY[i]);
      }
      
      //_______________________________________________________________________
      
      // first comunicate with Reset pic
      // sending command to reset pic__________________________________________
      COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(3, 1000,200);
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // executing received comand from rest pic_______________________________
      if( RPIC_TO_MPIC_ARRAY[0] == 0xA0 && RPIC_TO_MPIC_ARRAY[1] == 0x7A  && RPIC_TO_MPIC_ARRAY[31] == 0xA1)
      { 
         PRINT_RECIVED_COMMAND_FROM_RESET_PIC();
         SYNC_MAIN_PIC_AND_RESET_PIC_TIME();    // if there is a time difference time will be updated
         
         // calculating day number
         DAY_NUMBER = GET_DAY_NUMBER_OF_THE_YEAR(RPIC_TO_MPIC_ARRAY[2], RPIC_TO_MPIC_ARRAY[3], RPIC_TO_MPIC_ARRAY[4]);
         Fprintf(PC,"Day number = %03Lu\n\r", DAY_NUMBER);
         
         // calculating reset pic and FAB data writing location
         RST_PIC_HSK_DATA_START_ADD = (DAY_NUMBER + 24) << 16;                                  
         FAB_PIC_HSK_DATA_START_ADD = RST_PIC_HSK_DATA_START_ADD + 0x00008000 ;
         
         //Erasing 64KB sector to write data
         if(SUCCESFULL_COMUNICATION_WITH_RST_PIC == 0 )
         {
            OUTPUT_LOW(PIN_C5);  // put correct pic
            COM_FM_SECTOR_ERASE( RST_PIC_HSK_DATA_START_ADD, 64);
            fprintf(PC, "Reset PIC HSK data Starting address for today - 0x%LX\n\r",RST_PIC_HSK_DATA_START_ADD );
            fprintf(PC, "FAB - PIC HSK data Starting address for today - 0x%LX\n\r",FAB_PIC_HSK_DATA_START_ADD );
            OUTPUT_HIGH(PIN_C5);
         }
         
         if( CFM_ACCESS_FLAG == 0 )
         {  
            OUTPUT_LOW(PIN_C5);  // take fM access
            
            fprintf(PC, "RSTPIC comunication-> sucess:notsuccess = %Ld:%Ld\n\r",SUCCESFULL_COMUNICATION_WITH_RST_PIC+1 ,FAILED_COMUNICATION_WITH_RST_PIC );
            
            unsigned int32 SET_ADDRESS = RST_PIC_HSK_DATA_START_ADD + (26* SUCCESFULL_COMUNICATION_WITH_RST_PIC);
            for(unsigned int32 i = 0; i<24; i++)
            { 
               COM_FM_BYTE_WRITE( SET_ADDRESS + i,RPIC_TO_MPIC_ARRAY[i+1] );
               if( i == 0 ) fprintf(PC, "Recent HSK data writing location     - 0x%LX   Sec_num = %Lu\n\r",SET_ADDRESS, DAY_NUMBER + 24 );    
            }
            COM_FM_BYTE_WRITE( SET_ADDRESS + 24  , 0xAA);
            COM_FM_BYTE_WRITE( SET_ADDRESS + 25  , 0xAA);
            
            fprintf(PC, "Saved data Resetpic HSK - ");
            for(int i = 0; i<26; i++)
            {
               RP_SAVED_DATA[i] = COM_FM_BYTE_READ( SET_ADDRESS + i) ;
               fprintf(PC, "%X ", RP_SAVED_DATA[i]);
               
            }
            printline();
            
            SUCCESFULL_COMUNICATION_WITH_RST_PIC++;
            OUTPUT_High(PIN_C5);   //  release FM access
         }
         
         if( CFM_ACCESS_FLAG == 1 )
         {
            fprintf(PC, "Cannot write Reset PIC house keeping data in Com FM due to access is given to ComPIC - ");
         }
         RP_FLAG = 1 ;
      }
      
      else 
      {
         Fprintf(PC,"90 seconds comunication with reset pic was not succesfull\n\r");
         FAILED_COMUNICATION_WITH_RST_PIC++;
      }
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      
      
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // now we have already comunicate with reset and we have the reset pic reponse
      // and we have clearence to write data in COM FM, so we can comunicate with FAB 
      // and get FAB HSK
      CLEAR_DATA_ARRAY( MPIC_TO_FAB_ARRAY, 3 );
      MPIC_TO_FAB_ARRAY[0] = 0xFA;   // header
      MPIC_TO_FAB_ARRAY[1] = 0xA0;   // comand
      MPIC_TO_FAB_ARRAY[2] = 0xFB;   // footer
      
      // sending command to FAB pic__________________________________________
      COMUNICATION_WITH_FAB_PIC_AND_WAIT_FOR_RESPONE(3, 1000,200,35);
      
      if( FAB_TO_MPIC_ARRAY[0] == 0xFA && FAB_TO_MPIC_ARRAY[1] == 0xA0  && FAB_TO_MPIC_ARRAY[34] == 0xFB)
      {
         if( CFM_ACCESS_FLAG == 0 )
         {  
            OUTPUT_LOW(PIN_C5);  // take fM access
            fprintf(PC, "FABPIC comunication-> sucess:notsuccess = %Ld:%Ld\n\r",SUCCESFULL_COMUNICATION_WITH_FAB+1 ,FAILED_COMUNICATION_WITH_FAB );
            unsigned int32 DATA_SET_START_ADD = FAB_PIC_HSK_DATA_START_ADD + 37* SUCCESFULL_COMUNICATION_WITH_FAB ;
            fprintf(PC, "Recent FAB HSK data writing location     - 0x%LX   Sec_num = %Lu\n\r", DATA_SET_START_ADD, DAY_NUMBER+24 );  
            
            COM_FM_BYTE_WRITE( DATA_SET_START_ADD     , RPIC_TO_MPIC_ARRAY[5] ) ;
            COM_FM_BYTE_WRITE( DATA_SET_START_ADD + 1 , RPIC_TO_MPIC_ARRAY[6] ) ;
            COM_FM_BYTE_WRITE( DATA_SET_START_ADD + 2 , RPIC_TO_MPIC_ARRAY[7] ) ;
            
            unsigned int32 BYTE_ADDRESS_ = 0;
            for(int i = 3; i<35; i++)
            {
               BYTE_ADDRESS_ = DATA_SET_START_ADD + i  ;
               COM_FM_BYTE_WRITE( BYTE_ADDRESS_ ,FAB_TO_MPIC_ARRAY[i-1] );
            }
            COM_FM_BYTE_WRITE( DATA_SET_START_ADD + 35 , 0xAA ) ;
            COM_FM_BYTE_WRITE( DATA_SET_START_ADD + 36 , 0xAA ) ;
            
            fprintf(PC, "Saved data FAB pic HSK - ");
            for(int i = 0; i<37; i++)
            {
               BYTE_ADDRESS_ = DATA_SET_START_ADD + i;
               FP_SAVED_DATA[i] = COM_FM_BYTE_READ( BYTE_ADDRESS_ ) ;
               fprintf(PC, "%X ", FP_SAVED_DATA[i] );
            }
            printLine();
            
            SUCCESFULL_COMUNICATION_WITH_FAB++;
            OUTPUT_HIGH(PIN_C5);  // release fM access
         }
         
         if( CFM_ACCESS_FLAG == 1 )
         {
            fprintf(PC, "Cannot write FAB PIC house keeping data in Com FM due to access is given to ComPIC - ");
         }
         FP_FLAG = 1 ; 
      }
      
      else
      {
         Fprintf(PC,"90 seconds comunication with FAB pic was not succesfull\n\r");
         FAILED_COMUNICATION_WITH_FAB++;
      }
      printline();
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      
      // now we can genarate CW because we already comunicate with reset pic and FAB

      printline();
      
      // Printing RP Data 
      if( RP_FLAG == 1 )
      {
         RP_FLAG = 0 ;
         fprintf(PC, "Reset PIC Data ############\n\r");
         fprintf(PC, "Reset PIC Time = %02u/%02u/%02u  %02u:%02u:%02u\n\r", RP_SAVED_DATA[1], RP_SAVED_DATA[3], RP_SAVED_DATA[3], RP_SAVED_DATA[4], RP_SAVED_DATA[5], RP_SAVED_DATA[6]) ;
         float RAW_Vol = 6.6*( (float) make16(RP_SAVED_DATA[7],RP_SAVED_DATA[8]) )/4096 ;
         fprintf(PC, "RAW Voltage = %.3f V\n\r", RAW_Vol); 
         float _3V3_1_Cur = 0.4628*( (float) make16(RP_SAVED_DATA[9],RP_SAVED_DATA[10]) ) - 6.2077 ;
         fprintf(PC, "3V3_1_Current = %.3f mA\n\r", _3V3_1_Cur);
         float _3V3_2_Cur = 0.4628*( (float) make16(RP_SAVED_DATA[11],RP_SAVED_DATA[12]) ) - 6.2077 ;
         fprintf(PC, "3V3_2_Current = %.3f mA\n\r", _3V3_2_Cur); 
         float _5V0_Cur = 0.4628*( (float) make16(RP_SAVED_DATA[13],RP_SAVED_DATA[14]) ) - 6.2077 ;
         fprintf(PC, "5V0_Current = %.3f mA\n\r", _5V0_Cur);
         float UN1_Cur = 0.5758*( (float) make16(RP_SAVED_DATA[15],RP_SAVED_DATA[16]) ) - 2.4207 ;
         fprintf(PC, "UN_1_Current = %.3f mA\n\r", UN1_Cur);
         float UN2_Cur = 0.5758*( (float) make16(RP_SAVED_DATA[17],RP_SAVED_DATA[18]) ) - 2.4207 ;
         fprintf(PC, "UN_2_Current = %.3f mA\n\r", UN2_Cur);
         
         fprintf(PC, "Num of MainPIC Reset = %u\n\r", RP_SAVED_DATA[19]) ;
         fprintf(PC, "Num of ComPIC  Reset = %u\n\r", RP_SAVED_DATA[20]) ;
         fprintf(PC, "Power Line Status = %X\n\r",    RP_SAVED_DATA[21]) ;
         fprintf(PC, "Last Reset Hour   = %Lu\n\r",  make16( RP_SAVED_DATA[22], RP_SAVED_DATA[23] ) ) ;
         //fprintf(PC, "Num of MainPIC Reset = %u \n\r", RP_SAVED_DATA[19]);
         
         CLEAR_DATA_ARRAY( RP_SAVED_DATA , 40 );
         printline();
      }
      
      // Printing FAB PICC Data 
      if( FP_FLAG == 1 )
      {
         FP_FLAG = 0 ;
         fprintf(PC, "FAB PIC Data ############\n\r");
         float Fvar = 0;
         
         Fvar = 6.6*( (float) FP_SAVED_DATA[3] )/256 ;
         fprintf(PC, "RAW Vol = %.3f V   ", Fvar); 
         //RAW current
         unsigned int16 ADC_DEC = make16( FP_SAVED_DATA[4], FP_SAVED_DATA[32] );
         Fvar = 4.1443*( (float)ADC_DEC ) - 1741.6;          
         fprintf(PC, "Cur = %.1f mA\n\r", Fvar); 
         
         //battery voltage
         Fvar = 6.6*( (float) FP_SAVED_DATA[5] )/256 ;          
         fprintf(PC, "BAT Vol = %.3f V   ", Fvar);
         //Battery current
         ADC_DEC = make16( FP_SAVED_DATA[6], FP_SAVED_DATA[33] );
         Fvar = 3.0464*( (float)ADC_DEC ) - 6260;
         fprintf(PC, "Cur = %.1f mA   ", Fvar); 
         //Battery temperature
         fprintf(PC, "FP_SAVED_DATA[7] = %x  \n\r", FP_SAVED_DATA[7]);
         Fvar = 3.3*( (float) FP_SAVED_DATA[7] )/256 ;
         Fvar = 0.5534*Fvar*Fvar - 37.995*Fvar + 87.329 ;                      
         fprintf(PC, "Temp = %.1f C  \n\r", Fvar);  
         
         //solar bus voltage
         Fvar = 6.6*( (float) FP_SAVED_DATA[8] )/256 ;         
         fprintf(PC, "S_B Vol = %.3f V   ", Fvar); 
         //solar bus current
         ADC_DEC = make16( FP_SAVED_DATA[9], FP_SAVED_DATA[34] );
         Fvar = 4.266*( (float)ADC_DEC ) - 1796 ;
         fprintf(PC, "Cur = %.1f mA\n\r", Fvar); 
                 
         printline();       
         
         // voltages and current and TEMP -x
         V_MX = 6.25*( (float) FP_SAVED_DATA[19] )/256 ;         
         fprintf(PC, "-X Vol = %.3f V   ", V_MX);         
         C_MX = 5.102*( (float)FP_SAVED_DATA[20] )  +3.367;         
         fprintf(PC, "Cur = %.1f mA   ", C_MX);
         Fvar = -181.01*(2.5*( (float) FP_SAVED_DATA[21] )/256)  +186.9;         
         fprintf(PC, "Temp = %.1f C\n\r", Fvar);
         SOLP_STATUS_MX = CHECK_SUNSHINE(V_MX, C_MX); 
         
         // voltages and current and TEMP +X
         V_PX = 6.25*( (float) FP_SAVED_DATA[10] )/256 ;         
         fprintf(PC, "+X Vol = %.3f V   ", V_PX);
         C_PX = 4.749*( (float)FP_SAVED_DATA[11] )  +4.587;         
         fprintf(PC, "Cur = %.1f mA   ", C_PX);
         Fvar = -181.01*(2.5*( (float) FP_SAVED_DATA[12] )/256)  +186.9;         
         fprintf(PC, "Temp = %.1f C\n\r", Fvar);
         SOLP_STATUS_PX = CHECK_SUNSHINE(V_PX, C_PX);
         
         // voltages and current and TEMP -Y
         V_MY = 6.25*( (float) FP_SAVED_DATA[22] )/256 ;         
         fprintf(PC, "-Y Vol = %.3f V   ", V_MY);
         C_MY = 4.71*( (float)FP_SAVED_DATA[23] )  -1.0456;         
         fprintf(PC, "Cur = %.1f mA   ", C_MY);
         Fvar = -181.01*(2.5*( (float) FP_SAVED_DATA[15] )/256)  +186.9;         
         fprintf(PC, "Temp = %.1f C\n\r", Fvar);
         SOLP_STATUS_MY = CHECK_SUNSHINE(V_MY, C_MY);
         
         // voltages and current and TEMP +Y
         V_PY = 6.25*( (float) FP_SAVED_DATA[13] )/256 ;        
         fprintf(PC, "+Y Vol = %.3f V   ", V_PY);
         C_PY = 4.68*( (float)FP_SAVED_DATA[14] ) - 5.044;  
         fprintf(PC, "Cur = %.1f mA   ", C_PY);
         Fvar = -181.01*(2.5*( (float) FP_SAVED_DATA[24] )/256)  +186.9;         
         fprintf(PC, "Temp = %.1f C\n\r", Fvar);
         SOLP_STATUS_PY = CHECK_SUNSHINE(V_PY, C_PY);
         
         // voltages and current and TEMP -Z
         V_MZ = 6.25*( (float) FP_SAVED_DATA[25] )/256 ;         
         fprintf(PC, "-Z Vol = %.3f V   ", V_MZ); 
         C_MZ = 4.965*( (float)FP_SAVED_DATA[26] ) +0.802;         
         fprintf(PC, "Cur = %.1f mA   ", C_MZ);
         Fvar = -181.01*(2.5*( (float) FP_SAVED_DATA[27] )/256)  +186.9;         
         fprintf(PC, "Temp = %.1f C\n\r", Fvar);
         SOLP_STATUS_MZ = CHECK_SUNSHINE(V_MZ, C_MZ);
         
         // voltages and current and TEMP +Z
         V_PZ = 6.25*( (float) FP_SAVED_DATA[16] )/256 ;         
         fprintf(PC, "+Z Vol = %.3f V   ", V_PZ);
         C_PZ = 4.752*( (float)FP_SAVED_DATA[17] ) -0.4066;         
         fprintf(PC, "Cur = %.1f mA   ", C_PZ);
         Fvar = -181.01*(2.5*( (float) FP_SAVED_DATA[18] )/256)  +186.9;         
         fprintf(PC, "Temp = %.1f C\n\r", Fvar);
         SOLP_STATUS_PZ = CHECK_SUNSHINE(V_PZ, C_PZ);
         
         printline();
                  
         Fvar = -181.01*(2.5*( (float) FP_SAVED_DATA[28] )/256)  +178.9;       // -8 degree for callibration  
         fprintf(PC, "Backplane temp = %.1f C\n\r", Fvar);
         printline();
         
         if( FP_SAVED_DATA[30] == 0x01 ) fprintf(PC, "FAB Kill Switch Close\n\r");
         else fprintf(PC, "FAB Kill Switch Open\n\r");
         if( FP_SAVED_DATA[31] == 0x01 ) fprintf(PC, "OBC Kill Switch Close\n\r");
         else fprintf(PC, "OBC Kill Switch Open\n\r");
         
         CLEAR_DATA_ARRAY( FP_SAVED_DATA , 40 );
         printline();
      } 
      
      GENARATE_CW_DATA();
      CLEAR_DATA_ARRAY( FAB_TO_MPIC_ARRAY , 40 );
      CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 40 );
   }       
}


void SET_RESET_PIC_TIME(char yr, char mn, char dy, char hr, char mi, char sc)  //0x7B
{
   CLEAR_DATA_ARRAY( MPIC_TO_RPIC_ARRAY, 10 );
   MPIC_TO_RPIC_ARRAY[0] = 0xA0;   // header
   MPIC_TO_RPIC_ARRAY[1] = 0x7B;   // comand
   MPIC_TO_RPIC_ARRAY[2] = yr  ;
   MPIC_TO_RPIC_ARRAY[3] = mn  ;
   MPIC_TO_RPIC_ARRAY[4] = dy  ;
   MPIC_TO_RPIC_ARRAY[5] = hr  ;
   MPIC_TO_RPIC_ARRAY[6] = mi  ;
   MPIC_TO_RPIC_ARRAY[7] = sc  ;   
   MPIC_TO_RPIC_ARRAY[9] = 0xA1;   // footer
   
   // sending command to reset pic__________________________________________
   COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(3, 500);
   if( RPIC_TO_MPIC_ARRAY[1] && RPIC_TO_MPIC_ARRAY[0] == 0xA0 && RPIC_TO_MPIC_ARRAY[31] == 0xA1 )
   {
      Fprintf(PC,"Reset pic time updated\n\r"); 
      printline();
      
      CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32 );
   }
   
   else Fprintf(PC,"Reset pic time not updated\n\r"); 
}

void POWER_LINE_CONTROL_USING_MPIC_COMMAND_TO_RST_PIC(char _3v3_1, char _3v3_2, char _5v0, char UNR_2 ) // 0x7C
{
   Fprintf(PC,"Power line control using main pic command to reset pic\n\r");

   CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32);
   MPIC_TO_RPIC_ARRAY[0]  = 0xA0   ;
   MPIC_TO_RPIC_ARRAY[1]  = 0x7C   ;
   MPIC_TO_RPIC_ARRAY[2]  = _3v3_1 ;
   MPIC_TO_RPIC_ARRAY[3]  = _3v3_2 ;
   MPIC_TO_RPIC_ARRAY[4]  = _5v0   ;
   MPIC_TO_RPIC_ARRAY[5]  = UNR_2  ;
   MPIC_TO_RPIC_ARRAY[9]  = 0xA1   ;   // footer
   
   // sending command to reset pic__________________________________________
   COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(3, 500);
   if( RPIC_TO_MPIC_ARRAY[1] == 0x7C && RPIC_TO_MPIC_ARRAY[0] == 0xA0 && RPIC_TO_MPIC_ARRAY[31] == 0xA1 )
   { 
      Fprintf(PC,"Reset pic power line controlled\n\r"); 
      printline();
      CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32 ); 
   }
   
   else Fprintf(PC,"Reset pic did not respond\n\r");
}

// deploy antenna set -1 ______________________________________________________
Void DEPLOY_ANTENNAS_SET_1(char dep_time, char num_of_days)
{
   if(ANT_SET_1_DEP_FLAG == 1)
   {
      //deployment at 00:30:00
      if( (MP_RTC_HOUR==0) && (MP_RTC_MIN==dep_time) && (0<=MP_RTC_SEC) && (MP_RTC_SEC<=10) )
      {
         Fprintf(PC,"Time %u: %u: %u\n\r", MP_RTC_HOUR, MP_RTC_MIN, MP_RTC_SEC );
         _CW_STATUS_30MIN = Enabled ;
         printline();
         Fprintf(PC,"Antenna set-1 deployments\n\r");
         //reading antenna flags
         unsigned int16 DF1 = READ_PROGRAM_EEPROM(0x18000);
         unsigned int16 A1C = READ_PROGRAM_EEPROM(0x18002);
         unsigned int16 DF2 = READ_PROGRAM_EEPROM(0x18004); 
         unsigned int16 A2C = READ_PROGRAM_EEPROM(0x18006);
         
         // printing antenna related variables 
         Fprintf(PC,"current antenna related variable values before deployment\n\r");
         Fprintf(PC,"ANT_SET_1_DEP_FLAG = %LX\n\r",DF1);
         Fprintf(PC,"ANTSET_1_COUNT = %LX\n\r",A1C);
         Fprintf(PC,"ANT_SET_2_DEP_FLAG = %LX\n\r",DF2);
         Fprintf(PC,"ANTSET_2_COUNT = %LX\n\r",A2C);
         
         if(A1C < num_of_days)
         {  
            CLEAR_DATA_ARRAY( MPIC_TO_RPIC_ARRAY, 10 );
            MPIC_TO_RPIC_ARRAY[0] = 0xA0;   // header
            MPIC_TO_RPIC_ARRAY[1] = 0xDA;   // comand
            MPIC_TO_RPIC_ARRAY[9] = 0xA1;   // footer
      
            // sending command to reset pic wait for respone we will try 10 times
            // until reset pic reply 
            COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(5, 5000);
            
//!            if( MPIC_TO_RPIC_ARRAY[1] == 0xDA && RPIC_TO_MPIC_ARRAY[0] == 0xA0 && RPIC_TO_MPIC_ARRAY[31] == 0xA1 )
//!            {
               // then we start antena deployment 
               Fprintf(PC,"ResetPIC responded correctly\n\r");
               Fprintf(PC,"Antenna set-1 deployment started\n\r"); 
               
               OUTPUT_HIGH(PIN_B0);
               for( int i = 0; i<25; i++)
               {
                  Delay_ms(1000);
                  Fprintf(PC,"Counting deployment time %02d Sec\n\r", i+1);
               }
               OUTPUT_LOW(PIN_B0);
               
               Fprintf(PC,"Antenna-1 deployment finished\n\r"); 
            
               // now we are updating antenna related varibles to memory EEPROM
               A1C++; // we already try one time deployment, since we have to increase antenna 1 counter in memory
               
               if( A1C >=5) DF1 = 0; // antenna 1 counter reach this value antenna set 1 is not in deplyment mode
               
               // writing antenna related variable to EEPROM
               ERASE_PROGRAM_EEPROM(0x18000);
               WRITE_PROGRAM_EEPROM(0x18000,DF1);
               WRITE_PROGRAM_EEPROM(0x18002,A1C);
               WRITE_PROGRAM_EEPROM(0x18004,DF2);
               WRITE_PROGRAM_EEPROM(0x18006,A2C);
               
               ANT_SET_1_DEP_FLAG = READ_PROGRAM_EEPROM(0x18000);
               ANTSET_1_COUNT     = READ_PROGRAM_EEPROM(0x18002);
               ANT_SET_2_DEP_FLAG = READ_PROGRAM_EEPROM(0x18004); 
               ANTSET_2_COUNT     = READ_PROGRAM_EEPROM(0x18006);
               
                // printing antenna related variables 
               Fprintf(PC,"current antenna related variable values after deployment\n\r");
               Fprintf(PC,"ANT_SET_1_DEP_FLAG = %LX\n\r",ANT_SET_1_DEP_FLAG);
               Fprintf(PC,"ANTSET_1_COUNT = %LX\n\r",ANTSET_1_COUNT);
               Fprintf(PC,"ANT_SET_2_DEP_FLAG = %LX\n\r",DF2);
               Fprintf(PC,"ANTSET_2_COUNT = %LX\n\r",ANTSET_2_COUNT);
            //}
            
//!            else  
//!            {
//!               Fprintf(PC,"Reset PIC did not reply to antenna deployment request\n\r ");
//!            }
            
            // finaly clear the data array received from com port
            CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32 );
         }
         printline();
         READ_MP_RTC_();
      }
   } 
}

// deplying antenna set 2______________________________________________________
Void DEPLOY_ANTENNAS_SET_2(char dep_time, char num_of_days)
{
   if(ANT_SET_2_DEP_FLAG == 1)
   {
      //deployment at 00:30:00
      if( (MP_RTC_HOUR==0) && (MP_RTC_MIN==dep_time) && (0<=MP_RTC_SEC) && (MP_RTC_SEC<=10) )
      {
         
         _CW_STATUS_30MIN = Enabled ;
         
         printline();
         Fprintf(PC,"Antenna set-2 deployments\n\r");
         //reading antenna flags
         unsigned int16 DF1 = READ_PROGRAM_EEPROM(0x18000);
         unsigned int16 A1C = READ_PROGRAM_EEPROM(0x18002);
         unsigned int16 DF2 = READ_PROGRAM_EEPROM(0x18004); 
         unsigned int16 A2C = READ_PROGRAM_EEPROM(0x18006);
         
         // printing antenna related variables 
         Fprintf(PC,"current antenna related variable values before deployment\n\r");
         Fprintf(PC,"ANT_SET_1_DEP_FLAG = %LX\n\r",DF1);
         Fprintf(PC,"ANTSET_1_COUNT = %LX\n\r",A1C);
         Fprintf(PC,"ANT_SET_2_DEP_FLAG = %LX\n\r",DF2);
         Fprintf(PC,"ANTSET_2_COUNT = %LX\n\r",A2C);
         
         if(A2C < num_of_days)
         {  
            CLEAR_DATA_ARRAY( MPIC_TO_RPIC_ARRAY, 10 );
            MPIC_TO_RPIC_ARRAY[0] = 0xA0;   // header
            MPIC_TO_RPIC_ARRAY[1] = 0xDA;   // comand
            MPIC_TO_RPIC_ARRAY[9] = 0xA1;   // footer
      
            // sending command to reset pic wait for respone we will try 10 times
            // until reset pic reply 
            COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(5, 5000);
            
//!            if( MPIC_TO_RPIC_ARRAY[1] == 0xDA && RPIC_TO_MPIC_ARRAY[0] == 0xA0 && RPIC_TO_MPIC_ARRAY[31] == 0xA1 )
//!            {
               // then we start antena deployment            
               Fprintf(PC,"Antenna set-2 deployment started\n\r"); 
               
               OUTPUT_HIGH(PIN_B1);
               for( int i = 0; i<25; i++)
               {
                  Delay_ms(1000);
                  Fprintf(PC,"Counting deployment time %02d Sec\n\r", i+1);
               }               
               OUTPUT_LOW(PIN_B1);
               
               Fprintf(PC,"Antenna-2 deployment finished\n\r"); 
            
               // now we are updating antenna related varibles to memory EEPROM
               A2C++; // we already try one time deployment, since we have to increase antenna 2 counter in memory
               
               if( A2C >=5) DF2 = 0; // antenna 1 counter reach this value antenna set 2 is not in deplyment mode
               
               // writing antenna related variable to EEPROM
               ERASE_PROGRAM_EEPROM(0x18000);
               WRITE_PROGRAM_EEPROM(0x18000,DF1);
               WRITE_PROGRAM_EEPROM(0x18002,A1C);
               WRITE_PROGRAM_EEPROM(0x18004,DF2);
               WRITE_PROGRAM_EEPROM(0x18006,A2C);
               
               ANT_SET_1_DEP_FLAG = READ_PROGRAM_EEPROM(0x18000);
               ANTSET_1_COUNT     = READ_PROGRAM_EEPROM(0x18002);
               ANT_SET_2_DEP_FLAG = READ_PROGRAM_EEPROM(0x18004); 
               ANTSET_2_COUNT     = READ_PROGRAM_EEPROM(0x18006);
               
                // printing antenna related variables 
               Fprintf(PC,"current antenna related variable values after deployment\n\r");
               Fprintf(PC,"ANT_SET_1_DEP_FLAG = %LX\n\r",ANT_SET_1_DEP_FLAG);
               Fprintf(PC,"ANTSET_1_COUNT = %LX\n\r",ANTSET_1_COUNT);
               Fprintf(PC,"ANT_SET_2_DEP_FLAG = %LX\n\r",DF2);
               Fprintf(PC,"ANTSET_2_COUNT = %LX\n\r",ANTSET_2_COUNT);
            //}
            
//!            else  
//!            {
//!               Fprintf(PC,"Reset PIC did not reply to antenna deployment request\n\r ");
//!            }
            
            // finaly clear the data array received from com port
            CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32 );
         }
         printline();
         READ_MP_RTC_();
      }
   } 
}

// this function will load the antenaa flags when main pic reset
void LOAD_ANTENNA_DEPLOYMENT_VALUES()
{
      printline();
      Fprintf(PC,"Antenna deploymet values loaded\n\r");
      ANT_SET_1_DEP_FLAG = READ_PROGRAM_EEPROM(0x18000);
      ANTSET_1_COUNT     = READ_PROGRAM_EEPROM(0x18002);
      ANT_SET_2_DEP_FLAG = READ_PROGRAM_EEPROM(0x18004); 
      ANTSET_2_COUNT     = READ_PROGRAM_EEPROM(0x18006);
   
      if( ANT_SET_1_DEP_FLAG != 0x0001 ) ANT_SET_1_DEP_FLAG = 0;
      if( ANT_SET_2_DEP_FLAG != 0x0001 ) ANT_SET_2_DEP_FLAG = 0;
      // printing antenna related variables 
      

      
      Fprintf(PC,"ANT_SET_1_DEP_FLAG = %LX\n\r",ANT_SET_1_DEP_FLAG);
      Fprintf(PC,"ANTSET_1_COUNT = %LX\n\r",ANTSET_1_COUNT);
      Fprintf(PC,"ANT_SET_2_DEP_FLAG = %LX\n\r",ANT_SET_2_DEP_FLAG);
      Fprintf(PC,"ANTSET_2_COUNT = %LX\n\r",ANTSET_2_COUNT);
      
      if( ANTSET_1_COUNT == 0 )
      {
         _CW_STATUS_30MIN = Disabled ;
         Fprintf(PC,"Sat is not in CW transmit mode = %X\n\r",_CW_STATUS_30MIN);
      }
      else
      {
         _CW_STATUS_30MIN = Enabled ;
         Fprintf(PC,"Sat is in CW transmit mode = %X\n\r",_CW_STATUS_30MIN);
         Output_Low(LED_PIN);    // start led low
      }
      printline();
}

//!// 90 second comuniction with reset pic function_______________________________0x7A
//!void COMUNICATION_WITH_RST_PIC_90SEC(int sec)
//!{
//!   if(SEC90_Count>= sec)
//!   {
//!      SEC90_Count = 0;
//!      
//!      CLEAR_DATA_ARRAY( MPIC_TO_RPIC_ARRAY, 10 );
//!      MPIC_TO_RPIC_ARRAY[0] = 0xA0;   // header
//!      MPIC_TO_RPIC_ARRAY[1] = 0x7A;   // comand
//!      MPIC_TO_RPIC_ARRAY[9] = 0xA1;   // footer
//!      
//!      // just printing sending comand to reset pic_____________________________     
//!      Fprintf(PC,"Sent comand to Reset PIC >> ");
//!      for(int i = 0; i<10; i++)
//!      {
//!         Fprintf(PC,"%X ",MPIC_TO_RPIC_ARRAY[i]);
//!      }
//!      printline();
//!      //_______________________________________________________________________
//!      
//!      // sending command to reset pic__________________________________________
//!      COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(3, 500);
//!      
//!      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!      // executing received comand from rest pic_______________________________
//!      if( RPIC_TO_MPIC_ARRAY[0] == 0xA0 && RPIC_TO_MPIC_ARRAY[1] == 0x7A  && RPIC_TO_MPIC_ARRAY[31] == 0xA1)
//!      {
//!         PRINT_RECIVED_COMMAND_FROM_RESET_PIC();
//!         SYNC_MAIN_PIC_AND_RESET_PIC_TIME();    // if there is a time difference time will be updated
//!         
//!         // calculating day number
//!         DAY_NUMBER = GET_DAY_NUMBER_OF_THE_YEAR(RPIC_TO_MPIC_ARRAY[2], RPIC_TO_MPIC_ARRAY[3], RPIC_TO_MPIC_ARRAY[4]);
//!         Fprintf(PC,"Day number = %03Lu\n\r", DAY_NUMBER);
//!         
//!         RST_PIC_HSK_DATA_START_ADD = (DAY_NUMBER + 24) << 16;                                  
//!         FAB_PIC_HSK_DATA_START_ADD = RST_PIC_HSK_DATA_START_ADD + 0x00008000 ;
//!         
//!         //Erasing 64KB sector to write data
//!         if(SUCCESFULL_COMUNICATION_WITH_RST_PIC == 0 )
//!         {
//!            OUTPUT_LOW(PIN_C5);  // put correct pic
//!            COM_FM_SECTOR_ERASE( RST_PIC_HSK_DATA_START_ADD, 64);
//!            fprintf(PC, "Reset PIC HSK data Starting address for today - 0x%LX\n\r",RST_PIC_HSK_DATA_START_ADD );
//!            fprintf(PC, "FAB - PIC HSK data Starting address for today - 0x%LX\n\r",FAB_PIC_HSK_DATA_START_ADD );
//!            OUTPUT_HIGH(PIN_C5);
//!         }
//!         
//!         //Writing housekeeping data in the flash memory
//!         if( CFM_ACCESS_FLAG == 0 )
//!         {  
//!            OUTPUT_LOW(PIN_C5);      // get SFM access
//!            //_________________________________________________________________
//!            // now we have already comunicate with reset and we have the reset pic reponse
//!            // and we have clearence to write data in COM FM, so we can comunicate with FAB 
//!            // and get FAB HSK
//!            CLEAR_DATA_ARRAY( MPIC_TO_FAB_ARRAY, 3 );
//!            MPIC_TO_FAB_ARRAY[0] = 0xFA;   // header
//!            MPIC_TO_FAB_ARRAY[1] = 0xA0;   // comand
//!            MPIC_TO_FAB_ARRAY[2] = 0xFB;   // footer
//!            
//!            // sending command to reset pic__________________________________________
//!            COMUNICATION_WITH_FAB_PIC_AND_WAIT_FOR_RESPONE(3, 500,100,32);
//!            
//!            if( FAB_TO_MPIC_ARRAY[0] == 0xFA && FAB_TO_MPIC_ARRAY[1] == 0xA0  && FAB_TO_MPIC_ARRAY[31] == 0xFB)
//!            {
//!               fprintf(PC, "FABPIC comunication-> sucess:notsuccess = %Ld:%Ld\n\r",SUCCESFULL_COMUNICATION_WITH_FAB+1 ,FAILED_COMUNICATION_WITH_FAB );
//!               
//!               fprintf(PC, "Recent FAB HSK data writing location     - 0x%LX   Sec_num = %Lu\n\r", FAB_PIC_HSK_DATA_START_ADD  + (32* SUCCESFULL_COMUNICATION_WITH_FAB), DAY_NUMBER + 24 );  
//!               COM_FM_BYTE_WRITE( FAB_PIC_HSK_DATA_START_ADD     + (32* SUCCESFULL_COMUNICATION_WITH_FAB) , RPIC_TO_MPIC_ARRAY[2] ) ;
//!               COM_FM_BYTE_WRITE( FAB_PIC_HSK_DATA_START_ADD + 1 + (32* SUCCESFULL_COMUNICATION_WITH_FAB) , RPIC_TO_MPIC_ARRAY[3] ) ;
//!               COM_FM_BYTE_WRITE( FAB_PIC_HSK_DATA_START_ADD + 2 + (32* SUCCESFULL_COMUNICATION_WITH_FAB) , RPIC_TO_MPIC_ARRAY[4] ) ;
//!               
//!               unsigned int32 BYTE_ADDRESS_ = 0;
//!               for(int i = 3; i<=31; i++)
//!               {
//!                  BYTE_ADDRESS_ = FAB_PIC_HSK_DATA_START_ADD + i + (32* SUCCESFULL_COMUNICATION_WITH_FAB) ;
//!                  COM_FM_BYTE_WRITE( BYTE_ADDRESS_ ,FAB_TO_MPIC_ARRAY[i-1] );
//!               }
//!               
//!               fprintf(PC, "Saved data FAB pic HSK - ");
//!               for(int i = 0; i<32; i++)
//!               {
//!                  BYTE_ADDRESS_ = FAB_PIC_HSK_DATA_START_ADD + i + (32* SUCCESFULL_COMUNICATION_WITH_FAB);
//!                  fprintf(PC, "%X ",COM_FM_BYTE_READ( BYTE_ADDRESS_ ) );
//!               }
//!               printLine();
//!               
//!               SUCCESFULL_COMUNICATION_WITH_FAB++;
//!            }
//!            
//!            else
//!            {
//!               Fprintf(PC,"90 seconds comunication with FAB pic was not succesfull\n\r");
//!               FAILED_COMUNICATION_WITH_FAB++;
//!            }
//!                      
//!            
//!            printline();
//!            fprintf(PC, "RSTPIC comunication-> sucess:notsuccess = %Ld:%Ld\n\r",SUCCESFULL_COMUNICATION_WITH_RST_PIC+1 ,FAILED_COMUNICATION_WITH_RST_PIC );
//!            
//!            unsigned int32 BYTE_ADDRESS = 0;
//!            for(int i = 0; i<24; i++)
//!            {
//!               BYTE_ADDRESS = RST_PIC_HSK_DATA_START_ADD + i + (23* SUCCESFULL_COMUNICATION_WITH_RST_PIC) ;
//!               COM_FM_BYTE_WRITE( BYTE_ADDRESS ,RPIC_TO_MPIC_ARRAY[i+1] );
//!               if( i == 0 )
//!               {
//!                  //fprintf(PC, "HK data wrriting 64KB sector number - %Ld\n\r",(BYTE_ADDRESS>>16)&0x00000FFF );
//!                  fprintf(PC, "Recent HSK data writing location     - 0x%LX   Sec_num = %Lu\n\r",BYTE_ADDRESS, DAY_NUMBER + 24 );   
//!               }
//!            }
//!            
//!            fprintf(PC, "Saved data Resetpic HSK - ");
//!            for(int i = 0; i<24; i++)
//!            {
//!               BYTE_ADDRESS = RST_PIC_HSK_DATA_START_ADD  + i + (23* SUCCESFULL_COMUNICATION_WITH_RST_PIC) ;
//!               fprintf(PC, "%X ",COM_FM_BYTE_READ( BYTE_ADDRESS ) );
//!            }
//!            printLine();
//!            printLine();
//!            SUCCESFULL_COMUNICATION_WITH_RST_PIC++;
//!            OUTPUT_HIGH(PIN_C5);
//!         }
//!         
//!         if( CFM_ACCESS_FLAG == 1 )
//!         {
//!            fprintf(PC, "Cannot write house keeping data in Com FM due to access is given to ComPIC - ");
//!         }        
//!                  
//!         // finaly clear the data array received from reset pic
//!         CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32 );
//!         
//!      }
//!      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!      
//!      else 
//!      {
//!         Fprintf(PC,"90 seconds comunication with reset pic was not succesfull\n\r");
//!         FAILED_COMUNICATION_WITH_RST_PIC++;
//!      }
//!   }       
//!}


//!      fprintf(PC,"FAB data request command from main pic\n\r");
//!      
//!      CLEAR_DATA_ARRAY(FAB_TO_MPIC_ARRAY,35);
//!      
//!      _RAW_C = AVERAGE_HECS_READING(RAW_CURRENT,500, 400);
//!      _BAT_C = AVERAGE_HECS_READING(BATTERY_CURRENT,500, 400);
//!      _SOL_C = AVERAGE_HECS_READING(SP_BUS_CURRENT,500, 400);
//!      
//!      // preparing data
//!      FAB_TO_MPIC_ARRAY[0]  = 0xFA  ;        // header
//!      
//!      FAB_TO_MPIC_ARRAY[1]  = 0xA0  ;        // command identifier  
//!      
//!      FAB_TO_MPIC_ARRAY[2]  = AVERAGE_PICADC_READING(RAW_VOLTAGE, 10, 100) ;     //READ_FAB_PIC_ADC( RAW_VOLTAGE )           ;
//!      FAB_TO_MPIC_ARRAY[3]  = (char)( (_RAW_C>>8) & 0x00FF) ;       
//!      FAB_TO_MPIC_ARRAY[4]  = AVERAGE_PICADC_READING(BATTERY_VOLTAGE, 10, 100) ; //READ_FAB_PIC_ADC( BATTERY_VOLTAGE )       ;
//!      FAB_TO_MPIC_ARRAY[5]  = (char)( (_BAT_C>>8) & 0x00FF) ;   
//!      FAB_TO_MPIC_ARRAY[6]  = READ_FAB_PIC_ADC( BATTERY_TEMP )             ;
//!      FAB_TO_MPIC_ARRAY[7]  = AVERAGE_PICADC_READING(SP_BUS_VOLTAGE, 10, 100) ;  //READ_FAB_PIC_ADC( SP_BUS_VOLTAGE )        ;
//!      FAB_TO_MPIC_ARRAY[8]  = (char)( (_SOL_C>>8) & 0x00FF) ; 
//!            
//!      FAB_TO_MPIC_ARRAY[9]  = AVERAGE_EXTADC_READING(PLUS_X_VOLTAGE, 10,100);      //READ_EXT_ADC_CHANELS( PLUS_X_VOLTAGE )  ;
//!      FAB_TO_MPIC_ARRAY[10] = AVERAGE_PICADC_READING(PLUS_X_SP_CURRENT, 10, 100) ; //READ_FAB_PIC_ADC( PLUS_X_SP_CURRENT )   ;
//!      FAB_TO_MPIC_ARRAY[11] = READ_EXT_ADC_CHANELS( PLUS_X_TEMP )     ;
//!      
//!      FAB_TO_MPIC_ARRAY[12] = AVERAGE_EXTADC_READING(PLUS_Y_VOLTAGE, 10,100);   //READ_EXT_ADC_CHANELS( PLUS_Y_VOLTAGE )  ;
//!      FAB_TO_MPIC_ARRAY[13] = AVERAGE_PICADC_READING(PLUS_Y_SP_CURRENT, 10, 100) ; //READ_FAB_PIC_ADC( PLUS_Y_SP_CURRENT )   ;
//!      FAB_TO_MPIC_ARRAY[14] = READ_EXT_ADC_CHANELS( PLUS_Y_TEMP )     ;
//!      
//!      FAB_TO_MPIC_ARRAY[15] = AVERAGE_EXTADC_READING(PLUS_Z_VOLTAGE, 10,100);   //READ_EXT_ADC_CHANELS( PLUS_Z_VOLTAGE )  ;
//!      FAB_TO_MPIC_ARRAY[16] = AVERAGE_PICADC_READING(PLUS_Z_SP_CURRENT, 10, 100) ; //READ_FAB_PIC_ADC( PLUS_Z_SP_CURRENT )   ;
//!      FAB_TO_MPIC_ARRAY[17] = READ_EXT_ADC_CHANELS( PLUS_Z_TEMP )     ;
//!      
//!      FAB_TO_MPIC_ARRAY[18] = AVERAGE_EXTADC_READING(MINUS_X_VOLTAGE, 10,100);    //READ_EXT_ADC_CHANELS( MINUS_X_VOLTAGE ) ;
//!      FAB_TO_MPIC_ARRAY[19] = AVERAGE_PICADC_READING(MINUS_X_SP_CURRENT, 10, 100) ; //READ_FAB_PIC_ADC( MINUS_X_SP_CURRENT )  ;
//!      FAB_TO_MPIC_ARRAY[20] = READ_EXT_ADC_CHANELS( MINUS_X_TEMP )    ;
//!      
//!      FAB_TO_MPIC_ARRAY[21] = AVERAGE_EXTADC_READING(MINUS_Y_VOLTAGE, 10,100);   //READ_EXT_ADC_CHANELS( MINUS_Y_VOLTAGE ) ;
//!      FAB_TO_MPIC_ARRAY[22] = AVERAGE_PICADC_READING(MINUS_Y_SP_CURRENT, 10, 100) ; //READ_FAB_PIC_ADC( MINUS_Y_SP_CURRENT )  ;
//!      FAB_TO_MPIC_ARRAY[23] = READ_EXT_ADC_CHANELS( MINUS_Y_TEMP )    ;
//!      
//!      FAB_TO_MPIC_ARRAY[24] = AVERAGE_EXTADC_READING(MINUS_Z_VOLTAGE, 10,100);   //READ_EXT_ADC_CHANELS( MINUS_Z_VOLTAGE ) ;
//!      FAB_TO_MPIC_ARRAY[25] = AVERAGE_PICADC_READING(MINUS_Z_SP_CURRENT, 10, 100) ; //READ_FAB_PIC_ADC( MINUS_Z_SP_CURRENT )  ;
//!      FAB_TO_MPIC_ARRAY[26] = READ_EXT_ADC_CHANELS( MINUS_Z_TEMP )    ;
//!      
//!      FAB_TO_MPIC_ARRAY[27] = READ_EXT_ADC_CHANELS( BACKPLANE_TEMP )  ;
//!      FAB_TO_MPIC_ARRAY[28] = READ_EXT_ADC_CHANELS( TEMP8 )           ;
//!      FAB_TO_MPIC_ARRAY[29] = input(FAB_KILL_SWITCH_STATUS_PIN)       ;
//!      FAB_TO_MPIC_ARRAY[30] = input(OBC_KILL_STATUS_STATUS_PIN)       ;
//!      
//!      FAB_TO_MPIC_ARRAY[31] = (char)(_RAW_C & 0x00FF)     ;
//!      FAB_TO_MPIC_ARRAY[32] = (char)(_BAT_C & 0x00FF)     ;
//!      FAB_TO_MPIC_ARRAY[33] = (char)(_SOL_C & 0x00FF)     ;
//!      
//!      FAB_TO_MPIC_ARRAY[34] = 0xFB                                    ;   // footer


