// záložka cti_me!
#define FW    "1.0.0"  // aktuální verze FW 27.4.2020

// přiřazení pinů na procesoru ATTINY 1634
//#define SCK_SCL_PIN            12   // PC1 pin 16 pro info  (SCK je na stejném vodiči jako SCL)
//#define MOSI_SDA_PIN           16   // PB1 pin 20 pro info  (MOSI je na stejném vodiči jako SDA)
//#define RX_PIN                 1    // PA7 pin 2  pro info
//#define TX_PIN                 0    // PB0 pin 1  pro info
#define PWR_SERVO_ON_PIN         2    // PA6 pin 3  spíná napájení serva
#define SERVO_PIN                3    // PA5 pin 4  výstup pro servo PWM
#define PWR_ON_PIN               4    // PA4 pin 5  spíná samodržení napájení procesoru
#define LOW_BAT_PIN              A0   // PA3 pin 6  jako A0 vstup z baterie, že je málé napájení baterie
#define LED_LOWBAT_PIN           6    // PA2 pin 7  výstup pro LED, že je málé napájení baterie
#define SW1_PIN                  7    // PA1 pin 8  vstup DIP přepínač 1
#define SW2_PIN                  8    // PA0 pin 9  vstup DIP přepínač 2
#define SW3_PIN                  9    // PC5 pin 12 vstup DIP přepínač 3
#define SW4_PIN                  10   // PC4 pin 13 vstup DIP přepínač 4

#define VENTIL_OTEVRENO_UHEL     89   // úhel serva pro otevření vody
#define VENTIL_ZAVRENO_UHEL      3    // úhel serva pro zavření vody
#define DOBA_PREBEHU_SERVA    2000    // 2000 ms -> 2 sec (8MHz) (nesmí být delší než 4 sec, jinak začne padat watchdog!)

#include <avr/wdt.h>                  // watchdog (hlídá chod CPU proti zamrznutí)

// UART
//#define USE_UART                      // pokud není zakomentované používat sériovou linku (pro zakázání zakomentuj//)
//#define BAUD   9600                   // rychlost sériové linky 9600, 19200, 38400, bd
//#define DEBUG                         // pokud není zakomentované používat na sériovou linku pomocné výpisy (pro zakázání zakomentuj//)

// SERVO
#define USE_SERVO                     // pokud není zakomentované používat servo
  
// DS3231 (RTC na adrese 0x68)
#define USE_WIRE                      // používat I2C
#define USE_DS3231                    // pokud není zakomentované používat DS3231
#define DS_ADDRESS  0x68 
uint8_t SLEEP_PER = 2;                // nastavení intervalu spánku v minutách pro alarm v DS3231  

// Měření napájení pomocí A/D
//#define USE_AD                      // pokud není zakomentované používat měření napětí baterie pomocí AD
#define MIN_BATT    5.0               // minimální napětí z baterií
float volt;                           // napětí z baterií

// Časovač v loop
unsigned long previousMillis =    0;  // pomocná
const long interval =            100; // interval časovače

// Pomocné
bool hod_interval;                    // true = 24 hodin, false = 12 hodin interval probouzení (nastavuje DIP 4)
byte ZapNaDobu;                       // nastavení času otevření ventilu od 1 do 7 minut (nastavuje DIP 1-3)
byte pocet_hodin, pocet_minut;        // dle DIP se nastaví na 12:00 nebo 23:59 hodin interval pro RTC

//*******************************************************************
#ifdef DEBUG    
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   
#else
  #define DPRINT(...)     
  #define DPRINTLN(...)   
#endif

#ifdef USE_SERVO 
  //https://github.com/platformio/platformio-pkg-framework-arduinoavr/tree/master/libraries/__cores__/tiny/Servo_ATTinyCore
  #include <Servo_ATTinyCore.h> // knihovna pro servo
  Servo servo;
#endif

#ifdef USE_WIRE
  #include "Wire.h"             // knihovna pro I2C
