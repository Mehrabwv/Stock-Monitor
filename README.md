# Stock-Monitor
Uses SSTuino II (esp32), 0.96inch OLED display, and a button, to display stock information (Current price, day's high, day's low, change). Uses data from stooq.com. Default stocks: AAPL, GOOG, VOO, ACWI, NVDA, INTC, AMD, ORCL, GLD, XOM.

# Items needed:
1) SSTuino II (esp32)
2) 0.96inch Oled display
3) Button
4) 6 wires

# Connections: (default)
[Display]
VCC => 3.3v
GND => GND
SCL => SCL (Pin 20)
SDA => SDA (Pin 21)

[Button]
Leg 1 => Pin 8
Leg 2 => GND

# Manual adjustments:
Setting wifi name: Change under 'char ssid'\
Setting wifi password: Change under 'char pass'\
Change stock: Change under 'Asset'
