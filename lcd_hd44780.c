#include "lcd_hd44780.h"

#include <stdarg.h>
#include <stdio.h>

//Driverlib includes
#include "hw_types.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "prcm.h"
#include "spi.h"
#include "hw_memmap.h"
#define _delay_ms(x)  MAP_UtilsDelay(20000* x)

void lcd_send(uint8_t value, uint8_t mode);
void lcd_write_nibble_rs(uint8_t nibble);

static uint8_t lcd_displayparams;
static char lcd_buffer[LCD_COL_COUNT + 1];

void lcd_send(uint8_t value, uint8_t data_mode) {

  lcd_write_nibble_rs((value >> 4) | data_mode);
  lcd_write_nibble_rs((value & 15) | data_mode);
}

void lcd_write_nibble_rs(uint8_t nibble) {
  unsigned long dummy;

  MAP_SPICSEnable(GSPI_BASE);
  MAP_SPIDataPut(GSPI_BASE,nibble);
  MAP_SPIDataGet(GSPI_BASE,&dummy); //clear recv buffer.
  MAP_SPICSDisable(GSPI_BASE);
  _delay_ms(0.04);
}

void lcd_command(uint8_t command) {
  lcd_send(command, LCD_RS_CMD);
}

void lcd_write(uint8_t value) {
  lcd_send(value, LCD_RS_DATA);
}

void lcd_SPI_init(){
  MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK); //redundant, in pinmux.c

  MAP_SPIReset(GSPI_BASE);

  MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                   SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_0,
                   (SPI_SW_CTRL_CS |
                   SPI_4PIN_MODE |
                   SPI_TURBO_OFF |
                   SPI_CS_ACTIVEHIGH |
                   SPI_WL_8));

  MAP_SPIEnable(GSPI_BASE);

  MAP_SPICSDisable(GSPI_BASE);
}


void lcd_init(void) {

  lcd_SPI_init();
  // Wait for LCD to become ready (docs say 15ms+)
  _delay_ms(15);

  lcd_write_nibble_rs(0x03); // Switch to 4 bit mode
  _delay_ms(4.1);

  lcd_write_nibble_rs(0x03); // 2nd time
  _delay_ms(4.1);

  lcd_write_nibble_rs(0x03); // 3rd time
  _delay_ms(4.1);

  lcd_write_nibble_rs(0x02); // Set 8-bit mode (?)

  lcd_command(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);

  lcd_displayparams = LCD_CURSOROFF | LCD_BLINKOFF;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_on(void) {
  lcd_displayparams |= LCD_DISPLAYON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_off(void) {
  lcd_displayparams &= ~LCD_DISPLAYON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_clear(void) {
  lcd_command(LCD_CLEARDISPLAY);
  _delay_ms(2);
}

void lcd_return_home(void) {
  lcd_command(LCD_RETURNHOME);
  _delay_ms(2);
}

void lcd_enable_blinking(void) {
  lcd_displayparams |= LCD_BLINKON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_disable_blinking(void) {
  lcd_displayparams &= ~LCD_BLINKON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_enable_cursor(void) {
  lcd_displayparams |= LCD_CURSORON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_disable_cursor(void) {
  lcd_displayparams &= ~LCD_CURSORON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_scroll_left(void) {
  lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void lcd_scroll_right(void) {
  lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void lcd_set_left_to_right(void) {
  lcd_displayparams |= LCD_ENTRYLEFT;
  lcd_command(LCD_ENTRYMODESET | lcd_displayparams);
}

void lcd_set_right_to_left(void) {
  lcd_displayparams &= ~LCD_ENTRYLEFT;
  lcd_command(LCD_ENTRYMODESET | lcd_displayparams);
}

void lcd_enable_autoscroll(void) {
  lcd_displayparams |= LCD_ENTRYSHIFTINCREMENT;
  lcd_command(LCD_ENTRYMODESET | lcd_displayparams);
}

void lcd_disable_autoscroll(void) {
  lcd_displayparams &= ~LCD_ENTRYSHIFTINCREMENT;
  lcd_command(LCD_ENTRYMODESET | lcd_displayparams);
}

void lcd_create_char(uint8_t location, uint8_t *charmap) {
  int i;
  lcd_command(LCD_SETCGRAMADDR | ((location & 0x7) << 3));
  for (i = 0; i < 8; i++) {
    lcd_write(charmap[i]);
  }
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
  static uint8_t offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if (row > 1){ row = 1; }
  lcd_command(LCD_SETDDRAMADDR | (col + offsets[row]));
}
void lcd_putsc(char* string,int clear) {
	if(clear)
		lcd_clear();
	lcd_return_home();
	lcd_puts(string);
}

void lcd_puts(char* string) {
  while(*string != NULL){
	if(*string == '\n')
		lcd_set_cursor(0,1);
	else
		lcd_write(*string);

	string++;
  }
}

void lcd_printfc(char *format, ...) {
  va_list args;

  va_start(args, format);
  vsnprintf(lcd_buffer, LCD_HD44780_MAXCHAR, format, args);
  va_end(args);

  lcd_putsc(lcd_buffer, false);
}


void lcd_printf(char *format, ...) {
  va_list args;

  va_start(args, format);
  vsnprintf(lcd_buffer, LCD_COL_COUNT + 1, format, args);
  va_end(args);

  lcd_puts(lcd_buffer);
}