#endif

#ifdef USE_DS3231 
  #include "ds3231.h"          // knihovna pro DS3231
#endif

// makra pro datum kompilace
#define BUILDTM_YEAR (\
    __DATE__[7] == '?' ? 1900 \
    : (((__DATE__[7] - '0') * 1000 ) \
    + (__DATE__[8] - '0') * 100 \
    + (__DATE__[9] - '0') * 10 \
    + __DATE__[10] - '0'))

#define BUILDTM_MONTH (\
    __DATE__ [2] == '?' ? 1 \
    : __DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6) \
    : __DATE__ [2] == 'b' ? 2 \
    : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
    : __DATE__ [2] == 'y' ? 5 \
    : __DATE__ [2] == 'l' ? 7 \
    : __DATE__ [2] == 'g' ? 8 \
    : __DATE__ [2] == 'p' ? 9 \
    : __DATE__ [2] == 't' ? 10 \
    : __DATE__ [2] == 'v' ? 11 \
    : 12)

#define BUILDTM_DAY (\
    __DATE__[4] == '?' ? 1 \
    : ((__DATE__[4] == ' ' ? 0 : \
    ((__DATE__[4] - '0') * 10)) + __DATE__[5] - '0'))

#define BUILDTIME __TIME__    

void getBATT(){
 #ifdef USE_AD 
   int in = analogRead(LOW_BAT_PIN);
   volt = (in * 6.0) / 1024.0; // při 6V je za děličem 3.3V tedy 1023 max Ubat
   #ifdef DEBUG
     DPRINT("Baterie");
     DPRINT("=");
     DPRINT(volt);
     DPRINTLN("V");
   #endif
 #endif
}//konec void    

void read_DIP(){
   ZapNaDobu = 0;                            // doba 1-7 minut dle DIP
   if(!digitalRead(SW1_PIN)) ZapNaDobu  = 1; // 1 minuta
   if(!digitalRead(SW2_PIN)) ZapNaDobu += 2; // 2 minuty
   if(!digitalRead(SW3_PIN)) ZapNaDobu += 4; // 4 minuty
   
   if(!digitalRead(SW4_PIN)) hod_interval = true;
   else                      hod_interval = false;

  if(hod_interval) { pocet_hodin = 23; pocet_minut = 59; } // 24 hodin interval
  else             { pocet_hodin = 12; pocet_minut = 0;  } // 12 hodin interval
  
  #ifdef DEBUG
     DPRINT("DIP 1-3: ");
     DPRINT(ZapNaDobu);
     DPRINT(" minut, DIP 4: ");
     if(hod_interval) DPRINTLN("24 hodin.");
     else             DPRINTLN("12 hodin.");
  #endif
}//konec void

byte decToBcd(byte val){
    return( (val/10*16) + (val%10) );
}//konec byte

void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year){
    // sets time and date data to DS3231
    // DS3231 seconds, minutes, hours, day, date, month, year
    // setDS3231time(30,42,21,4,26,11,14);
    Wire.beginTransmission(DS3231_I2C_ADDR); 
    Wire.write(0); // set next input to start at the seconds register
    Wire.write(decToBcd(second)); // set seconds
    Wire.write(decToBcd(minute)); // set minutes
    Wire.write(decToBcd(hour)); // set hours
    Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
    Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
    Wire.write(decToBcd(month)); // set month
    Wire.write(decToBcd(year)); // set year (0 to 99)
    Wire.endTransmission();
}//konec void 

