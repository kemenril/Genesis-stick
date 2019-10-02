/* Genesis joystick breakout for AVR microcontrollers,
 *  by Chris Smith (protheus@byteorder.net), based on
 * excellent documentation written by Charles Rosenberg.
 *
 * You can find Charles' document at
 * http://www.cs.cmu.edu/~chuck/infopg/segasix.txt
 */

/* Requires avr-libc package */
#include <avr/io.h>
#include <util/delay.h>

#include "avr-general.h"


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

	Joystick		| ATTiny2313	| Out
	(Select low/high/pulse)	|		|  
	--------------------------------------------------
	[9 Pin Joystick port]
	1 (Up/Up/Z)		A2
	2 (Down/Down/Y)		D0
	3 (Low/Left/X)		D1
	4 (Low/Right)		A1
	5 (VCC)			VCC
	6 (A/B) 		A0
	7 (Select)		D2
	8 GND			GND
	9 (Start/C)		D3


	[Output, 11 pins]
				  D4		  Up
				  D5		  Down
				  D6		  Left
				  B0		  Right
				  B1		  A
				  B2		  B
				  B3		  C
				  B4		  X
				  B5		  Y
				  B6		  Z
				  B7		  Start
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

#define STICK_GENESIS6	0
#define STICK_GENESIS3	1
#define STICK_ATARI	2



const short pulsepad = 10; //Make sure there's this much space (microseconds) after each select state change.
const short polldelay = 8; //Sleep between poll periods in milliseconds

//Set during startup; just hold button A on your stick if it's not a 6 button.
unsigned char sticktype = STICK_GENESIS6;

//We'll need to activate the select line to read the state of certain controller buttons.
void selectUp() {
   On(D,2);
   _delay_us(pulsepad);
}
void selectDown() {
   Off(D,2);
  _delay_us(pulsepad); 
}

//Write a pulse to the select line.
void selectPulse() {
   selectUp();
   selectDown();
}

//Each poll interval consists of three or so pulses, separated by maybe 17 millisec.
void poll() {
  
  //Throw an initial pulse at a six button controller:
  if (!sticktype) selectPulse();
  
  //On all Genesis sticks, read A/Start with select low.
  if (sticktype < STICK_ATARI) {
	if(Read(A,0)) { //A
		On(B,1);
	} else { Off(B,1); }

	if (sticktype == STICK_GENESIS6) { //Start
	if(Read(D,3)) {
		On(B,7);
	} else { Off(B,7); }

	} else { 
		/* On STICK_GENESIS3, joystick start is remapped to button Z
			to provide a fourth button for gameplay.  Add a start
			override on the console. */
		if(Read(D,3)) {
			On(B,6);
		} else { Off(B,6); }		
	}


  }

  //Raising select makes the entire joypad available on the genesis port, and
  // swaps in some new buttons. 
if (sticktype < STICK_ATARI) selectUp();

if(Read(A,2)) {		//Up
	On(D,4);
} else { Off(D,4); }

if(Read(D,0)) {		//Down
	On(D,5);
} else { Off(D,5); }

if(Read(D,1)) {		//Left
	On(D,6);
} else { Off(D,6); }

if(Read(A,1)) {		//Right
	On(B,0);
} else { Off(B,0); }

/* Atari A, Genesis B */
if (sticktype == STICK_ATARI) {
	if(Read(A,0)) {         //A
		On(B,7);
	} else { Off(B,7); }
} else {
	if(Read(A,0)) {		//B
		On(B,2);
	} else { Off(B,2); }
}

if(Read(D,3)) {		//C, or Atari "paddle B" should it exist.
	On(B,3);
} else {
	Off(B,3);
}


if (sticktype < STICK_ATARI) {
	selectDown();
}

if (sticktype == STICK_GENESIS6) {
	selectUp();
	if (Read(D,1)) {	//X
		On(B,4);
	} else { Off(B,4); }

	if(Read(D,0)) {		//Y
		On(B,5);
	} else { Off(B,5); }

	if(Read(A,2)) {		//Z
		On(B,6);
	} else { Off(B,6); }

  selectDown();  
}
  //They say we need a fourth pulse to reset the system
  selectPulse();
  
}

void main () {
	
	/* Set up direction registers. */
	InputBank(A);	//All joystick port pins.
	BankOn(A);	//Pull up bank A
	OutputBank(B);	//All output pins.
	BankOn(B);	//Inverted logic; this causes the buttons to be up.
	InputBank(D);	//Mostly joystick port pins.
	On(D,0);On(D,1);On(D,3); //Pull up the input pins in bank D.
	Output(D,2);	//Select (on the joystick port)
	Output(D,4);	//Up out
	Output(D,5);	//Down out
	Output(D,6);	//Left out

	/* The arcade system is using inverted logic; low is +5v, so we want
		to initialize everything to make sure that the output pins 
		are up by default, in case we're not using some of the buttons.
	*/
	BankOn(B);
	PORTD = 0b01110100; //pins 2, 4, 5, and 6 on.
	

	/* We don't have any place on the controller for a switch to force
		a certain type of joystick to be used, but we can detect
		a button press at boot.  We'll check for the A button or
		Atari fire button.  */
	selectUp();	/* In this mode, the Genesis B/Atari A  is on A0 */
	if(ReadInverted(A,0)) {
		sticktype = STICK_ATARI;
	} else {
		selectDown();	// Now if that didn't work, this is Genesis A  
		if(ReadInverted(A,0)) {
			sticktype = STICK_GENESIS3;
		}
	}

	/* Leave select up for STICK_ATARI, which will allow us to test the
	 	result with a Genesis stick.	*/
	if (sticktype < STICK_ATARI) selectUp();	

	/* We've just effectively sent a single pulse, and the controller 
		likes them in groups of three.  Send a couple more to reset. */
	if (sticktype == STICK_GENESIS6) {
		selectPulse();
		selectPulse();
		//Reset
		selectPulse();
	}

	for (;;_delay_ms(polldelay)) {
		poll();
	}

}
