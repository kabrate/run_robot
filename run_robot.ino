#include <Servo.h> 
int RECV_PIN = A4;//端口声明
IRrecv irrecv(RECV_PIN);
decode_results results;//结构声明
int on = 0;//标志位
unsigned long last = millis();
char getstr; //指令
int Left_motor=8;     //左电机(IN3) 输出0  前进   输出1 后退
int Left_motor_pwm=9;     //左电机PWM调速

int Right_motor_pwm=10;    // 右电机PWM调速
int Right_motor=11;    // 右电机后退(IN1)  输出0  前进   输出1 后退

#define PORT_KEY  A2     //定义按键 数字A2 接口
#define PORT_LED1  7    //定义LED 数字7 接口
#define PORT_LED2  12   //定义LED2 数字12 接口
int beep=A3;//定义蜂鸣器 数字A3 接口

#define KEYMODE_1   1  // 定义按键模式1
#define KEYMODE_2   2  // 定义按键模式2
#define KEYMODE_3   3  // 定义按键模式3
uint8_t keyMode;      //指无符号8bit整型数
const int SensorRight = 3;   	//右循迹红外传感器(P3.2 OUT1)
const int SensorLeft = 4;     	//左循迹红外传感器(P3.3 OUT2)
const int SensorMid = 13; 
const int SensorRight_2 = 5;   	//左边红外避障传感器()
const int SensorLeft_2 = 6;   	//右边红外避障传感器()

int SR_2;    //右边红外避障传感器状态
int SL_2;    //左边红外避障传感器状态
int SL;    //左循迹红外传感器状态
int SR;    //右循迹红外传感器状态
int SM;    //中循迹红外传感器状态

void setup()
{
  //初始化电机驱动IO为输出方式
  pinMode(Left_motor,OUTPUT); // PIN 8 8脚无PWM功能
  pinMode(Left_motor_pwm,OUTPUT); // PIN 9 (PWM)
  pinMode(Right_motor_pwm,OUTPUT);// PIN 10 (PWM) 
  pinMode(Right_motor,OUTPUT);// PIN 11 (PWM)
  pinMode(beep,OUTPUT);

  pinMode(SensorRight, INPUT); //定义右循迹红外传感器为输入
  pinMode(SensorLeft, INPUT); //定义左循迹红外传感器为输入
  pinMode(SensorMid, INPUT); //定义左循迹红外传感器为输入
  pinMode(SensorRight_2, INPUT); //定义右红外传感器为输入
  pinMode(SensorLeft_2, INPUT); //定义右红外传感器为输入
  //pinMode(13, OUTPUT);////端口模式，输出
  Serial.begin(9800);	//波特率9800
  irrecv.enableIRIn(); // Start the receiver 这个函数对寻迹  壁障的PWM控制有影响  右转函数
  
   KeyScanInit();
   LEDInit();
}

//=======================智能小车的基本动作=========================
//void run(int time)     // 前进
void run()
{
  digitalWrite(Right_motor,LOW);  // 右电机前进
  digitalWrite(Right_motor_pwm,HIGH);  // 右电机前进     
  analogWrite(Right_motor_pwm,80);//PWM比例0~255调速，左右轮差异略增减
  
  
  digitalWrite(Left_motor,LOW);  // 左电机前进
  digitalWrite(Left_motor_pwm,HIGH);  //左电机PWM     
  analogWrite(Left_motor_pwm,80);//PWM比例0~255调速，左右轮差异略增减
  //delay(time * 80);   //执行时间，可以调整  
}
void walk(int time)
{
  digitalWrite(Right_motor,LOW);  // 右电机前进
  digitalWrite(Right_motor_pwm,HIGH);  // 右电机前进     
  analogWrite(Right_motor_pwm,60);//PWM比例0~255调速，左右轮差异略增减
  
  
  digitalWrite(Left_motor,LOW);  // 左电机前进
  digitalWrite(Left_motor_pwm,HIGH);  //左电机PWM     
  analogWrite(Left_motor_pwm,60);//PWM比例0~255调速，左右轮差异略增减
  delay(time * 80);   //执行时间，可以调整  
}
//void brake(int time)  //刹车，停车
void brake()
{
 digitalWrite(Right_motor_pwm,LOW);  // 右电机PWM 调速输出0      
  analogWrite(Right_motor_pwm,0);//PWM比例0~255调速，左右轮差异略增减

  digitalWrite(Left_motor_pwm,LOW);  //左电机PWM 调速输出0          
  analogWrite(Left_motor_pwm,0);//PWM比例0~255调速，左右轮差异略增减
  //delay(time * 80);//执行时间，可以调整  
}

