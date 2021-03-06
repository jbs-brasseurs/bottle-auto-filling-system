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
#include "string.h"

const uint8_t U8_JBS_NUMBER_LINES{4U};                          // 4 lines can be used
const uint32_t U32_JBS_CALIBRATION_START_PAUSE_MS{3000U};       // 3000ms are waited before calibration

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

C_JBS_Line m_Lines[U8_JBS_NUMBER_LINES];
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
    void Auto();
    void Cleaning();
    
    void WriteOutput();
    
    void SelectLine();

    int8_t GetBtCalPressed(const uint8_t u8_Line) const;
    int8_t GetBtResetPressed() const {return mb_BtReset;};
    int8_t GetBtPedalePressed() const {return mb_BtPedale;};

protected:
    bool CheckBottlesEmtyAndReady(const uint8_t u8_UsedLine) const;

private:
	C_JBS_FillingSystem( const C_JBS_FillingSystem &c );
	C_JBS_FillingSystem& operator=( const C_JBS_FillingSystem &c );

  uint16_t mu16_CyclePeriodMS;
  uint16_t mu16_FillingCounter;
  uint16_t mu16_ClickingCounter;
  uint16_t mu16_FilledBottleCounter;
  uint16_t mu16_FillingCalibrationTicks;
  uint16_t mu16_FillingTicks;

  uint16_t mu16_CleaningState;
  uint16_t mu16_FillingState;

  uint8_t mu8_UsedLineIndex;
  uint8_t mu8_CalibratedLine;
  
    //bool CleaningOperation(uint8_t u8_Line, uint32_t u32_Time, uint32_t& ru32_Timekeeper);
    
}; //C_JBS_FillingSystem

#endif //__C_JBS_FILLINGSYSTEM_H__
