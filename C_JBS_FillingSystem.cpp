/* 
* C_JBS_FillingSystem.cpp
*
* Created: 15/04/2020 18:09:24
* Author: JB
*/


#include "C_JBS_FillingSystem.h"

// default constructor
C_JBS_FillingSystem::C_JBS_FillingSystem()
{
} //C_JBS_FillingSystem

// default destructor
C_JBS_FillingSystem::~C_JBS_FillingSystem()
{
} //~C_JBS_FillingSystem

void C_JBS_FillingSystem::Init(const uint16_t u16_CyclePeriodMS, const uint8_t u8_ResetPin, const uint8_t u8_PedalePin, const uint8_t* pu8_ValvePin, const uint8_t* pu8_BtCalPin, const uint8_t* pu8_CtPin, C_JBS_LedNeoPixel* p_Leds)
{
    mu16_CyclePeriodMS = u16_CyclePeriodMS;
    mu8_BtResetPin = u8_ResetPin;
    pinMode(mu8_BtResetPin, INPUT_PULLUP);
    
    mu8_BtPedalePin = u8_PedalePin;
    pinMode(mu8_BtPedalePin, INPUT_PULLUP);
    mp_Leds = p_Leds;
    
    for(uint8_t u8_Index{0u}; u8_Index < JBS_NUMBER_LINES; u8_Index++)
    {
        m_Lines[u8_Index].Init(*pu8_ValvePin, *pu8_BtCalPin, *pu8_CtPin);
        pu8_ValvePin++;
        pu8_BtCalPin++;
        pu8_CtPin++;
    }
}

void C_JBS_FillingSystem::Reset()
{
    mb_BtPedale = 1u;
    mb_BtPedaleLast = 1u;
    mb_BtPedaleClick = 0u;

    mb_BtReset = 1u;
    mb_BtResetLast = 1u;
    mb_BtResetClick = 0u;
    
    for(uint8_t u8_Index{0u}; u8_Index < JBS_NUMBER_LINES; u8_Index++)
    {
        m_Lines[u8_Index].Reset();
    }
    
    g_Leds.init();

    mb_CleaningActive = 0u;
    WriteOutput();
}

void C_JBS_FillingSystem::UpdateInput()
{
    mb_BtReset = digitalRead(mu8_BtResetPin);
    if( mb_BtReset != mb_BtResetLast)
    {
        mb_BtResetLast = mb_BtReset;
        mb_BtResetClick = !mb_BtReset;
    }
    else
    {
        mb_BtPedaleClick = 0u;
    }
    
    mb_BtPedale = digitalRead(mu8_BtPedalePin);
    if( mb_BtPedale != mb_BtPedaleLast)
    {
        mb_BtPedaleLast = mb_BtPedale;
        mb_BtPedaleClick = !mb_BtPedale;
    }
    else
    {
        mb_BtPedaleClick = 0u;
    }
    
    for(uint8_t u8_Index{0u}; u8_Index < JBS_NUMBER_LINES; u8_Index++)
    {
        m_Lines[u8_Index].UpdateInput();
    }            
}


void C_JBS_FillingSystem::Manual()
{
    for(uint8_t u8_Index{0u}; u8_Index < JBS_NUMBER_LINES; u8_Index++)
    {
        EN_LedColor en_LedColor {m_Lines[u8_Index].Manual()};
        g_Leds.setPixelColor(u8_Index*2+1, AU32_JBS_ColorMap[en_LedColor]);
    }
}


