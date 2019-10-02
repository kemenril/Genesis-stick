# Genesis-stick
An interface for converting Sega Genesis/Master System/Atari joysticks to interface with arcade machines.

This project contains AVR C code to make an ATTiny2313 (or probably a different AVR, which a bit of adjustment)
manage a Sega Genesis (... or Master System, or Atari 2600/7800, or similar) joystick, presenting the button 
states as normally normally open/shorted-to-ground on closed, such as an arcade machine would expect.  I have
used this successfully in my console conversion of a Neo Geo system.  Relative to the other option, which 
appears to be buying a converter for a Nintendo stick, I tend to prefer the Sega hardware, and the DE9 ports 
are dirt cheap and easy to get, so it makes a good home-build.

The ATTiny2313 is pretty much dirt-cheap, and has exactly the right amount of pins for this project.
We don't require much memory at all, and we can use the internal clock of the microcontroller, so 
there's nothing required outside of the chip itself except for 5V power.

The description of *how* the interface works once it's programmed is included in a comment at the top of genesis.c,
and is reproduced here for your convenience:

```
/* The 2313 has three banks of pins, PORTA, PORTB, PORTD.  Pinout is:

                --v--
 RST/ PortA2 - |     | - VCC
      PortD0 - |     | - PortB7
      PortD1 - |     | - PortB6
      PortA1 - |     | - PortB5
      PortA0 - |     | - PortB4
      PortD2 - |     | - PortB3
      PortD3 - |     | - PortB2
      PortD4 - |     | - PortB1
      PortD5 - |     | - PortB0
         GND - |     | - PortD6
                -----

        This implements a Genesis joystick interface breakout, wired as 
                follows:

        Joystick                | ATTiny2313    | Out
        (Select low/high/pulse) |               |  
        --------------------------------------------------
        [9 Pin Joystick port]
        1 (Up/Up/Z)             A2
        2 (Down/Down/Y)         D0
        3 (Low/Left/X)          D1
        4 (Low/Right)           A1
        5 (VCC)                 VCC
        6 (A/B)                 A0
        7 (Select)              D2
        8 GND                   GND
        9 (Start/C)             D3


        [Output, 11 pins]
                                  D4              Up
                                  D5              Down
                                  D6              Left
                                  B0              Right
                                  B1              A
                                  B2              B
                                  B3              C
                                  B4              X
                                  B5              Y
                                  B6              Z
                                  B7              Start
        Aside from the obvious interface wires, also run the chip's VCC to 
        +5v from some power supply (most systems will provide it), and connect
        the ground to the same supply.                  

 We'll need to set the fuses to use the internal oscillator so that we can
        make use of A0,1 and disable reset so that we can use A2.  There are
        exactly enough pins available on ATTiny2313.

In order to allow old Genesis sticks and Atari sticks to work on this interface,
we remap some buttons depending on user input during boot.

The procedure will be: 

Default to six buttons.  Raise select, and check to see whether button B (pin 6)
is high.  Pin six is the Atari fire button.  If it's high here, this will
distinguish the Atari button from the Genesis button A.  In this case, consider
the joystick to be an Atari style unit.  Leave select up permanently because
the Atari pinout shows VCC here. Pin 6 becomes A.  C stays where it is.

If the above check fails, lower select and check pin 6 again, now for Genesis
button A.  If it's high, we can assume that the user wants to force the
interface into 3-button mode.  Skip the extra select pulse. Start is remapped
to Z so that you can use it as a game button.
*/
```


I should thank Charles Rosenberg at CMU for his original work on the six-button protocol.  I've included the text 
file he originally produced to describe that protocol in the 90s, segasix.txt, in the repository.
