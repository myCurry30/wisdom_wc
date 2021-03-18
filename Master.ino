//Master
#include <SoftwareSerial.h>
SoftwareSerial Master(10, 11);
#include <Wire.h>
// GY-30
// BH1750FVI
// in ADDR 'L' mode 7bit addr
#define ADDR 0b0100011
// addr 'H' mode
// #define ADDR 0b1011100
unsigned char  index1 = 0, index2 = 0, index3 = 0, index4 = 0, index5 = 0;
unsigned char num[13];
unsigned int num11=0,num22=0,num33=0,num44=0;
float NH4;
unsigned char hexEND[] = {0xff, 0xff, 0xff};
#include <stdio.h>
#include <DS1302.h>

namespace {

const int kCePin   = 5;  // 复位引脚
const int kIoPin   = 6;  // 数据引脚
const int kSclkPin = 7;  // 时钟引脚

DS1302 rtc(kCePin, kIoPin, kSclkPin);

//String dayAsString(const Time::Day day) {
//  switch (day) {
//    case Time::kSunday: return "Sunday";
//    case Time::kMonday: return "Monday";
//    case Time::kTuesday: return "Tuesday";
//    case Time::kWednesday: return "Wednesday";
//    case Time::kThursday: return "Thursday";
//    case Time::kFriday: return "Friday";
//    case Time::kSaturday: return "Saturday";
//  }
//  return "(unknown day)";
//}

void printTime() {
  // 从模块获得当前时间
  Time t = rtc.time();

//  // 星期几？
//  const String day = dayAsString(t.day);
//
//  // 格式化时间数据
//  char buf[50];
//  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
//           day.c_str(),
//           t.yr, t.mon, t.date,
//           t.hr, t.min, t.sec);
//  Serial.println(buf);
    Serial.print("t17.txt=\"");
    Serial.print(t.hr);
    Serial.print(":");
    Serial.print(t.min);
    Serial.print(":");
    Serial.print(t.sec);
    Serial.print("\"");
    Serial.write(hexEND, 3);
}

}  // namespace

void setup()
{
  Serial.begin(9600);                         //初始化
  Serial.println("Master is ready!");
  Master.begin(9600);
  
  Wire.begin();
  Wire.beginTransmission(ADDR);
  Wire.write(0b00000001);
  Wire.endTransmission();
  rtc.writeProtect(false);
  rtc.halt(false);

   // 设置初始时间为 2018-1-18,17:28:00 星期四
//   Time t(2020, 11, 6, 19, 27, 00, Time::kThursday);
//   rtc.time(t);
  while (Serial.read() >= 0) {}              //清空串口0缓存
  while (Master.read() >= 0) {}            //清空串口1缓存
}




