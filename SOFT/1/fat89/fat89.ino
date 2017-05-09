#include <SoftwareSerial.h>

#define PWR_KEY   4
#define RESET_KEY 5
#define PWR_STAT_PIN A7
#define LINK_STAT_PIN 3

unsigned long previosMillis = 0;
bool b100Hz, b10Hz, b1Hz;
unsigned char cnt0,cnt1;
signed char powerOnCnt;
signed char resetCnt;
enum enumModemState {MS_UNKNOWN, MS_POWEROFF, MS_UNLINKED, MS_LINKED, MS_GPRS};
enumModemState modemState = MS_UNKNOWN;
char modemStatCnt0=0;
signed short modemStatPwrAnalogBuffer=0;
#define MODEM_PWR_STAT_CONST=500;
char net_l_cnt_up,net_l_cnt_down;
short net_l_cnt_one, net_l_cnt_zero, net_l_cnt_one_temp; 

char toMainRxBuffer[20];
char toMainRxBufferPtr;
bool toMainRxBufferIsEmpty=1;

SoftwareSerial toMain(8, 7);


//***********************************************
//Отладка
bool bBLINK=1;


//-----------------------------------------------
//анализ данных пришедших от платы контроллера
void toMainAn(void)
{
bool p;

if(toMainRxBufferIsEmpty==0)
p=strstr(toMainRxBuffer,"LEDON");
if(p)
  {
  toMain.println("OK");  
  bBLINK=!bBLINK;
  Serial.println("ATD + +79139294352;");
  }
toMainRxBufferPtr=0;
toMainRxBufferIsEmpty=1;
}
//-----------------------------------------------
//оценка текущего состояния модема
void modem_stat_drv(void)
  {
  signed short temp_SS;
  bool net_l,net_l_old;
  
  if(modemStatCnt0<10)
    {
    modemStatCnt0++;
    if(modemStatCnt0>=10)
      {
      modemStatCnt0=0;
      temp_SS=analogRead(PWR_STAT_PIN);
      modemStatPwrAnalogBuffer=(modemStatPwrAnalogBuffer*3+temp_SS)/4;
      if(modemStatPwrAnalogBuffer>100)modemState=MS_POWEROFF;      
      }
    }
  pinMode(LINK_STAT_PIN,INPUT_PULLUP);
  if(digitalRead(LINK_STAT_PIN)==0) //если светодиод LINK горит
    {
    net_l_cnt_down=0;
    if(net_l_cnt_up<3)
      {
      net_l_cnt_up++;
      if(net_l_cnt_up==3)
        {
        if(modemStatPwrAnalogBuffer>100)modemState=MS_POWEROFF;
        else 
          {
          if((net_l_cnt_one_temp>4) && (net_l_cnt_one_temp<8))
            {
            if((net_l_cnt_zero>70) && (net_l_cnt_zero<90))modemState=MS_UNLINKED; 
            else if((net_l_cnt_zero>250) && (net_l_cnt_zero<350))modemState=MS_LINKED;  
            else if((net_l_cnt_zero>25) && (net_l_cnt_zero<35))modemState=MS_GPRS; 
            }
          }
        net_l_cnt_zero=0;   
        }
      }
    if(net_l_cnt_one<1000)
      {
      net_l_cnt_one++;  
      }      
    }
  else  //если светодиод LINK не горит
    {
    net_l_cnt_up=0; 
    if(net_l_cnt_down<3)
      {
      net_l_cnt_down++;
      if(net_l_cnt_down==3)
        {
        net_l_cnt_one_temp=net_l_cnt_one;
        net_l_cnt_one=0;   
        }
      }
    if(net_l_cnt_zero<1000)
      {
      net_l_cnt_zero++;  
      }
    }
  }
  
//-----------------------------------------------
//включение модема
void power_on(void)
  {
  powerOnCnt=5;  
  }

//-----------------------------------------------
//выключение модема
void power_off(void)
  {
  powerOnCnt=15;  
  }

//-----------------------------------------------
//сброс модема
void reset_modem(void)
  {
  resetCnt=2;  
  }
  
//-----------------------------------------------
//обработка входов включения-выключения и сброса модема
void power_on_off_reset_drv(void)
{
pinMode(PWR_KEY,OUTPUT);
if(powerOnCnt)
  {
  digitalWrite(PWR_KEY,1);
  powerOnCnt--;  
  }
else digitalWrite(PWR_KEY,0);

pinMode(RESET_KEY,OUTPUT);
if(resetCnt)
  {
  digitalWrite(RESET_KEY,1);
  resetCnt--;  
  }
else digitalWrite(RESET_KEY,0);
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  power_on();
  Serial.println("SMS Messages Receiver");
  toMain.begin(9600);
  toMain.println("To Main");
}

void loop()
{
if(millis()-previosMillis>=10)
  {
  previosMillis=millis();
  b100Hz=1;
  if(cnt0<10)
    {
    cnt0++;
    if(cnt0>=10)
      {
      cnt0=0;
      b10Hz=1;  
      }
    }
  if(cnt1<100)
    {
    cnt1++;
    if(cnt1>=100)
      {
      cnt1=0;
      b1Hz=1;  
      }
    }    
  }

while(toMain.available())
  {
  toMainRxBuffer[toMainRxBufferPtr++]=toMain.read();
  toMainRxBufferIsEmpty=0;  
  }
if(b100Hz)
  {
  b100Hz=0;
  modem_stat_drv();
  //if(modemState==MS_UNLINKED)digitalWrite(LED_BUILTIN, 0);
  //else digitalWrite(LED_BUILTIN, 1);
  digitalWrite(LED_BUILTIN, bBLINK);
  toMainAn();
  }
if(b10Hz)
  {
  b10Hz=0;
  power_on_off_reset_drv();
  //bBLINK=!bBLINK;
  }
if(b1Hz)
  {
  b1Hz=0;
   
  

  //Serial.print("analog signal    ");
  //Serial.println(modemStatPwrAnalogBuffer );
  }    

}
