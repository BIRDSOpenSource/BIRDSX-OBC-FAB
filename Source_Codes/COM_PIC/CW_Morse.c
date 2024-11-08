
#define CW_PIN PIN_D1
// T = 60ms 20wpm

//___character_space_function___///////////////////////////////////
void CH_SPACE()
{
   Output_low(CW_PIN);
   delay_ms(180);  
}

//___Word_space_function___////////////////////////////////////////
void WORD_SPACE()
{
   Output_low(CW_PIN); ;
   delay_ms(420); 
}

//___dit_function___///////////////////////////////////////////////
void DIT()
{
   Output_high(CW_PIN);
   delay_ms(60);
   Output_low(CW_PIN);
   delay_ms(60);
}

//___dit_function___///////////////////////////////////////////////
void DAH()
{
   Output_high(CW_PIN);;
   delay_ms(180);
   Output_low(CW_PIN);;
   delay_ms(60);
}

void CW_A() { DIT(); DAH();               CH_SPACE(); }
void CW_B() { DAH(); DIT(); DIT(); DIT(); CH_SPACE(); }
void CW_C() { DAH(); DIT(); DAH(); DIT(); CH_SPACE(); }
void CW_D() { DAH(); DIT(); DIT();        CH_SPACE(); }
void CW_E() { DIT();                      CH_SPACE(); }
void CW_F() { DIT(); DIT(); DAH(); DIT(); CH_SPACE(); }
void CW_G() { DAH(); DAH(); DIT();        CH_SPACE(); }
void CW_H() { DIT(); DIT(); DIT(); DIT(); CH_SPACE(); }
void CW_I() { DIT(); DIT();               CH_SPACE(); }
void CW_J() { DIT(); DAH(); DAH(); DAH(); CH_SPACE(); }
void CW_K() { DAH(); DIT(); DAH();        CH_SPACE(); }
void CW_L() { DIT(); DAH(); DIT(); DIT(); CH_SPACE(); }
void CW_M() { DAH(); DAH();               CH_SPACE(); }
void CW_N() { DAH(); DIT();               CH_SPACE(); }
void CW_O() { DAH(); DAH(); DAH();        CH_SPACE(); }
void CW_P() { DIT(); DAH(); DAH(); DIT(); CH_SPACE(); }
void CW_Q() { DAH(); DAH(); DIT(); DAH(); CH_SPACE(); }
void CW_R() { DIT(); DAH(); DIT();        CH_SPACE(); }
void CW_S() { DIT(); DIT(); DIT();        CH_SPACE(); }
void CW_T() { DAH();                      CH_SPACE(); }
void CW_U() { DIT(); DIT(); DAH();        CH_SPACE(); }
void CW_V() { DIT(); DIT(); DIT(); DAH(); CH_SPACE(); }
void CW_W() { DIT(); DAH(); DAH();        CH_SPACE(); }
void CW_X() { DAH(); DIT(); DIT(); DAH(); CH_SPACE(); }
void CW_Y() { DAH(); DIT(); DAH(); DAH(); CH_SPACE(); }
void CW_Z() { DAH(); DAH(); DIT(); DIT(); CH_SPACE(); }

// Numbers morse code  
void CW_0() { DAH(); DAH(); DAH(); DAH(); DAH(); CH_SPACE(); }
void CW_1() { DIT(); DAH(); DAH(); DAH(); DAH(); CH_SPACE(); }
void CW_2() { DIT(); DIT(); DAH(); DAH(); DAH(); CH_SPACE(); }
void CW_3() { DIT(); DIT(); DIT(); DAH(); DAH(); CH_SPACE(); }
void CW_4() { DIT(); DIT(); DIT(); DIT(); DAH(); CH_SPACE(); }
void CW_5() { DIT(); DIT(); DIT(); DIT(); DIT(); CH_SPACE(); }
void CW_6() { DAH(); DIT(); DIT(); DIT(); DIT(); CH_SPACE(); }
void CW_7() { DAH(); DAH(); DIT(); DIT(); DIT(); CH_SPACE(); }
void CW_8() { DAH(); DAH(); DAH(); DIT(); DIT(); CH_SPACE(); }
void CW_9() { DAH(); DAH(); DAH(); DAH(); DIT(); CH_SPACE(); }

