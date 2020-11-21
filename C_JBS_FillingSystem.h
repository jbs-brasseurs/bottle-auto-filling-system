/* 
* C_JBS_FillingSystem.h
*
* Created: 15/04/2020 18:09:24
* Author: JB
*/


#ifndef __C_JBS_FILLINGSYSTEM_H__
#define __C_JBS_FILLINGSYSTEM_H__

#include "Arduino.h"
#include "C_JBS_LCD_I2C.h"
#include "C_JBS_LedNeoPixel.h"
#include "C_JBS_Line.h"

#define JBS_NUMBER_LINES 4

extern C_JBS_LCD_I2C g_Lcd;
extern C_JBS_LedNeoPixel g_Leds;

class C_JBS_FillingSystem
{
//variables
public:
protected:
private:
uint8_t mu8_BtResetPin;
bool mb_BtReset;
bool mb_BtResetLast;
bool mb_BtResetClick;

uint8_t mu8_BtPedalePin;
bool mb_BtPedale;
bool mb_BtPedaleLast;
bool mb_BtPedaleClick;

C_JBS_Line m_Lines[JBS_NUMBER_LINES];
C_JBS_LedNeoPixel* mp_Leds;

uint32_t mu32_TimeKeeper;
bool mb_CleaningActive;    
//functions
public:
	C_JBS_FillingSystem();
	~C_JBS_FillingSystem();
    
    void Init(const uint16_t u16_CyclePeriodMS, const uint8_t u8_ResetPin, const uint8_t u8_PedalePin, const uint8_t* pu8_ValvePin, const uint8_t* pu8_BtCalPin, const uint8_t* pu8_CtPin, C_JBS_LedNeoPixel* p_Leds);
    void Reset();
    
    void UpdateInput();
    
    void Manual();
    void SemiAuto();
    void Auto() {}
    void Cleaning();
    
    void WriteOutput();
    
    int8_t GetBtCalPressed(const uint8_t u8_Line) const;
    int8_t GetBtResetPressed() const {return mb_BtReset;};
    int8_t GetBtPedalePressed() const {return mb_BtPedale;};

protected:
    bool CheckBottlesEmtyAndReady(const uint8_t u8_UsedLine) const;

private:
	C_JBS_FillingSystem( const C_JBS_FillingSystem &c );
	C_JBS_FillingSystem& operator=( const C_JBS_FillingSystem &c );

  int16_t mu16_CyclePeriodMS;
  int16_t mu16_FillingCounter;
  int16_t mu16_ClickingCounter;
  int16_t mu16_FilledBottleCounter;
  int16_t mu16_FillingCalibrationTicks;
  int16_t mu16_FillingTicks;

  int16_t mu16_CleaningState;
  int16_t mu16_FillingState;
  int8_t mu8_NumberOfUsedLines;

    //bool CleaningOperation(uint8_t u8_Line, uint32_t u32_Time, uint32_t& ru32_Timekeeper);
    
}; //C_JBS_FillingSystem

#endif //__C_JBS_FILLINGSYSTEM_H__
