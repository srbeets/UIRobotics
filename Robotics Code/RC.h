#ifndef _RC_H
	#define _RC_H

	/* ----------- Macros for configuring RC pins as Digital Outputs --------- */
    #define cfgRC1()    PORTSetPinsDigitalOut(IOPORT_D, BIT_9)
    #define cfgRC2()    PORTSetPinsDigitalOut(IOPORT_D, BIT_11)
	#define cfgRC3()    PORTSetPinsDigitalOut(IOPORT_D, BIT_10)
    #define cfgRC4()    PORTSetPinsDigitalOut(IOPORT_D, BIT_8)

    /* -------------------- RC Channel pin assignment macros ----------------- */
	#define RC_1(a)     LATDbits.LATD9  = a
	#define RC_2(a) 	LATDbits.LATD11 = a
	#define RC_3(a) 	LATDbits.LATD10 = a
	#define RC_4(a) 	LATDbits.LATD8  = a

	#define NRC		4		// Number of RC channels

    /* --------- RC Setting Range - How wide the PWM Signal should be -------- */
    #define RC_MIN		100
	#define RC_MAX		200
	#define RC_SPAN		100

    /* ---------------------- Public Function declarations ------------------- */
    void initRC(void);
    void set_RC(int ch, int ctrl);
    int powerMotors (int leftCh, float leftPercent, int rightCh, float rightPercent, float duration);
#endif
