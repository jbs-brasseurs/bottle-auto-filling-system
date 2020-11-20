/* 
* C_JBS_Line.h
*
* Created: 13/04/2020 17:47:42
* Author: JB_Notebook
*/

#ifndef __C_JBS_LINE_H__
#define __C_JBS_LINE_H__

#include <Arduino.h>
#include "C_JBS_LCD_I2C.h"
#include "C_JBS_LedNeoPixel.h"

//extern C_JBS_LCD_I2C g_Lcd;
//extern C_JBS_LedNeoPixel g_Leds;

enum EN_LedColor
{
    E_LedColor_None,
    E_LedColor_Green,
    E_LedColor_GreenBlinking,
    E_LedColor_Orange,
    E_LedColor_OrangeBlinking,
    E_LedColor_Red,
    E_LedColor_RedBlinking
};
    
class C_JBS_Line
{
//variables
public:
protected:
private:
uint8_t mu8_ValvePin;
uint8_t mu8_BtCalPin;
uint8_t mu8_CtPin;

bool mb_V;

bool mb_BtCal;
bool mb_BtCalLast;
bool mb_BtCalClick; 

bool mb_Ct;
//bool mb_CtLast;

bool mb_BottleTaken;

EN_LedColor men_AI_Color;
EN_LedColor men_SI_Color;
//uint8_t mu8_SI_Blink;

//functions
public:
	C_JBS_Line();
	~C_JBS_Line();

    void Init(uint8_t u8_ValvePin, uint8_t u8_BtCalPin, uint8_t u8_CtPin);
    void Reset();
    
    void UpdateInput();
    
    EN_LedColor Manual();
    void SemiAuto();
    void Auto() {}
        
    void SetValve(const bool b_Valve);
    
    void WriteOutput();
    
    bool GetBtCal() const {return mb_BtCal;}
    bool GetV() const {return mb_V;}
    
protected:

private:
	C_JBS_Line( const C_JBS_Line &c );
	C_JBS_Line& operator=( const C_JBS_Line &c );

}; //C_JBS_Line

#endif //__C_JBS_LINE_H__
