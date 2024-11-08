//This function is used to enable RTC over writing_____________________________
Void RTC_WRITE_ENABLE(int1 i)
{
   if(i == 1)
   {
      #ASM
      movlw 0x55
      movwf EECON2
      movlw 0xAA
      movwf EECON2
      bsf RTCCON1,5
      #ENDASM
   }
   
      if(i == 0)
   {
      #ASM
      movlw 0x55
      movwf EECON2
      movlw 0xAA
      movwf EECON2
      bcf RTCCON1,5
      #ENDASM
   }
}

// This function will be used to ovewrite hour value in RTC____________________
Void WRITE_OBC_RTC(char year, char month, char day, char hour,char min, char sec)
{
   char i,lower,higher;
   RTC_WRITE_ENABLE(1); 
   
   // writing year
   i = year ;
   lower  = i % 10;
   higher = ( ( i - (i%10) )/10 )<<4 ;
   i = lower|higher; 
   RTCPTR0 = 1;
   RTCPTR1 = 1;
   RTCVALL = i ;
   
   // writing month
   i = month ;
   lower  = i % 10;
   higher = ( ( i - (i%10) )/10 )<<4 ;
   i = lower|higher; 
   RTCPTR0 = 0;
   RTCPTR1 = 1;
   RTCVALH = i ;
   
   // writing day
   i = day ;
   lower  = i % 10;
   higher = ( ( i - (i%10) )/10 )<<4 ;
   i = lower|higher; 
   RTCPTR0 = 0;
   RTCPTR1 = 1;
   RTCVALL = i ;
   
   // writing hour
   i = hour ;
   lower  = i % 10;
   higher = ( ( i - (i%10) )/10 )<<4 ;
   i = lower|higher; 
   RTCPTR0 = 1;
   RTCPTR1 = 0;
   RTCVALL = i ;
   
   // writing min
   i = min ;
   lower  = i % 10;
   higher = ( ( i - (i%10) )/10 )<<4 ;
   i = lower|higher; 
   RTCPTR0 = 0;
   RTCPTR1 = 0;
   RTCVALH = i ;
   
   // writing sec
   i = sec ;
   lower  = i % 10;
   higher = ( ( i - (i%10) )/10 )<<4 ;
   i = lower|higher; 
   RTCPTR0 = 0;
   RTCPTR1 = 0;
   RTCVALL = i ;
   
   RTC_WRITE_ENABLE(0); 
   return;
}

// function for normal rtc reading_____________________________________________
void Print_RTC_1()
{
   rtc_time_t  read_clock; 
   rtc_read(&read_clock); 

      Fprintf(PC,"MainPIC Time >>> ");
      Fprintf(PC,"%02d", read_clock.tm_year );
      Fprintf(PC,"-%02d", read_clock.tm_mon );
      Fprintf(PC,"-%02d__", read_clock.tm_mday );
      Fprintf(PC,"%02d:", read_clock.tm_hour );
      Fprintf(PC,"%02d:", read_clock.tm_min ); 
      Fprintf(PC,"%02d", read_clock.tm_sec ); 
      
      Fprintf(PC,"\n\r");

}

// function for rtc reading frequently_________________________________________
unsigned int16 SECONDS_COUNTER = 0;


void READ_MP_RTC_AND_PRINT()
{
  if(MLC > 1000)
  {
      MLC = 0;
      rtc_time_t  read_clock; 
      rtc_read(&read_clock);
      MP_RTC_SEC   = read_clock.tm_sec  ;
     
      if(MP_RTC_SEC != sec_prev)
      {
         MP_RTC_YEAR  = read_clock.tm_year ;
         MP_RTC_MONTH = read_clock.tm_mon  ;
         MP_RTC_DAY   = read_clock.tm_mday ;
         MP_RTC_HOUR  = read_clock.tm_hour ;
         MP_RTC_MIN   = read_clock.tm_min  ;
      
         Fprintf(PC,"MP %02u",  MP_RTC_YEAR )  ;
         Fprintf(PC,"/%02u", MP_RTC_MONTH ) ;
         Fprintf(PC,"/%02u  ", MP_RTC_DAY ) ;
         Fprintf(PC,"%02u:", MP_RTC_HOUR )  ;
         Fprintf(PC,"%02u:", MP_RTC_MIN )   ; 
         Fprintf(PC,"%02u ", MP_RTC_SEC )   ; 
         
         Fprintf(PC,"%03u ", SEC90_Count )   ;
         Fprintf(PC,"%03Lu ", SECONDS_COUNTER )   ;
         
         Fprintf(PC,"%u-%03u",CFM_ACCESS_FLAG, CFM_ACCESS_TIME  );  
         Fprintf(PC,"\n\r")                 ;

         SEC90_Count ++;  
         if(CFM_ACCESS_TIME!=0) CFM_ACCESS_TIME--;
         
         if( SECONDS_COUNTER <= 300 ) SECONDS_COUNTER++;
         
         
      }
      
      if( _CW_STATUS_30MIN == Disabled )
      {
         if( SECONDS_COUNTER < 120 )
         {
            Output_Toggle(LED_PIN);
         }
      
         else
         {
            if( LED_FLAG == 0xAA)
            {
               Output_low(LED_PIN);
               LED_FLAG = 0xBB ;
            }
         }
      }
      sec_prev = MP_RTC_SEC ;    
  }  
}

unsigned int32 GET_DAY_NUMBER_OF_THE_YEAR(char year, char month, char day)
{
   unsigned int32 DAY_NUM;
   
   if( year%4 != 0 )
   {
      if( month == 1  ) DAY_NUM = day       ;
      if( month == 2  ) DAY_NUM = day + 31  ;
      if( month == 3  ) DAY_NUM = day + 59  ;
      if( month == 4  ) DAY_NUM = day + 90  ;
      if( month == 5  ) DAY_NUM = day + 120 ;
      if( month == 6  ) DAY_NUM = day + 151 ;
      if( month == 7  ) DAY_NUM = day + 181 ;
      if( month == 8  ) DAY_NUM = day + 212 ;
      if( month == 9  ) DAY_NUM = day + 243 ;
      if( month == 10 ) DAY_NUM = day + 273 ;
      if( month == 11 ) DAY_NUM = day + 304 ;
      if( month == 12 ) DAY_NUM = day + 334 ;
   }
   
   else
   {
      if( month == 1  ) DAY_NUM = day       ;
      if( month == 2  ) DAY_NUM = day + 31  ;
      if( month == 3  ) DAY_NUM = day + 60  ;
      if( month == 4  ) DAY_NUM = day + 91  ;
      if( month == 5  ) DAY_NUM = day + 121 ;
      if( month == 6  ) DAY_NUM = day + 152 ;
      if( month == 7  ) DAY_NUM = day + 182 ;
      if( month == 8  ) DAY_NUM = day + 213 ;
      if( month == 9  ) DAY_NUM = day + 244 ;
      if( month == 10 ) DAY_NUM = day + 274 ;
      if( month == 11 ) DAY_NUM = day + 305 ;
      if( month == 12 ) DAY_NUM = day + 335 ;
   }
   
   return DAY_NUM ;
}

void READ_MP_RTC_()
{
   rtc_time_t  read_clock; 
   rtc_read(&read_clock);
   MP_RTC_SEC   = read_clock.tm_sec  ;
  

   MP_RTC_YEAR  = read_clock.tm_year ;
   MP_RTC_MONTH = read_clock.tm_mon  ;
   MP_RTC_DAY   = read_clock.tm_mday ;
   MP_RTC_HOUR  = read_clock.tm_hour ;
   MP_RTC_MIN   = read_clock.tm_min  ;
}
