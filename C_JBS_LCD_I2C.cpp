/* 
* C_JBS_LCD_I2C.cpp
*
* Created: 13/04/2020 13:27:58
* Author: JB_Notebook
*/

#include "C_JBS_LCD_I2C.h"
#include <string.h>
#include "Arduino.h"

// default constructor
C_JBS_LCD_I2C::C_JBS_LCD_I2C():
LiquidCrystal_I2C(0x27u, U8_JBS_LCD_I2C_NUMBER_COL, U8_JBS_LCD_I2C_NUMBER_ROW),
mau8_BierCharacter{0x06,0x09,0x19,0x19,0x19,0x19,0x0F,0x00},

MAC8_INIT{" Initialisation "},
MAC8_INIT_ABOUT{"V1.0-2020-11-27 Made in JBS"},
MAC8_INIT_JBS_MESSAGE{"Bon courage     Bon remplissage "},
MAC8_INIT_STARTUP{"Startup ..."},
MAC8_BEER{8,0},
MAC8_BEER_L2_01{8,0},
MAC8_BEER_L2_01_03{8,' ',8,0},
MAC8_BEER_L2_01_03_05{8,' ',8,' ',8,0},
MAC8_BEER_L2_01_03_05_07{8,' ',8,' ',8,' ',8,0},
MAC8_BEER_L2_01_03_05_07_09{8,' ',8,' ',8,' ',8,' ',8,0},
MAC8_BEER_L2_01_03_05_07_09_11{8,' ',8,' ',8,' ',8,' ',8,' ',8,0},
MAC8_BEER_L2_01_03_05_07_09_11_13{8,' ',8,' ',8,' ',8,' ',8,' ',8,' ',8,0},
MAC8_BEER_L2_01_03_05_07_09_11_13_15{8,' ',8,' ',8,' ',8,' ',8,' ',8,' ',8,' ',8,0},
MAC8_MODE_00{"1:M|2:SA|3:A|4:C"},
MAC8_MODE_01{"Manuel"},
MAC8_MODE_02{"Semi-Automatique"},
MAC8_MODE_03{"Automatique"},
MAC8_MODE_04{"Nettoyage"},
MAC8_MODE_CHOOSE{"Choose mode:"}
{
} //JBS_LCD_I2C

// default destructor
C_JBS_LCD_I2C::~C_JBS_LCD_I2C()
{
} //~JBS_LCD_I2C

void C_JBS_LCD_I2C::Init()
{
    LiquidCrystal_I2C::init();                      // initialize the LCD
    backlight();
    createChar(8u, mau8_BierCharacter);
}

void C_JBS_LCD_I2C::ClearPrint(const char * pc8_Text)
{
    clear();
    if(strlen(pc8_Text) > U8_JBS_LCD_I2C_NUMBER_COL)
    {
        char ac8_Line[U8_JBS_LCD_I2C_NUMBER_COL];
        memcpy(ac8_Line, pc8_Text, U8_JBS_LCD_I2C_NUMBER_COL);
        print(ac8_Line);
        setCursor(0u, 1u);
        print(pc8_Text+U8_JBS_LCD_I2C_NUMBER_COL);
    }
    else
    {
        print(pc8_Text);
    }
}

void C_JBS_LCD_I2C::ClearPrint2(const char * pc8_Line1, const char * pc8_Line2)
{
    clear();
    if(strlen(pc8_Line1) > U8_JBS_LCD_I2C_NUMBER_COL)
    {
        char ac8_Line[U8_JBS_LCD_I2C_NUMBER_COL];
        memcpy(ac8_Line, pc8_Line1, U8_JBS_LCD_I2C_NUMBER_COL);
        print(ac8_Line);
    }
    else
    {
        print(pc8_Line1);
    }
    
    setCursor(0u, 1u);
    if(strlen(pc8_Line2) > U8_JBS_LCD_I2C_NUMBER_COL)
    {
        char ac8_Line[U8_JBS_LCD_I2C_NUMBER_COL];
        memcpy(ac8_Line, pc8_Line2, U8_JBS_LCD_I2C_NUMBER_COL);
        print(ac8_Line);
    }
    else
    {
        print(pc8_Line2);
    }
}


void C_JBS_LCD_I2C::ClearPrintL2(const char * pc8_Line2)
{
    setCursor(0u, 1u);
    char ac8_Line[U8_JBS_LCD_I2C_NUMBER_COL];
    memset(&ac8_Line[0U], ' ', U8_JBS_LCD_I2C_NUMBER_COL);
    memcpy(ac8_Line, pc8_Line2, min(U8_JBS_LCD_I2C_NUMBER_COL,strlen(pc8_Line2)));
    print(ac8_Line);
}

void C_JBS_LCD_I2C::ClearPrintDebug(const char * pc8_Line1, const int u16_V1, const int u16_V2, const int u16_V3, const int u16_V4)
{
    clear();
    if(strlen(pc8_Line1) > U8_JBS_LCD_I2C_NUMBER_COL)
    {
        char ac8_Line[U8_JBS_LCD_I2C_NUMBER_COL];
        memcpy(ac8_Line, pc8_Line1, U8_JBS_LCD_I2C_NUMBER_COL);
        print(ac8_Line);
    }
    else
    {
        print(pc8_Line1);
    }
    
    setCursor(0u, 1u);
    char ac8_Line[U8_JBS_LCD_I2C_NUMBER_COL];
    //sprinf(ac8_Line, "%d", u16_V1);
    //sprinf(ac8_Line+4, "%d", u16_V2);
    //sprinf(ac8_Line+8, "%d", u16_V3);
    //sprinf(ac8_Line+12, "%d", u16_V4)
    print(ac8_Line);
}

//void C_JBS_LCD_I2C::PrintInit()
//{
//    vTaskDelay( 5000u / portTICK_PERIOD_MS ); // wait for one second
//
//    ClearPrint(MAC8_INIT_JBS_MESSAGE);
//    vTaskDelay( 10000u / portTICK_PERIOD_MS ); // wait for one second
//    
//    ClearPrint(MAC8_INIT_STARTUP);
//    vTaskDelay( 3000u / portTICK_PERIOD_MS ); // wait for one second
//}