void set_next_alarm(void){
  #ifdef USE_DS3231 
    // nastavime datum a cas na 0:0:1 1.1.2020
    // DS3231 seconds, minutes, hours, day, date, month, year
    setDS3231time(1,0,0,1,1,1,20);

    // flags define what calendar component to be checked against the current time in order
    // to trigger the alarm
    // A2M2 (minutes) (0 to enable, 1 to disable)
    // A2M3 (hour)    (0 to enable, 1 to disable) 
    // A2M4 (day)     (0 to enable, 1 to disable)
    // DY/DT          (dayofweek == 1/dayofmonth == 0)
    byte flags[4] = { 0, 0, 1, 1 };
    // set Alarm2. only the minute is set since we ignore the hour and day component
    DS3231_set_a2(pocet_minut, pocet_hodin, 0, flags);
    // activate Alarm2
    DS3231_set_creg(DS3231_CONTROL_INTCN | DS3231_CONTROL_A2IE);
  #endif 
}//konec void 
  
//******************************************************************************************************************************

void setup() { // nastavení
  pinMode(PWR_ON_PIN,OUTPUT);         // pin pro samodržení napájení CPU
  pinMode(PWR_SERVO_ON_PIN,OUTPUT);   // pin pro napájení serva
  pinMode(SERVO_PIN,OUTPUT);          // pin pro PWM serva
  pinMode(LED_LOWBAT_PIN,OUTPUT);     // pin pro LED
  pinMode(LOW_BAT_PIN,INPUT);         // pin na AD
  pinMode(SW1_PIN,INPUT_PULLUP);      // pin DIP1 
  pinMode(SW2_PIN,INPUT_PULLUP);      // pin DIP2
  pinMode(SW3_PIN,INPUT_PULLUP);      // pin DIP3
  pinMode(SW4_PIN,INPUT_PULLUP);      // pin DIP4
  
  digitalWrite(PWR_ON_PIN,HIGH);      // podržet napájení (po uvolnění tlačítka)
  
  digitalWrite(PWR_SERVO_ON_PIN,LOW); // vypnout napájení serva
  digitalWrite(SERVO_PIN,LOW);        // vypnout výstup pro PWM serva      
  digitalWrite(LED_LOWBAT_PIN,HIGH);  // zapnout LED

  wdt_enable(WDTO_4S);                // povolit watchdog ping 4s
  wdt_reset();                        // vynulovat časovač pro wdt
  
  #ifdef USE_UART
     Serial.begin(BAUD);              // rychlost linky 
     delay(1000);                     // počkáme na UART
  #endif
  wdt_reset();                        // vynulovat časovač pro wdt
  
  #ifdef DEBUG
     DPRINTLN();
     DPRINT("FW:");
     DPRINT(FW);                      // verze FW
     DPRINT(",");
     DPRINT(BUILDTM_DAY);             // den
     DPRINT(".");
     DPRINT(BUILDTM_MONTH);           // měsíc
     DPRINT(".");
     DPRINT(BUILDTM_YEAR);            // rok
     DPRINT(",");
     DPRINTLN(BUILDTIME);             // čas kompilace  
  #endif  

  #ifdef USE_WIRE
     #ifdef DEBUG
       DPRINT("I2C init...");
     #endif 
     Wire.begin();                    // aktivace I2C 
  #endif
     
  #ifdef USE_DS3231 
    #ifdef DEBUG
       DPRINT("RTC init...");
    #endif                                     
    Wire.beginTransmission(DS_ADDRESS);           // probuď zařízení na 0x68
    if(Wire.endTransmission()==0) {               // test zda byla odpověď ze zařízení na adrese 0x68 (RTC DS3231)
       DS3231_init(DS3231_CONTROL_INTCN);         // aktivace DS3231
       DS3231_clear_a2f();                        // smazat příznak alarmu a nastavit pin INT na vysokou impedanci
       #ifdef DEBUG
          DPRINTLN("OK");
       #endif   
    }
    else {
      #ifdef DEBUG
         DPRINT("Chyba RTC na adrese: ");
         DPRINTLN(DS_ADDRESS);
      #endif   
    }
  #endif  

  read_DIP();                                     // čteme nastavení DIP spínačů
    
  #ifdef USE_AD
    getBATT();
    if(volt<MIN_BATT) {
      #ifdef DEBUG
         DPRINTLN("Chyba - baterie <5V!");  // pro výpis na UART
      #endif
    }// konec if
    
    while(volt<5.0){ // je malé napětí baterie budeme neustále točit a dál nic nebude dokud bat >5V
       digitalWrite(LED_LOWBAT_PIN,HIGH);   // zapnout LED
       delay(50);
       digitalWrite(LED_LOWBAT_PIN,LOW);    // vypnout LED
       delay(50);
       digitalWrite(LED_LOWBAT_PIN,HIGH);   // zapnout LED
       delay(50);
       digitalWrite(LED_LOWBAT_PIN,LOW);    // vypnout LED
       delay(5000);
       getBATT();
       wdt_reset();                        // vynulovat časovač pro wdt
    }//konec while  
  #endif // AD

  #ifdef USE_DS3231 
    #ifdef DEBUG
       DPRINT("RTC nastavuji interval...");
    #endif
    set_next_alarm();                      // nastavit kdy se probudi CPU podle DIP
    #ifdef DEBUG
       DPRINTLN("OK");
    #endif
  #endif

  #ifdef USE_SERVO
     #ifdef DEBUG
        DPRINTLN("Servo init...");
     #endif

     // OTEVRIT
     pinMode(PWR_SERVO_ON_PIN,OUTPUT);      // napájení serva výstupní pin
     digitalWrite(PWR_SERVO_ON_PIN, HIGH);  // zapnout napájení serva
     servo.attach(SERVO_PIN);               // aktivovat servo
     servo.write(VENTIL_OTEVRENO_UHEL);     // otočit na pozici xx stupňů
     
     #ifdef DEBUG
       DPRINTLN("Oteviram ventil...");
     #endif

     digitalWrite(LED_LOWBAT_PIN,HIGH);     // zapnout LED
     delay(DOBA_PREBEHU_SERVA);             // počkáme než servo dojede
     digitalWrite(LED_LOWBAT_PIN,LOW);      // vypnout LED
     
     digitalWrite(SERVO_PIN,LOW);           // vypnout výstup PWM serva
     digitalWrite(PWR_SERVO_ON_PIN,LOW);    // vypnout napájení serva

     // CEKAT
     for(long t=0; t<ZapNaDobu*60; t++){    // př: 1 minuta = 1000ms * 60 cyklů -> 60 000ms
       digitalWrite(LED_LOWBAT_PIN,HIGH);   // zapnout krátce LED (blik)
       delay(20);                           // 20 + 980 = 1000ms -> 1s
       digitalWrite(LED_LOWBAT_PIN,LOW);    // vypnout LED
       delay(980);
       wdt_reset();                         // vynulovat časovač pro wdt
     }//end for

     // ZAVRIT
     digitalWrite(PWR_SERVO_ON_PIN, HIGH);  // zapnout napájení serva
     servo.attach(SERVO_PIN);               // aktivovat servo
     servo.write(VENTIL_ZAVRENO_UHEL);      // otočit na pozici xx stupňů
     #ifdef DEBUG
       DPRINTLN("Zaviram ventil...");
     #endif

     digitalWrite(LED_LOWBAT_PIN,HIGH);     // zapnout LED
     delay(DOBA_PREBEHU_SERVA);             // počkáme než servo dojede
     digitalWrite(LED_LOWBAT_PIN,LOW);      // vypnout LED

     digitalWrite(SERVO_PIN,LOW);           // vypnout výstup PWM serva
     digitalWrite(PWR_SERVO_ON_PIN,LOW);    // vypnout napájení serva
  #endif  
        
  #ifdef DEBUG
    DPRINT("Vypinam napajeni!");
    delay(1000);
  #endif
  
  digitalWrite(PWR_ON_PIN,LOW);             // vypnout napájení CPU
  while(1);                                 // pokud nespadne CPU sám, tak WDT ho schodí
}//konec setup

void loop(){} // konec loop
