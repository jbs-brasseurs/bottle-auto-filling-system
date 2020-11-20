/* 
* C_JBS_LedNeoPixel.cpp
*
* Created: 13/04/2020 13:46:50
* Author: JB_Notebook
*/

#include "C_JBS_LedNeoPixel.h"

// default constructor
C_JBS_LedNeoPixel::C_JBS_LedNeoPixel(): Adafruit_NeoPixel(U8_JBS_LED_NEO_PIXEL_NUMBER_LED, U8_JBS_LED_NEO_PIXEL_DATA_PIN, NEO_RGB + NEO_KHZ800)
{
} //C_JBS_LedNeoPixel

// default destructor
C_JBS_LedNeoPixel::~C_JBS_LedNeoPixel()
{
} //~C_JBS_LedNeoPixel



void C_JBS_LedNeoPixel::init()
{
  C_JBS_LedNeoPixel::begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  C_JBS_LedNeoPixel::clear(); // Set all pixel colors to 'off'
}