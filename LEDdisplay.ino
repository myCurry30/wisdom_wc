#include <SoftwareSerial.h>
#include <Wire.h>
#include <math.h>
#include <MsTimer2.h>		//定时器中断库
SoftwareSerial Master(10, 11);

#define 	lxVal_max 	1000

int led = 13;
int pinDelay = 4; //管脚D4连接到继电器模块的信号脚
int flag_t=0;	//判断是否仍在计时
int tick2 = 0; //计数值,测光照强度定时中断
int tick1 = 0; //计数值，测臭味定时中断
int BH1750address = 0x23;

byte buff[2];


unsigned int val=0;
unsigned char Temperature=0,Humidity=0,Peoples=0,Csum=15;
unsigned char Tn,Hn,Nn,Pn,Cn=0;
unsigned int NH4=0;
unsigned char Temperature_[3]={0};
unsigned char Humidity_[3]={0};
unsigned int NH4_[3]={0};
//unsigned char body[6]={0};
unsigned char dst[70]={0};
unsigned char src_con[30]={0};
unsigned char num[10]={0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32};

void Time1_interrupts(void)
{
  //初始化
  noInterrupts();         //禁用全局中断
  TCCR1A = TCCR1A & B11111100;    //设置TCCR1A中WGN11和WGM10的值0，设置为模式0
  TCCR1B = TCCR1B & B11110000;    //设置TCCR1B中CS12、CS11、CS10的值为0，关闭定时器计数器
  TIMSK1 = TIMSK1 & B11111001;    //设置TIMSK1中OCIE1B和OCIE1A的值为0，
  
  //配置寄存器
  OCR1A = 15999;      //设置初值 32M/1Hz-1
  TCCR1B |= (1<<WGM12)|(1<<CS10)|(1<<CS12) ;//预分频为1024
  TIMSK1 |= (1<<OCIE1A);       //开启定时器中断
  tick1 = 0;

  //开启中断
  interrupts();
}

ISR(TIMER1_COMPA_vect)
{
  tick1++;
  if(tick1%300==0)//5分钟看一次臭味状况，决定是否打开排气扇
  {
    tick1=0;
    //除臭，放在getdata后面或里面
    float nh4 = NH4/100.0;
    //判断是否在计时中，还在计时则不进入判断
      if(nh4>3.0)
      {
        //打开排风扇，延时5分钟,定时器计时
        //打开继电器，定义继电器输入引脚为D3
        digitalWrite(pinDelay, HIGH);//输出HIGH电平,继电器模块闭合
      }
      else
      {
        digitalWrite(pinDelay, LOW);//输出LOW电平,继电器模块断开
      } 
  }
}


