#define MBOSS_EN   PIN_D1


void COMUNICATION_WITH_FAB_PIC_AND_WAIT_FOR_RESPONE(int numof_times, int16 time_delay = 200, int16 wait_time = 70, int inc_array_length = 3);
void GIVE_COMFM_ACCESS_TO_COMPIC_FOR_DATA_DOWNLOAD();


// this variable count the number of main loop running
   unsigned int32 MLC = 0; 

// this array is used to save comands coming from debug port___________________
   char DEBUG_TO_MPIC_ARRAY[55];

// this arrays is used to save and send comands with reset pic_________________
   char MPIC_TO_RPIC_ARRAY[10];
   char RPIC_TO_MPIC_ARRAY[55];            // command is only 32 bytes

// this arrays is used to save and send comands with reset pic_________________
   char MPIC_TO_CPIC_ARRAY[32];
   char CPIC_TO_MPIC_ARRAY[55];            // command is only 32 bytes

// this arrays is used to save and send comands with fab pic_________________
   char MPIC_TO_FAB_ARRAY[32];
   char FAB_TO_MPIC_ARRAY[55];            // 
   
// this arrays is used to save and send comands with mboss pic_________________
   char MPIC_TO_MBOSS_ARRAY[40];          // only 20 is used
   char MBOSS_TO_MPIC_ARRAY[40];          
   
   
// This section has the variables related to antenna deployement_______________
   // this two variables will define whether antenna sets are in deployment mode
  
   // 1 = in deployment mode
   unsigned int16 ANT_SET_1_DEP_FLAG = 0;
   unsigned int16 ANT_SET_2_DEP_FLAG = 0;
   
   
   // this two variables will count how many times antenna depoloyment tried
   unsigned int16 ANTSET_1_COUNT = 0;
   unsigned int16 ANTSET_2_COUNT = 0;
   
// This section has the reset pic house keeping data writing variables
   unsigned int32 SUCCESFULL_COMUNICATION_WITH_RST_PIC = 0;             // this will count how many sccesful comunication happen with reset pic
   unsigned int32 FAILED_COMUNICATION_WITH_RST_PIC = 0;          // this will count how many not sccesful comunication happen with reset pic
   unsigned int32 DAY_NUMBER = 0;
   unsigned int32 RST_PIC_HSK_DATA_START_ADD = 0;     // this is the reset pic housekeeping data start address for a given date
 
   int  CFM_ACCESS_FLAG = 0 ;             // 0 means com flash memory acces to main pic , 1 means com pic
   char CFM_ACCESS_TIME = 0 ;             // value is in seconds, com fm acces time to com pic
   
// This section has the FAB pic house keeping data writing variables
   unsigned int32 SUCCESFULL_COMUNICATION_WITH_FAB = 0;      // this will count how many sccesful comunication happen with FAB pic
   unsigned int32 FAILED_COMUNICATION_WITH_FAB = 0;          // this will count how many not sccesful comunication happen with FAB pic
   unsigned int32 FAB_PIC_HSK_DATA_START_ADD = 0;            // this is the FAB pic housekeeping data start address for a given date

// this section has the variables related to main pic RTC______________________
   char MP_RTC_YEAR  = 0;
   char MP_RTC_MONTH = 0;
   char MP_RTC_DAY   = 0;
   char MP_RTC_HOUR  = 0;
   char MP_RTC_MIN   = 0;
   char MP_RTC_SEC   = 0;
   char SEC90_Count  = 70;
   char sec_prev;
   
// this section has a variable related to making CW
   unsigned int8 CW_DATA_ARRAY[30];
   
//Missioin flag
   unsigned int8 APRS_REFERENSE_1_FLAG = 0;                                      // 0: turn OFF 1: turn ON
   unsigned int8 APRS_REFERENSE_2_FLAG = 0;                                      // 0: turn OFF 1: turn ON
   unsigned int8 APRS_PAYLOAD_1_FLAG = 0;                                        // 0: turn OFF 1: turn ON
   unsigned int8 APRS_PAYLOAD_2_FLAG = 0;                                        // 0: turn OFF 1: turn ON
   unsigned int8 APRS_PAYLOAD_3_FLAG = 0;                                        // 0: turn OFF 1: turn ON
   unsigned int8 APRS_PAYLOAD_4_FLAG = 0;                                        // 0: turn OFF 1: turn ON
   unsigned int8 APRS_PAYLOAD_5_FLAG = 0;                                        // 0: turn OFF 1: turn ON
   unsigned int8 MISSION_STATUS = 0;                                             // 0: no mission 1: Digipeater 2:S&F ward 3: GT competition 4: Competitor's own mission   
 
   #define Enabled  0xAA
   #define Disabled 0xBB
   unsigned int8 _CW_STATUS_30MIN; 
   
   unsigned int8 CW30DAY_COUNTER = 0;
   unsigned int8 CW30DAY_FLAG = 1;

   #define LED_PIN  PIN_G2       // put correct pin(PIN_G2)
   char LED_FLAG = 0xAA ;
   
