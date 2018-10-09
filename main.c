#include "config_bits.h"    // Processor configuration
#include "hardware.h"       // Platform declarations

// XC32 libraries
#include <plib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

// Included files for platform 
#include "LCDlib.h"
#include "led7.h"
// #include "comm_lib.h" 	// Name change
#include "uart2.h"
#include "uart4.h"
#include "swDelay.h"
#include "main.h"
#include "DHT.h"
// #include "TMP03.h"		// Not used
// #include "GPS_I2C.h"		// Not used
#include "MAG3110.h"

float temp1;
float temp2;
float temp3, rh;
float tmp4, tmp5;
BOOL sensor_okay;

float length;
float angle;

static void initialize_nav_sens(void);

/* ----------------------------------------------------------- */
int main(void)
{
char lcdstr[40];	// Local LCD display buffers
I2C_RESULT i2cFlag;
int len = 255;
int pkt_len = 0;
int x,y,z;
int hour, min, sec;
float heading = 0.0;

	Hardware_Setup();       // Initialize common hardware
    initialize_nav_sens();
	while(1)
	{
//        length = distance(lat1, lon1, lat2, lon2, 'K');

    	clrLCD();				// Clear hello messages

        sensor_okay = DHT11_read_temp_rh(&temp3, &rh);
    	sprintf(lcdstr,"T3:%d RH:%d", (int) temp3, (int) rh  );        
		putsLCD(lcdstr);	// Post channel settings to LCD line 1			
        printf("\n\r%s \n\r", lcdstr);	// Send channel setting to UART
		gotoLCD(16);			// Blank out LCD line 1    
        read_ad22100(&tmp4, &tmp5);
    	sprintf(lcdstr,"T4:%d T5:%d", (int) tmp4, (int) tmp5 );        
		putsLCD(lcdstr);	// Post channel settings to LCD line 1			
        printf("temp #4 = %d   temp #5 = %d \n\r", (int) tmp4, (int) tmp5 );
        do 
        {
            DelayMs(2);     // Give GPS time to up load new buffer
            len = MAX_PACKET_SIZE;
            i2cFlag = I2C_Read(I2C1, GPS_DEV_ID, gps_str, &len);
            if(i2cFlag == I2C_SUCCESS);
            {
                invLED6();
                gps_str[len] = 0;
                pkt_len = GPS_DECODE_RMC(gps_str);
            }
        } while(pkt_len == 0);
        
        hour = (gps.utc_time / 10000);
        min = (gps.utc_time - hour*10000)/100;
        sec = (gps.utc_time - (hour*10000 + min*100));
        hour -= 7;
        if(hour < 0)
            hour += 24;     
        
        printf("Time: %2d:%2d:%2d  LAT:%f  LON:%f\n\r", hour, min, sec, gps.lon, gps.lat);
        
        i2cFlag = MAG3110_readMag(&x, &y, &z);
        if(i2cFlag == I2C_SUCCESS)
        {
            heading = MAG3110_readHeading();
            if(heading<0.0)
            {
                heading += 360.0;
            }
            printf("Magnetic X: %4d   Y:%4d   Z:%4d  Dir:%f\n\r", x,y,z, heading);
        }
        
    	DelayMs(2000);			// Wait a second
  	}
	return 0;		// The program should never get to this LOC 
}

static void initialize_nav_sens(void)
{
char lcdstr[40];	// Local LCD display buffers
I2C_RESULT i2cFlag;
int len = 255;
int pkt_len = 0;

    initLCD();				// Initialize LCD
	uart4_init(38400, NO_PARITY);	// Initialize UART
	initTimer1();			// Start timer interrupt
    init_ad22100();
    i2cFlag = I2C_Init(I2C1, I2C_SPEED_STANDARD);
    if(!i2cFlag)
    {
        printf("I2C setup error\n\r");
    }
    len = 1;
    do
    {
        DelayMs(100);			// Wait a second
        
        i2cFlag = MAG3110_initialize();
        if(!i2cFlag)
        {
            printf("Magnetometer not found\n\r");
        }
    }while(!i2cFlag);
    printf("Magnetometer is calibrating\n\r");
    MAG3110_enterCalMode();
    while(MAG3110_isCalibrating())
    {
        MAG3110_calibrate();
    }
    if(MAG3110_isCalibrated())
    {
        printf("Magnetometer is calibrated\n\r");
    }
    
    i2cFlag = I2C_Read(I2C1, GPS_DEV_ID, gps_str, &len);
    if(i2cFlag == I2C_SUCCESS)
    {
        printf("GPS on line\n\r");    
    }    
//	initRC();				// Initialize RC channel output pins
//    printf("RC Channels ready\n\r");

    printf("\n\r");
	DelayMs(2000);			// Wait a second
	clrLCD();				// Clear hello messages

}
