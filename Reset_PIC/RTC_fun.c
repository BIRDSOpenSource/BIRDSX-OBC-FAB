// this section handle the RTC related functions_______________________________

// this function set the RTC time______________________________________________
void Set_RTC( char y, char mo, char d, char h, char mi, char s )
{
   year   = y  ;
   month  = mo ;
   day    = d  ;
   hour   = h  ;
   minute = mi ;
   second = s  ;
}

int previous_second ;

// this function will update the time when timer 1 interupt happens every 1 second
Void RTC_Function()
{
   if (second < 59)                                                            // updating seconds
   {
      second++;
   }
      
   else
   {
      second = 0;
      minute++;
   }
      
   if (minute >= 60)                                                           // updating minutes
   {
      minute = 0;
      hour++;
      LAST_RESET_HOUR ++ ;
   }
      
   if (hour >= 24)                                                             // updating day
   {
      hour = 0;
      day++;
   }
   
   if( (day >= 31) && (month == 4 || month == 6 || month == 9 || month == 11) )     // 30 days months
   {
      day = 1;
      month++;
   }
   
   if( (day >= 32) && (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10) )  // 31 days months
   {
      day = 1;
      month++;
   }
   
   if(year%4 == 0)
   {
      if( (day >= 30) && (month == 2) )                                           // february
      {
         day = 1;
         month++;
      } 
   }
   
   else
   {
      if( (day >= 29) && (month == 2) )                                           // february
      {
         day = 1;
         month++;
      } 
   }
   
   
   if( (day >= 32) && (month == 12) )                                          //december
   {
      day = 1;
      month = 1;
      year++;
   }
      
}

void PRINT_POWER_LINE_STATUS()
{
   fprintf(PC,"M=%d,", (POWER_LINE_STATUS>>7) & 0x01 ); 
   fprintf(PC,"C=%d,", (POWER_LINE_STATUS>>6) & 0x01 );
   fprintf(PC,"3=%d,", (POWER_LINE_STATUS>>5) & 0x01 );
   fprintf(PC,"3=%d,", (POWER_LINE_STATUS>>4) & 0x01 );
   fprintf(PC,"5=%d,", (POWER_LINE_STATUS>>3) & 0x01 );
   fprintf(PC,"U=%d,", (POWER_LINE_STATUS>>2) & 0x01 );
   fprintf(PC,"U=%d" , (POWER_LINE_STATUS>>1) & 0x01 );
   printline();
}

// this function will print the Reset pic time in every 1 second
void Print_RTC()
{
   if(previous_second != second )
   {
      fprintf(PC,"RP ");
      fprintf(PC,"%02d", year);
      fprintf(PC,"/%02d", month);
      fprintf(PC,"/%02d", day);
      fprintf(PC," %02d", hour);
      fprintf(PC,":%02d", minute);
      fprintf(PC,":%02d ", second);                                         // 20-01-01__05:20:22
      
      fprintf(PC,"%d>",NUMOF_MPIC_RST);
      fprintf(PC,"%03Ld", MPIC_TIME_COUNTER);
      fprintf(PC," %d>",NUMOF_CPIC_RST);
      fprintf(PC,"%03Ld ", CPIC_TIME_COUNTER);
      
      PRINT_POWER_LINE_STATUS();
   } 
   previous_second = second ;
}
