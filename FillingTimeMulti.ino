#include <Arduino_FreeRTOS.h>

// define two tasks for Blink & AnalogRead
void TaskFilling( void *pvParameters );
void TaskMonitoring( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup() {
  
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


int pin = 1 ;
pinMode(pin, OUTPUT);         // INPUT, OUTPUT, or INPUT_PULLUP
digitalWrite(pin, LOW);       // HIGH, LOW
int test=digitalRead(pin);    // Read digital


// initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(13, INPUT_PULLUP);
  pinMode(12, OUTPUT);
Serial.println("Program start:");


}
    
void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

unsigned int u32TimingFilling=20; 

void TaskFilling(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  bool bBtP=1;
  bool bBtPLast=1;

  bool bValve=1;
  bool bFilling=0;
  unsigned int u32ctClick=0;
  unsigned int u32ctValve=0; 
  unsigned int Fillingtime=0;
  
  for (;;) // A Task shall never return or exit.
  {
    //Serial.print("Value: ");
    //Serial.println(digitalRead(13));
    bBtP=digitalRead(13);
    
    if (bBtP==0 and bBtPLast==1)
    {
      bFilling=1;
      u32ctClick=1;
      u32ctValve=0;
    }
    else if (bBtP==0)
    {
      u32ctClick++;
    }
    else if (u32ctClick > 3000/u32TimingFilling and bBtPLast==0 and bBtP==1)
    {
      Fillingtime=u32ctClick;
      Serial.print("Saved filling Time: ");
      Serial.println(Fillingtime*u32TimingFilling);
    }
    //else

    if (u32ctValve>=Fillingtime and bBtP==1)
    { 
      //Serial.println("Valve Closed ");
      u32ctValve=0;
      bFilling=0;
      bValve=1;
    }
    
      //Serial.print("Filling state: ");
      //Serial.println(bFilling);
    if (bFilling)
    {
      u32ctValve++;
      Serial.print("Valve open Time: ");
      Serial.println(u32ctValve*u32TimingFilling);
      bValve=0;
    }
    else
    {
      bValve=1;
    }
    digitalWrite(12, bValve);
    bBtPLast=bBtP;
    vTaskDelay( u32TimingFilling / portTICK_PERIOD_MS ); // wait for one second
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