#use rs232(baud=9600,parity=N,xmit=pin_G0,rcv=pin_G1,bits=8,stream=MBOSS,ERRORS, force_sw)  //UART MAIN to MB DCM
//#use rs232(baud=9600, parity=N, rcv=PIN_B7,  bits=8, stream = MBOSS, errors, force_sw )  // dummy port

///////////////////////////////////////////////////////////////////////////////
#define PC_BFR_SIZE 50
// UART PC functions___________________________________________________________
#pin_select TX1=PIN_C6                                                         
#pin_select RX1=PIN_C7                                                         
#use rs232(baud=115200, parity=N, UART1, bits=8, stream=PC, errors)                      


unsigned int8  PC_Buffer[PC_BFR_SIZE];
unsigned int16 PC_Byte_Counter = 0;
unsigned int8  PC_Overflow = 0;
unsigned int16 PC_Read_Byte_counter = 0;
unsigned int8  PC_Temp_byte = 0;
#INT_RDA
Void SERIAL_ISR1()                                                             // MAIN PIC uart interupt loop
{
   if( kbhit(PC) )
   {
      if( PC_Byte_Counter < PC_BFR_SIZE )
      {
         PC_Buffer[PC_Byte_Counter] = fgetc(PC);
         PC_Byte_Counter++;
      }
      else PC_Overflow = fgetc(PC);
   }
}

unsigned int8 PC_Available()
{
   if (PC_Byte_Counter > 0) return PC_Byte_Counter ;
   else return 0;
}

unsigned int8 PC_Read()
{
   if (PC_Byte_Counter>0)
   {    
      PC_Temp_byte = PC_Buffer[PC_Read_Byte_counter];
      
      PC_Byte_Counter--;
      PC_Read_Byte_counter++;
      if(PC_Byte_Counter == 0) PC_Read_Byte_counter = 0;
      return PC_Temp_byte; 
   }
   
   if (PC_Byte_Counter == 0)
   { 
      PC_Read_Byte_counter = 0;
      PC_Temp_byte = 0x00;
      return PC_Temp_byte; 
   }
 
}

void PC_flush()
{
   while( PC_Available() ) PC_Read() ;
}
//_____________________________________________________________________________





///////////////////////////////////////////////////////////////////////////////
// Reset pic comunication______________________________________________________
///////////////////////////////////////////////////////////////////////////////
#define RP_BFR_SIZE 50
#pin_select TX4=PIN_E3  //D5 is dummy port
#pin_select RX4=PIN_F2  //
#use rs232(baud=38400,parity=N,UART4,bits=8,stream=RPic,ERRORS)                  

unsigned int8  RP_Buffer[RP_BFR_SIZE];
unsigned int16 RP_Byte_Counter = 0;
unsigned int8  RP_Overflow = 0;
unsigned int16 RP_Read_Byte_counter = 0;
unsigned int8  RP_Temp_byte = 0;

#INT_RDA4
Void SERIAL_ISR4()                                                             
{
   if( kbhit(RPic) )
   {
      if( RP_Byte_Counter < RP_BFR_SIZE )
      {
         RP_Buffer[RP_Byte_Counter] = fgetc(RPic);
         RP_Byte_Counter++;
      }
      else RP_Overflow = fgetc(RPic);
   }
}

unsigned int8 RPic_Available()
{
   //if (RP_Byte_Counter > 0) 
   return RP_Byte_Counter ;
   //else return 0;
}

unsigned int8 RPic_Read()
{
   if (RP_Byte_Counter>0)
   {    
      RP_Temp_byte = RP_Buffer[RP_Read_Byte_counter];
      
      RP_Byte_Counter--;
      RP_Read_Byte_counter++;
      if(RP_Byte_Counter == 0) RP_Read_Byte_counter = 0;
      return RP_Temp_byte; 
   }
   
   if (RP_Byte_Counter == 0)
   { 
      RP_Read_Byte_counter = 0;
      RP_Temp_byte = 0x00;
      return RP_Temp_byte; 
   }
 
}

void RPic_flush()
{
   while( Rpic_Available() ) Rpic_Read() ;
}
//_____________________________________________________________________________



