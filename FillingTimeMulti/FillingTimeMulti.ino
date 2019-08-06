#include <Arduino_FreeRTOS.h>

// define two tasks for Blink & AnalogRead
void TaskFilling( void *pvParameters );
void TaskMonitoring( void *pvParameters );

/* Global Variable */
// Pin assignement
const unsigned int PIN_CtBottle_1 = 7;  //22;
const unsigned int PIN_CtBottle_2 = 23;
const unsigned int PIN_CtBottle_3 = 24;
const unsigned int PIN_CtBottle_4 = 25;

const unsigned int PIN_OtLedBottle_1 = 26;  //22;
const unsigned int PIN_OtLedBottle_2 = 27;
const unsigned int PIN_OtLedBottle_3 = 28;
const unsigned int PIN_OtLedBottle_4 = 29;

const unsigned int PIN_BtCal_1 = 13; //30
const unsigned int PIN_BtCal_2 = 31;
const unsigned int PIN_BtCal_3 = 32;
const unsigned int PIN_BtCal_4 = 33;

const unsigned int PIN_OtValve_1 = 12;  //34
const unsigned int PIN_OtValve_2 = 35;
const unsigned int PIN_OtValve_3 = 36;
const unsigned int PIN_OtValve_4 = 37;

const unsigned int PIN_BtMain   = 53;

// Program variable
unsigned int gmachineState=0;
// 0 Wait for bottle
// 1 Filling ready
// 2 Filling
// 3 Remove bottle

unsigned int gMachineMode=1;   // number of used valve
unsigned int gu32TimingFilling=20; // 20 ms cycle time



// the setup function runs once when you press reset or power the board
void setup() {

int pin = 1 ;
pinMode(pin, OUTPUT);         // INPUT, OUTPUT, or INPUT_PULLUP
digitalWrite(pin, LOW);       // HIGH, LOW
int test=digitalRead(pin);    // Read digital

  pinMode(PIN_CtBottle_1, INPUT_PULLUP);
  //pinMode(PIN_CtBottle_2, INPUT_PULLUP);
  //pinMode(PIN_CtBottle_3, INPUT_PULLUP);
  //pinMode(PIN_CtBottle_4, INPUT_PULLUP);

  pinMode(PIN_OtLedBottle_1, OUTPUT);
  //pinMode(PIN_OtLedBottle_2, OUTPUT);
  //pinMode(PIN_OtLedBottle_3, OUTPUT);
  //pinMode(PIN_OtLedBottle_4, OUTPUT);
  
  pinMode(PIN_BtCal_1, INPUT_PULLUP);
  //pinMode(PIN_BtCal_2, INPUT_PULLUP);
  //pinMode(PIN_BtCal_3, INPUT_PULLUP);
  //pinMode(PIN_BtCal_4, INPUT_PULLUP);
  
  //pinMode(PIN_BtMain, INPUT_PULLUP);
  
  pinMode(PIN_OtValve_1, OUTPUT);
  //pinMode(PIN_OtValve_2, OUTPUT);
  //pinMode(PIN_OtValve_3, OUTPUT);
  //pinMode(PIN_OtValve_4, OUTPUT);
  
  digitalWrite(PIN_OtValve_1, 0);
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskFilling
    ,  (const portCHAR *)"Fill"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskMonitoring
    ,  (const portCHAR *) "AnalogRead"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
  
Serial.println("Program start:");
}
    
