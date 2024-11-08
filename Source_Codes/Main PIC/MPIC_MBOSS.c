int board_number;                                                              // write in Settings.c later


unsigned int8 MissionBoss_flag = 0;
//unsigned int8 MBOSS_command = 0;

//_____________________________________________________________________________
void CHECK_UART_INCOMING_FROM_MBOSS_PIC(unsigned int32 looping = 100000)
{
   int MBC = 0;
   
   for(unsigned int32 i = 0; i<looping; i++)
   {
      if( kbhit(MBOSS) && MissionBoss_flag == 1 )
      {
        MBOSS_TO_MPIC_ARRAY[MBC] = fgetc(MBOSS);
        //fprintf(PC, "%X ", MBOSS_TO_MPIC_ARRAY[MBC]);
        
        MBC++;
        if( MBC >= 13 ) break ;
      }
   }
   fprintf(PC, "CHECK_UART_INCOMING_FROM_MBOSS_PIC\r\n");

   fprintf(PC, "Data from MBOSS:");
   
   for(int i = 0; i<13; i++)
   {
      fprintf(PC, "%X ", MBOSS_TO_MPIC_ARRAY[i]);
   }
   printline();
}

// this functions will print received data from Mission Boss
void PRINT_RECIVED_COMMAND_FROM_MISSION_BOSS()
{
   Fprintf(PC,"Received comand from Mission Boss >> ");
   for(int i = 0; i<40; i++)
   {
      Fprintf(PC,"%X ",MBOSS_TO_MPIC_ARRAY[i]);
   }
   printline();
   printline();
}


void ACK_APRS_COMMAND_TO_COM(unsigned int board_number, unsigned int mission_number)
{
   // Acknoleging to com pic
   CLEAR_DATA_ARRAY( MPIC_TO_CPIC_ARRAY, 32 );
   MPIC_TO_CPIC_ARRAY[0]  = 0xB0;   // header
   MPIC_TO_CPIC_ARRAY[1]  = board_number;   // board number
   MPIC_TO_CPIC_ARRAY[2]  = mission_number;   // mission number
   MPIC_TO_CPIC_ARRAY[31] = 0xB1;   // footer
   
   for(int i = 0; i<32; i++)
   {
      fputc(MPIC_TO_CPIC_ARRAY[i], CPic);
   }
}

int APRS_BOARD_IDENTIFY(unsigned int a)
{
   char c[3];                                                                 //Null is added at the end, so 3 characters are needed
   int bn; //board number
   sprintf(c,"%X",a);                                                         // Convert a 1-byte command to a string (null is added at the end, so 3 characters are needed)
   
   bn = c[1]-'0';                                                             //Convert strings to numbers(ex: "3" --> 3)
   
   if(bn <= 7)
   {
     printf("Board Number --> %X",bn);
   }
   else
   {
     printf("Not correct board number");
   }
     
   return bn;
}


void TURN_ON_MISSIONBOSS()
{
   output_high(PIN_D1);
   MissionBoss_flag = 1;
}


void TURN_OFF_MISSIONBOSS()
{
   output_low(PIN_D1);
   MissionBoss_flag = 0;
}


void GIVE_SFM_ACCESS_TO_MISSIONBOSS()
{
   Fprintf(PC,"GIVE_SFM_ACCESS_TO\MISSIONBOSS\n\r");
   
   output_high(PIN_A5);
}


void STOP_SFM_ACCESS_TO_MISSIONBOSS()
{
   Fprintf(PC,"STOP_SFM_ACCESS_TO\MISSIONBOSS\n\r");
   output_low(PIN_A5);
}

void RESET_ALL_APRSBOARD_NUMBER_FLAGS()
{
   if(MISSION_STATUS == 0)
   {
      APRS_REFERENSE_1_FLAG = 0;                                      // 0: turn OFF 1: turn ON
      APRS_REFERENSE_2_FLAG = 0;                                      // 0: turn OFF 1: turn ON
      APRS_PAYLOAD_1_FLAG = 0;                                        // 0: turn OFF 1: turn ON
      APRS_PAYLOAD_2_FLAG = 0;                                        // 0: turn OFF 1: turn ON
      APRS_PAYLOAD_3_FLAG = 0;                                        // 0: turn OFF 1: turn ON
      APRS_PAYLOAD_4_FLAG = 0;                                        // 0: turn OFF 1: turn ON
      APRS_PAYLOAD_5_FLAG = 0;                                        // 0: turn OFF 1: turn ON
   } 
}

void SEND_APRS_COMMAND_TO_MISSIONBOSS_THROUGH_MAIN()
{
   if( CPIC_TO_MPIC_ARRAY[1] == 0x00 && (CPIC_TO_MPIC_ARRAY[2]&0xF0) == 0xB0 )
   {
      //Acknowleding to Com PIC
      CLEAR_DATA_ARRAY( MPIC_TO_CPIC_ARRAY, 32 );
      MPIC_TO_CPIC_ARRAY[0]  = 0xB0;   // header
      MPIC_TO_CPIC_ARRAY[1]  = 0xB0;   // comand
      MPIC_TO_CPIC_ARRAY[31] = 0xB1;   // footer
      for(int i = 0; i<32; i++)
      {
         fputc(MPIC_TO_CPIC_ARRAY[i], CPic);
      }
      
     
      Fprintf(PC,"Sending APRS command to Mission Boss\n\r");
      TURN_ON_MISSIONBOSS();
      delay_ms(1000);
      CLEAR_DATA_ARRAY( MPIC_TO_MBOSS_ARRAY, 40 );      
      CLEAR_DATA_ARRAY( MBOSS_TO_MPIC_ARRAY, 40 );

      MPIC_TO_MBOSS_ARRAY[0] = CPIC_TO_MPIC_ARRAY[2];
      MPIC_TO_MBOSS_ARRAY[1] = CPIC_TO_MPIC_ARRAY[3];
      MPIC_TO_MBOSS_ARRAY[2] = 0xE0;

      
      for(int i = 3; i<10; i++)
      {
         MPIC_TO_MBOSS_ARRAY[i] = CPIC_TO_MPIC_ARRAY[i+1];
      }
      MPIC_TO_MBOSS_ARRAY[10] = 0xED;
      Fprintf(PC,"APRS command:");
      fputc(0xAA,MBOSS);                                                       // Header for Mission Boss
      for(int i = 0; i<11; i++)
      {
         fputc(MPIC_TO_MBOSS_ARRAY[i], MBOSS);
         Fprintf(PC,"%X ",MPIC_TO_MBOSS_ARRAY[i]);
      }
      if(MPIC_TO_MBOSS_ARRAY[3] == 0x11)                                       // GIVE ACESS IF DATA TRANSFER COMMAND IS RECEIVED
      {
         GIVE_SFM_ACCESS_TO_MISSIONBOSS();
         delay_ms(5000);
         STOP_SFM_ACCESS_TO_MISSIONBOSS();
      }
      CLEAR_DATA_ARRAY( MPIC_TO_MBOSS_ARRAY, 40 );                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
      printline();
      CHECK_UART_INCOMING_FROM_MBOSS_PIC(500000)  ;      
   }
}