//void left(int time)         //左转(左轮不动，右轮前进)
void left()
{
  digitalWrite(Right_motor,LOW);  // 右电机前进
  digitalWrite(Right_motor_pwm,LOW);  // 右电机停车     
  analogWrite(Right_motor_pwm,0);//PWM比例0~255调速，左右轮差异略增减
  
  
  digitalWrite(Left_motor,HIGH);  // 左电机后退
  digitalWrite(Left_motor_pwm,HIGH);  //左电机PWM     
  analogWrite(Left_motor_pwm,80);//PWM比例0~255调速，左右轮差异略增减
  //delay(time * 80);	//执行时间，可以调整  
}

void spin_left()         //左转(左轮后退，右轮前进)
{
   digitalWrite(Right_motor,LOW);  // 右电机前进
  digitalWrite(Right_motor_pwm,HIGH);  // 右电机前进     
  analogWrite(Right_motor_pwm,80);//PWM比例0~255调速，左右轮差异略增减
  
  digitalWrite(Left_motor,HIGH);  // 左电机后退
  digitalWrite(Left_motor_pwm,HIGH);  //左电机PWM     
  analogWrite(Left_motor_pwm,80);//PWM比例0~255调速，左右轮差异略增减
  //delay(time * 80);	//执行时间，可以调整   
}

//void right(int time)        //右转(右轮不动，左轮前进)
void right()//
//irrecv.enableIRIn(); // Start the receiver 这个函数对寻迹  壁障的PWM控制有影响  右转函数
{
   digitalWrite(Right_motor,HIGH);  // 右电机后退
  digitalWrite(Right_motor_pwm,HIGH);  // 右电机后退     
  analogWrite(Right_motor_pwm,80);//PWM比例0~255调速，左右轮差异略增减
  
  digitalWrite(Left_motor,LOW);  // 左电机前进
  digitalWrite(Left_motor_pwm,LOW);  //左电机PWM0  停车     
  analogWrite(Left_motor_pwm,0);//PWM比例0~255调速，左右轮差异略增减
  //delay(time * 80);	//执行时间，可以调整  
}

void spin_right()        //右转(右轮后退，左轮前进)
////irrecv.enableIRIn(); // Start the receiver 这个函数对寻迹  壁障的PWM控制有影响  右转函数
{
   digitalWrite(Right_motor,HIGH);  // 右电机后退
  digitalWrite(Right_motor_pwm,HIGH);  // 右电机PWM输出1     
  analogWrite(Right_motor_pwm,80);//PWM比例0~255调速，左右轮差异略增减
  
  digitalWrite(Left_motor,LOW);  // 左电机前进
  digitalWrite(Left_motor_pwm,HIGH);  //左电机PWM     
  analogWrite(Left_motor_pwm,80);//PWM比例0~255调速，左右轮差异略增减
  //delay(time * 80);	//执行时间，可以调整    
}

//void back(int time)          //后退
void back()
{
  digitalWrite(Right_motor,HIGH);  // 右电机后退
  digitalWrite(Right_motor_pwm,HIGH);  // 右电机前进     
  analogWrite(Right_motor_pwm,80);//PWM比例0~255调速，左右轮差异略增减
  
  digitalWrite(Left_motor,HIGH);  // 左电机后退
  digitalWrite(Left_motor_pwm,HIGH);  //左电机PWM     
  analogWrite(Left_motor_pwm,80);//PWM比例0~255调速，左右轮差异略增减
  //delay(time * 80);     //执行时间，可以调整  
}
//==========================================================


