//!// here we check START pic UART is available or not if available we save in MPIC_TO_RPIC array
//!void CHECK_UART_INCOMING_FROM_START_PIC()
//!{
//!   if( SPic_available() )
//!   {
//!      Delay_ms(30);
//!      for(int i = 0; i<=5; i++)
//!      {
//!         SPIC_TO_RPIC_ARRAY[i] = SPic_Read();
//!      }
//!   }
//!}
//!
//!
//!// this function will print recived command from START pic_______________________
//!void PRINT_RECIVED_COMMAND_FROM_START_PIC()
//!{
//!   printline();
//!   Fprintf(PC,"RCVD CMD FROM START PIC >> ");
//!   for(int i = 0; i<=5; i++)
//!   {
//!      Fprintf(PC,"%X ",SPIC_TO_RPIC_ARRAY[i]);
//!   }
//!   printline();
//!}

void INFORM_WORKING_TO_START_PIC()
{
   fputc(0xAA, SPic);
   delay_ms(1);
   fputc(0xBB, SPic);
   delay_ms(1);
   fputc(0xCC, SPic);
   delay_ms(1);
   output_low(PIN_C6);
}