///////////////////////////////////////////////////////////////////////////////
// COM pic comunication______________________________________________________
///////////////////////////////////////////////////////////////////////////////
#define CP_BFR_SIZE 50
#pin_select TX2=PIN_D3
#pin_select RX2=PIN_D2
#use rs232(baud=38400,parity=N,UART2,bits=8,stream=cpic,ERRORS)                         // MAIN to Ext +/- 6v


unsigned int8  CP_Buffer[CP_BFR_SIZE];
unsigned int16 CP_Byte_Counter = 0;
unsigned int8  CP_Overflow = 0;
unsigned int16 CP_Read_Byte_counter = 0;
unsigned int8  CP_Temp_byte = 0;
#INT_RDA2
Void SERIAL_ISR2()                                                             // MAIN PIC uart interupt loop
{
   if( kbhit(CPic) )
   {
      if( CP_Byte_Counter < CP_BFR_SIZE )
      {
         CP_Buffer[CP_Byte_Counter] = fgetc(CPic);
         CP_Byte_Counter++;
      }
      else CP_Overflow = fgetc(CPic);
   }
}

unsigned int8 CPic_Available()
{
   if (CP_Byte_Counter > 0) return CP_Byte_Counter ;
   else return 0;
}

unsigned int8 CPic_Read()
{
   if (CP_Byte_Counter>0)
   {    
      CP_Temp_byte = CP_Buffer[CP_Read_Byte_counter];
      
      CP_Byte_Counter--;
      CP_Read_Byte_counter++;
      if(CP_Byte_Counter == 0) CP_Read_Byte_counter = 0;
      return CP_Temp_byte; 
   }
   
   if (CP_Byte_Counter == 0)
   { 
      CP_Read_Byte_counter = 0;
      CP_Temp_byte = 0x00;
      return CP_Temp_byte; 
   }
 
}

void CPic_flush()
{
   while( Cpic_Available() ) Cpic_Read() ;
}



///////////////////////////////////////////////////////////////////////////////
// FAB pic comunication______________________________________________________
///////////////////////////////////////////////////////////////////////////////
#define FP_BFR_SIZE 50
#pin_select TX3=PIN_E5
#pin_select RX3=PIN_E4
#use rs232(baud=38400,parity=N,UART3,bits=8,stream=FAB,ERRORS)                  //UART MAIN to FAB PIC

unsigned int8  FP_Buffer[FP_BFR_SIZE];
unsigned int16 FP_Byte_Counter = 0;
unsigned int8  FP_Overflow = 0;
unsigned int16 FP_Read_Byte_counter = 0;
unsigned int8  FP_Temp_byte = 0;
#INT_RDA3
Void SERIAL_ISR3()                                                             // MAIN PIC uart interupt loop
{
   if( kbhit(FAB) )
   {
      if( FP_Byte_Counter < FP_BFR_SIZE )
      {
         FP_Buffer[FP_Byte_Counter] = fgetc(FAB);
         FP_Byte_Counter++;
      }
      else FP_Overflow = fgetc(FAB);
   }
}

unsigned int8 FABPic_Available()
{
   if (FP_Byte_Counter > 0) return FP_Byte_Counter ;
   else return 0;
}

unsigned int8 FABPic_Read()
{
   if (FP_Byte_Counter>0)
   {    
      FP_Temp_byte = FP_Buffer[FP_Read_Byte_counter];
      
      FP_Byte_Counter--;
      FP_Read_Byte_counter++;
      if(FP_Byte_Counter == 0) FP_Read_Byte_counter = 0;
      return FP_Temp_byte; 
   }
   
   if (FP_Byte_Counter == 0)
   { 
      FP_Read_Byte_counter = 0;
      FP_Temp_byte = 0x00;
      return FP_Temp_byte; 
   }
 
}

void FABPic_flush()
{
   while( FABpic_Available() ) FABpic_Read() ;
}
//_____________________________________________________________________________



// this function will just print a line in debug port__________________________
void printline()
{
   fprintf( PC, "\n\r");
}

void printtable(unsigned int8 table[], int table_size, int column_size)
{
   for(int i = 0; i < table_size; i++) 
   {
      fprintf( PC, "%x ", table[0]);
      if ((i+1) %column_size  == 0 ) 
      {
         printline();
      }
   }
}

// using this function we can make any data array clear________________________
void CLEAR_DATA_ARRAY(unsigned int8 array[], int array_size)
{   
   for(int i = 0; i < array_size; i++)
   {
      array[i] = 0 ;
   }
}

//
