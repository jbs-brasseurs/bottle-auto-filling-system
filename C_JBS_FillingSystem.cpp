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
    
    for(uint8_t u8_Index{0u}; u8_Index < U8_JBS_NUMBER_LINES; u8_Index++)
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
    
    for(uint8_t u8_Index{0u}; u8_Index < U8_JBS_NUMBER_LINES; u8_Index++)
    {
        m_Lines[u8_Index].Reset();
    }
    
    g_Leds.init();

    mb_CleaningActive = 0u;
    mu8_NumberOfUsedLines = 0U;
    mu16_FillingCalibrationTicks = 0U; 
    mu8_CalibratedLine = 0U;
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
    
    for(uint8_t u8_Index{0u}; u8_Index < U8_JBS_NUMBER_LINES; u8_Index++)
    {
        m_Lines[u8_Index].UpdateInput();
    }            
}

void C_JBS_FillingSystem::Manual()
{
    for(uint8_t u8_Index{0u}; u8_Index < U8_JBS_NUMBER_LINES; u8_Index++)
    {
        EN_LedColor en_LedColor {m_Lines[u8_Index].Manual()};
        g_Leds.setPixelColor(u8_Index*2+1, AU32_JBS_ColorMap[en_LedColor]);
    }

    for(uint8_t u8_I{0U}; u8_I < U8_JBS_NUMBER_LINES; u8_I++)
    {
      if (m_Lines[u8_I].GetV())
      {
        mu8_CalibratedLine = u8_I+1U;
        mu16_FillingCounter++;
        String test{mu8_CalibratedLine};
        test.concat(" ");
        float f32_OpenTime{mu16_FillingCounter*mu16_CyclePeriodMS/1000.0F};
        test.concat(String(f32_OpenTime,2U));
        g_Lcd.ClearPrintL2(test.c_str());
        break;
      } 
      mu8_CalibratedLine=0U;
    }
    if (mu8_NumberOfUsedLines != mu8_CalibratedLine)
    {
      mu8_NumberOfUsedLines = mu8_CalibratedLine;
      mu16_FillingCounter=0U;
    }
}