void getData(unsigned char addr,String str)
{
  unsigned char  index1 = 0, index2 = 0, index3 = 0, index4 = 0, index5 = 0;
  if(addr==36)
  {
	//body[3]=body[0];//记录上一次有人无人
    index1 = str.indexOf('$');//找到位置
    index2 = str.indexOf(',');
    index3 = str.substring((index2) + 1, str.length()).indexOf(',') + 4;
    index4 = str.substring((index3) + 1, str.length()).indexOf(',') + 7;
    index5 = str.substring((index2) + 1, str.length()).indexOf('$') + 4;
    if ((index1 != -1) && (index2 != -1) && (index3 != -1)&& (index4 != -1)&& (index5 != -1)) //如果位置不为空
    {
      Temperature_[0] = str.substring(index1 + 1, index2).toInt();
      Humidity_[0] = str.substring(index2 + 1, index3).toInt();
      //body[0] = str.substring(index4 + 1, index5).toInt();
      NH4_[0] = str.substring(index3 + 1, index4).toInt();
    }
  }
  else if(addr==37)
  {
	//body[4]=body[1];//记录上一次有人无人
    index1 = str.indexOf('%');//找到位置
    index2 = str.indexOf(',');
    index3 = str.substring((index2) + 1, str.length()).indexOf(',') + 4;
    index4 = str.substring((index3) + 1, str.length()).indexOf(',') + 7;
    index5 = str.substring((index2) + 1, str.length()).indexOf('%') + 4;
    if ((index1 != -1) && (index2 != -1) && (index3 != -1)&& (index4 != -1)&& (index5 != -1)) //如果位置不为空
    {
      Temperature_[1] = str.substring(index1 + 1, index2).toInt();
      Humidity_[1] = str.substring(index2 + 1, index3).toInt();
      //body[1] = str.substring(index4 + 1, index5).toInt();
      NH4_[1] = str.substring(index3 + 1, index4).toInt();
    }
  }
  else if(addr==40)
  {
	//body[5]=body[2];//记录上一次有人无人
    index1 = str.indexOf('(');//找到位置
    index2 = str.indexOf(',');
    index3 = str.substring((index2) + 1, str.length()).indexOf(',') + 4;
    index4 = str.substring((index3) + 1, str.length()).indexOf(',') + 7;
    index5 = str.substring((index2) + 1, str.length()).indexOf('(') + 4;
    if ((index1 != -1) && (index2 != -1) && (index3 != -1)&& (index4 != -1)&& (index5 != -1)) //如果位置不为空
    {
      Temperature_[2] = str.substring(index1 + 1, index2).toInt();
      Humidity_[2] = str.substring(index2 + 1, index3).toInt();
      //body[2] = str.substring(index4 + 1, index5).toInt();
      NH4_[2] = str.substring(index3 + 1, index4).toInt();
    }
  }
  else if(addr==38)
  {
    Peoples = str.substring(1,2).toInt();
  }
  Temperature = (Temperature_[0]+Temperature_[1]+Temperature_[2])/3;
  Humidity = (Humidity_[0]+Humidity_[1]+Humidity_[2])/3;
  NH4 = (NH4_[0]+NH4_[1]+NH4_[2])/3;
}

void convertData(unsigned char Temperature,unsigned char Humidity,unsigned int NH4,unsigned char Peoples)
{
  unsigned char t,h,p=0;
  char c=0;
  unsigned int n=0;
  t = Temperature;
  h = Humidity;
  n = NH4;
  p = Peoples;
  c = Csum-Peoples;
  if(c<0)
  c=0;
  if(t>9)
  {
    
    src_con[0] = t/10;
    src_con[1] = t%10;
    Tn=2;
  }
  else
  {
    
    src_con[0] = t;
    Tn=1;
  }
  if(h>9)
  {   
    src_con[Tn] = h/10;
    src_con[Tn+1] = h%10;
    Hn=2;
  }
  else
  {
    src_con[Tn] = h;
    Hn=1;
  }
  if(n<100)
  {
    src_con[Tn+Hn] = n/100;
    src_con[Tn+Hn+1] = 0x2e;
    src_con[Tn+Hn+2] = n/10;
    src_con[Tn+Hn+3] = n%10;
    Nn=4;
  }
  else if(n<1000)
  {
    src_con[Tn+Hn] = n/100;
    src_con[Tn+Hn+1] = 0x2e;
    src_con[Tn+Hn+2] = (n/10)%10;
    src_con[Tn+Hn+3] = n%10;
    Nn=4;
  }
  else if(n<10000)
  {
    src_con[Tn+Hn] = n/1000;
    src_con[Tn+Hn+1] = (n/100)%10;
    src_con[Tn+Hn+2] = 0x2e;
    src_con[Tn+Hn+3] = (n/10)%10;
    src_con[Tn+Hn+4] = n%10;
    Nn=5;
  }
  if(p<10)
  {
    src_con[Tn+Hn+Nn] = p;
    Pn = 1;
  }
  else if(p<100)
  {
    src_con[Tn+Hn+Nn] = p/10;
    src_con[Tn+Hn+Nn+1] = p%10;
    Pn = 2;
  }
  if(c<10)
  {
    src_con[Tn+Hn+Nn+Pn] = c;
    Cn = 1;
  }
  else if(c<100)
  {
    src_con[Tn+Hn+Nn+Pn] = c/10;
    src_con[Tn+Hn+Nn+Pn+1] = c%10;
    Cn = 2;
  }
}

