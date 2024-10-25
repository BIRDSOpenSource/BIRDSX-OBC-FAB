// here we check main pic UART is available or not if available we save in MPIC_TO_RPIC array
void CHECK_UART_INCOMING_FROM_MAIN_PIC()
{
   if( MPic_Available() )
   {
      Delay_ms(100);
      for( int i = 0; i<10; i++ )
      {
         if( MPic_Read() == 0xA0 )
         {
            MPIC_TO_RPIC_ARRAY[0] = 0xA0 ;
            break;
         }
      }

      for(int i = 1; i<=30; i++)
      {
         MPIC_TO_RPIC_ARRAY[i] = MPic_Read();
         fprintf(PC, "%X ", MPIC_TO_RPIC_ARRAY[i]);
      }
      printline();
   }
}




// this function will print recived command from main pic______________________
void PRINT_RECIVED_COMMAND_FROM_MAIN_PIC()
{
   printline();
   Fprintf(PC,"RCVD CMD FROM MAIN PIC >> ");
   for(int i = 0; i<10; i++)
   {
      Fprintf(PC,"%X ",MPIC_TO_RPIC_ARRAY[i]);
   }
   printline();
}




/* this fucntons monitor the main pic, if no response within 10min reset pic
will restart the main pic*/
void MONITOR_MAIN_PIC_90SEC_COMUNICATION(int time)
{
   if( MPIC_TIME_COUNTER >= 600 )
   {
      MPIC_TIME_COUNTER = 0;
      NUMOF_MPIC_RST++;
      
      Fprintf(PC,"Hang up reset, MainPIC is turned off\n\r");
      MainPic_Power(0);   // turn off main pic
      for( int i = 0; i<time; i++)
      {
         Delay_ms(1000);
         Fprintf(PC,"Waiting to turn on Main Pic %02d Sec\n\r",i);
      }
      MainPic_Power(1);   // turn on main pic
      Fprintf(PC,"MainPIC is restarted\n\r");
   }
}

  
/* this fuction will measure house keeping data and will send to main pic______
when 90 sec command arrive from main pic*/
void RESEPOND_TO_MPIC_90SEC_CMD()
{
   if(MPIC_TO_RPIC_ARRAY[1] == 0x7A)
   {
      Fprintf(PC,"90 seconds comunication command\n\r");
      
      // we make main pic counter zero because succesful comunication
      MPIC_TIME_COUNTER = 0;
      
      // clear RPIC_TO_MPIC array before putting new data______________________
      CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32 );
      
      // reading ADC values____________________________________________________
      _Raw_power_ADC_val       = Measure_Raw_voltage() ;          
      _3V3_1_current_ADC_val   = Measure_3V3_1_current() ;          
      _3V3_2_current_ADC_val   = Measure_3V3_2_current() ;
      _5V0_current_ADC_val     = Measure_5V0_current() ;          
      _UNREG_1_current_ADC_val = Measure_UNREG_1_current() ;      
      _UNREG_2_current_ADC_val = Measure_UNREG_2_current() ; 
      
      RPIC_TO_MPIC_ARRAY[0]    = 0xA0   ;     // header
      RPIC_TO_MPIC_ARRAY[1]    = 0x7A   ;
      RPIC_TO_MPIC_ARRAY[2]    = year   ;
      RPIC_TO_MPIC_ARRAY[3]    = month  ;
      RPIC_TO_MPIC_ARRAY[4]    = day    ;
      RPIC_TO_MPIC_ARRAY[5]    = hour   ;
      RPIC_TO_MPIC_ARRAY[6]    = minute ;
      RPIC_TO_MPIC_ARRAY[7]    = second ;     // sending reset pic RTC time to main pic
      
      RPIC_TO_MPIC_ARRAY[8]    = (unsigned int8)((_Raw_power_ADC_val>>8)     & 0xFF)     ;   
      RPIC_TO_MPIC_ARRAY[9]    = (unsigned int8)((_Raw_power_ADC_val)        & 0xFF)     ;   
      RPIC_TO_MPIC_ARRAY[10]   = (unsigned int8)((_3V3_1_current_ADC_val>>8) & 0xFF)     ;   
      RPIC_TO_MPIC_ARRAY[11]   = (unsigned int8)((_3V3_1_current_ADC_val)    & 0xFF)     ; 
      RPIC_TO_MPIC_ARRAY[12]   = (unsigned int8)((_3V3_2_current_ADC_val>>8) & 0xFF)     ;   
      RPIC_TO_MPIC_ARRAY[13]   = (unsigned int8)((_3V3_2_current_ADC_val)    & 0xFF)     ; 
      RPIC_TO_MPIC_ARRAY[14]   = (unsigned int8)((_5V0_current_ADC_val>>8)   & 0xFF)     ;   
      RPIC_TO_MPIC_ARRAY[15]   = (unsigned int8)((_5V0_current_ADC_val)      & 0xFF)     ;
      RPIC_TO_MPIC_ARRAY[16]   = (unsigned int8)((_UNREG_1_current_ADC_val>>8) & 0xFF)   ;   
      RPIC_TO_MPIC_ARRAY[17]   = (unsigned int8)((_UNREG_1_current_ADC_val)    & 0xFF)   ;
      RPIC_TO_MPIC_ARRAY[18]   = (unsigned int8)((_UNREG_2_current_ADC_val>>8) & 0xFF)   ;   
      RPIC_TO_MPIC_ARRAY[19]   = (unsigned int8)((_UNREG_2_current_ADC_val) )            ;
      
      RPIC_TO_MPIC_ARRAY[20]   = NUMOF_MPIC_RST ;    // number of main pic reset
      RPIC_TO_MPIC_ARRAY[21]   = NUMOF_CPIC_RST ;    // number of com pic reset
      RPIC_TO_MPIC_ARRAY[22]   = POWER_LINE_STATUS ; // power line status
      RPIC_TO_MPIC_ARRAY[23]   = (unsigned int8)((LAST_RESET_HOUR>>8) & 0xFF) ;
      RPIC_TO_MPIC_ARRAY[24]   = (unsigned int8)((LAST_RESET_HOUR)    & 0xFF) ;
      RPIC_TO_MPIC_ARRAY[25]   = 0xAA   ;  
      
      RPIC_TO_MPIC_ARRAY[31]  = 0xA1   ;  // footer  
      // sending data to main pic______________________________________________
      for(int i = 0; i<32; i++)
      {
         fputc( RPIC_TO_MPIC_ARRAY[i] , MPic);
      }
      
      // just printing sent reply to main pic__________________________________
      Fprintf(PC,"Reply sent to main pic >> ");
      for(int i = 0; i<32; i++)
      {
         Fprintf(PC,"%X ",RPIC_TO_MPIC_ARRAY[i]);
      }
      printline();
      printline();
   }
}

