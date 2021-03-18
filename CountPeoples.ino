#include <SoftwareSerial.h>
//#include <Wire.h>
#include <Adafruit_AMG88xx.h>
//Definitions
#define placa "Arduino UNO"
#define Voltage_Resolution 5
#define ADC_Bit_Resolution 10 // For arduino UNO/MEGA/NANO
SoftwareSerial Slave(10,11);
Adafruit_AMG88xx amg;
float pixels[AMG88xx_PIXEL_ARRAY_SIZE];
float ROW,COLUMN=0;
unsigned char num=0;
float Arr[5]={0};
void coordinate()
{
  unsigned char num=1,temp = 1;
  unsigned char i=0,j=0,Size=0,x=0,y=0,row=0,column = 0;
  unsigned char pixelMake[8][8]={0};
  unsigned char count[6]={0};
  float pixelFive[64]={0};
  float pixelDif[64]={0};
  //连续取5帧像素
  for( Size=0;Size<5;Size++)
  {
    amg.readPixels(pixels);
    for( i=0;i<64;i++)
    {
      pixelFive[i] += pixels[i];
    }
    delay(70);
  }
  //取第6帧
  delay(70);
  amg.readPixels(pixels);
  for( i=0;i<64;i++)
  {
    pixelDif[i] += pixels[i];
  }
  //计算前5帧的平均值，第6帧与前5帧做差统计出差值
  //差值大于1.5为，否则为0，做出二维灰度值像素图
  for(i=0;i<64;i++)
  {
    pixelDif[i]=(pixelDif[i]-(pixelFive[i]*100/5/100+(((int)(pixelFive[i]*100/5))%100)*0.01));
    if(pixelDif[i]>1.5)
      pixelDif[i]=1;
    else
      pixelDif[i]=0;
  }
  for(i=0;i<8;i++)
  {
    for(j=0;j<8;j++)
    {
      pixelMake[i][j]=pixelDif[i*8+j];  //注意这步把像素点反转了一下，一维变二维
    }
  }

  //开始四邻域，逐行标记num标签从1开始
  for(x=0;x<8;x++)
  {
    for(y=0;y<8;y++)
    {
      if((x==0)&&(y==0))
      {
        if(pixelMake[x][y])
          {
            pixelMake[x][y]=num;
            num++;
          }
      } 
      if((x==0)&&(y!=0))
      {
        if(pixelMake[x][y])
        {
          if(pixelMake[x][y-1])
          {
            pixelMake[x][y]=pixelMake[x][y-1];
          }
          else 
          {
            pixelMake[x][y] =num;
            num++;
          } 
        }
      }
      if((y==0)&&(x!=0))
      {
        if(pixelMake[x][y])
        {
          if(pixelMake[x-1][y])
          {
            pixelMake[x][y]=pixelMake[x-1][y];
          }
          else
          {
            pixelMake[x][y]=num;
            num++;
          }
        }
      }
    if((x!=0)&&(y!=0))
    {
    if(pixelMake[x][y])
      {
      if(pixelMake[x-1][y]&&pixelMake[x][y-1])
      {
        pixelMake[x][y]=min(pixelMake[x-1][y],pixelMake[x][y-1]);
        pixelMake[x-1][y]=pixelMake[x][y];
        pixelMake[x][y-1]=pixelMake[x][y];
        
      }
      else if(pixelMake[x-1][y])
      {
        pixelMake[x][y]=pixelMake[x-1][y];
      }
      else if(pixelMake[x][y-1])
      {
        pixelMake[x][y]=pixelMake[x][y-1];
      }
      else
      {
        pixelMake[x][y]=num;
        num++;
      }
      }
    } 
    }
  }
/*计算每个标签的面积，只计算前五个*/  
  for(i=0;i<8;i++)
  {
    for(j=0;j<8;j++)
    {
      switch(pixelMake[i][j])
      {
        case 1:
        {
          count[1]++;
        }break;
          
        case 2:
        {
          count[2]++;
        }break;
          
        case 3:
        {
          count[3]++;
        }break;
          
        case 4:
        {
          count[4]++;
        }break;
        case 5:
        {
          count[5]++;
        }break;
          
        default:break;
      }
      
    }
  }
  //确定哪个标签最多
  for(i=2;i<6;i++)
  {
    if(count[temp] < count[i]) 
    {                    
      temp = i;
    }  
  }
//把面积最大的那个标签传回，标签temp最多
//  统计标签为temp的数组横纵坐标，确定热源中心点。
  row=0;
  column =0;
  for(i=0;i<8;i++)
  {
    for(j=0;j<8;j++)
    {
      if(pixelMake[i][j] == temp )
      {   
        count[0]++; 
        row +=i;
        column +=j;
      }
    }
  }

  //中心点坐标精确到2位小数
  ROW =(row*10/count[0]/10+(((int)(row*10/count[0]))%10)*0.1);
  COLUMN =(column*100/count[0]/100+(((int)(column*100/count[0]))%100)*0.01);
  if(row==0)
  ROW=0;
  if(column==0)
  COLUMN=0;
}

//统计人数
void CountPeoples(void)
{
  for (char i = 0; i < 4; i++)
  {
    Arr[i] = Arr[i + 1];
  }
  Arr[4] = ROW;
  if((Arr[0]==0||(Arr[0]<Arr[1]))&&Arr[1]<Arr[2]&&Arr[2]<Arr[3]&&(Arr[4]==0||(Arr[3]<Arr[4])))
  {
    num++;
    for(char i=0;i<5;i++)
    {
      Arr[i]=0;
    }
  }
  if((Arr[0]==0||(Arr[0]>Arr[1]))&&Arr[1]>Arr[2]&&Arr[2]>Arr[3]&&((Arr[3]>Arr[4])||Arr[4]==0))
  {
    if(num==0)
      num=0;
    else
      num--;
    for(char i=0;i<5;i++)
    {
      Arr[i]=0;
    }
  }   
}

void setup() {
  Serial.begin(9600);
  Slave.begin(9600);
    bool status;
    // default settings
    status = amg.begin();
    if (!status) {
        Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
        while (1); 
    }
    delay(100); // let sensor boot up
    while(Serial.read() >= 0){}
    while(Slave.read() >= 0){}
}

void loop() {     
    char value;
    
    coordinate();
    CountPeoples();
    Serial.print("x=");
    Serial.print(ROW);
    Serial.print(", y=");
    Serial.println(COLUMN);
//    
//    Serial.print("num=");
//    Serial.println(num);

    while(Slave.available()>0)
    {
      String str="";
      delay(2);
      value = Slave.read();
      //Serial.println(str);          //把地址写到串口中去
      if(value=='&') 
      {
        str+='&';
        str +=num;
        str+='&';
       // Serial.println(value);          //把地址写到串口中去
        Slave.print(str);
       Serial.println(str);
      }
//          else
//    break;
    }
    
    //delay a second
    //delay(30);

}
