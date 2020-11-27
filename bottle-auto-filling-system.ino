#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include "LiquidCrystal_I2C.h"

#include "IO_Mapping.h"
#include "C_JBS_FillingSystem.h"

/* Global Variable */
// Global variable begin with g 

// Program variable
unsigned int gmachineState{0U};
// Possible states
//   0 Wait for bottle
//   1 Filling ready
//   2 Filling
//   3 Remove bottle


uint8_t gu8_Mode{0u};
// Possible mode by JBS Brasseurs filling system
//   0 Idle mode UserChoice
//   1 Manual mode - 4 lines can be used independantly
//   2 Semi-Auto mode - Filling starts by pressing the pedal  
//   3 Auto - Filling start automatically when bottle is present
//   4 Cleaning - Used to clean all 4 valves
const uint16_t g_CYCLE_TIME_MS {100U};
unsigned int gMachineMode{1U};   // number of used valve
unsigned int gu32TimingFilling{20U}; // 20 ms cycle time

C_JBS_LCD_I2C g_Lcd; //(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
C_JBS_LedNeoPixel g_Leds; // Set leds object
C_JBS_FillingSystem g_FillingSystem; // Set filling system object 


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) {
       ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }
    
  Serial.println("Init phase 01 : Initialisation"); // Message used to debug
  // Initialize leds
  g_Leds.init();
  g_Leds.show();   // Send the updated pixel colors to the hardware.
  
  // Initialize LCD 
  g_Lcd.Init();   // Initialize the LCD
  g_Lcd.ClearPrint(g_Lcd.MAC8_INIT);  // Init message
  
  Serial.println("Init phase 02 : Start carroussel"); // Message used to debug
  // Display leds and displas carroussel 
  g_Lcd.ClearPrintL2(g_Lcd.MAC8_BEER_L2_01);
  g_Leds.setPixelColor(0,0,0,255);
  g_Leds.show();
  Delay(1000);
  
  g_Leds.init();
  g_Lcd.ClearPrintL2(g_Lcd.MAC8_BEER_L2_01_03);
  g_Leds.setPixelColor(2,0,0,255);
  g_Leds.show();
  Delay(1000);

  g_Leds.init();
  g_Lcd.ClearPrintL2(g_Lcd.MAC8_BEER_L2_01_03_05);
  g_Leds.setPixelColor(4,0,0,255);
  g_Leds.show();
  Delay(1000);

  g_Leds.init();
  g_Lcd.ClearPrintL2(g_Lcd.MAC8_BEER_L2_01_03_05_07);
  g_Leds.setPixelColor(6,0,0,255);
  g_Leds.show();
  Delay(1000);

  g_Lcd.ClearPrintL2(g_Lcd.MAC8_BEER_L2_01_03_05_07_09);
  g_Leds.setPixelColor(7,0,0,255);
  g_Leds.show();
  Delay(1000);
  
  g_Leds.init();
  g_Lcd.ClearPrintL2(g_Lcd.MAC8_BEER_L2_01_03_05_07_09_11);
  g_Leds.setPixelColor(5,0,0,255);
  g_Leds.show();
  Delay(1000);

  g_Leds.init();
  g_Lcd.ClearPrintL2(g_Lcd.MAC8_BEER_L2_01_03_05_07_09_11_13);
  g_Leds.setPixelColor(3,0,0,255);
  g_Leds.show();
  Delay(1000);

  g_Leds.init();
  g_Lcd.ClearPrintL2(g_Lcd.MAC8_BEER_L2_01_03_05_07_09_11_13_15);
  g_Leds.setPixelColor(1,0,0,255);
  g_Leds.show();
  Delay(1000);


  Serial.println("Init phase 03 : Start About"); // Message used to debug
  // About message
  g_Lcd.ClearPrint(g_Lcd.MAC8_INIT_ABOUT);  // Init message
  g_Leds.init();
  g_Leds.setPixelColor(0,0,0,255);
  g_Leds.setPixelColor(2,0,0,255);
  g_Leds.setPixelColor(4,0,0,255);
  g_Leds.setPixelColor(6,0,0,255);
  g_Leds.setPixelColor(7,0,0,255);
  g_Leds.setPixelColor(5,0,0,255);
  g_Leds.setPixelColor(3,0,0,255);
  g_Leds.setPixelColor(1,0,0,255);
  g_Leds.show();
  Delay(4000);

  Serial.println("Init phase 04 : Start Welcome"); // Message used to debug
  // Welcome message
  g_Lcd.ClearPrint(g_Lcd.MAC8_INIT_JBS_MESSAGE);  // Init message
  g_Leds.init();
  g_Leds.setPixelColor(0,U32_JBS_GREEN_LED);
  g_Leds.setPixelColor(1,U32_JBS_GREEN_LED);
  g_Leds.show();
  Delay(1000);

  g_Leds.setPixelColor(2,U32_JBS_GREEN_LED);
  g_Leds.setPixelColor(3,U32_JBS_GREEN_LED);
  g_Leds.show();
  Delay(1000);

  g_Leds.setPixelColor(4,U32_JBS_GREEN_LED);
  g_Leds.setPixelColor(5,U32_JBS_GREEN_LED);
  g_Leds.show();
  Delay(1000);

  g_Leds.setPixelColor(6,U32_JBS_GREEN_LED);
  g_Leds.setPixelColor(7,U32_JBS_GREEN_LED);
  g_Leds.show();
  Delay(1000);
  
  // Initialize filling system object
  Serial.println("Init phase 05 : System start"); // Message used to debug
  g_FillingSystem.Init(g_CYCLE_TIME_MS, U8_PIN_BT_RESET, U8_PIN_BT_PEDALE, AU8_PIN_V, AU8_PIN_BT_CAL, AU8_PIN_CT, &g_Leds);

  Serial.println("Init phase 06 : ??"); // Message used to debug
  uint32_t u32_LoopTime{0U};       
  uint8_t u8_Last_Mode{0xffU};
  
  // Infinity loop
  for (;;)
  {
    u32_LoopTime = millis(); // ?? Set units ms
    g_FillingSystem.UpdateInput();
    
    // Detect reset btn
    if (g_FillingSystem.GetBtResetPressed() == 0U)
    {
      g_FillingSystem.Reset();
      gu8_Mode = 0U;
    }

    // Detect mode change
    if (u8_Last_Mode!=gu8_Mode)
    {
      if (gu8_Mode == 0U)
      {
        g_Lcd.ClearPrint(g_Lcd.MAC8_MODE_CHOOSE);
      }
      
      else if (gu8_Mode == 1U)
      {
        g_Lcd.ClearPrint("Manual 1");
        g_Leds.setPixelColor(0U, U32_JBS_GREEN_LED);
        g_Leds.setPixelColor(2U, U32_JBS_GREEN_LED);
        g_Leds.setPixelColor(4U, U32_JBS_GREEN_LED);
        g_Leds.setPixelColor(6U, U32_JBS_GREEN_LED);
      }
      
      else if (gu8_Mode == 2U)
      {
        g_Lcd.ClearPrint("Semi Auto 2");
        g_Leds.setPixelColor(0U, U32_JBS_ORANGE_LED);
        g_Leds.setPixelColor(2U, U32_JBS_ORANGE_LED);
        g_Leds.setPixelColor(4U, U32_JBS_ORANGE_LED);
        g_Leds.setPixelColor(6U, U32_JBS_ORANGE_LED);
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
    else if (gu8_Mode == 2U)
    {      
      g_FillingSystem.SemiAuto();
      g_FillingSystem.WriteOutput();
    }
    else if (gu8_Mode == 4U)
    {      
      g_FillingSystem.Cleaning();
      g_FillingSystem.WriteOutput();
    }

    //////
    while (millis()<u32_LoopTime+g_CYCLE_TIME_MS);
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