void C_JBS_FillingSystem::SemiAuto()
{
  if (mu8_NumberOfUsedLines>0U && mu16_FillingCalibrationTicks>0U && mb_BtPedaleClick && CheckBottlesEmtyAndReady(mu8_NumberOfUsedLines))
  {

    mu16_FillingTicks = mu16_FillingCalibrationTicks + (mu16_FilledBottleCounter/5U); // Calibrate ticks to add for lower perssue
    mu16_FillingCounter = 0U;
    mu16_FillingState = 1U;
  }

  if (mu16_FillingState == 1U)
  {
    if (mu16_FillingCounter>=mu16_FillingTicks || !CheckBottlesEmtyAndReady(mu8_NumberOfUsedLines))
    { 
      mu16_FillingCounter=0U;
      mu16_FillingState=0U;
      for(uint8_t u8_I{0U}; u8_I < mu8_NumberOfUsedLines; u8_I++)
      {
        m_Lines[u8_I].SetValve(false);
        m_Lines[u8_I].SetBottleFull();
      }
      mu16_FilledBottleCounter+=mu8_NumberOfUsedLines;
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
    // Calibration
    if (mu8_CalibratedLine>0U)                            // IF calibatration active
    {
      if(!CheckBottlesEmtyAndReady(mu8_CalibratedLine))      // IF button click release
      {
        for(uint8_t u8_I{0U}; u8_I < mu8_NumberOfUsedLines; u8_I++)
        {
          m_Lines[u8_I].SetBottleFull();
          mu16_FilledBottleCounter+=mu8_NumberOfUsedLines;
          m_Lines[u8_I].SetValve(false);
        }
        mu8_CalibratedLine = 0U;                              // Reset Calibrated line
        mu16_ClickingCounter = 0U;                            // Reset Click counter
        mu8_NumberOfUsedLines = 0U;
      }
      else
      {
        if (m_Lines[mu8_CalibratedLine-1U].GetBtCal()!=0U)
        {
          if (mu16_ClickingCounter>= U32_JBS_CALIBRATION_START_PAUSE_MS/mu16_CyclePeriodMS)
          {
            mu16_FilledBottleCounter = 0U;
            mu16_FillingCalibrationTicks = mu16_ClickingCounter - U32_JBS_CALIBRATION_START_PAUSE_MS/mu16_CyclePeriodMS;
            Serial.println("Used Line:");
            Serial.println(mu8_NumberOfUsedLines);
            Serial.println("Calibration ticks:");
            Serial.println(mu16_FillingCalibrationTicks);
            for(uint8_t u8_I{0U}; u8_I < mu8_NumberOfUsedLines; u8_I++)
            {
              m_Lines[u8_I].SetBottleFull();
              mu16_FilledBottleCounter+=mu8_NumberOfUsedLines;
            }
          }
          else
          {
            //
          }
          mu8_CalibratedLine = 0U;                              // Reset Calibrated line
          mu16_ClickingCounter = 0U ;                           // Reset Click counter
          mu16_FillingState = 0U;
          for(uint8_t u8_I{0U}; u8_I < mu8_NumberOfUsedLines; u8_I++)
          {
            m_Lines[u8_I].SetValve(false);
          }
        }
        else
        {
        mu16_ClickingCounter++;
        Serial.println("Calibration ticks counter:");
        Serial.println(mu16_ClickingCounter);
        }
      }
    }
    else                                                  // Calibration inactive CHECK
    {
      for(uint8_t u8_Line{0U}; u8_Line<U8_JBS_NUMBER_LINES; u8_Line++)
      {
        if(m_Lines[u8_Line].GetBtCal()==0U)
        {
          mu8_CalibratedLine = u8_Line+1U;
        }
      }
      Serial.println("Calibrated Line:");
      Serial.println(mu8_CalibratedLine);
    }

    if (mu8_CalibratedLine > 0U && mu16_ClickingCounter >= U32_JBS_CALIBRATION_START_PAUSE_MS/mu16_CyclePeriodMS)
    {
      mu8_NumberOfUsedLines = mu8_CalibratedLine;
      for(uint8_t u8_I{0U}; u8_I < mu8_NumberOfUsedLines; u8_I++)
      {
        m_Lines[u8_I].SetValve(true);
      }
    }
    else
    {
      for(uint8_t u8_I{0U}; u8_I < U8_JBS_NUMBER_LINES; u8_I++)
      {
         m_Lines[U8_JBS_NUMBER_LINES].SetValve(false);
      }
    }
  }  
  for(uint8_t u8_Index{0u}; u8_Index < U8_JBS_NUMBER_LINES; u8_Index++)
  {
   uint32_t u32_LedColor {(m_Lines[u8_Index].GetBottleEmtyAndReady() ? U32_JBS_GREEN_LED : U32_JBS_RED_LED)};
    if ( m_Lines[u8_Index].GetV())
    {
      u32_LedColor = U32_JBS_ORANGE_LED;
    }
    g_Leds.setPixelColor(u8_Index*2+1, u32_LedColor);
  }

  uint8_t LineInUse{max(mu8_NumberOfUsedLines, mu8_CalibratedLine)};
  for(uint8_t u8_Index{0u}; u8_Index < LineInUse; u8_Index++)
  {
    g_Leds.setPixelColor(u8_Index*2, U32_JBS_GREEN_LED);
  }
  for(uint8_t u8_Index{LineInUse}; u8_Index < U8_JBS_NUMBER_LINES; u8_Index++)
  {
    g_Leds.setPixelColor(u8_Index*2, U32_JBS_ORANGE_LED);
  }
}

void C_JBS_FillingSystem::Auto()
{
  if (mu8_NumberOfUsedLines>0U && mu16_FillingCalibrationTicks>0U && CheckBottlesEmtyAndReady(mu8_NumberOfUsedLines) && mu16_FillingState == 0U &&  mu8_CalibratedLine == 0U)
  {
    mu16_FillingTicks = mu16_FillingCalibrationTicks + (mu16_FilledBottleCounter/5U); // Calibrate ticks to add for lower perssue
    mu16_FillingCounter = 0U;
    mu16_FillingState = 1U;
  }

  if (mu16_FillingState == 1U)
  {
    if (mu16_FillingCounter>=mu16_FillingTicks || !CheckBottlesEmtyAndReady(mu8_NumberOfUsedLines))
    { 
      mu16_FillingCounter=0U;
      mu16_FillingState=0U;
      for(uint8_t u8_I{0U}; u8_I < mu8_NumberOfUsedLines; u8_I++)
      {
        m_Lines[u8_I].SetValve(false);
        m_Lines[u8_I].SetBottleFull();
      }
        
      mu16_FilledBottleCounter+=mu8_NumberOfUsedLines;
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
    // Calibration
    if (mu8_CalibratedLine>0U)                            // IF calibatration active
    {
      if(!CheckBottlesEmtyAndReady(mu8_CalibratedLine))      // IF button click release
      {
          for(uint8_t u8_I{0U}; u8_I < mu8_NumberOfUsedLines; u8_I++)
          {
            m_Lines[u8_I].SetBottleFull();
            mu16_FilledBottleCounter+=mu8_NumberOfUsedLines;
            m_Lines[u8_I].SetValve(false);
          }
        mu8_CalibratedLine = 0U;                              // Reset Calibrated line
        mu16_ClickingCounter = 0U;                            // Reset Click counter
        mu8_NumberOfUsedLines = 0U;
      }
      else
      {
        if (m_Lines[mu8_CalibratedLine-1U].GetBtCal()!=0U)
        {
          Serial.println("END CALL");          //Reset();
          if (mu16_ClickingCounter>= U32_JBS_CALIBRATION_START_PAUSE_MS/mu16_CyclePeriodMS)
          {
            mu16_FilledBottleCounter = 0U;
            mu16_FillingCalibrationTicks = mu16_ClickingCounter - U32_JBS_CALIBRATION_START_PAUSE_MS/mu16_CyclePeriodMS;
            Serial.println("Used Line:");
            Serial.println(mu8_NumberOfUsedLines);
            Serial.println("Calibration ticks:");
            Serial.println(mu16_FillingCalibrationTicks);
            for(uint8_t u8_I{0U}; u8_I < mu8_NumberOfUsedLines; u8_I++)
            {
              m_Lines[u8_I].SetBottleFull();
              mu16_FilledBottleCounter+=mu8_NumberOfUsedLines;
            }
          }
          else
          {
            //
          }
          mu8_CalibratedLine = 0U;                              // Reset Calibrated line
          mu16_ClickingCounter = 0U ;                           // Reset Click counter
          mu16_FillingState = 0U;
          for(uint8_t u8_I{0U}; u8_I < mu8_NumberOfUsedLines; u8_I++)
          {
            m_Lines[u8_I].SetValve(false);
          }
        }
        else
        {
        mu16_ClickingCounter++;
        Serial.println("Calibration ticks counter:");
        Serial.println(mu16_ClickingCounter);
        }
      }
    }
    else                                                  // Calibration inactive CHECK
    {
      for(uint8_t u8_Line{0U}; u8_Line<U8_JBS_NUMBER_LINES; u8_Line++)
      {
        if(m_Lines[u8_Line].GetBtCal()==0U)
        {
          mu8_CalibratedLine = u8_Line+1U;
        }
      }
      Serial.println("Calibrated Line:");
      Serial.println(mu8_CalibratedLine);
    }

    if (mu8_CalibratedLine > 0U && mu16_ClickingCounter >= U32_JBS_CALIBRATION_START_PAUSE_MS/mu16_CyclePeriodMS)
    {
      mu8_NumberOfUsedLines = mu8_CalibratedLine;
      for(uint8_t u8_I{0U}; u8_I < mu8_NumberOfUsedLines; u8_I++)
      {
        m_Lines[u8_I].SetValve(true);
      }
    }
    else
    {
      for(uint8_t u8_I{0U}; u8_I < U8_JBS_NUMBER_LINES; u8_I++)
      {
         m_Lines[U8_JBS_NUMBER_LINES].SetValve(false);
      }
    }
  }  
  for(uint8_t u8_Index{0u}; u8_Index < U8_JBS_NUMBER_LINES; u8_Index++)
  {
   uint32_t u32_LedColor {(m_Lines[u8_Index].GetBottleEmtyAndReady() ? U32_JBS_GREEN_LED : U32_JBS_RED_LED)};
    if ( m_Lines[u8_Index].GetV())
    {
      u32_LedColor = U32_JBS_ORANGE_LED;
    }
    g_Leds.setPixelColor(u8_Index*2+1, u32_LedColor);
  }

  uint8_t LineInUse{max(mu8_NumberOfUsedLines, mu8_CalibratedLine)};
  for(uint8_t u8_Index{0u}; u8_Index < LineInUse; u8_Index++)
  {
    g_Leds.setPixelColor(u8_Index*2, U32_JBS_GREEN_LED);
  }
  for(uint8_t u8_Index{LineInUse}; u8_Index < U8_JBS_NUMBER_LINES; u8_Index++)
  {
    g_Leds.setPixelColor(u8_Index*2, U32_JBS_ORANGE_LED);
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
        if (mu16_FillingCounter >= 10000/mu16_CyclePeriodMS)
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
        if (mu16_FillingCounter >= 1000/mu16_CyclePeriodMS)
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
        if (mu16_FillingCounter >= 10000/mu16_CyclePeriodMS)
        {
          mu16_CleaningState++;
          mu16_FillingCounter = 0U;
        }
      }      
      else if (mu16_CleaningState == 3U)
      {
        m_Lines[1].SetValve(0);
        g_Lcd.ClearPrintL2("1X 2X 3X 4X 4/10");
        mp_Leds->setPixelColor(3,U32_JBS_OFF_LED);
        if (mu16_FillingCounter >= 1000/mu16_CyclePeriodMS)
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
        if (mu16_FillingCounter >= 10000/mu16_CyclePeriodMS)
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
        if (mu16_FillingCounter >= 1000/mu16_CyclePeriodMS)
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
        if (mu16_FillingCounter >= 10000/mu16_CyclePeriodMS)
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
        if (mu16_FillingCounter >= 1000/mu16_CyclePeriodMS)
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
        if (mu16_FillingCounter >= 10000/mu16_CyclePeriodMS)
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
        if (mu16_FillingCounter >= 1000/mu16_CyclePeriodMS)
        {
          mu16_CleaningState++;
          mu16_FillingCounter = 0U;
        }
      }
      else if (mu16_CleaningState == 10U)
      {
        m_Lines[0].SetValve(1);
        m_Lines[1].SetValve(1);
        m_Lines[2].SetValve(1);
        m_Lines[3].SetValve(1);
        mp_Leds->setPixelColor(1,U32_JBS_ORANGE_LED);
        mp_Leds->setPixelColor(3,U32_JBS_ORANGE_LED);
        mp_Leds->setPixelColor(5,U32_JBS_ORANGE_LED);
        mp_Leds->setPixelColor(7,U32_JBS_ORANGE_LED);
        g_Lcd.ClearPrintL2("1O 2O 3O 4O 10/10");
        if (mu16_FillingCounter >= 200/mu16_CyclePeriodMS)
        {
          mu16_CleaningState++;
          mu16_FillingCounter = 0U;
        }
      }      
      else if (mu16_CleaningState == 11U)
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
        if (mu16_FillingCounter >= 200/mu16_CyclePeriodMS)
        {
          mu16_CleaningState++;
          mu16_FillingCounter = 0U;
        }
      }
      else if (mu16_CleaningState == 12U)
      {
        m_Lines[0].SetValve(1);
        m_Lines[1].SetValve(1);
        m_Lines[2].SetValve(1);
        m_Lines[3].SetValve(1);
        mp_Leds->setPixelColor(1,U32_JBS_ORANGE_LED);
        mp_Leds->setPixelColor(3,U32_JBS_ORANGE_LED);
        mp_Leds->setPixelColor(5,U32_JBS_ORANGE_LED);
        mp_Leds->setPixelColor(7,U32_JBS_ORANGE_LED);
        g_Lcd.ClearPrintL2("1O 2O 3O 4O 10/10");
        if (mu16_FillingCounter >= 200/mu16_CyclePeriodMS)
        {
          mu16_CleaningState++;
          mu16_FillingCounter = 0U;
        }
      }      
      else if (mu16_CleaningState == 13U)
      {
        m_Lines[0].SetValve(0);
        m_Lines[1].SetValve(0);
        m_Lines[2].SetValve(0);
        m_Lines[3].SetValve(0);
        mp_Leds->setPixelColor(1,U32_JBS_OFF_LED);
        mp_Leds->setPixelColor(3,U32_JBS_OFF_LED);
        mp_Leds->setPixelColor(5,U32_JBS_OFF_LED);
        mp_Leds->setPixelColor(7,U32_JBS_OFF_LED);
        mb_CleaningActive = 0U;
      }
      mu16_FillingCounter++;
    }
    else
    {
      for(uint8_t u8_Index{0u}; u8_Index < U8_JBS_NUMBER_LINES; u8_Index++)
      {
        m_Lines[u8_Index].SetValve(m_Lines[u8_Index].GetBtCal()==0U);
        mp_Leds->setPixelColor(u8_Index*2+1, m_Lines[u8_Index].GetBtCal()==0U ? U32_JBS_ORANGE_LED : U32_JBS_OFF_LED);
        
        g_Lcd.ClearPrintL2("Manual Clean");
      }
    } 
}

void C_JBS_FillingSystem::WriteOutput()
{
    mp_Leds->show();
    for(uint8_t u8_Index{0u}; u8_Index < U8_JBS_NUMBER_LINES; u8_Index++)
    {
        m_Lines[u8_Index].WriteOutput();
    }
}

int8_t C_JBS_FillingSystem:: GetBtCalPressed(const uint8_t u8_Line) const
{
    int8_t i8_BtCalPressed{-1};
    if(u8_Line>0 && u8_Line<=U8_JBS_NUMBER_LINES)
    {
        i8_BtCalPressed = m_Lines[u8_Line-1].GetBtCal();
    }    
    
    return i8_BtCalPressed;
}