// updating reset pic RTC time useing main pic command_____________________________________________MP_HF___0x70
Void UPDATE_RTC_BY_MAIN_PIC_CMD()
{
   if(MPIC_TO_RPIC_ARRAY[1] == 0x7B) //MP HF
   {
      Fprintf(PC,"Reset pic RTC update command Received\n\r");
      
      // Acknowleging to the comand____________________________________________
      CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32);
      RPIC_TO_MPIC_ARRAY[0]  = 0xA0   ;
      RPIC_TO_MPIC_ARRAY[1]  = 0x7B   ;
      RPIC_TO_MPIC_ARRAY[31] = 0xA1   ;
      for(int i = 0; i<32; i++)
      {
         fputc(RPIC_TO_MPIC_ARRAY[i],MPic);
      }
      //_______________________________________________________________________
      
      //Updating the RTC
      year   = MPIC_TO_RPIC_ARRAY[2]  ;
      month  = MPIC_TO_RPIC_ARRAY[3]  ;
      day    = MPIC_TO_RPIC_ARRAY[4]  ;
      hour   = MPIC_TO_RPIC_ARRAY[5]  ;
      minute = MPIC_TO_RPIC_ARRAY[6]  ;
      second = MPIC_TO_RPIC_ARRAY[7]  ;
     

      //Printing New RTC value
      Fprintf(PC,"Updated New Time >> ") ;
      Fprintf(PC,"%u-", year)        ;
      Fprintf(PC,"%u-", month)       ;
      Fprintf(PC,"%u__", day)        ;
      Fprintf(PC,"%u:", hour)        ;
      Fprintf(PC,"%u:", minute)      ;
      Fprintf(PC,"%u\n\r", second)   ;
       
   }
}
   