void dump(decode_results *results)
{
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) 
  {
    //Serial.println("Could not decode message");
    brake();
  } 
}
void Bluetooth()                             //机器人蓝牙遥控子程序
{
   
  getstr=Serial.read(); // 
  if(getstr=='A') 
  { 
    Serial.println("go forward!"); 
    run();//调用前进函数
  } 
  else if(getstr=='B'){ 
    Serial.println("go back!"); 
   back(); //调用后退函数
  } 
  else if(getstr=='C'){ 
    Serial.println("go left!"); 
   left();//调用左转函数
  } 
  else if(getstr=='D'){ 
    Serial.println("go right!"); 
   right();//调用右转函数
     
  } 
   else if(getstr=='F'){ 
    Serial.println("Stop!"); 
   brake(); //调用暂停函数
     
  } 
   else if(getstr=='E'){ 
    Serial.println("Stop!"); 
   brake(); //调用暂停函数 
  } 

}
void Robot_Avoidance()                   //机器人避障子程序
{
   //有信号为LOW  没有信号为HIGH
    SL_2 = digitalRead(SensorLeft_2);
    if (SL_2 == HIGH)
      {
        run();   //调用前进函数
         digitalWrite(beep,LOW);
      }

    else // 都是有障碍物, 后退
    {
        digitalWrite(beep,HIGH);		//蜂鸣器响
         brake();
    }
}
void Robot_Traction()                     //机器人循迹子程序
{
   //有信号为LOW  没有信号为HIGH
  SR = digitalRead(SensorRight);//有信号表明在白色区域，车子底板上L1亮；没信号表明压在黑线上，车子底板上L1灭
  SL = digitalRead(SensorLeft);//有信号表明在白色区域，车子底板上L2亮；没信号表明压在黑线上，车子底板上L2灭
  SM=digitalRead(SensorMid);
  //有信号为LOW  没有信号为HIGH
  SL_2 = digitalRead(SensorLeft_2);
      //if (SL_2 == HIGH)//前方没有障碍
      
          if (SL == LOW&&SR==LOW)
            run();   //调用前进函数
          else if (SL == HIGH && SR == LOW)// 左循迹红外传感器,检测到信号，车子向右偏离轨道，向左转 
            spin_left();
          else if (SR == HIGH &&SL == LOW) // 右循迹红外传感器,检测到信号，车子向左偏离轨道，向右转  
            spin_right();
          else if (SR==HIGH && SL==HIGH)
            brake();
         //digitalWrite(beep,LOW);
      
    // else // 都是有障碍物, 停止
    //{
       // digitalWrite(beep,HIGH);    //蜂鸣器响
        // brake();
   // }

   
}
// 按键处理初始化
void KeyScanInit(void)
{
    pinMode(PORT_KEY,INPUT_PULLUP); //输入模式，内部上拉。
    keyMode = KEYMODE_1;
}

// 任务：按键处理
void KeyScanTask(void)//
{
    static uint8_t keypre = 0; //按键被按下时置1.
    if( (keypre == 0) && (digitalRead(PORT_KEY) == HIGH) ) //按键被按下
    {
        keypre = 1; //置1，避免持续按下按键时再次进入此函数体。
        switch(keyMode)
        {
        case KEYMODE_1:
            keyMode = KEYMODE_3;  //关键点错位赋值
            break;
        case KEYMODE_3:
            keyMode = KEYMODE_1;
            break;
        default:
            break;
        }
    }
    if(digitalRead(PORT_KEY) == LOW) //按键被放开
    {
        keypre = 0; //置0，允许再次切换LED模式
    }
}
// LED初始化
void LEDInit(void)
{
    pinMode(PORT_LED1,OUTPUT);
    pinMode(PORT_LED2,OUTPUT);
    digitalWrite(PORT_LED1,LOW);
    digitalWrite(PORT_LED2,LOW);
}
// 任务：循迹、避障、遥控模式处理
void LEDTask(void)
{
    switch(keyMode)
    {
    case KEYMODE_1:
        digitalWrite(PORT_LED1,HIGH);
        digitalWrite(PORT_LED2,LOW);
        Robot_Traction(); //调用循迹子程序
        break;

    case KEYMODE_3:
        digitalWrite(PORT_LED1,HIGH);
        digitalWrite(PORT_LED2,HIGH);
        int i=digitalRead(PORT_LED1);
        //irrecv.enableIRIn();
        Bluetooth();    //调用蓝牙遥控子程序
        break;
    default:
        break;
    }
}
void loop()
{ 
  while(1)
  {
   KeyScanTask();
   LEDTask();
   //Robot_Avoidance();
  }
  
}
