

// this functions will check reset pic UART port incoming
void CHECK_UART_INCOMING_FROM_RESET_PIC()
{
   if( RPic_available() )
   {
      Delay_ms(200);
      for( int i = 0; i<10; i++ )
      {
         if( RPic_Read() == 0xC0 ) 
         {
            RPIC_TO_CPIC_ARRAY[0] = 0xC0;
            break;
         }
      }
      
      for(int i=1; i<=50; i++)
      {
         RPIC_TO_CPIC_ARRAY[i] = RPIC_Read();
      }
   }
}

// this functions will print received response prom reset pic
void PRINT_RECIVED_COMMAND_FROM_RESET_PIC()
{
   Fprintf(PC,"RP_R >> ");
   for(int i = 0; i<20; i++)
   {
      Fprintf(PC,"%X ",RPIC_TO_CPIC_ARRAY[i]);
   }
   printline();
   //printline();
}

void COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(int16 numof_times, int16 time_delay, int16 WT)
{
   printline();
   for(int j=0; j<numof_times; j++)
   {
      RPic_Flush();
      Fprintf(PC,"Num of try to reset pic = %d\n\r", j+1)                ;
      
      for( int i = 0; i<20; i++)
      {
        fputc(CPIC_TO_RPIC_ARRAY[i], RPic);
      }
      Delay_ms(WT);
      CHECK_UART_INCOMING_FROM_RESET_PIC() ;
      
      if( RPIC_TO_CPIC_ARRAY[0] == 0xC0 && RPIC_TO_CPIC_ARRAY[19] == 0xc1 )
      {
         Fprintf(PC,"Correct reply from reset pic\n\r");
         // it this is the case we have recived response from reset pic we can
         // stop tying
         break;
      }
      
      else
      {
         Fprintf(PC,"Wrong response from reset pic >> ");
         printline();
         Delay_ms(time_delay);
      }
   }
}

void SEND_CW_OLDTRX();
void SEND_CW_NEWTRX();

void COMUNICATE_WITH_RSTPIC_EVERY90SEC_AND_ASK_CW_DATA_FROM_MAINPIC(unsigned int16 time)
{
   if( MLC > time*100 )
   {      
      // Reset pic comunication part___________________________________________
      CLEAR_DATA_ARRAY(CPIC_TO_RPIC_ARRAY, 20);
      CLEAR_DATA_ARRAY(RPIC_TO_CPIC_ARRAY, 20);
      CPIC_TO_RPIC_ARRAY[0]  = 0xC0;
      CPIC_TO_RPIC_ARRAY[1]  = 0xE0;
      CPIC_TO_RPIC_ARRAY[19] = 0xC1;
      
      COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(2, 2000, 500);
      PRINT_RECIVED_COMMAND_FROM_RESET_PIC(); 
      
      // requesting and transmiting CW data or GMSK packet____________________________________
      CLEAR_DATA_ARRAY(CPIC_TO_MPIC_ARRAY, 40);
   
      CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;    // header
      CPIC_TO_MPIC_ARRAY[1]  = 0x00 ;    // 
      CPIC_TO_MPIC_ARRAY[2]  = 0xCA ;    // command
      CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;    // footer
      
      COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(2, 500,500);
     
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 && MPIC_TO_CPIC_ARRAY[1] == 0xCA && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         for(int i = 0; i<16; i++)
         {
            CW_DATA[i] = MPIC_TO_CPIC_ARRAY[i+2];
         }
         Fprintf(PC,"Received CW data ==> ");
         for(int i = 0; i<16; i++) fprintf(PC, "%X ", CW_DATA[i]);
         printline();
         
              if( CW_FLAG == NEWTRX_CW    ) { SEND_CW_NEWTRX()             ;  CW_INTERVAL = 150 ; }  // AA
         else if( CW_FLAG == OLDTRX_CW    ) { SEND_CW_OLDTRX()             ;  CW_INTERVAL = 125  ; }  // BB  
         else if( CW_FLAG == NEWTRX_1PCKT ) { NEW_TRX_SEND_1PCKT_BEACON()  ;  CW_INTERVAL = 100 ; }  // CC
         else if( CW_FLAG == OLDTRX_1PCKT ) { OLD_TRX_SEND_1PCKT_BEACON()  ;  CW_INTERVAL = 100; }  // DD
      }
       
      MLC = 0;
      CLEAR_DATA_ARRAY(MPIC_TO_CPIC_ARRAY, 32);
   }
}

void UPDATE_RESET_PIC_TIME_BY_COM_PIC(char yr, char mo, char dy, char hr, char mn, char sc)
{
   CLEAR_DATA_ARRAY(CPIC_TO_RPIC_ARRAY,20);
   
   CPIC_TO_RPIC_ARRAY[0]  = 0xC0;  // header
   CPIC_TO_RPIC_ARRAY[1]  = 0xEA;  // CI
   CPIC_TO_RPIC_ARRAY[2]  = yr  ; 
   CPIC_TO_RPIC_ARRAY[3]  = mo  ;
   CPIC_TO_RPIC_ARRAY[4]  = dy  ;
   CPIC_TO_RPIC_ARRAY[5]  = hr  ;
   CPIC_TO_RPIC_ARRAY[6]  = mn  ;
   CPIC_TO_RPIC_ARRAY[7]  = sc  ;
 
   CPIC_TO_RPIC_ARRAY[19] = 0xC1;  // footer
   
   COMUNICATION_WITH_RST_PIC_AND_WAIT_FOR_RESPONE(2, 2000, 400);
   PRINT_RECIVED_COMMAND_FROM_RESET_PIC();
}
