// this functions will check reset pic UART port incoming
void CHECK_UART_INCOMING_FROM_MAIN_PIC()
{
   if( MPic_available() )
   {
      Delay_ms(200);
      for( int i = 0; i<5; i++ )
      {
         if( MPic_Read() == 0xB0 ) 
         {
            MPIC_TO_CPIC_ARRAY[0] = 0xB0;
            break;
         }
      }
      
      for(int i=1; i<=50; i++)
      {
         MPIC_TO_CPIC_ARRAY[i] = MPIC_Read();         
      }
   }
}

// this functions will print received response prom reset pic
void PRINT_RECIVED_COMMAND_FROM_MAIN_PIC()
{
   Fprintf(PC,"MP_R >> ");
   for(int i = 0; i<32; i++)
   {
      Fprintf(PC,"%X ",MPIC_TO_CPIC_ARRAY[i]);
   }
   printline();
   //printline();
}

void COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(int16 numof_times, int16 time_delay, int16 wait_time = 200)
{
   printline();
   for(int j=0; j<numof_times; j++)
   {
      Fprintf(PC,"Num of try to Main pic = %d\n\r", j+1)                ;
      
      for( int i = 0; i<41; i++)
      {
        fputc(CPIC_TO_MPIC_ARRAY[i], MPic);
      }
      Delay_ms(wait_time);
      CHECK_UART_INCOMING_FROM_MAIN_PIC() ;
      
      if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
      {
         Fprintf(PC,"Correct response from main pic\n\r");
         // it this is the case we have recived response from reset pic we can
         // stop tying
         break;
      }
      
      else
      {
         Fprintf(PC,"Wrong response from main pic");
         printline(); 
         Delay_ms(time_delay);
      }
   }
}


// This function is used get shared flash memory acces for given time from main pic______________
Void GET_FM_ACCESS_FROM_MAIN_PIC(unsigned int8 sec_)
{
   CLEAR_DATA_ARRAY(CPIC_TO_MPIC_ARRAY, 40);
   
   CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;    // header
   CPIC_TO_MPIC_ARRAY[1]  = 0x00 ;    // 
   CPIC_TO_MPIC_ARRAY[2]  = 0xFA ;    // command
   CPIC_TO_MPIC_ARRAY[3]  = sec_ ;    // acces time
   CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;    // footer
   
   COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(2, 500,200);
}


//!Void REQUEST_CW_DATA_FROM_MAIN_PIC()
//!{
//!   CLEAR_DATA_ARRAY(CPIC_TO_MPIC_ARRAY, 40);
//!   
//!   CPIC_TO_MPIC_ARRAY[0]  = 0xB0 ;    // header
//!   CPIC_TO_MPIC_ARRAY[1]  = 0x00 ;    // 
//!   CPIC_TO_MPIC_ARRAY[2]  = 0xCA ;    // command
//!   CPIC_TO_MPIC_ARRAY[39] = 0xB1 ;    // footer
//!   
//!   COMUNICATION_WITH_MAIN_PIC_AND_WAIT_FOR_RESPONE(1, 500,200);
//!  
//!   if( MPIC_TO_CPIC_ARRAY[0] == 0xB0 && MPIC_TO_CPIC_ARRAY[1] == 0xCA && MPIC_TO_CPIC_ARRAY[31] == 0xB1 )
//!   {
//!      for(int i = 0; i<16; i++)
//!      {
//!         CW_DATA[i] = MPIC_TO_CPIC_ARRAY[i+2];
//!      }
//!      Fprintf(PC,"Received CW data ==> ");
//!      for(int i = 0; i<16; i++) fprintf(PC, "%X ", CW_DATA[i]);
//!      printline();
//!   }
//!   
//!   else Fprintf(PC,"CW data not received from Main pic\n\r");
//!
//!}

