#define ON  1
#define OFF 0

#define BB_ON_OCP_ON   1
#define BB_ON_OCP_OFF  2
#define BB_OFF_OCP_OFF 3
// this variable will have the power line status as an example
// bit 7 of the variable will say whether main pic is on or off 1 = on, 0 = off
unsigned int8 POWER_LINE_STATUS = 0;
unsigned int8 RESET_TIME = 0;

// this will handle the Main pic compic buck boost converter___________________
void MP_CP_BuckBoost(int1 status)
{
   if( status == ON ) 
   { 
      output_LOW(PIN_C4); 
   }
   
   if( status == OFF ) 
   {
      output_HIGH(PIN_C4) ;
   }
   RST_EXT_WDT(); 
}

// this will handle main pic power_____________________________________________
void MainPic_Power(int1 status)
{
   if( status == 1 )
   {
      output_high(PIN_F5);
      BIT_SET(POWER_LINE_STATUS,7); 
   }
   if( status == 0 )
   {
      output_low(PIN_F5) ;
      BIT_CLEAR(POWER_LINE_STATUS,7);
   }
   delay_ms(50);
   RST_EXT_WDT(); 
}

// this will handle com pic power______________________________________________
void ComPic_Power(int1 status)
{
   if( status == 1 )
   {
      output_high(PIN_F6);
      BIT_SET(POWER_LINE_STATUS,6);
   }
   if( status == 0 )
   {
      output_low(PIN_F6) ;
      BIT_CLEAR(POWER_LINE_STATUS,6);
   }
   delay_ms(50);
   RST_EXT_WDT(); 
}

// controlling 3v3 1 power line________________________________________________
void _3V3Power_Line1(int1 status )
{
   if( status == BB_ON_OCP_ON )
   {
      output_high(PIN_D1); 
      Delay_ms(50);
      output_high(PIN_D4); 
      BIT_SET(POWER_LINE_STATUS,5);
   }
   
   if( status == BB_ON_OCP_OFF )
   {
      output_high(PIN_D1); 
      output_low(PIN_D4); 
      BIT_CLEAR(POWER_LINE_STATUS,5);
   }

   if( status == BB_OFF_OCP_OFF )
   {
      output_low(PIN_D1); 
      output_low(PIN_D4); 
      BIT_CLEAR(POWER_LINE_STATUS,5);
   }
   delay_ms(50);
   RST_EXT_WDT(); 
}

// controlling 3v3 1 power line________________________________________________
void _3V3Power_Line2(int1 status )
{
   if( status == BB_ON_OCP_ON )
   {
      output_high(PIN_D2);
      Delay_ms(50);
      output_high(PIN_D5); 
      BIT_SET(POWER_LINE_STATUS,4);
   }
   
   if( status == BB_ON_OCP_OFF )
   {
      output_high(PIN_D2); 
      output_low(PIN_D5); 
      BIT_CLEAR(POWER_LINE_STATUS,4);
   }

   if( status == BB_OFF_OCP_OFF )
   {
      output_low(PIN_D2); 
      output_low(PIN_D5); 
      BIT_CLEAR(POWER_LINE_STATUS,4);
   }
   delay_ms(50);
   RST_EXT_WDT(); 
}

// controlling 5V0 power line__________________________________________________
void _5V0Power_Line(int1 status )
{
   if( status == BB_ON_OCP_ON )
   {
      output_high(PIN_D3); 
      Delay_ms(50);
      output_high(PIN_D6); 
      BIT_SET(POWER_LINE_STATUS,3);
   }
   
   if( status == BB_ON_OCP_OFF )
   {
      output_high(PIN_D3); 
      output_low(PIN_D6); 
      BIT_CLEAR(POWER_LINE_STATUS,3);
   }

   if( status == BB_OFF_OCP_OFF )
   {
      output_low(PIN_D3); 
      output_low(PIN_D6); 
      BIT_CLEAR(POWER_LINE_STATUS,3);
   }
   delay_ms(50);
   RST_EXT_WDT(); 
}


void Unreg1_Line(int1 status)
{
   if( status == 1 )
   {
      output_high(PIN_D7) ;
      BIT_SET(POWER_LINE_STATUS,2);
   }
   if( status == 0 ) 
   {
      output_low(PIN_D7)  ;
      BIT_CLEAR(POWER_LINE_STATUS,2);
   }
   delay_ms(50);
   RST_EXT_WDT(); 
}

void Unreg2_Line(int1 status)
{
   if( status == 1 ) 
   {
      output_high(PIN_D0) ;
      BIT_SET(POWER_LINE_STATUS,1);
   }
   if( status == 0 ) 
   {
      output_low(PIN_D0)  ;
      BIT_CLEAR(POWER_LINE_STATUS,1);
   }
   delay_ms(50);
   RST_EXT_WDT(); 
}