/*
  计算校验和
  除包头外所有数据的异或和XOR
  s_addr数据包，len校验长度
  checksum 返回的校验和
*/
unsigned char check_sum(unsigned char *s_addr,unsigned int len)
{
     unsigned char checksum;
     unsigned int  i;
     checksum=0;
     for(i=0;i<len;i++)
         checksum = checksum ^ *(s_addr+i);
     return(checksum);
}

/*
  把数据打包成485格式
  打包好的数据放在uint8_t dst[]数组中，
  cmd指令码固定0x37,
  mac485地址00 00 00 00 00 00 00 00全部广播模式
  unsigned char scr_con[conlen]存放发送内容的数组
  unsigned int conlen 发送内容长度
  num 实时采集编号
*/
unsigned char pack485Buffer(unsigned char *dst)
{
  unsigned char  *ptr;
  unsigned int len;
  unsigned int i;
  
  ptr = dst;
  *ptr++ = 0xfe;
  *ptr++ = 0x98;
  /* 长度 */
  *ptr++ = 0x00;    //数据长度不含包头和校验
  *ptr++ = 0x00;    //先设为0，具体长度最后填进去
  /* 源地址*/
  *ptr++ = 0x97;
  
  /* 命令字*/
  *ptr++ = 0x37;    //cmd固定0x37
  /* 保留 */
  *ptr++ = 0x00;
  
  /* mac地址*/      //485地址00 00 00 00 00 00 00 00/01
  for(i=0;i<8;i++)
  *ptr++ = 0x00;  //全部为0
  /* 编号 */
  *ptr++ = num[0];   //41号 温度
  /* 保留 */
  *ptr++ = 0x00;
  /* 显示颜色 */
  *ptr++ = 0x11;
  /* 显示字体及字号 */
  *ptr++ = 0x31;
//**************************************//
  /* 显示内容长度 */
  //conlen转十六进制
  *ptr++ = Tn;
  /* 具体内容1温度 */
  for(i=0;i<Tn;i++)
  {
    *ptr++=src_con[i]+0x30;
  }
//**************************************//
  /* 具体内容2 */
  /* 编号 */
  *ptr++ = num[1];   //42号    湿度
  /* 保留 */
  *ptr++ = 0x00;
  /* 显示颜色 */
  *ptr++ = 0x11;
  /* 显示字体及字号 */
  *ptr++ = 0x31;
  /* 显示内容长度 */
  //conlen转十六进制
  *ptr++ = Hn;
  for(i=Tn;i<Tn+Hn;i++)
  {
    *ptr++=src_con[i]+0x30;
  }
//**************************************//
  /* 具体内容3 */
     /* 编号 */
  *ptr++ = num[2];   //43号    氨气浓度
  /* 保留 */
  *ptr++ = 0x00;
  /* 显示颜色 */
  *ptr++ = 0x11;
  /* 显示字体及字号 */
  *ptr++ = 0x31;
  /* 显示内容长度 */
  //conlen转十六进制
  *ptr++ = Nn;
  for(i=Tn+Hn;i<Tn+Hn+Nn;i++)
  {
    if((src_con[i])==0x2e)
    {
      *ptr=0x2e;
      ptr++;
      continue;
    }
    else
    {
      *ptr++ = src_con[i]+0x30;
    }
  }
//**************************************//  
  /* 具体内容4 */
  /* 编号 */
  *ptr++ = num[3];   //44号    人数
  /* 保留 */
  *ptr++ = 0x00;
  /* 显示颜色 */
  *ptr++ = 0x11;
  /* 显示字体及字号 */
  *ptr++ = 0x31;
  /* 显示内容长度 */
  //conlen转十六进制
  *ptr++ = Pn;
  for(i=Tn+Hn+Nn;i<Tn+Hn+Nn+Pn;i++)
  {
    *ptr++=src_con[i]+0x30;
  }
//**************************************//
  /* 具体内容5 */
  /* 编号 */
  *ptr++ = num[4];   //45号    空位
  /* 保留 */
  *ptr++ = 0x00;
  /* 显示颜色 */
  *ptr++ = 0x11;
  /* 显示字体及字号 */
  *ptr++ = 0x31;
  /* 显示内容长度 */
  //conlen转十六进制
  *ptr++ = Cn;
  for(i=Tn+Hn+Nn+Pn;i<Tn+Hn+Nn+Pn+Cn;i++)
  {
    *ptr++=src_con[i]+0x30;
  }
  /* 计算长度 */
  len = ptr-dst-1;//-1是减去包头长度
  /* 填充长度 */
  dst[2] = (len>>8);
  dst[3] = (len);
  /* 计算校验 */
  *ptr++ = check_sum(dst+1,len);//地址+1是跳过包头
  /* 需要加上头部以及尾部校验 */
  len+=2;
  return len;
}

