#include <Wire.h>
#include "DS3231.h"
DS3231 RTC; //Create the DS3231 object

#include "OLED.h"
#include "stdlib.h"

const int theDeviceAddress = 87;


char Director1 = 0;//flag
int VisitorsIn = 0;
int VisitorsOut = 0;
int count = 0;

/*-------------------Initial Function---------------------*/
void setup() {
  // put your setup code here, to run once:
  LEDPIN_Init();
  LED_Init();
  LED_P8x16Str(20, 2, "Prototyping");
  LED_P8x16Str(20, 4, "Prototyping");
  Serial.begin(9600);
  Wire.begin();
  attachInterrupt(0, Detect1, RISING  );//下降沿说明有
  attachInterrupt(1, Detect2, RISING );//下降沿说明人
}


/*-------------------RTC  Function---------------------*/


/*-------------------OLED Function----------------------*/


/*-------------------SaveData Function--------------------*/
void SaveData()
{
  //获取当前时间信息
  DateTime now = RTC.now();
  char buffer[10];
  //read time information
  buffer[0] = now.year() - 2000;
  buffer[1] = now.month();
  buffer[2] = now.date();
  buffer[3] = now.hour();
  buffer[4] = now.minute();
  buffer[5] = now.second();

  buffer[6] = VisitorsIn % 255;
  buffer[7] = (VisitorsIn >> 8) % 255; //

  buffer[8] = VisitorsOut % 255;
  buffer[9] = (VisitorsOut >> 8) % 255;
  //save count number
  WireEepromWriteByte(theDeviceAddress, 2 + count * 10, (char)count % 255);
  WireEepromWriteByte(theDeviceAddress, 2 + count * 10, (char)(count >> 8) % 255);
  //save time and people in and out
  WireEepromWriteByte(theDeviceAddress, 2 + count * 10, buffer[0]);
  WireEepromWriteByte(theDeviceAddress, 2 + count * 10 + 1, buffer[1]);
  WireEepromWriteByte(theDeviceAddress, 2 + count * 10 + 2, buffer[2]);
  WireEepromWriteByte(theDeviceAddress, 2 + count * 10 + 3, buffer[3]);
  WireEepromWriteByte(theDeviceAddress, 2 + count * 10 + 4, buffer[4]);
  WireEepromWriteByte(theDeviceAddress, 2 + count * 10 + 5, buffer[5]);
  WireEepromWriteByte(theDeviceAddress, 2 + count * 10 + 6, buffer[6]);
  WireEepromWriteByte(theDeviceAddress, 2 + count * 10 + 7, buffer[7]);
  WireEepromWriteByte(theDeviceAddress, 2 + count * 10 + 8, buffer[8]);
  WireEepromWriteByte(theDeviceAddress, 2 + count * 10 + 9, buffer[9]);

}

/*-------------------ReadData Function--------------------*/
void ReadData()
{
  int all;
  char buffer[10];
  char buffer1[2];
  buffer1[0] = WireEepromReadByte(theDeviceAddress, 0);
  buffer1[1] = WireEepromReadByte(theDeviceAddress, 1);
  all = buffer1[0] + buffer1[1] * 255;
  Serial.println( buffer1[0], DEC);
  Serial.println( buffer1[1], DEC);
  Serial.println(all);
  int i;
  for (i = 0; i < all; i++)
  {
    // AT24C32.ReadMem(2+i*10, buffer, 10);
    buffer[0] = WireEepromReadByte(theDeviceAddress, 2 + i * 10);
    buffer[1] = WireEepromReadByte(theDeviceAddress, 2 + i * 10 + 1);
    buffer[2] = WireEepromReadByte(theDeviceAddress, 2 + i * 10 + 2);
    buffer[3] = WireEepromReadByte(theDeviceAddress, 2 + i * 10 + 3);
    buffer[4] = WireEepromReadByte(theDeviceAddress, 2 + i * 10 + 4);
    buffer[5] = WireEepromReadByte(theDeviceAddress, 2 + i * 10 + 5);
    buffer[6] = WireEepromReadByte(theDeviceAddress, 2 + i * 10 + 6);
    buffer[7] = WireEepromReadByte(theDeviceAddress, 2 + i * 10 + 7);
    buffer[8] = WireEepromReadByte(theDeviceAddress, 2 + i * 10 + 8);
    buffer[9] = WireEepromReadByte(theDeviceAddress, 2 + i * 10 + 9);
    Serial.print(2000 + buffer[0], DEC);  //year
    Serial.print('-');
    Serial.print(buffer[1], DEC);//month
    Serial.print('-');
    Serial.print(buffer[2], DEC);//day
    Serial.print('/');

    Serial.print(buffer[3], DEC);//hour
    Serial.print(':');
    Serial.print(buffer[4], DEC);//min
    Serial.print(':');
    Serial.print(buffer[5], DEC);//sec
    Serial.print('#');

    int temp = buffer[6] + buffer[7] * 255;
    Serial.print(temp, DEC);
    Serial.print('/');
    int temp2 = buffer[8] + buffer[9] * 255;
    Serial.print(temp2, DEC);
    Serial.println();
  }
}