void loop()
{
  
  char address;
  int Temperature;
  int Humidity;
  // put your main code here, to run repeatedly:
  int vall = 0;
  // reset
  Wire.beginTransmission(ADDR);
  Wire.write(0b00000111);
  Wire.endTransmission();
 
  Wire.beginTransmission(ADDR);
  Wire.write(0b00100000);
  Wire.endTransmission();
  // typical read delay 120ms
  delay(120);
  Wire.requestFrom(ADDR, 2); // 2byte every time
  for (vall = 0; Wire.available() >= 1; ) {
    char c = Wire.read();
    //Serial.println(c, HEX);
    vall = (vall << 8) + (c & 0xFF);
  }
  vall = vall / 1.2;
//  Serial.println(val);
  Serial.print("t9.txt=\"");
  Serial.print(vall);
  Serial.print(" LX\"");
  Serial.write(hexEND, 3);
  for (int i = 36 ; i < 42 ; i++)     //用来对每个从机进行一次大轮询
  {
    printTime();
    if(i==39)
    {
      continue;
    }
    address = i;
    Master.print(address);
    delay(1800);
          String str = "";
         while (Master.available())
    { 
      delay(2);
      str += char(Master.read());
    }

//          Serial.println(str);
    //      Serial.println(str.charAt(0));
    //      Serial.println(str.charAt(str.length()));
    if (i == 36)
    {

      //      Serial.println();
//            Serial.println(36);
//            Serial.print("36:");
//            Serial.println(str);
//            Serial.println(str.length());
      index1 = str.indexOf('$');//找到位置
      index2 = str.indexOf(',');
      index3 = str.substring((index2) + 1, str.length()).indexOf(',') + 4;
      index4 = str.substring((index3) + 1, str.length()).indexOf(',') + 7;
      index5 = str.substring((index2) + 1, str.length()).indexOf('$') + 4;
//      if(index1>1)
//      {
//        index3 +=3 ;
//        index4 +=3 ;
//      }
//              Serial.println(index1);
//              Serial.println(index2);
//              Serial.println(index3);
//              Serial.println(index4);
      if ((index1 != -1) && (index2 != -1) && (index3 != -1)&& (index4 != -1)&& (index5 != -1)) //如果位置不为空
      {
        num[1] = str.substring(index1 + 1, index2).toInt();
        num[2] = str.substring(index2 + 1, index3).toInt();
        num[3] = str.substring(index4 + 1, index5).toInt();
        num11 = str.substring(index3 + 1, index4).toInt();
        unsigned char t1=num11/100;
        unsigned char t2=num11%100;
        NH4 = t1+t2*0.01;
        Serial.print("t6.txt=\"");
        Serial.print(num[1]);
        Serial.print("\"");
        Serial.write(hexEND, 3);
        Serial.print("t7.txt=\"");
        Serial.print(num[2]);
        Serial.print("\"");
        Serial.write(hexEND, 3);
        Serial.print("t8.txt=\"");
        Serial.print(NH4);
        Serial.print("\"");
        Serial.write(hexEND, 3);
        if(num[3])
        {
          Serial.print("t15.txt=\"No\"");
          Serial.write(hexEND, 3);
        }
        else
        {
          Serial.print("t15.txt=\"Yes\"");
          Serial.write(hexEND, 3);
        }
//      Serial.println(num1);
//      Serial.println(num2);
      }
    }
    if (i == 37)
    {

      //      Serial.println();
//            Serial.println(49);
//            Serial.print("37:");
//            Serial.println(str);
//            Serial.println(str.length());
      index1 = str.indexOf('%');//找到位置
      index2 = str.indexOf(',');
      index3 = str.substring((index2) + 1, str.length()).indexOf(',') + 4;
      index4 = str.substring((index3) + 1, str.length()).indexOf(',') + 7;
      index5 = str.substring((index2) + 1, str.length()).indexOf('%') + 4;
//      if(index1>1)
//      {
//        index3 +=3 ;
//        index4 +=3 ;
//      }
//              Serial.println(index1);
//              Serial.println(index2);
//              Serial.println(index3);
//              Serial.println(index4);
      if ((index1 != -1) && (index2 != -1) && (index3 != -1)&& (index4 != -1)&& (index5 != -1)) //如果位置不为空
      {
        num[4] = str.substring(index1 + 1, index2).toInt();
        num[5] = str.substring(index2 + 1, index3).toInt();
        num[6] = str.substring(index4 + 1, index5).toInt();
        num22 = str.substring(index3 + 1, index4).toInt();
        unsigned char t1=num22/100;
        unsigned char t2=num22%100;
        NH4 = t1+t2*0.01;
        Serial.print("t18.txt=\"");
        Serial.print(num[4]);
        Serial.print("\"");
        Serial.write(hexEND, 3);
        Serial.print("t19.txt=\"");
        Serial.print(num[5]);
        Serial.print("\"");
        Serial.write(hexEND, 3);
        Serial.print("t20.txt=\"");
        Serial.print(NH4);
        Serial.print("\"");
        Serial.write(hexEND, 3);
        if(num[6])
        {
          Serial.print("t22.txt=\"No\"");
          Serial.write(hexEND, 3);
        }
        else
        {
          Serial.print("t22.txt=\"Yes\"");
          Serial.write(hexEND, 3);
        }
//      Serial.println(num1);
//      Serial.println(num2);
      }
    }
    
    if (i == 38)
    {

//      Serial.println();
//      Serial.println(50);
//        Serial.print("38:");
//        Serial.println(str);
//        Serial.write(hexEND, 3);
//      index1 = str.indexOf('&');//找到位置
//      index2 = str.substring((index1) + 1, str.length()).indexOf('&') + 1;
//      Serial.println(index1);
//      Serial.println(index2);
//      Serial.println(index3);
//      if ((index1 != -1) && (index2 != -1)) //如果位置不为空
//      {
//        if(str.length()>0)
        num[0] = str.substring(1, 2).toInt();
//        
        Serial.print("t5.txt=\"");
        Serial.print(num[0]);
        Serial.print("\"");
        Serial.write(hexEND, 3);
//      Serial.println(num3);
//      } 
    }
    
    if (i == 40)
    {

//            Serial.println();
//            Serial.println(49);
//            Serial.print("40:");
//            Serial.println(str);
//            Serial.println(str.length());
      index1 = str.indexOf('(');//找到位置
      index2 = str.indexOf(',');
      index3 = str.substring((index2) + 1, str.length()).indexOf(',') + 4;
      index4 = str.substring((index3) + 1, str.length()).indexOf(',') + 7;
      index5 = str.substring((index2) + 1, str.length()).indexOf('(') + 4;
//      if(index1>1)
//      {
//        index3 +=3 ;
//        index4 +=3 ;
//      }
//              Serial.println(index1);
//              Serial.println(index2);
//              Serial.println(index3);
//              Serial.println(index4);
      if ((index1 != -1) && (index2 != -1) && (index3 != -1)&& (index4 != -1)&& (index5 != -1)) //如果位置不为空
      {
        num[7] = str.substring(index1 + 1, index2).toInt();
        num[8] = str.substring(index2 + 1, index3).toInt();
        num[9] = str.substring(index4 + 1, index5).toInt();
        num33 = str.substring(index3 + 1, index4).toInt();
        unsigned char t1=num33/100;
        unsigned char t2=num33%100;
        NH4 = t1+t2*0.01;
        Serial.print("t23.txt=\"");
        Serial.print(num[7]);
        Serial.print("\"");
        Serial.write(hexEND, 3);
        Serial.print("t24.txt=\"");
        Serial.print(num[8]);
        Serial.print("\"");
        Serial.write(hexEND, 3);
        Serial.print("t25.txt=\"");
        Serial.print(NH4);
        Serial.print("\"");
        Serial.write(hexEND, 3);
        if(num[9])
        {
          Serial.print("t27.txt=\"No\"");
          Serial.write(hexEND, 3);
        }
        else
        {
          Serial.print("t27.txt=\"Yes\"");
          Serial.write(hexEND, 3);
        }
//      Serial.println(num1);
//      Serial.println(num2);
      }
    }
    
    if (i == 41)
    {

//            Serial.println();
//            Serial.println(49);
//            Serial.print("40:");
//            Serial.println(str);
//            Serial.println(str.length());
      index1 = str.indexOf(')');//找到位置
      index2 = str.indexOf(',');
      index3 = str.substring((index2) + 1, str.length()).indexOf(',') + 4;
      index4 = str.substring((index3) + 1, str.length()).indexOf(',') + 7;
      index5 = str.substring((index2) + 1, str.length()).indexOf(')') + 4;
//      if(index1>1)
//      {
//        index3 +=3 ;
//        index4 +=3 ;
//      }
//              Serial.println(index1);
//              Serial.println(index2);
//              Serial.println(index3);
//              Serial.println(index4);
      if ((index1 != -1) && (index2 != -1) && (index3 != -1)&& (index4 != -1)&& (index5 != -1)) //如果位置不为空
      {
        num[10] = str.substring(index1 + 1, index2).toInt();
        num[11] = str.substring(index2 + 1, index3).toInt();
        num[12] = str.substring(index4 + 1, index5).toInt();
        num44 = str.substring(index3 + 1, index4).toInt();
        unsigned char t1=num44/100;
        unsigned char t2=num44%100;
        NH4 = t1+t2*0.01;
        Serial.print("t28.txt=\"");
        Serial.print(num[10]);
        Serial.print("\"");
        Serial.write(hexEND, 3);
        Serial.print("t29.txt=\"");
        Serial.print(num[11]);
        Serial.print("\"");
        Serial.write(hexEND, 3);
        Serial.print("t30.txt=\"");
        Serial.print(NH4);
        Serial.print("\"");
        Serial.write(hexEND, 3);
        if(num[12])
        {
          Serial.print("t32.txt=\"No\"");
          Serial.write(hexEND, 3);
        }
        else
        {
          Serial.print("t32.txt=\"Yes\"");
          Serial.write(hexEND, 3);
        }
//      Serial.println(num1);
//      Serial.println(num2);
      }
    }
    //delay(500);
  }
  
}