//把dst的内容通过串口发出去
//发送数据
void sendData()  //num数据采集编号
{
  unsigned char conlen;
  convertData(Temperature,Humidity,NH4,Peoples);
  conlen=pack485Buffer(dst);
  Serial.write(dst,conlen);
  Serial.println();
  for(int i=0;i<conlen;i++)
  {
  Serial.print(dst[i],HEX);
  }
  Serial.println();
  delay(100);
}




int BH1750_Read(int address)
{
	int i=0;
	Wire.beginTransmission(address);
	Wire.requestFrom(address, 2);
	while(Wire.available())
	{
	  buff[i] = Wire.read();
	  i++;
	}
	Wire.endTransmission();
	return i;
}

void BH1750_Init(int address)
{
	Wire.beginTransmission(address);
	Wire.write(0x10);
	Wire.endTransmission();
}

//中断服务程序
void onTimer()
{
  tick2++;
  if(tick2==1800)		//隔半个小时测一次光照强度，改变灯光亮度
  {
    tick2=0;
  	BH1750_Init(BH1750address);
  	delay(200);
  	if(2==BH1750_Read(BH1750address))
  	{
  	  val=((buff[0]<<8)|buff[1])/1.2;
  	}
  	
  	if(val>lxVal_max)
  	  val=lxVal_max;
  	char a=((lxVal_max-val)*255)/lxVal_max;
  	analogWrite(led,a);	
  }
}


void setup()
{
  Time1_interrupts();//定时器1定时中断初始化
  pinMode(led,OUTPUT);//定义数字接口13 为输出
  pinMode(pinDelay, OUTPUT); //设置pinRelay脚为输出状态
  Wire.begin();
  Serial.begin(9600);                         //初始化
  Serial.println("Master is ready!");
  Master.begin(9600);
  MsTimer2::set(1000, onTimer); //设置中断，每1000ms进入一次中断服务程序 onTimer()
  MsTimer2::start(); //开始计时_开启定时器中断
  //MsTimer2::stop(); //关闭定时器中断

  BH1750_Init(BH1750address);
  delay(1000);
  if(2==BH1750_Read(BH1750address))
  {
	val=((buff[0]<<8)|buff[1])/1.2;
  }
  Serial.print(val,DEC);     
  Serial.println("[lx]"); 
  if(val>lxVal_max)
	  val=lxVal_max;
  char a=((lxVal_max-val)*255)/lxVal_max;
  analogWrite(led,a);
  while (Serial.read() >= 0) {}              //清空串口0缓存
  while (Master.read() >= 0) {}            //清空串口1缓存
}

void loop()
{
  char address;
  for (int i = 36 ; i < 41 ; i++)     //用来对每个从机进行一次大轮询
  {
    if(i==39)
    {
      continue;
    }
    address = i;
    Master.print(address);
    delay(1800);
    String str = "";
    while(Master.available())
    { 
      delay(2);
      str += char(Master.read());
    }
//    Serial.println(str);
    getData(i,str);
    sendData();
  }

}
