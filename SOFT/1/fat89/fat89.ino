#define PWR_KEY   4
#define RESET_KEY 5
#define PWR_STAT_PIN A7
#define LINK_STAT_PIN 3

unsigned long previosMillis = 0;
bool b100Hz, b10Hz, b1Hz;
unsigned char cnt0,cnt1;
signed char powerOnCnt;
signed char resetCnt;
enum enumModemState {MS_UNKNOWN, MS_POWEROFF, MS_UNLINKED, MS_LINKED};
enumModemState modemState = MS_UNKNOWN;
char modemStatCnt0=0;
signed short modemStatPwrAnalogBuffer=0;
#define MODEM_PWR_STAT_CONST=500; 



//-----------------------------------------------
//оценка текущего состояния модема
void modem_stat_drv(void)
  {
  signed short temp_SS;
  
  if(modemStatCnt0<10)
    {
    modemStatCnt0++;
    if(modemStatCnt0>=10)
      {
      modemStatCnt0=0;
      temp_SS=analogRead(PWR_STAT_PIN);
      modemStatPwrAnalogBuffer=(modemStatPwrAnalogBuffer*3+temp_SS)/4;      
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

if(b100Hz)
  {
  b100Hz=0;
  modem_stat_drv();
  }
if(b10Hz)
  {
  b10Hz=0;
  power_on_off_reset_drv();
  }
if(b1Hz)
  {
  b1Hz=0;
  }    

}
