// NEC Protocol IR remote control decoder using PIC16F877A CCS C code
 
//LCD module connections
#define LCD_RS_PIN PIN_D0
#define LCD_RW_PIN PIN_D1
#define LCD_ENABLE_PIN PIN_D2
#define LCD_DATA4 PIN_D3
#define LCD_DATA5 PIN_D4
#define LCD_DATA6 PIN_D5
#define LCD_DATA7 PIN_D6
//End LCD module connections

#define LED1 PIN_C0
#define LED2 PIN_C1
#define LED3 PIN_C2
#define LED4 PIN_C3
 
#include <16F877A.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP                       
#use delay(clock = 8000000)
#include <lcd.c>
#use fast_io(B)
 
unsigned int32 ir_code;
unsigned int16 address;
unsigned int8 command, inv_command;
short nec_remote_read(){
  unsigned int8 count = 0, i;
  // Check 9ms pulse (remote control sends logic high)
  while((input(PIN_B0) == 0) && (count < 200)){
    count++; 
    delay_us(50);}
    if( (count > 199) || (count < 160))        // NEC protocol?
      return FALSE;                          
  count = 0;
  // Check 4.5ms space (remote control sends logic low)
  while((input(PIN_B0)) && (count < 100)){
    count++;
    delay_us(50);}
    if( (count > 99) || (count < 60))          // NEC protocol?
      return FALSE;                         
  // Read code message (32-bit)
  for(i = 0; i < 32; i++){
    count = 0;
    while((input(PIN_B0) == 0) && (count < 14)){
      count++;
      delay_us(50);}
    if( (count > 13) || (count < 8))      // NEC protocol?
      return FALSE;                          
    count = 0;
    while((input(PIN_B0)) && (count < 40)){
      count++; 
      delay_us(50);}
    if( (count > 39) || (count < 8))      // NEC protocol?
      return FALSE;
    if( count > 20)                       // If space width > 1ms
      bit_set(ir_code, (31 - i));         // Write 1 to bit (31 - i)
    else                                  // If space width < 1ms
      bit_clear(ir_code, (31 - i));       // Write 0 to bit (31 - i)
  }
  return TRUE;
}
void control_leds(unsigned int16 button_code) {
    output_low(LED1);
    output_low(LED2);
    output_low(LED3);
    output_low(LED4);

    if (ir_code == 0xFF906F) {
        output_high(LED1);
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "LED 1 ON   ");
    } else if (ir_code == 0xFFB847) {
        output_high(LED2);
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "LED 2 ON   ");
    } else if (ir_code == 0xFFF807) {
        output_high(LED3);
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "LED 3 ON   ");
    } else if (ir_code == 0xFFB04F) {
        output_high(LED4);
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "LED 4 ON   ");
    }
}
void main(){
  output_b(0);                           // PORTB initial state
  set_tris_b(1);
  lcd_init();                            // Initialize LCD module
  lcd_putc('\f');                        // LCD clear
  lcd_gotoxy(2, 1);                      // Go to column 3 row 1
  lcd_putc("Home Automation");
  lcd_gotoxy(6, 2);                      // Go to column 6 row 2
  printf(lcd_putc, "System");
  delay_ms(2000);
  lcd_putc('\f');
  lcd_gotoxy(1, 1);                      // Go to column 1 row 1
  printf(lcd_putc, "Address:0x0000");
  //lcd_gotoxy(1, 2);                      // Go to column 1 row 2
  //printf(lcd_putc, "Com:0x00 In:0x00");
  while(TRUE){
    while(input(PIN_B0));                // Wait until RB0 pin falls
    if(nec_remote_read()){
      address = ir_code >> 16;
      command = ir_code >> 8;
      inv_command = ir_code;
     
      control_leds(command); // Control LEDs based on the button pressed
      
      lcd_gotoxy(11, 1);
      printf(lcd_putc,"%4LX",ir_code);  
      //lcd_gotoxy(7, 2);          
      //printf(lcd_putc,"%2X",command);
      //lcd_gotoxy(15, 2);
     // printf(lcd_putc,"%2X",inv_command);
      
       ir_code = 0; // Reset ir_code
       
      delay_ms(200);}
  }
}