void CW_LTR(char CWL)
{
   //Hexa decimal values
        if(CWL == 0x00||CWL ==0x30) CW_0();
   else if(CWL == 0x01||CWL ==0x31) CW_1();
   else if(CWL == 0x02||CWL ==0x32) CW_2();
   else if(CWL == 0x03||CWL ==0x33) CW_3();
   else if(CWL == 0x04||CWL ==0x34) CW_4();
   else if(CWL == 0x05||CWL ==0x35) CW_5();
   else if(CWL == 0x06||CWL ==0x36) CW_6();
   else if(CWL == 0x07||CWL ==0x37) CW_7();
   else if(CWL == 0x08||CWL ==0x38) CW_8();
   else if(CWL == 0x09||CWL ==0x39) CW_9();
   else if(CWL == 0x0A||CWL ==0x61||CWL ==0x41) CW_A();
   else if(CWL == 0x0B||CWL ==0x62||CWL ==0x42) CW_B();
   else if(CWL == 0x0C||CWL ==0x63||CWL ==0x43) CW_C();
   else if(CWL == 0x0D||CWL ==0x64||CWL ==0x44) CW_D();
   else if(CWL == 0x0E||CWL ==0x65||CWL ==0x45) CW_E();
   else if(CWL == 0x0F||CWL ==0x66||CWL ==0x46) CW_F();
   //-------------------------
   
   //normal ascii for CW mission
   else if(CWL == 0x67||CWL ==0x47) CW_G();
   else if(CWL == 0x68||CWL ==0x48) CW_H();
   else if(CWL == 0x69||CWL ==0x49) CW_I();
   else if(CWL == 0x6a||CWL ==0x4a) CW_J();
   else if(CWL == 0x6b||CWL ==0x4b) CW_K();
   else if(CWL == 0x6c||CWL ==0x4c) CW_L();
   else if(CWL == 0x6d||CWL ==0x4d) CW_M();
   else if(CWL == 0x6e||CWL ==0x4e) CW_N();
   else if(CWL == 0x6f||CWL ==0x4f) CW_O();
   else if(CWL == 0x70||CWL ==0x50) CW_P();
   else if(CWL == 0x71||CWL ==0x51) CW_Q();
   else if(CWL == 0x72||CWL ==0x52) CW_R();
   else if(CWL == 0x73||CWL ==0x53) CW_S();
   else if(CWL == 0x74||CWL ==0x54) CW_T();
   else if(CWL == 0x75||CWL ==0x55) CW_U();
   else if(CWL == 0x76||CWL ==0x56) CW_V();
   else if(CWL == 0x77||CWL ==0x57) CW_W();
   else if(CWL == 0x78||CWL ==0x58) CW_X();
   else if(CWL == 0x79||CWL ==0x59) CW_Y();
   else if(CWL == 0x7a||CWL ==0x6a) CW_Z();
}

//char ACK[3];

void SEND_CW_NEWTRX()
{
   CLEAR_DATA_ARRAY(CW_DATA_TO_NEWTRX,53);
   Fprintf(PC,"Transmismiting CW using NEW TRX\n\r");  
   
   
   CW_DATA_TO_NEWTRX[0]  = 0x7E;
   
   if( CW_FLAG == NEWTRX_CW ) CW_DATA_TO_NEWTRX[1]  = 0xAA ;
   else                       CW_DATA_TO_NEWTRX[1]  = 0x00 ;
   
   CW_DATA_TO_NEWTRX[2]  = 0x1E;    // 0x1E = 30 = 6 letters of sat name + word space + 6 leters of call sign + word space + 16 letters HSK 
   
   // BIRDSx = 62 69 72 64 73 78
   CW_DATA_TO_NEWTRX[3]  = 0x62;    // B
   CW_DATA_TO_NEWTRX[4]  = 0x69;    // I
   CW_DATA_TO_NEWTRX[5]  = 0x72;    // R
   CW_DATA_TO_NEWTRX[6]  = 0x64;    // D
   CW_DATA_TO_NEWTRX[7]  = 0x73;    // S
   CW_DATA_TO_NEWTRX[8]  = 0x78;    // X
   
   CW_DATA_TO_NEWTRX[9]  = 0xAA;    // word space
   
   // JG6YOW = 6A 67 36 79 6F 77
   CW_DATA_TO_NEWTRX[10] = 0x6A ;
   CW_DATA_TO_NEWTRX[11] = 0x67 ;
   CW_DATA_TO_NEWTRX[12] = 0x36 ;
   CW_DATA_TO_NEWTRX[13] = 0x79 ;
   CW_DATA_TO_NEWTRX[14] = 0x6F ;
   CW_DATA_TO_NEWTRX[15] = 0x77 ;
   
   CW_DATA_TO_NEWTRX[16] = 0xAA; // word space
   
   // CW data
   for(int i = 0; i<16; i++)
   {
      CW_DATA_TO_NEWTRX[17+i] = CW_DATA[i];
   }
   
   CW_DATA_TO_NEWTRX[50]  = 0x7E;
   
   // sending data to tranceiver
   for(int i = 0; i<51; i++)
   {
      fputc(CW_DATA_TO_NEWTRX[i],NEW_TRX);
   }
   
   Fprintf(PC,"Sent CW data to New TRX = ");
   for(int i = 0; i<51; i++)
   {
      Fprintf(PC,"%X",CW_DATA_TO_NEWTRX[i]);
   }
   Fprintf(PC,"\n\r");
   Delay_ms(100);  
}

void SEND_CW_OLDTRX()
{
   Fprintf(PC,"Transmismiting CW using OLD TRX\n\r");  
   
   OLD_TRX_CW_MODE();
   
   CW_LTR("B"); 
   CW_LTR("I"); 
   CW_LTR("R"); 
   CW_LTR("D"); 
   CW_LTR("S"); 
   CW_LTR("X");
   
   WORD_SPACE(); 
   
   CW_LTR(ST_callsign_leter_1);
   CW_LTR(ST_callsign_leter_2);
   CW_LTR(ST_callsign_leter_3);
   CW_LTR(ST_callsign_leter_4);
   CW_LTR(ST_callsign_leter_5);
   CW_LTR(ST_callsign_leter_6); 
   
   WORD_SPACE();
   Fprintf(PC,"CW :");  
   for(int i = 0; i<16; i++)
   {
      CW_LTR(CW_DATA[i]);
      Fprintf(PC,"%X ", CW_DATA[i]);  
   }
   Fprintf(PC,"\n\r");
   OLD_TRX_RX_MODE();
}


   





