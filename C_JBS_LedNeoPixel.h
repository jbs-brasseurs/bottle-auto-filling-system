/* 
* C_JBS_LedNeoPixel.h
*
* Created: 13/04/2020 13:46:50
* Author: JB_Notebook
*/

#ifndef __C_JBS_LEDNEOPIXEL_H__
#define __C_JBS_LEDNEOPIXEL_H__

#include <Adafruit_NeoPixel.h>
#include <Wire.h>

const uint8_t U8_JBS_LED_NEO_PIXEL_DATA_PIN {6U};       // Data pin for Neo pixel led
const uint8_t U8_JBS_LED_NEO_PIXEL_NUMBER_LED {16U};	//{8U}; // Number of used led in chain

const uint32_t U32_JBS_RED_LED {Adafruit_NeoPixel::Color(0xFFU,0X00U,0x00U,0xFFU)};
const uint32_t U32_JBS_GREEN_LED {Adafruit_NeoPixel::Color(0x00U,0X20U,0x00U,0x10U)};
const uint32_t U32_JBS_ORANGE_LED {Adafruit_NeoPixel::Color(0x80U,0X40U,0x00U,0xFFU)};
const uint32_t U32_JBS_BLUE_LED {Adafruit_NeoPixel::Color(0x00U,0X00U,0x40U,0xFFU)};
const uint32_t U32_JBS_PINK_LED {Adafruit_NeoPixel::Color(0XE6U,0x00U,0x7EU,0xFFU)};
const uint32_t U32_JBS_OFF_LED {Adafruit_NeoPixel::Color(0x00U,0X00U,0x00U,0x00U)};

const uint32_t AU32_JBS_ColorMap[7] {U32_JBS_OFF_LED, U32_JBS_GREEN_LED, U32_JBS_GREEN_LED, U32_JBS_ORANGE_LED, U32_JBS_ORANGE_LED, U32_JBS_RED_LED, U32_JBS_RED_LED};

class C_JBS_LedNeoPixel: public Adafruit_NeoPixel
{
//variables
public:
protected:
private:

//functions
public:
	C_JBS_LedNeoPixel();
	~C_JBS_LedNeoPixel();

    void init();
protected:
private:
	C_JBS_LedNeoPixel( const C_JBS_LedNeoPixel &c );
	C_JBS_LedNeoPixel& operator=( const C_JBS_LedNeoPixel &c );

}; //C_JBS_LedNeoPixel

#endif //__C_JBS_LEDNEOPIXEL_H__