void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskFilling(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  bool bBtCal_1=1;
  bool bBtCalLast_1=1;

  bool bBottleEmpty_1=1;
  bool bBottleEmpty_2=1;
  bool bBottleEmpty_3=1;
  bool bBottleEmpty_4=1;
  
  bool bValve=0;
  bool bFilling=0;
  unsigned int u32ctClick=0;
  unsigned int u32ctValve=0; 
  unsigned int u32FillingTimeCal=0;
  unsigned int u32FillingTime=0;
  
  unsigned int NumberFilledBottle=0;
  
  
  for (;;) // A Task shall never return or exit.
  {
    
    bBtCal_1=digitalRead(PIN_BtCal_1);
    
    if (CheckBotlleReady(gMachineMode, bBottleEmpty_1, bBottleEmpty_2, bBottleEmpty_3, bBottleEmpty_4))
    {
      if (bBtCal_1==0 and bBtCalLast_1==1)
      {
        u32FillingTime=u32FillingTimeCal-(NumberFilledBottle/5*1);
        bFilling=1;
        u32ctClick=0;
      }
      else if (bBtCal_1==0)
      {
        u32ctClick++;
      }
      else if (u32ctClick > 3000/gu32TimingFilling and bBtCalLast_1==0 and bBtCal_1==1)
      {
        bFilling=0;
        u32FillingTimeCal=u32ctClick;
        Serial.print("Saved filling Time: ");
        Serial.println(u32FillingTimeCal*gu32TimingFilling);
      }
    }
    
      //Serial.print("Filling state: ");
      //Serial.println(bFilling);
    if (bFilling)
    {
      u32ctValve++;
      bValve=1;

      if (u32ctValve>=u32FillingTime and bBtCal_1==1)
      { 
        //Serial.println("Valve Closed ");
        u32ctValve=0;
        bFilling=0;
        bValve=1;
        
        SetBotlleFull(gMachineMode, bBottleEmpty_1, bBottleEmpty_2, bBottleEmpty_3, bBottleEmpty_4);
        NumberFilledBottle+=gMachineMode;
        Serial.print("Number of filled bottles: ");
        Serial.println(NumberFilledBottle);
      }
    }
    else
    {
      bValve=0;
    }
    digitalWrite(PIN_OtValve_1, bValve);
    bBtCalLast_1=bBtCal_1;
    vTaskDelay( gu32TimingFilling / portTICK_PERIOD_MS ); // wait for one second
  }
}

unsigned int SetBotlleFull(unsigned int machineMode, bool& bBottleEmpty_1, bool& bBottleEmpty_2, bool& bBottleEmpty_3, bool& bBottleEmpty_4)
{
  bBottleEmpty_1&=machineMode<1;
  bBottleEmpty_2&=machineMode<2;
  bBottleEmpty_3&=machineMode<3;
  bBottleEmpty_4&=machineMode<4;
}

unsigned int CheckBotlleReady(unsigned int machineMode, bool& bBottleEmpty_1, bool& bBottleEmpty_2, bool& bBottleEmpty_3, bool& bBottleEmpty_4)
{
  bBottleEmpty_1|=digitalRead(PIN_CtBottle_1);
  bBottleEmpty_2|=digitalRead(PIN_CtBottle_2);
  bBottleEmpty_3|=digitalRead(PIN_CtBottle_3);
  bBottleEmpty_4|=digitalRead(PIN_CtBottle_4);
    
  switch(machineMode)
  {
    case 1:
    return !digitalRead(PIN_CtBottle_1) && bBottleEmpty_1;
    break;
    
    case 2:
    return !digitalRead(PIN_CtBottle_1)&&!digitalRead(PIN_CtBottle_2) && !digitalRead(PIN_CtBottle_1)&&!digitalRead(PIN_CtBottle_2);
    break;
    
    case 3:
    return digitalRead(PIN_CtBottle_1)&&digitalRead(PIN_CtBottle_2)&&digitalRead(PIN_CtBottle_3) && !digitalRead(PIN_CtBottle_1)&&!digitalRead(PIN_CtBottle_2)&&!digitalRead(PIN_CtBottle_3);
    break;
    
    case 4:
    return digitalRead(PIN_CtBottle_1)&&digitalRead(PIN_CtBottle_2)&&digitalRead(PIN_CtBottle_3)&&digitalRead(PIN_CtBottle_4) && !digitalRead(PIN_CtBottle_1)&&!digitalRead(PIN_CtBottle_2)&&!digitalRead(PIN_CtBottle_3)&& !digitalRead(PIN_CtBottle_4);
    break;
  }
}


void TaskMonitoring(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  
/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.
*/

  for (;;)
  {
    // read the input on analog pin 0:
    int sensorValue = analogRead(A0);
    // print out the value you read:
    //Serial.println(sensorValue);
    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}
