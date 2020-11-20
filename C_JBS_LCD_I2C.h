/* 
* C_JBS_LCD_I2C.h
*
* Created: 13/04/2020 13:27:59
* Author: JB_Notebook
*/

#ifndef __C_JBS_LCD_I2C_H__
#define __C_JBS_LCD_I2C_H__

#include <Arduino_FreeRTOS.h>
#include <LiquidCrystal_I2C.h>

const uint8_t U8_JBS_LCD_I2C_NUMBER_ROW {2u};
const uint8_t U8_JBS_LCD_I2C_NUMBER_COL {16u};

class C_JBS_LCD_I2C : public LiquidCrystal_I2C
{
//variables
public:
const char MAC8_INIT[9];
const char MAC8_INIT_JBS[32];
const char MAC8_INIT_STARTUP[32];

const char MAC8_MODE_CHOOSE[32];

protected:

private:
uint8_t mau8_BierCharacter[8];

//functions
public:
	C_JBS_LCD_I2C();
	~C_JBS_LCD_I2C();

	void Init();
    void PrintInit();
    void ClearPrint(const char * pc8_Text);
    void ClearPrint2(const char * pc8_Line1, const char * pc8_Line2);
	void ClearPrintL2(const char * pc8_Line2);
    void ClearPrintDebug(const char * pc8_Line1, const int u16_V1, const int u16_V2, const int u16_V3, const int u16_V4);
protected:
private:
	C_JBS_LCD_I2C( const C_JBS_LCD_I2C &c );
	C_JBS_LCD_I2C& operator=( const C_JBS_LCD_I2C &c );

}; //JBS_LCD_I2C

#endif //__JBS_LCD_I2C_H__
