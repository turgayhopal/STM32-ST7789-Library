## USAGE

Review TestCode.txt

- The LCD I use as a reference

![[https://github.com/turgayhopal/STM32-ST7789-Library/13-inc-ips-tft-lcd-240240-renkli-hd-lcd-ekran-st7789-surucu-modulu-tft-lcd-display-sincerepromise-42178-15-B.jpg]]

#### Pins must be set as output beforehand.

- RES (Pin) ->  Use **LCD_setRST()** to configure your pin.
- DC (Pin) ->  Use **LCD_setDC()** to configure your pin.
- CS (Pin) ->  Use **LCD_setCE()** to configure your pin.

- SPI ->USE **LCD_setSPI()** to configure the SPI interface.