void C_JBS_FillingSystem::SemiAuto()
{

  if (m_Lines[3].GetBtCalClick() && CheckBottlesEmtyAndReady(4))
  {
    //Callib 4
    mu8_NumberOfUsedLines = 4;
    mu16_FillingState = 2;
    mu16_ClickingCounter = 0;
    mu16_FillingTicks = mu16_FillingCalibrationTicks + (mu16_FilledBottleCounter/5); // Calibrate ticks to add for lower perssue

  }
  else if(mu8_NumberOfUsedLines == 4 && m_Lines[3].GetBtCal() == 0)
  {
    mu16_ClickingCounter++;
  }
  else if( mu16_ClickingCounter > 3000/mu16_CyclePeriodMS)
  {
    mu16_FillingState = 2;
    if(m_Lines[3].GetBtCal() == 1 && m_Lines[3].GetBtCalLast() == 0)
    {
      mu16_FillingState = 0;
      mu16_FilledBottleCounter = 0;
      mu16_FillingCalibrationTicks = mu16_ClickingCounter;
    }
  }


    if (mu16_FillingState == 1)
    {
      for(uint8_t u8_I{0U}; u8_I < mu8_NumberOfUsedLines; u8_I++)
      {
        m_Lines[u8_I].SetValve(true);
      }
    }
    else if (mu16_FillingState == 2)
    {
      if (mu16_FillingCounter>=mu16_FillingTicks)
      { 
        //Serial.println("Valve Closed ");
        mu16_FillingCounter=0;
        mu16_FillingState=0;
        for(uint8_t u8_I{0U}; u8_I < mu8_NumberOfUsedLines; u8_I++)
        {
          m_Lines[u8_I].SetValve(false);
          m_Lines[u8_I].SetBottleFull();
        }
        
        mu16_FilledBottleCounter+=mu8_NumberOfUsedLines;
        //Serial.print("Number of filled bottles: ");
        //Serial.println(NumberFilledBottle);
      }
      else
      {
        mu16_FillingCounter++;
        for(uint8_t u8_I{0U}; u8_I < mu8_NumberOfUsedLines; u8_I++)
        {
          m_Lines[u8_I].SetValve(true);
        }
      }
      
      
    }
    else
    {
      for(uint8_t u8_I{0U}; u8_I < mu8_NumberOfUsedLines; u8_I++)
      {
         m_Lines[u8_I].SetValve(false);
      }
    }


}

bool C_JBS_FillingSystem::CheckBottlesEmtyAndReady(const uint8_t u8_UsedLine) const
{
  bool b_Result{true};

  for(uint8_t u8_I{0U}; u8_I < u8_UsedLine; u8_I++)
  {
    b_Result &= m_Lines[u8_I].GetBottleEmtyAndReady();
  }

  return b_Result;
}

