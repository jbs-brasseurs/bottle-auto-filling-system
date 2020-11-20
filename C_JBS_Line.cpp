/* 
* C_JBS_Line.cpp
*
* Created: 13/04/2020 17:47:41
* Author: JB_Notebook
*/


#include "C_JBS_Line.h"

// default constructor
C_JBS_Line::C_JBS_Line():
    mu8_ValvePin{0u},
    mu8_BtCalPin{0u},
    mu8_CtPin{0u},
    mb_V{0u},
    mb_BtCal{1u},
    mb_BtCalLast{1u},
    mb_BtCalClick{0u},
    mb_Ct{1u},
    //mb_CtLast{1u},
    mb_BottleTaken{1u},
    men_AI_Color{E_LedColor_None},
    men_SI_Color{E_LedColor_None}
{
} //C_JBS_Line

// default destructor
C_JBS_Line::~C_JBS_Line()
{
} //~C_JBS_Line

void C_JBS_Line::Init(uint8_t u8_ValvePin, uint8_t u8_BtCalPin, uint8_t u8_CtPin)
{
    mu8_ValvePin = u8_ValvePin;
    pinMode(mu8_ValvePin, OUTPUT);
    mu8_BtCalPin = u8_BtCalPin;
    pinMode(mu8_BtCalPin, INPUT_PULLUP);
    mu8_CtPin = u8_CtPin;
    pinMode(mu8_CtPin, INPUT_PULLUP);
    Reset();
    WriteOutput();
}

void C_JBS_Line::Reset()
{
    mb_V = 0u;
    
    mb_BtCal = 1u;
    mb_BtCalLast = 1u;
    mb_BtCalClick = 0u;
    
    mb_Ct = 1u;
    //mb_CtLast = 1u;

    mb_BottleTaken = 1u;
    men_AI_Color = E_LedColor_None;
    men_SI_Color = E_LedColor_None;
}

void C_JBS_Line::UpdateInput()
{
    mb_BtCal = digitalRead(mu8_BtCalPin);
    if( mb_BtCal != mb_BtCalLast)
    {
        mb_BtCalLast = mb_BtCal;
        mb_BtCalClick = !mb_BtCal;
    }
    else
    {
        mb_BtCalClick = 0u;
    }
    
    mb_Ct = digitalRead(mu8_CtPin);
    mb_BottleTaken |= mb_Ct;
}


EN_LedColor C_JBS_Line::Manual()
{
    men_AI_Color = E_LedColor_Green;
    
    if (mb_Ct)                                  // If no bottle
    {
        men_SI_Color = E_LedColor_Red;              // Red indicator
        mb_V = 0u;                                  // Valve close
    }
    else                                        // else bottle present
    {
        if (!mb_BtCal)                               // If calibration button active
        {
            men_SI_Color = E_LedColor_Orange;           // Orange indicator
            mb_V = 1u;                                  // Valve open
        }
        else                                        // else button not pressed
        {
            men_SI_Color = E_LedColor_Green;            // Green indicator
            mb_V = 0u;                                  // Valve close
        }
    }
    return men_SI_Color;
}
    

void C_JBS_Line::SemiAuto()
{
    
}

void C_JBS_Line::SetValve(const bool b_Valve)
{
    mb_V = b_Valve;
}    

void C_JBS_Line::WriteOutput()
{
    digitalWrite(mu8_ValvePin, mb_V);
}