// here we check main pic UART is available or not if available we save in MPIC_TO_RPIC array
void CHECK_UART_INCOMING_FROM_COM_PIC()
{
   if( CPic_Available() )
   {
      Delay_ms(100);
      for( int i = 0; i<10; i++ )
      {
         if( CPic_Read() == 0xC0 )
         {
            CPIC_TO_RPIC_ARRAY[0] = 0xC0 ;
            break;
         }
      }

      for(int i = 1; i<=30; i++)
      {
         CPIC_TO_RPIC_ARRAY[i] = CPic_Read();
      }
   }
}


// this function will print recived command from com pic_______________________
void PRINT_RECIVED_COMMAND_FROM_COM_PIC()
{
   printline();
   Fprintf(PC,"RCVD CMD FROM COM PIC >> ");
   for(int i = 0; i<20; i++)
   {
      Fprintf(PC,"%X ",CPIC_TO_RPIC_ARRAY[i]);
   }
   printline();
}

/* this fucntons monitor the com pic. if no response within 10min reset pic
will restart the com pic*/
void MONITOR_COM_PIC_90SEC_COMUNICATION(int time)
{
   if( CPIC_TIME_COUNTER >= 600 )
   {
      CPIC_TIME_COUNTER = 0;
      NUMOF_CPIC_RST++;
      
      ComPic_Power(0);   // turn off main pic
      for( int i = 0; i<time ; i++)
      {
         Delay_ms(1000);
        Fprintf(PC,"Waiting to turn on com Pic %02d Sec\n\r",i);
      }
      ComPic_Power(1);   // turn on main pic
   }
}


void RESEPOND_TO_CPIC_90SEC_CMD()
{
   if(CPIC_TO_RPIC_ARRAY[1] == 0xE0)
   {
      Fprintf(PC,"90 seconds comunication command\n\r");
      
      CPIC_TIME_COUNTER = 0;             // because comunication happaned corretly
      CLEAR_DATA_ARRAY( RPIC_TO_CPIC_ARRAY, 20);
      RPIC_TO_CPIC_ARRAY[0]  = 0xC0   ;
      RPIC_TO_CPIC_ARRAY[1]  = 0xE0   ; 
      
      RPIC_TO_CPIC_ARRAY[2]  = year   ;
      RPIC_TO_CPIC_ARRAY[3]  = month  ;
      RPIC_TO_CPIC_ARRAY[4]  = day    ;
      RPIC_TO_CPIC_ARRAY[5]  = hour   ;
      RPIC_TO_CPIC_ARRAY[6]  = minute ;
      RPIC_TO_CPIC_ARRAY[7]  = second ;
      
      RPIC_TO_CPIC_ARRAY[19] = 0xC1   ;
      for(int i = 0; i<20; i++)
      {
         fputc(RPIC_TO_CPIC_ARRAY[i],CPic);
      }
      printline();
      //_______________________________________________________________________
      
   } 
}


Void UPDATE_RTC_BY_COM_PIC_CMD()
{
   if(CPIC_TO_RPIC_ARRAY[1] == 0xEA) //MP HF
   {
      Fprintf(PC,"Reset pic RTC update command Received from COM PIC\n\r");
      
      // Acknowleging to the comand____________________________________________
      Delay_ms(10);
      CLEAR_DATA_ARRAY( RPIC_TO_CPIC_ARRAY, 20);
      RPIC_TO_CPIC_ARRAY[0]  = 0xC0   ;
      RPIC_TO_CPIC_ARRAY[1]  = 0xEA   ;
      RPIC_TO_CPIC_ARRAY[19] = 0xC1   ;
      for(int i = 0; i<20; i++)
      {
         fputc(RPIC_TO_CPIC_ARRAY[i],CPic);
      }
      //_______________________________________________________________________
      
      //Updating the RTC
      year   = CPIC_TO_RPIC_ARRAY[2]  ;
      month  = CPIC_TO_RPIC_ARRAY[3]  ;
      day    = CPIC_TO_RPIC_ARRAY[4]  ;
      hour   = CPIC_TO_RPIC_ARRAY[5]  ;
      minute = CPIC_TO_RPIC_ARRAY[6]  ;
      second = CPIC_TO_RPIC_ARRAY[7]  ;
     

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

void SENDING_TIME_TO_COMPIC()
{
   if(CPIC_TO_RPIC_ARRAY[1] == 0xEB)
   {
      Fprintf(PC,"SENDING_TIME_TO_COMPIC\n\r");
      
      CPIC_TIME_COUNTER = 0;             // because comunication happaned corretly
      CLEAR_DATA_ARRAY( RPIC_TO_CPIC_ARRAY, 20);
      RPIC_TO_CPIC_ARRAY[0]  = 0xC0   ;
      RPIC_TO_CPIC_ARRAY[1]  = 0xEB   ; 
      
      RPIC_TO_CPIC_ARRAY[2]  = year   ;
      RPIC_TO_CPIC_ARRAY[3]  = month  ;
      RPIC_TO_CPIC_ARRAY[4]  = day    ;
      RPIC_TO_CPIC_ARRAY[5]  = hour   ;
      RPIC_TO_CPIC_ARRAY[6]  = minute ;
      RPIC_TO_CPIC_ARRAY[7]  = second ;
      
      RPIC_TO_CPIC_ARRAY[19] = 0xC1   ;
      for(int i = 0; i<20; i++)
      {
         fputc(RPIC_TO_CPIC_ARRAY[i],CPic);
      }
      printline();
      //_______________________________________________________________________
      
   } 
}

