/*
 * Serial  =======computer
 * softSerial ======uno Serial1
 */
//Slave_4
//从机4，实现测异味，温湿度，红外
#include <SoftwareSerial.h>
#include <TroykaDHT.h>
#include <MQUnifiedsensor.h>
SoftwareSerial Slave(10, 11);
DHT dht(4, DHT11);

#define placa "Arduino UNO"
#define Voltage_Resolution 5
#define pin A0 //Analog input 0 of your arduino
#define type "MQ-135" //MQ135
#define ADC_Bit_Resolution 10 // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6//RS / R0 = 3.6 ppm  
//#define calibration_button 13 //Pin to calibrate your sensor

//Declare Sensor
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);
unsigned char body;
void setup()
{
  Serial.begin(9600);
  pinMode(2,INPUT);    
  Serial.println("Slave is ready!");
  Slave.begin(9600); 
  dht.begin();
  MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ135.init(); 
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"); while(1);}
  while(Serial.read()>= 0){}                 //清空串口0缓存
  while(Slave.read()>= 0){}               //清空串口1缓存
}

void loop()
{   
  char value;
  unsigned char Temperature;
  unsigned char Humidity;
  MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
  MQ135.setA(102.2 ); MQ135.setB(-2.473); // Configurate the ecuation values to get NH4 concentration
  float NH4 = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  dht.read();
  //Serial.print("Temperature = ");
  Temperature=dht.getTemperatureC();
  if(digitalRead(2)==HIGH)  {
    body = 0;
  }
  else  {
    body = 1;
  }
  //Serial.print(Temperature);
 //Serial.println(" C \t");
  //Serial.print("Humidity = ");
  Humidity = dht.getHumidity();
  //Serial.print(Humidity);
        //Serial.println(" %");
  if(Slave.available())
  {
    String str="";
    delay(2);
    value = Slave.read();
    //Serial.print(value);          //把地址写到串口中去
    if(value==')') 
    {
        str+=')';
        str +=Temperature;
        str +=',';
        str +=Humidity;
        str +=',';
        str +=(unsigned int)(NH4*100);
        str +=',';
        str +=body;
        str+=')';
        Slave.print(str);
       Serial.println(str); 
    }
//    else
//    break;
  }
  delay(80);

}
