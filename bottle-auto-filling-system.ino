#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include "LiquidCrystal_I2C.h"

#include "IO_Mapping.h"
#include "C_JBS_FillingSystem.h"

/* Global Variable */

// Program variable
unsigned int gmachineState=0;
// 0 Wait for bottle
// 1 Filling ready
// 2 Filling
// 3 Remove bottle
 
uint8_t gu8_Mode{0u};

// 0 UserChoice
// 1 Manual
// 2 Semi Auto
// 3 Auto
// 4 Cleaning

unsigned int gMachineMode=1;   // number of used valve
unsigned int gu32TimingFilling=20; // 20 ms cycle time

C_JBS_LCD_I2C g_Lcd;//(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
C_JBS_LedNeoPixel g_Leds;
C_JBS_FillingSystem g_FillingSystem;


void setup() {
  // put your setup code here, to run once:
    g_Lcd.Init();                      // initialize the LCD
    g_Lcd.ClearPrint(g_Lcd.MAC8_INIT);
  
    g_Leds.init();
    g_Leds.show();   // Send the updated pixel colors to the hardware.

    Serial.begin(115200);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
    }
    Serial.println("Program start:");

    g_FillingSystem.Init(100, U8_PIN_BT_RESET, U8_PIN_BT_PEDALE, AU8_PIN_V, AU8_PIN_BT_CAL, AU8_PIN_CT, &g_Leds);
  
  
  pinMode(52, INPUT_PULLUP);
  Serial.write("Start");
  uint32_t u32_LoopTime{0U};
        
  uint8_t u8_Last_Mode{0xffU};
  uint8_t cpt{45U};
  
  for (;;)
  {
    u32_LoopTime = millis();
    //////

    //Input
    g_FillingSystem.UpdateInput();

    if (g_FillingSystem.GetBtResetPressed() ==0)
    {
      g_FillingSystem.Reset();
      gu8_Mode = 0;
    }
    if (u8_Last_Mode!=gu8_Mode)
    {
      if (gu8_Mode == 0U)
      {
        g_Lcd.ClearPrint(g_Lcd.MAC8_MODE_CHOOSE);
      }
      
      else if (gu8_Mode == 1U)
      {
        g_Lcd.ClearPrint("Manual 1");
        g_Leds.setPixelColor(0,U32_JBS_GREEN_LED);
        g_Leds.setPixelColor(2,U32_JBS_GREEN_LED);
        g_Leds.setPixelColor(4,U32_JBS_GREEN_LED);
        g_Leds.setPixelColor(6,U32_JBS_GREEN_LED);
      }
      
      else if (gu8_Mode == 2U)
      {
        g_Lcd.ClearPrint("Semi Auto 2");
      }

      else if (gu8_Mode == 3U)
      {
        g_Lcd.ClearPrint("Auto 3");
      }
      else if (gu8_Mode == 4U)
      {
        g_Lcd.ClearPrint("Cleaning 4");
      }
      
      u8_Last_Mode = gu8_Mode;
    }
    if (gu8_Mode == 0U)
    {      
      MainMenuHandling();
    }
    else if (gu8_Mode == 1U)
    {      
      g_FillingSystem.Manual();
      g_FillingSystem.WriteOutput();
    }
    else if (gu8_Mode == 4U)
    {      
      g_FillingSystem.Cleaning();
      g_FillingSystem.WriteOutput();
    }
    
    cpt++;
    
    
    //////
    while (millis()<u32_LoopTime+100U);
  }
}

void MainMenuHandling()
{
  if(g_FillingSystem.GetBtCalPressed(1U)==0)
      {
        g_Lcd.ClearPrint("Mode 1");
        gu8_Mode = 1U;
      }
      else if(g_FillingSystem.GetBtCalPressed(2U)==0)
      {
        gu8_Mode = 2U;
      }
      
      else if(g_FillingSystem.GetBtCalPressed(3U)==0)
      {
        gu8_Mode = 3U;
      }
      else if(g_FillingSystem.GetBtCalPressed(4U)==0)
      {
        gu8_Mode = 4U;
      }
}

void loop() {
  // put your main code here, to run repeatedly:

}