// reset satellite by command from PC or GS
void SYSTEM_RESET()
{  
   int sec_c = 0;
   Fprintf(PC,"system reset by command \n\r");
   // turn off system
   MainPic_Power(OFF)                 ;    Delay_ms(250); 
   ComPic_Power(OFF)                  ;    Delay_ms(250);     
   _3V3Power_Line1(BB_OFF_OCP_OFF)    ;    Delay_ms(250);     
   _3V3Power_Line2(BB_OFF_OCP_OFF)    ;    Delay_ms(250);      
   _5V0Power_Line(BB_OFF_OCP_OFF)     ;    Delay_ms(250);     
   Unreg1_Line(OFF)                   ;    Delay_ms(250);     
   Unreg2_Line(OFF)                   ;    Delay_ms(250);   
   
   RESET_TIME = hour;
   
   for( int i = 0; i<10 ; i++)
   {
      Delay_ms(500);
      RST_EXT_WDT();
      Delay_ms(500);
      RST_EXT_WDT();
      sec_c++;
      Fprintf(PC,"Waiting to turn on system again %02d Sec\n\r",sec_c);
   }
   
   // turn on system
   MainPic_Power(ON)                ;    Delay_ms(250);      
   ComPic_Power(ON)                 ;    Delay_ms(250);      
   _3V3Power_Line1(BB_ON_OCP_ON)    ;    Delay_ms(250);     
   _3V3Power_Line2(BB_ON_OCP_ON)  ;    Delay_ms(250);       
   _5V0Power_Line(BB_ON_OCP_ON)   ;    Delay_ms(250);      
   Unreg1_Line(ON)                  ;    Delay_ms(250);     
   Unreg2_Line(OFF)                 ;    Delay_ms(250);  
}


// 24 hour system reset________________________________________________________
void SYSTEM_RESET_24H()
{   
   if( hour==0 )
   {
      if(minute==0)
      {
         if( ( 0 <= second ) && ( second <= 5 ) )
         {
            Fprintf(PC,"24 hour system reset\n\r");
            // turn off system
            MainPic_Power(OFF)                 ;    Delay_ms(250); 
            ComPic_Power(OFF)                  ;    Delay_ms(250);     
            _3V3Power_Line1(BB_OFF_OCP_OFF)    ;    Delay_ms(250);     
            _3V3Power_Line2(BB_OFF_OCP_OFF)    ;    Delay_ms(250);      
            _5V0Power_Line(BB_OFF_OCP_OFF)     ;    Delay_ms(250);     
            Unreg1_Line(OFF)                   ;    Delay_ms(250);     
            Unreg2_Line(OFF)                   ;    Delay_ms(250);   
            
            RESET_TIME = hour;
            
            for( int i = 0; i<10 ; i++)
            {
               Delay_ms(500);
               RST_EXT_WDT();
               Delay_ms(500);
               RST_EXT_WDT();
               Fprintf(PC,"Waiting to turn on system again %u Sec\n\r",i);
            }
            
            // turn on system
            MainPic_Power(ON)                ;    Delay_ms(250);     
            ComPic_Power(ON)                 ;    Delay_ms(250);      
            _3V3Power_Line1(BB_ON_OCP_ON)    ;    Delay_ms(250);      
            _3V3Power_Line2(BB_ON_OCP_ON)    ;    Delay_ms(250);       
            _5V0Power_Line(BB_ON_OCP_ON)     ;    Delay_ms(250);      
            Unreg1_Line(ON)                  ;    Delay_ms(250);   
            Unreg2_Line(OFF)                 ;    Delay_ms(250); 
         }
      }
   }
}


///////////////////////////////////////////////////////////////////////////////
/////////////////////////// ADC Related Function //////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// C3 = Raw power monitor enable
// A2 (AN2) = Raw voltage measure
// A1 (AN1) = 3V3-1 current measure
// A0 (AN0) = 3V3-2 current measure
// A5 (AN4) = 5V0 current measure
// A4 (AN6) = UNREG-1 current measure
// C2 (AN9) = UNREG-2 current measure

unsigned int16 _Raw_power_ADC_val       = 0 ;          
unsigned int16 _3V3_1_current_ADC_val   = 0 ;          
unsigned int16 _3V3_2_current_ADC_val   = 0 ;          
unsigned int16 _5V0_current_ADC_val     = 0 ;          
unsigned int16 _UNREG_1_current_ADC_val = 0 ;      
unsigned int16 _UNREG_2_current_ADC_val = 0 ;    


// below functions are current measuring functions_____________________________
unsigned int16 Measure_Raw_voltage() // A2 (AN2) = Raw voltage measure
{
   Output_low(PIN_C3);
   Delay_us(100);
   
   SET_ADC_CHANNEL(2); 
   delay_us(20);
   Output_high(PIN_C3);
   return READ_ADC(); 
}

unsigned int16 Measure_3V3_1_current() // A1 (AN1) = 3V3-1 current measure
{
   SET_ADC_CHANNEL(1); 
   delay_us(20);
   return READ_ADC(); 
}

unsigned int16 Measure_3V3_2_current() // A0 (AN0) = 3V3-2 current measure
{
   SET_ADC_CHANNEL(0); 
   delay_us(20);
   return READ_ADC(); 
}

unsigned int16 Measure_5V0_current()  // A5 (AN4) = 5V0 current measure
{
   SET_ADC_CHANNEL(4); 
   delay_us(20);
   return READ_ADC(); 
}

unsigned int16 Measure_UNREG_1_current() // A4 (AN6) = UNREG-1 current measure
{
   SET_ADC_CHANNEL(6); 
   delay_us(20);
   return READ_ADC(); 
}

unsigned int16 Measure_UNREG_2_current() // C2 (AN9) = UNREG-2 current measure
{
   SET_ADC_CHANNEL(9); 
   delay_us(20);
   return READ_ADC(); 
}


