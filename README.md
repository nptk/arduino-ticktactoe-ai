# arduino-ticktactoe-ai
using minimax algorithm with lowest depth

# Require Hardware
  - Arduino UNO ( can be any Arduino, just change the hardware/software pin defined in code )
  - Keypad (4x3)
  - RBG OLED 96x64 display with SSD1331
# Require Libaries
Library is available via the Arduino IDE library manager.

If you are using a modern IDE (1.6.2 or above), you can simply use the menu:

Sketch->Include Library->Manage Libraries-> install these libs
- [Keypad](https://playground.arduino.cc/Code/Keypad)
- [Ucglib v1.3](https://github.com/olikraus/ucglib/wiki)

# SPI pins
you can use either hardware or software pin. Hardware pin is faster and less wiring.

If you want to connect more hardwares to Ardiono UNO but you dont enough available digital/analog pins.

Here is the tutorial how to wiring the all keypad pins using 1 analog pin.

https://www.youtube.com/watch?v=URO042VrCKU
