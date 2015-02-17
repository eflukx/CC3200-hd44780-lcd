# hd44780 LCD driver suited for TI CC3200 wireless controller

LCD driver specifically for use on the Texas Instruments CC3200 controller and the related Launchpad. This lib assumes the LCD is tied to the SPI bus using a shift register. Schematics included.
It uses only 3 wires for communitation (DATA, CLK, Enable). The SPI CS pin is tied to enable input of LCD module and is software controlled. Beware that CS control is different from _standard_ SPI.

Have fun!


![schematic](https://raw.githubusercontent.com/eflukx/CC3200-hd44780-lcd/master/SPI_HD44780.png)

Note to the [schematic](https://raw.githubusercontent.com/eflukx/CC3200-hd44780-lcd/master/SPI_HD44780.png): The LCD module (and 595) VDD is tied to the +5V pin on the CC3200 launchpad. CC3200 has 3.3V IO, typically this is accepted by the 74HCT595 and LCD modules.

_This driver was based on https://github.com/aostanin/avr-hd44780_
