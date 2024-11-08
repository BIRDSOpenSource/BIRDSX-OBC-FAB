//commands
#define READ_ID           0x9F
#define READ_STATUS_REG   0x05 

#use spi(MASTER, CLK=PIN_E1, DI=PIN_E0, DO=PIN_E6,  BAUD=1000000, BITS=8, STREAM=MAIN_FM, MODE=0)        //MAIN flash memory port
#use spi(MASTER, CLK=PIN_B2, DI=PIN_B5, DO=PIN_B4,  BAUD=1000000, BITS=8, STREAM=COM_FM, MODE=0)         //COM flash memory port
#use spi(MASTER, CLK=PIN_A3, DI=PIN_A0, DO=PIN_A1,  BAUD=1000000, BITS=8, STREAM=MSN_FM, MODE=0) 

//______________________Write Enable Codes____________________________________________________\\
void MAIN_FM_WRITE_ENABLE()
{
  Output_low(Pin_E2);
  spi_xfer(MAIN_FM,0x06);                
  Output_high(Pin_E2);
  return;
}

void COM_FM_WRITE_ENABLE()
{
  Output_low(Pin_B3);
  spi_xfer(COM_FM,0x06);                
  Output_high(Pin_B3);
  return;
}

void MSN_FM_WRITE_ENABLE()
{

  Output_low(Pin_A2);
  spi_xfer(MSN_FM,0x06);                
  Output_high(Pin_A2);
  return;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------\\
unsigned int8 adsress[4];
unsigned int8 _data;

//______________________4KB,32KB,64KB Sector Erase____________________________________________________\\
void MAIN_FM_SECTOR_ERASE(unsigned int32 sector_address,char sector_size,  int16 delay = 1000)
{
   
   adsress[0]  = (unsigned int8)((sector_address>>24) & 0xFF);   // 0x __ 00 00 00
   adsress[1]  = (unsigned int8)((sector_address>>16) & 0xFF);   // 0x 00 __ 00 00
   adsress[2]  = (unsigned int8)((sector_address>>8)  & 0xFF);   // 0x 00 00 __ 00
   adsress[3]  = (unsigned int8)((sector_address)     & 0xFF);   // 0x 00 00 00 __
   
   MAIN_FM_WRITE_ENABLE();
   Output_low(Pin_E2);             //lower the CS PIN

   ///////////////////////////////////////////////////////////////////

   if( Sector_size == 4  ) spi_xfer(MAIN_FM,0x21);                    // 4KB Sector erase
   if( Sector_size == 32 ) spi_xfer(MAIN_FM,0x5C);                    // 32KB Sector erase
   if( Sector_size == 64 ) spi_xfer(MAIN_FM,0xDC);                    // 64KB Sector erase
   
   spi_xfer(MAIN_FM,adsress[0]);   
   spi_xfer(MAIN_FM,adsress[1]);    
   spi_xfer(MAIN_FM,adsress[2]);    
   spi_xfer(MAIN_FM,adsress[3]);
   //////////////////////////////////////////////////////////////////
   Output_high(Pin_E2);           //take CS PIN higher back

   delay_ms(delay); 
   return;
}

void COM_FM_SECTOR_ERASE(unsigned int32 sector_address, char sector_size, unsigned int16 delay = 1000)
{
   
   adsress[0]  = (unsigned int8)((sector_address>>24) & 0xFF);   // 0x __ 00 00 00
   adsress[1]  = (unsigned int8)((sector_address>>16) & 0xFF);   // 0x 00 __ 00 00
   adsress[2]  = (unsigned int8)((sector_address>>8)  & 0xFF);   // 0x 00 00 __ 00
   adsress[3]  = (unsigned int8)((sector_address)     & 0xFF);   // 0x 00 00 00 __
   
   COM_FM_WRITE_ENABLE();
   Output_low(Pin_B3);             //lower the CS PIN
   ///////////////////////////////////////////////////////////////////
   if( Sector_size == 4  ) spi_xfer(COM_FM,0x21);                    // 4KB Sector erase
   if( Sector_size == 32 ) spi_xfer(COM_FM,0x5C);                    // 32KB Sector erase
   if( Sector_size == 64 ) spi_xfer(COM_FM,0xDC);                    // 64KB Sector erase
      
   spi_xfer(COM_FM,adsress[0]);   
   spi_xfer(COM_FM,adsress[1]);    
   spi_xfer(COM_FM,adsress[2]);    
   spi_xfer(COM_FM,adsress[3]);
   //////////////////////////////////////////////////////////////////
   Output_high(Pin_B3);           //take CS PIN higher back
   delay_ms(delay); 
   return;
}

void MSN_FM_SECTOR_ERASE(unsigned int32 sector_address,char sector_size, unsigned int16 delay = 1000)
{
   
   adsress[0]  = (unsigned int8)((sector_address>>24) & 0xFF);   // 0x __ 00 00 00
   adsress[1]  = (unsigned int8)((sector_address>>16) & 0xFF);   // 0x 00 __ 00 00
   adsress[2]  = (unsigned int8)((sector_address>>8)  & 0xFF);   // 0x 00 00 __ 00
   adsress[3]  = (unsigned int8)((sector_address)     & 0xFF);   // 0x 00 00 00 __
   
   MSN_FM_WRITE_ENABLE();
   Output_low(Pin_A2);             //lower the CS PIN
   ///////////////////////////////////////////////////////////////////
   if( Sector_size == 4  ) spi_xfer(MSN_FM,0x21);                    // 4KB Sector erase
   if( Sector_size == 32 ) spi_xfer(MSN_FM,0x5C);                    // 32KB Sector erase
   if( Sector_size == 64 ) spi_xfer(MSN_FM,0xDC);                    // 64KB Sector erase
   
   spi_xfer(MSN_FM,adsress[0]);   
   spi_xfer(MSN_FM,adsress[1]);    
   spi_xfer(MSN_FM,adsress[2]);    
   spi_xfer(MSN_FM,adsress[3]);
   //////////////////////////////////////////////////////////////////
   Output_high(Pin_A2);           //take CS PIN higher back
   delay_ms(delay); 
   return;
}
//-----------------------------------------------------------------------------------------------\\


//________________________BYTE WRITE Function____________________________________________________\\
void MAIN_FM_BYTE_WRITE(unsigned int32 byte_address, unsigned int8 data)
{
   
   //Byte extraction
   adsress[0]  = (unsigned int8)((byte_address>>24) & 0xFF);   // 0x __ 00 00 00
   adsress[1]  = (unsigned int8)((byte_address>>16) & 0xFF);   // 0x 00 __ 00 00
   adsress[2]  = (unsigned int8)((byte_address>>8)  & 0xFF);   // 0x 00 00 __ 00
   adsress[3]  = (unsigned int8)((byte_address)     & 0xFF);   // 0x 00 00 00 __
 
   MAIN_FM_WRITE_ENABLE();
   Output_low(Pin_E2);             //lower the CS PIN
   ///////////////////////////////////////////////////////////////////
   spi_xfer(MAIN_FM,0x12);         //Byte WRITE COMAND  (0x12)
   spi_xfer(MAIN_FM,adsress[0]);    
   spi_xfer(MAIN_FM,adsress[1]);    
   spi_xfer(MAIN_FM,adsress[2]);    
   spi_xfer(MAIN_FM,adsress[3]);

   spi_xfer(MAIN_FM,data); 
   //////////////////////////////////////////////////////////////////
   Output_high(Pin_E2);           //take CS PIN higher back 
   
   return;
}

void COM_FM_BYTE_WRITE(unsigned int32 byte_address, unsigned int8 data)
{
   
   //Byte extraction
   adsress[0]  = (unsigned int8)((byte_address>>24) & 0xFF);   // 0x __ 00 00 00
   adsress[1]  = (unsigned int8)((byte_address>>16) & 0xFF);   // 0x 00 __ 00 00
   adsress[2]  = (unsigned int8)((byte_address>>8)  & 0xFF);   // 0x 00 00 __ 00
   adsress[3]  = (unsigned int8)((byte_address)     & 0xFF);   // 0x 00 00 00 __
 
   COM_FM_WRITE_ENABLE();
   Output_low(Pin_B3);             //lower the CS PIN
   ///////////////////////////////////////////////////////////////////
   spi_xfer(COM_FM,0x12);         //Byte WRITE COMAND  (0x12)
   spi_xfer(COM_FM,adsress[0]);    
   spi_xfer(COM_FM,adsress[1]);    
   spi_xfer(COM_FM,adsress[2]);    
   spi_xfer(COM_FM,adsress[3]);

   spi_xfer(COM_FM,data); 
   //////////////////////////////////////////////////////////////////
   Output_high(Pin_B3);           //take CS PIN higher back 
   
   return;
}

void MSN_FM_BYTE_WRITE(unsigned int32 byte_address, unsigned int8 data)
{
   //Byte extraction
   adsress[0]  = (unsigned int8)((byte_address>>24) & 0xFF);   // 0x __ 00 00 00
   adsress[1]  = (unsigned int8)((byte_address>>16) & 0xFF);   // 0x 00 __ 00 00
   adsress[2]  = (unsigned int8)((byte_address>>8)  & 0xFF);   // 0x 00 00 __ 00
   adsress[3]  = (unsigned int8)((byte_address)     & 0xFF);   // 0x 00 00 00 __
 
   MSN_FM_WRITE_ENABLE();
   Output_low(Pin_A2);             //lower the CS PIN
   ///////////////////////////////////////////////////////////////////
   spi_xfer(MSN_FM,0x12);         //Byte WRITE COMAND  (0x12)
   spi_xfer(MSN_FM,adsress[0]);    
   spi_xfer(MSN_FM,adsress[1]);    
   spi_xfer(MSN_FM,adsress[2]);    
   spi_xfer(MSN_FM,adsress[3]);

   spi_xfer(MSN_FM,data); 
   //////////////////////////////////////////////////////////////////
   Output_high(Pin_A2);           //take CS PIN higher back 
   
   return;
}

//________________________BYTE read Function____________________________________________________\\
unsigned int8 MAIN_FM_BYTE_READ(unsigned INT32 ADDRESS)
{
   //Byte extraction
   adsress[0]  = (unsigned int8)((ADDRESS>>24) & 0xFF);   // 0x __ 00 00 00
   adsress[1]  = (unsigned int8)((ADDRESS>>16) & 0xFF);   // 0x 00 __ 00 00
   adsress[2]  = (unsigned int8)((ADDRESS>>8)  & 0xFF);   // 0x 00 00 __ 00
   adsress[3]  = (unsigned int8)((ADDRESS)     & 0xFF);   // 0x 00 00 00 __
   

   output_low(PIN_E2);             //lower the CS PIN
   ///////////////////////////////////////////////////////////////////
   spi_xfer(MAIN_FM,0X13);  //READ DATA COMAND   (0x13)
   spi_xfer(MAIN_FM,adsress[0]);
   spi_xfer(MAIN_FM,adsress[1]);
   spi_xfer(MAIN_FM,adsress[2]);
   spi_xfer(MAIN_FM,adsress[3]);

   _data = spi_xfer(MAIN_FM); 
   //////////////////////////////////////////////////////////////////
   output_high(PIN_E2);            //take CS PIN higher back 

   return _data;
}

unsigned int8 COM_FM_BYTE_READ(unsigned INT32 ADDRESS)
{
   //Byte extraction
   adsress[0]  = (unsigned int8)((ADDRESS>>24) & 0xFF);   // 0x __ 00 00 00
   adsress[1]  = (unsigned int8)((ADDRESS>>16) & 0xFF);   // 0x 00 __ 00 00
   adsress[2]  = (unsigned int8)((ADDRESS>>8)  & 0xFF);   // 0x 00 00 __ 00
   adsress[3]  = (unsigned int8)((ADDRESS)     & 0xFF);   // 0x 00 00 00 __
      
   Output_low(PIN_B3);            //lower the CS PIN
   ///////////////////////////////////////////////////////////////////
   spi_xfer(COM_FM,0X13);  //READ DATA COMAND   (0x13)
   spi_xfer(COM_FM,adsress[0]);
   spi_xfer(COM_FM,adsress[1]);
   spi_xfer(COM_FM,adsress[2]);
   spi_xfer(COM_FM,adsress[3]);

   _data = spi_xfer(COM_FM); 
   //////////////////////////////////////////////////////////////////
   Output_high(PIN_B3);          //take CS PIN higher back 
   
   return _data;
}

unsigned int8 MSN_FM_BYTE_READ(unsigned INT32 ADDRESS)
{
   //Byte extraction
   adsress[0]  = (unsigned int8)((ADDRESS>>24) & 0xFF);   // 0x __ 00 00 00
   adsress[1]  = (unsigned int8)((ADDRESS>>16) & 0xFF);   // 0x 00 __ 00 00
   adsress[2]  = (unsigned int8)((ADDRESS>>8)  & 0xFF);   // 0x 00 00 __ 00
   adsress[3]  = (unsigned int8)((ADDRESS)     & 0xFF);   // 0x 00 00 00 __
      
   Output_low(PIN_A2);            //lower the CS PIN
   ///////////////////////////////////////////////////////////////////
   spi_xfer(MSN_FM,0X13);  //READ DATA COMAND   (0x13)
   spi_xfer(MSN_FM,adsress[0]);
   spi_xfer(MSN_FM,adsress[1]);
   spi_xfer(MSN_FM,adsress[2]);
   spi_xfer(MSN_FM,adsress[3]);

   _data = spi_xfer(MSN_FM); 
   //////////////////////////////////////////////////////////////////
   Output_high(PIN_A2);          //take CS PIN higher back 
   
   return _data;
}



// these are new flash memory functions, these functions are used for fast read write

#define READ  1
#define WRITE 0

// _______________________________________________________________________________________________
void START_COM_FM_TRANSACTION(unsigned int32 ADDRESS, int mode )
{
   adsress[0]  = (unsigned int8)( (ADDRESS & 0xFF000000) >> 24 );   
   adsress[1]  = (unsigned int8)( (ADDRESS & 0x00FF0000) >> 16 );   
   adsress[2]  = (unsigned int8)( (ADDRESS & 0x0000FF00) >>  8 );   
   adsress[3]  = (unsigned int8)( (ADDRESS & 0x000000FF)       ); 
   
   if( mode == WRITE )
   {
      COM_FM_WRITE_ENABLE();
      Output_low(Pin_B3);             //lower the CS PIN  
      spi_xfer(COM_FM,0x12);         //Byte WRITE COMAND  (0x12)
      spi_xfer(COM_FM,adsress[0]);    
      spi_xfer(COM_FM,adsress[1]);    
      spi_xfer(COM_FM,adsress[2]);    
      spi_xfer(COM_FM,adsress[3]);
   }
   
   if( mode == READ )
   {
      Output_low(Pin_B3);             //lower the CS PIN  
      spi_xfer(COM_FM,0x13);          //Byte read COMAND  (0x13)
      spi_xfer(COM_FM,adsress[0]);    
      spi_xfer(COM_FM,adsress[1]);    
      spi_xfer(COM_FM,adsress[2]);    
      spi_xfer(COM_FM,adsress[3]);
   }
}

unsigned int8 COM_FM_BYTE_TRANSFER(unsigned int8 data, int mode)
{
   if( mode == WRITE )
   {
      spi_xfer(COM_FM,data);
   }
   
   if( mode == READ )
   {
      return spi_xfer(COM_FM);
   }
}

void FINISH_COM_FM_TRANSACTION()
{
   Output_high(Pin_B3);           //take CS PIN higher back 
   delay_ms(1);
}



// _______________________________________________________________________________________________
void START_MAIN_FM_TRANSACTION(unsigned int32 ADDRESS, int mode )
{
   adsress[0]  = (unsigned int8)( (ADDRESS & 0xFF000000) >> 24 );   
   adsress[1]  = (unsigned int8)( (ADDRESS & 0x00FF0000) >> 16 );   
   adsress[2]  = (unsigned int8)( (ADDRESS & 0x0000FF00) >>  8 );   
   adsress[3]  = (unsigned int8)( (ADDRESS & 0x000000FF)       ); 
   
   if( mode == WRITE )
   {
      MAIN_FM_WRITE_ENABLE();
      Output_low(Pin_E2);             //lower the CS PIN  
      spi_xfer(MAIN_FM,0x12);         //Byte WRITE COMAND  (0x12)
      spi_xfer(MAIN_FM,adsress[0]);    
      spi_xfer(MAIN_FM,adsress[1]);    
      spi_xfer(MAIN_FM,adsress[2]);    
      spi_xfer(MAIN_FM,adsress[3]);
   }
   
   if( mode == READ )
   {
      Output_low(Pin_E2);             //lower the CS PIN  
      spi_xfer(MAIN_FM,0x13);          //Byte read COMAND  (0x13)
      spi_xfer(MAIN_FM,adsress[0]);    
      spi_xfer(MAIN_FM,adsress[1]);    
      spi_xfer(MAIN_FM,adsress[2]);    
      spi_xfer(MAIN_FM,adsress[3]);
   }
}

unsigned int8 MAIN_FM_BYTE_TRANSFER(unsigned int8 data, int mode)
{
   if( mode == WRITE )
   {
      spi_xfer(MAIN_FM,data);
   }
   
   if( mode == READ )
   {
      return spi_xfer(MAIN_FM);
   }
}

void FINISH_MAIN_FM_TRANSACTION()
{
   Output_high(Pin_E2);           //take CS PIN higher back 
   delay_ms(1);
}



// _______________________________________________________________________________________________
void START_MSN_FM_TRANSACTION(unsigned int32 ADDRESS, int mode )
{
   adsress[0]  = (unsigned int8)( (ADDRESS & 0xFF000000) >> 24 );   
   adsress[1]  = (unsigned int8)( (ADDRESS & 0x00FF0000) >> 16 );   
   adsress[2]  = (unsigned int8)( (ADDRESS & 0x0000FF00) >>  8 );   
   adsress[3]  = (unsigned int8)( (ADDRESS & 0x000000FF)       ); 
   
   if( mode == WRITE )
   {
      MSN_FM_WRITE_ENABLE();
      Output_low(Pin_A2);             //lower the CS PIN  
      spi_xfer(MSN_FM,0x12);          //Byte WRITE COMAND  (0x12)
      spi_xfer(MSN_FM,adsress[0]);    
      spi_xfer(MSN_FM,adsress[1]);    
      spi_xfer(MSN_FM,adsress[2]);    
      spi_xfer(MSN_FM,adsress[3]);
   }
   
   if( mode == READ )
   {
      Output_low(Pin_A2);             //lower the CS PIN  
      spi_xfer(MSN_FM,0x13);          //Byte read COMAND  (0x13)
      spi_xfer(MSN_FM,adsress[0]);    
      spi_xfer(MSN_FM,adsress[1]);    
      spi_xfer(MSN_FM,adsress[2]);    
      spi_xfer(MSN_FM,adsress[3]);
   }
}

unsigned int8 MSN_FM_BYTE_TRANSFER(unsigned int8 data, int mode)
{
   if( mode == WRITE )
   {
      spi_xfer(MSN_FM,data);
   }
   
   if( mode == READ )
   {
      return spi_xfer(MSN_FM);
   }
}

void FINISH_MSN_FM_TRANSACTION()
{
   Output_high(Pin_A2);           //take CS PIN higher back 
   delay_ms(1);
}


void DATA_COPY_FROM_MSN_FM_TO_CFM(unsigned int32 address, unsigned int32 num_of_bytes )
{
   OUTPUT_LOW(PIN_C5); // get cfm access
   OUTPUT_LOW(PIN_A5); // get msn access
   
   COM_FM_SECTOR_ERASE( address, 64);
   for( unsigned int32 i=0; i<num_of_bytes ; i++) 
   {
      COM_FM_BYTE_WRITE( address+i, MSN_FM_BYTE_READ(address+i) );
   }

   OUTPUT_HIGH(PIN_C5); // give back cfm access
   OUTPUT_HIGH(PIN_A5); // give back msn access
   
   CFM_ACCESS_FLAG = 1;
   CFM_ACCESS_TIME = 50;
}

void DATA_COPY_FROM_MAIN_FM_TO_CFM(unsigned int32 address, unsigned int32 num_of_bytes )
{
   OUTPUT_LOW(PIN_C5); // get cfm access
   
   COM_FM_SECTOR_ERASE( address, 64);
   
   for(unsigned int32 i = 0; i< num_of_bytes; i++)
   {
      COM_FM_BYTE_WRITE( address+i, MAIN_FM_BYTE_READ(address+i) );
   }
     
   OUTPUT_HIGH(PIN_C5); // give back cfm access
   CFM_ACCESS_FLAG = 1;
   CFM_ACCESS_TIME = 50;
}



//!void DATA_COPY_FROM_MSN_FM_TO_CFM(unsigned int32 address, unsigned int32 num_of_bytes )
//!{
//!   OUTPUT_LOW(PIN_C5); // get cfm access
//!   OUTPUT_LOW(PIN_A5); // get msn access
//!   
//!   COM_FM_SECTOR_ERASE( address, 64);
//!   
//!   START_COM_FM_TRANSACTION(address, WRITE);
//!   START_MSN_FM_TRANSACTION(address, READ );
//!   
//!   for( unsigned int32 i=0; i<num_of_bytes ; i++) 
//!   {
//!      COM_FM_BYTE_TRANSFER( MSN_FM_BYTE_TRANSFER(address+i, READ), WRITE );
//!   }
//!
//!   FINISH_COM_FM_TRANSACTION();
//!   FINISH_MSN_FM_TRANSACTION();
//!   
//!
//!   OUTPUT_HIGH(PIN_C5); // give back cfm access
//!   OUTPUT_HIGH(PIN_A5); // give back msn access
//!   
//!   CFM_ACCESS_FLAG = 1;
//!   CFM_ACCESS_TIME = 50;
//!}

//!void DATA_COPY_FROM_MAIN_FM_TO_CFM(unsigned int32 address, unsigned int32 num_of_bytes )
//!{
//!   OUTPUT_LOW(PIN_C5); // get cfm access
//!   
//!   COM_FM_SECTOR_ERASE( address, 64);
//!   
//!   START_COM_FM_TRANSACTION(address, WRITE);
//!   START_MAIN_FM_TRANSACTION(address, READ );
//!   
//!   for( unsigned int32 i=0; i<num_of_bytes ; i++) 
//!   {
//!      COM_FM_BYTE_TRANSFER( MAIN_FM_BYTE_TRANSFER(address+i, READ), WRITE );
//!   } 
//!   
//!   FINISH_COM_FM_TRANSACTION();
//!   FINISH_MAIN_FM_TRANSACTION();
//!   
//!   for( unsigned int32 i = 0; i<num_of_bytes; i++)
//!   {
//!      fprintf( PC, "%X ", COM_FM_BYTE_READ(address+i) );
//!   }
//!   printline();
//!   
//!   
//!   OUTPUT_HIGH(PIN_C5); // give back cfm access
//!   delay_ms(100);
//!   CFM_ACCESS_FLAG = 1;
//!   CFM_ACCESS_TIME = 50;
//!}



//________________________BYTE WRITE Function____________________________________________________\\
//!void MAIN_FM_BYTE_WRITE(unsigned int32 byte_address, unsigned int8 data)
//!{
//!   
//!   //Byte extraction
//!   adsress[0]  = (unsigned int8)((byte_address>>24) & 0xFF);   // 0x __ 00 00 00
//!   adsress[1]  = (unsigned int8)((byte_address>>16) & 0xFF);   // 0x 00 __ 00 00
//!   adsress[2]  = (unsigned int8)((byte_address>>8)  & 0xFF);   // 0x 00 00 __ 00
//!   adsress[3]  = (unsigned int8)((byte_address)     & 0xFF);   // 0x 00 00 00 __
//! 
//!   MAIN_FM_WRITE_ENABLE();
//!   Output_low(Pin_E2);             //lower the CS PIN
//!   ///////////////////////////////////////////////////////////////////
//!   spi_xfer(MAIN_FM,0x12);         //Byte WRITE COMAND  (0x12)
//!   spi_xfer(MAIN_FM,adsress[0]);    
//!   spi_xfer(MAIN_FM,adsress[1]);    
//!   spi_xfer(MAIN_FM,adsress[2]);    
//!   spi_xfer(MAIN_FM,adsress[3]);
//!
//!   spi_xfer(MAIN_FM,data); 
//!   //////////////////////////////////////////////////////////////////
//!   Output_high(Pin_E2);           //take CS PIN higher back 
//!   
//!   return;
//!}







//!
//!Void SECTOR_COPY_MSN_FM_TO_COM_FM(unsigned int32 sector_number, unsigned int32 num_of_sec, unsigned int32 sector_size)
//!{
//!   unsigned int32 top_sector = sector_number + num_of_sec ;
//!   OUTPUT_LOw(PIN_C4); 
//!   OUTPUT_LOw(PIN_A5);
//!   if( sector_size == 0x64 )
//!   {  
//!      for(unsigned int32 i = sector_number; i < top_sector; i++)
//!      {
//!         Fprintf(PC,"(MSN FM TO COM FM, 4KB sec) Copying Sector-%Ld\n\r",i);
//!         
//!         Unsigned int32 start_address = (i<<16) & 0x0FFF0000;   // 0x 00 _ _ 00 00
//!         fprintf(PC,"Sector start adress = 0x%Lx\n\r",start_address);
//!         COM_FM_SECTOR_ERASE( start_address , 64 );
//!         
//!         for(unsigned int32 k = 0; k < 65536 ; k++)
//!         {
//!            COM_FM_BYTE_WRITE( start_address + k, MSN_FM_BYTE_READ(start_address + k) );
//!         }
//!         
//!         Fprintf(PC,"Sector-%Ld copying finish\n\r",i);
//!      }
//!   }
//!   
//!   if( sector_size == 0x04 )
//!   {  
//!      for(Unsigned int32 i = sector_number; i < top_sector; i++)
//!      {
//!         fprintf(PC,"(MSN FM TO COM FM, 4KB sec) Copying Sector-%Ld\n\r",i);
//!         
//!         Unsigned int32 start_address = (i<<12) & 0x0FFFF000 ;   // 0x 00 _ _ _0 00
//!         fprintf(PC,"Sector start adress = 0x%Lx\n\r",start_address);
//!         COM_FM_SECTOR_ERASE( start_address , 4 );
//!         
//!         for(unsigned int32 k = 0; k < 4096 ; k++)
//!         {
//!            COM_FM_BYTE_WRITE( start_address + k, MSN_FM_BYTE_READ(start_address + k) );
//!         }
//!         
//!         Fprintf(PC,"Sector-%Ld copying finish\n\r",i);
//!      }
//!   }
//!   OUTPUT_HIGH(PIN_C4);
//!   OUTPUT_HIGH(PIN_A5);
//!}
//!
//!Void SECTOR_COPY_MAIN_FM_TO_COM_FM(unsigned int32 sector_number, unsigned int32 num_of_sec, unsigned int32 sector_size)
//!{
//!   OUTPUT_LOW(PIN_C4);
//!   unsigned int32 top_sector = sector_number + num_of_sec ;
//!    
//!   if( sector_size == 0x64 )
//!   {  
//!      for(unsigned int32 i = sector_number; i < top_sector; i++)
//!      {
//!         Fprintf(PC,"(MAIN FM TO CFM, 64KB sec) Copying Sector-%Ld\n\r",i);
//!         
//!         Unsigned int32 start_address = (i<<16) & 0x0FFF0000 ;   // 0x 00 _ _ 00 00
//!         fprintf(PC,"Sector start adress = 0x%Lx\n\r",start_address);
//!         COM_FM_SECTOR_ERASE( start_address , 64 );
//!         
//!         for(unsigned int32 k = 0; k < 65536 ; k++)
//!         {
//!            COM_FM_BYTE_WRITE( start_address + k, MAIN_FM_BYTE_READ(start_address + k) );
//!         }
//!         
//!         Fprintf(PC,"Sector-%Ld copying finish\n\r",i);
//!      }
//!   }
//!   
//!   if( sector_size == 0x04 )
//!   {  
//!      for(Unsigned int32 i = sector_number; i < top_sector; i++)
//!      {
//!         fprintf(PC,"(MAIN FM TO CFM, 4KB sec) Copying Sector-%Ld\n\r",i);
//!         
//!         Unsigned int32 start_address = (i<<12) & 0x0FFFF000 ;   // 0x 00 _ _ _0 00
//!         fprintf(PC,"Sector start adress = 0x%Lx\n\r",start_address);
//!         COM_FM_SECTOR_ERASE( start_address , 4 );
//!         
//!         for(unsigned int32 k = 0; k < 4096 ; k++)
//!         {
//!            COM_FM_BYTE_WRITE( start_address + k, MAIN_FM_BYTE_READ(start_address + k) );
//!         }
//!         
//!         Fprintf(PC,"Sector-%Ld copying finish\n\r",i);
//!      }
//!   }
//!   OUTPUT_HIGH(PIN_C4);
//!}
//!
//!
//!Void PACKETS_COPY_FROM_MSN_FM_TO_COM_FM(unsigned int32 start_add, unsigned int32 num_of_pckt)
//!{
//!   //get FM acces
//!   OUTPUT_LOw(PIN_C4); 
//!   OUTPUT_LOw(PIN_A5);
//!   
//!   //erase the sector
//!   COM_FM_SECTOR_ERASE( start_add , 64 );
//!   unsigned int32 num_of_bytes = 81 * num_of_pckt ;
//!
//!   Fprintf(PC,"LoRa data copying to SFM, Start address = 0x%LX\n\r",start_add);
//!   Fprintf(Debug,"No. of bytes = %Ld (%Ld)\n\r",num_of_bytes,num_of_pckt);
//!   
//!   for(unsigned int32 k = 0; k < num_of_bytes ; k++)
//!   {
//!      COM_FM_BYTE_WRITE( (start_add + k), MSN_FM_BYTE_READ(start_add + k) );
//!   }
//!   
//!   OUTPUT_HIGH(PIN_C4);
//!   OUTPUT_HIGH(PIN_A5);
//!}
//!
//!
//!Void PACKETS_COPY_FROM_MAIN_FM_TO_COM_FM(unsigned int32 start_add, unsigned int32 num_of_pckt)
//!{
//!   //get FM acces
//!   OUTPUT_LOw(PIN_C4); 
//!   
//!   //erase the sector
//!   COM_FM_SECTOR_ERASE( start_add , 64 );
//!   
//!   unsigned int32 num_of_bytes = 81* num_of_pckt;
//!   
//!   Fprintf(Debug,"Data copying from Main FM to SFM, Start address = 0x%LX\n\r",start_add);
//!   Fprintf(Debug,"No. of bytes = %Ld (%Ld)\n\r",num_of_bytes,num_of_pckt);
//!   
//!   for(unsigned int32 k = 0; k < num_of_bytes ; k++)
//!   {
//!      COM_FM_BYTE_WRITE( (start_add + k), MAIN_FM_BYTE_READ(start_add + k) );
//!   }
//!   
//!   OUTPUT_HIGH(PIN_C4);
//!}





































