#ifndef _AVR_GENERAL_H
#define _AVR_GENERAL_H

/* Flip individual pins on and off given PORTx and pin. */
#define On(bank,pin) PORT##bank |= (1<<pin)
#define Off(bank,pin) PORT##bank &= ~(1<<pin)

/* Flip entire banks on and off. */
#define BankOn(bank) PORT##bank = 0xFF
#define BankOff(bank) PORT##bank = 0x00

/* Set input/outpud mode given bank and pin number */
#define Input(bank,pin) DDR##bank &= ~(1<<pin)
#define Output(bank,pin) DDR##bank |= (1<<pin)

/* Set input/output mode for entire banks. */
#define InputBank(bank) DDR##bank = 0x00
#define OutputBank(bank) DDR##bank = 0xFF

/* Read a value from a pin */
#define Read(bank,pin) ((PIN##bank>>pin) & 1U)
/* Read a value from a pin, logic inverted */
#define ReadInverted(bank,pin) !((PIN##bank>>pin)|0xFE)

#endif