// controling power line using main pic command________________________________
Void POWER_LINE_CONTROL_USING_MAIN_PIC_CMD()
{
   if(MPIC_TO_RPIC_ARRAY[1] == 0x7C) 
   {
      Fprintf(PC,"Power line control command Received from main pic\n\r");
      // Acknowleging to the comand____________________________________________
      CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32);
      RPIC_TO_MPIC_ARRAY[0]  = 0xA0   ;
      RPIC_TO_MPIC_ARRAY[1]  = 0x7C   ;
      RPIC_TO_MPIC_ARRAY[31] = 0xA1   ;
      for(int i = 0; i<32; i++)
      {
         fputc(RPIC_TO_MPIC_ARRAY[i],MPic);
      }
      //_______________________________________________________________________
      
      if(MPIC_TO_RPIC_ARRAY[2] == 0x01) _3V3Power_Line1(BB_ON_OCP_ON)  ;
      else _3V3Power_Line1(BB_OFF_OCP_OFF)  ;
      
      if(MPIC_TO_RPIC_ARRAY[3] == 0x01) _3V3Power_Line2(BB_ON_OCP_ON)  ;
      else _3V3Power_Line2(BB_OFF_OCP_OFF)   ;
      
      if(MPIC_TO_RPIC_ARRAY[4] == 0x01) _5V0Power_Line(BB_ON_OCP_ON)   ;
      else _5V0Power_Line(BB_OFF_OCP_OFF)  ;
      
      if(MPIC_TO_RPIC_ARRAY[5] == 0x01) Unreg2_Line(ON)         ;
      else Unreg2_Line(OFF)  ;
      
   }
}
Void RESET_SATELLITE_CMD()
{
   if(MPIC_TO_RPIC_ARRAY[1] == 0x7D)
   {  
      Fprintf(PC, "Reset satellite command Received from main PIC");
      SYSTEM_RESET();
   } 
   // Acknowleging to the comand____________________________________________
   CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32);
   RPIC_TO_MPIC_ARRAY[0]  = 0xA0   ;
   RPIC_TO_MPIC_ARRAY[1]  = 0x7C   ;
   RPIC_TO_MPIC_ARRAY[31] = 0xA1   ;
   for(int i = 0; i<32; i++)
   {
      fputc(RPIC_TO_MPIC_ARRAY[i],MPic);
   }
}

// antenna deployment command________________________________
Void TURN_ON_UNREG_2_LINE_FOR_ANT_DEPLOYMENT()
{
   if(MPIC_TO_RPIC_ARRAY[1] == 0xDA) 
   {
      Fprintf(PC,"Antenna deployment command received\n\r"); 
      // Acknowleging to the comand____________________________________________
      
      CLEAR_DATA_ARRAY( RPIC_TO_MPIC_ARRAY, 32);
      RPIC_TO_MPIC_ARRAY[0]  = 0xA0   ;
      RPIC_TO_MPIC_ARRAY[1]  = 0xDA   ;
      RPIC_TO_MPIC_ARRAY[31] = 0xA1   ;
      for(int i = 0; i<32; i++)
      {
         fputc(RPIC_TO_MPIC_ARRAY[i],MPic);
      }
      //_______________________________________________________________________
      
      int sec_count = 0 ;
      Unreg2_Line(1);
      Fprintf(PC,"UNREG-2 line is turned ON \n\r") ;
            
      for(int i = 0; i<120 ;i++)  // waiting 30 seconds, we can change this
      { 
         RST_EXT_WDT();
         Delay_ms(250); 
         Fprintf(PC,"Counting deployment time %02d Sec \n\r",sec_count++) ;
      }
      
      Unreg2_Line(0);
      Fprintf(PC,"UNREG-2 line is turned OFF \n\r") ;   
   } 
}