/*-------------------Interrupt Function------------------*/
void Detect1()
{
  Serial.println("Detect1()");
  if (Director1 == 0) //means people is go in and not yet pass
  {
    Director1 = 1;
  }
  if (Director1 == 2) //means people is going out
  {
    Director1 = 0;
    VisitorsOut ++;
    count++;
    SaveData();
  }
}
void Detect2()
{
  Serial.println("Detect2()");
  if (Director1 == 0) //means people is go out and not yet pass
  {
    Director1 = 2;
  }
  if (Director1 == 1)
  {
    Director1 = 0;
    VisitorsIn ++;
    count++;
    SaveData();
  }
}

/*-------------------E2PRAM------------------*/
void WireEepromRead(int theDeviceAddress, unsigned int theMemoryAddress, int theByteCount, byte* theByteArray) {
  for (int theByteIndex = 0; theByteIndex < theByteCount; theByteIndex++) {
    Wire.beginTransmission(theDeviceAddress);
    Wire.write((byte)((theMemoryAddress + theByteIndex) >> 8));
    Wire.write((byte)((theMemoryAddress + theByteIndex) >> 0));
    Wire.endTransmission();
    delay(5);
    Wire.requestFrom(theDeviceAddress, sizeof(byte));
    theByteArray[theByteIndex] = Wire.read();
  }
}

byte WireEepromReadByte(int theDeviceAddress, unsigned int theMemoryAddress) {
  byte theByteArray[sizeof(byte)];
  WireEepromRead(theDeviceAddress, theMemoryAddress, sizeof(byte), theByteArray);
  return (byte)(((theByteArray[0] << 0)));
}

int WireEepromReadInt(int theDeviceAddress, unsigned int theMemoryAddress) {
  byte theByteArray[sizeof(int)];
  WireEepromRead(theDeviceAddress, theMemoryAddress, sizeof(int), theByteArray);
  return (int)(((theByteArray[0] << 8)) | (int)((theByteArray[1] << 0)));
}

void WireEepromWrite(int theDeviceAddress, unsigned int theMemoryAddress, int theByteCount, byte* theByteArray) {
  for (int theByteIndex = 0; theByteIndex < theByteCount; theByteIndex++) {
    Wire.beginTransmission(theDeviceAddress);
    Wire.write((byte)((theMemoryAddress + theByteIndex) >> 8));
    Wire.write((byte)((theMemoryAddress + theByteIndex) >> 0));
    Wire.write(theByteArray[theByteIndex]);
    Wire.endTransmission();
    delay(5);
  }
}

void WireEepromWriteByte(int theDeviceAddress, unsigned int theMemoryAddress, byte theByte) {
  byte theByteArray[sizeof(byte)] = {(byte)(theByte >> 0)};
  WireEepromWrite(theDeviceAddress, theMemoryAddress, sizeof(byte), theByteArray);
}

void WireEepromWriteInt(int theDeviceAddress, unsigned int theMemoryAddress, int theInt) {
  byte theByteArray[sizeof(int)] = {(byte)(theInt >> 8), (byte)(theInt >> 0)};
  WireEepromWrite(theDeviceAddress, theMemoryAddress, sizeof(int), theByteArray);
}

/*----------------------mian -------------------*/

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    //读取数据
    int instruct = Serial.read();
    switch (instruct) {
      case 'P':
        {
          //先eeprom 指定地址写入数据

          break;
        }
      case 'G':
        {
          ReadData();
          break;
        }
      case 'F':
        {
          //获取当前时间信息

          break;
        }
    }
  }
}