void C_JBS_FillingSystem::Cleaning()
{
    if(mb_BtPedaleClick)
    {
      mb_CleaningActive ^= mb_BtPedaleClick;
      mu16_FillingCounter = 0U;
      mu16_CleaningState = 0U;
      g_Lcd.ClearPrintL2("1X 2X 3X 4X");
      mp_Leds->setPixelColor(0,U32_JBS_GREEN_LED);
      mp_Leds->setPixelColor(2,U32_JBS_GREEN_LED);
      mp_Leds->setPixelColor(4,U32_JBS_GREEN_LED);
      mp_Leds->setPixelColor(6,U32_JBS_GREEN_LED);
      
      mp_Leds->setPixelColor(1,U32_JBS_OFF_LED);
      mp_Leds->setPixelColor(3,U32_JBS_OFF_LED);
      mp_Leds->setPixelColor(5,U32_JBS_OFF_LED);
      mp_Leds->setPixelColor(7,U32_JBS_OFF_LED);
    }
    
    if(mb_CleaningActive)
    {
      if (mu16_CleaningState == 0U)
      {
        m_Lines[0].SetValve(1);
        g_Lcd.ClearPrintL2("1O 2X 3X 4X 1/10");
        mp_Leds->setPixelColor(1,U32_JBS_ORANGE_LED);
        if (mu16_FillingCounter >= 5000/mu16_CyclePeriodMS)
        {
          mu16_CleaningState++;
          mu16_FillingCounter = 0U;
        }
      }
      else if (mu16_CleaningState == 1U)
      {
        m_Lines[0].SetValve(0);       
        g_Lcd.ClearPrintL2("1X 2X 3X 4X 2/10");
        mp_Leds->setPixelColor(1,U32_JBS_OFF_LED);
        if (mu16_FillingCounter >= 5000/mu16_CyclePeriodMS)
        {
          mu16_CleaningState++;
          mu16_FillingCounter = 0U;
        }
      }
      else if (mu16_CleaningState == 2U)
      {
        m_Lines[1].SetValve(1);
        g_Lcd.ClearPrintL2("1X 2O 3X 4X 3/10");
        mp_Leds->setPixelColor(3,U32_JBS_ORANGE_LED);
        if (mu16_FillingCounter >= 5000/mu16_CyclePeriodMS)
        {
          mu16_CleaningState++;
          mu16_FillingCounter = 0U;
        }
      }      
      else if (mu16_CleaningState == 3U)
      {
        m_Lines[1].SetValve(0);
        g_Lcd.ClearPrintL2("1X 2X 3X 4X  4/10");
        mp_Leds->setPixelColor(3,U32_JBS_OFF_LED);
        if (mu16_FillingCounter >= 5000/mu16_CyclePeriodMS)
        {
          mu16_CleaningState++;
          mu16_FillingCounter = 0U;
        }
      }
      else if (mu16_CleaningState == 4U)
      {
        m_Lines[2].SetValve(1);
        g_Lcd.ClearPrintL2("1X 2X 3O 4X 5/10");
        mp_Leds->setPixelColor(5,U32_JBS_ORANGE_LED);
        if (mu16_FillingCounter >= 5000/mu16_CyclePeriodMS)
        {
          mu16_CleaningState++;
          mu16_FillingCounter = 0U;
        }
      }      
      else if (mu16_CleaningState == 5U)
      {
        m_Lines[2].SetValve(0);
        g_Lcd.ClearPrintL2("1X 2X 3X 4X 6/10");
        mp_Leds->setPixelColor(5,U32_JBS_OFF_LED);
        if (mu16_FillingCounter >= 5000/mu16_CyclePeriodMS)
        {
          mu16_CleaningState++;
          mu16_FillingCounter = 0U;
        }
      }
      else if (mu16_CleaningState == 6U)
      {
        m_Lines[3].SetValve(1);
        g_Lcd.ClearPrintL2("1X 2X 3X 4O 7/10");
        mp_Leds->setPixelColor(7,U32_JBS_ORANGE_LED);
        if (mu16_FillingCounter >= 5000/mu16_CyclePeriodMS)
        {
          mu16_CleaningState++;
          mu16_FillingCounter = 0U;
        }
      }      
      else if (mu16_CleaningState == 7U)
      {
        m_Lines[3].SetValve(0);
        g_Lcd.ClearPrintL2("1X 2X 3X 4X 8/10");
        mp_Leds->setPixelColor(7,U32_JBS_OFF_LED);
        if (mu16_FillingCounter >= 5000/mu16_CyclePeriodMS)
        {
          mu16_CleaningState++;
          mu16_FillingCounter = 0U;
        }
      }
      else if (mu16_CleaningState == 8U)
      {
        m_Lines[0].SetValve(1);
        m_Lines[1].SetValve(1);
        m_Lines[2].SetValve(1);
        m_Lines[3].SetValve(1);
        mp_Leds->setPixelColor(1,U32_JBS_ORANGE_LED);
        mp_Leds->setPixelColor(3,U32_JBS_ORANGE_LED);
        mp_Leds->setPixelColor(5,U32_JBS_ORANGE_LED);
        mp_Leds->setPixelColor(7,U32_JBS_ORANGE_LED);

        g_Lcd.ClearPrintL2("1O 2O 3O 4O 9/10");
        if (mu16_FillingCounter >= 5000/mu16_CyclePeriodMS)
        {
          mu16_CleaningState++;
          mu16_FillingCounter = 0U;
        }
      }      
      else if (mu16_CleaningState == 9U)
      {
        m_Lines[0].SetValve(0);
        m_Lines[1].SetValve(0);
        m_Lines[2].SetValve(0);
        m_Lines[3].SetValve(0);
        mp_Leds->setPixelColor(1,U32_JBS_OFF_LED);
        mp_Leds->setPixelColor(3,U32_JBS_OFF_LED);
        mp_Leds->setPixelColor(5,U32_JBS_OFF_LED);
        mp_Leds->setPixelColor(7,U32_JBS_OFF_LED);
        g_Lcd.ClearPrintL2("1X 2X 3X 4X10/10");
        if (mu16_FillingCounter >= 5000/mu16_CyclePeriodMS)
        {
          mu16_CleaningState++;
          mu16_FillingCounter = 0U;
        }
      }
      mu16_FillingCounter++;
    }
    else
    {
      Reset();
    }
}

void C_JBS_FillingSystem::WriteOutput()
{
    mp_Leds->show();
    for(uint8_t u8_Index{0u}; u8_Index < JBS_NUMBER_LINES; u8_Index++)
    {
        m_Lines[u8_Index].WriteOutput();
    }
}

int8_t C_JBS_FillingSystem:: GetBtCalPressed(const uint8_t u8_Line) const
{
    int8_t i8_BtCalPressed{-1};
    if(u8_Line>0 && u8_Line<=JBS_NUMBER_LINES)
    {
        i8_BtCalPressed = m_Lines[u8_Line-1].GetBtCal();
    }    
    
    return i8_BtCalPressed;
}
