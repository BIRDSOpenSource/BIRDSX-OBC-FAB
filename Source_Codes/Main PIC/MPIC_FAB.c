// this functions will check com pic FAB port incoming
void CHECK_UART_INCOMING_FROM_FAB_PIC()
{
   if(FABPic_available())
   {
      Delay_ms(100);
      for( int i = 0; i<5; i++)
      {
         if( FABPIC_Read() == 0xFA )
         {
            FAB_TO_MPIC_ARRAY[0] = 0xFA;
            break;
         }
      }
      
      for (int i=1; i<=50; i++)
      {
         FAB_TO_MPIC_ARRAY[i] = FABPIC_Read();
      }
   }
}


// this functions will print received data from FAB pic
void PRINT_RECIVED_COMMAND_FROM_FAB_PIC()
{
   Fprintf(PC,"Received comand from FAB pic >> ");
   for(int i = 0; i<35; i++)
   {
      Fprintf(PC,"%X ",FAB_TO_MPIC_ARRAY[i]);
   }
   printline();
   printline();
}

void COMUNICATION_WITH_FAB_PIC_AND_WAIT_FOR_RESPONE(int numof_times, int16 wait_time = 200, int16 time_delay = 70, int inc_array_length = 3)
{
   printline();
   for(int j=0; j<numof_times; j++)
   {
      Fprintf(PC,"Num of comuniction tries to FAB PIC = %d\n\r", j+1)                ;
      FABPic_flush();
      for( int i = 0; i<3; i++)
      {
        fputc(MPIC_TO_FAB_ARRAY[i], FAB);
      }
      Delay_ms(wait_time);
      
      CHECK_UART_INCOMING_FROM_FAB_PIC() ;
      PRINT_RECIVED_COMMAND_FROM_FAB_PIC();
      
      if( FAB_TO_MPIC_ARRAY[0] == 0xFA && FAB_TO_MPIC_ARRAY[inc_array_length-1] == 0xFB )
      {
         Fprintf(PC,"FAB responded correctly\n\r");
         break;
      } 
      
      else
      {  
         Delay_ms(time_delay);
         Fprintf(PC,"Received wrong response from FAB pic >> ");
         for(int i = 0; i<inc_array_length; i++)
         {
            Fprintf(PC,"%X ",FAB_TO_MPIC_ARRAY[i]);
         }
         printline();  
      }
   }
}


// kill switch handling _______________________________________________________
// closing FAB kill switch
void _CLOSE_FAB_KILL_SWITCH()
{
   CLEAR_DATA_ARRAY(FAB_TO_MPIC_ARRAY, 10);
   MPIC_TO_FAB_ARRAY[0] = 0xFA;
   MPIC_TO_FAB_ARRAY[1] = 0xF1;
   MPIC_TO_FAB_ARRAY[2] = 0xFB;
   COMUNICATION_WITH_FAB_PIC_AND_WAIT_FOR_RESPONE( 3, 500,200);
   
   if( FAB_TO_MPIC_ARRAY[0] == 0xFA && FAB_TO_MPIC_ARRAY[1] == 0xF1 && FAB_TO_MPIC_ARRAY[2] == 0xFB )
   {
      Fprintf(PC,"FAB kill switch succeesfully closed\n\r");    
   }
   
   else Fprintf(PC,"FAB kill switch closing not succesful\n\r");
   
   //CLEAR_DATA_ARRAY(FAB_TO_MPIC_ARRAY, 10);
   printline();
}

// closing OBC kill switch
void _CLOSE_OBC_KILL_SWITCH()
{
   // high the obc kill switch pin here
   CLEAR_DATA_ARRAY(FAB_TO_MPIC_ARRAY, 10);
   MPIC_TO_FAB_ARRAY[0] = 0xFA;
   MPIC_TO_FAB_ARRAY[1] = 0xF2;
   MPIC_TO_FAB_ARRAY[2] = 0xFB;
   COMUNICATION_WITH_FAB_PIC_AND_WAIT_FOR_RESPONE( 3, 500,200);
   
   if( FAB_TO_MPIC_ARRAY[0] == 0xFA && FAB_TO_MPIC_ARRAY[1] == 0xF2 && FAB_TO_MPIC_ARRAY[2] == 0xFB )
   {
      Output_high(PIN_A4);
      Delay_ms(1000);
      Output_low(PIN_A4);
      Fprintf(PC,"OBC kill switch succeesfully closed\n\r");
   }
   
   else Fprintf(PC,"OBC kill switch closing not succesful\n\r");
   
   //CLEAR_DATA_ARRAY(FAB_TO_MPIC_ARRAY, 10);
   Output_low(PIN_A4);
   printline();
}

// Opening FAB kill switch
void _OPEN_FAB_KILL_SWITCH()
{
   CLEAR_DATA_ARRAY(FAB_TO_MPIC_ARRAY, 10);
   MPIC_TO_FAB_ARRAY[0] = 0xFA;
   MPIC_TO_FAB_ARRAY[1] = 0xF3;
   MPIC_TO_FAB_ARRAY[2] = 0xFB;
   COMUNICATION_WITH_FAB_PIC_AND_WAIT_FOR_RESPONE( 1, 200,200);
   
   if( FAB_TO_MPIC_ARRAY[0] == 0xFA && FAB_TO_MPIC_ARRAY[1] == 0xF3 && FAB_TO_MPIC_ARRAY[2] == 0xFB )
   {
      Fprintf(PC,"FAB kill open comand succesfully sent to FAB\n\r");
   }
   
   else Fprintf(PC,"Comunication with FAB not succesful\n\r"); 
   
   //CLEAR_DATA_ARRAY(FAB_TO_MPIC_ARRAY, 10);
   printline();
}

// Opening OBC kill switch
// Opening FAB kill switch
void _OPEN_OBC_KILL_SWITCH()
{
   CLEAR_DATA_ARRAY(FAB_TO_MPIC_ARRAY, 10);
   MPIC_TO_FAB_ARRAY[0] = 0xFA;
   MPIC_TO_FAB_ARRAY[1] = 0xF4;
   MPIC_TO_FAB_ARRAY[2] = 0xFB;
   COMUNICATION_WITH_FAB_PIC_AND_WAIT_FOR_RESPONE( 1, 200,200);
   
   if( FAB_TO_MPIC_ARRAY[0] == 0xFA && FAB_TO_MPIC_ARRAY[2] == 0xFB )
   {
      Fprintf(PC,"OBC kill open comand succesfully sent to FAB\n\r");
      if( FAB_TO_MPIC_ARRAY[1] == 0xF4 )
      {
         Output_high(PIN_A4);
         Delay_ms(1000);
         Output_low(PIN_A4);
         Fprintf(PC,"OBC kill is opened\n\r");
      }
   }
   
   else Fprintf(PC,"Comunication with FAB not succesful\n\r"); 
   
   //CLEAR_DATA_ARRAY(FAB_TO_MPIC_ARRAY, 10);
   Output_low(PIN_A4);
   printline();
}


