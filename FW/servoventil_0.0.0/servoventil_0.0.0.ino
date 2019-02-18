// záložka cti_me!
#define FW    "0.0.0"  // aktuální verze FW 18.2.2019

// přiřazení pinů na procesoru ATTINY 1634
//#define SCK_SCL_PIN      12 // PC1 pin 16 pro info  (SCK je na stejném vodiči jako SCL)
//#define MOSI_SDA_PIN     16 // PB1 pin 20 pro info  (MOSI je na stejném vodiči jako SDA)
//#define RX_PIN           1  // PA7 pin 2  pro info
//#define TX_PIN           0  // PB0 pin 1  pro info
#define PWR_SERVO_ON_PIN   2  // PA6 pin 3  spíná napájení serva
#define SERVO_PIN          3  // PA5 pin 4  výstup pro servo PWM
#define PWR_ON_PIN         4  // PA4 pin 5  spíná samodržení napájení procesoru
#define LOW_BAT_PIN        A0 // PA3 pin 6  jako A0 vstup z baterie, že je málé napájení baterie
#define LED_LOWBAT_PIN     6  // PA2 pin 7  výstup pro LED, že je málé napájení baterie
#define SW1_PIN            7  // PA1 pin 8  vstup DIP přepínač 1
#define SW2_PIN            8  // PA0 pin 9  vstup DIP přepínač 2
#define SW3_PIN            9  // PC5 pin 12 vstup DIP přepínač 3
#define SW4_PIN            10 // PC4 pin 13 vstup DIP přepínač 4

#include <avr/wdt.h>  // watchdog

// UART
#define USE_UART      // pokud není zakomentované používat sériovou linku (pro zakázání zakomentuj//)
#define BAUD   115200 // rychlost sériové linky 9600, 19200, 38400, bd
#define DEBUG         // pokud není zakomentované používat na sériovou linku pomocné výpisy (pro zakázání zakomentuj//)

// SERVO
#define USE_SERVO     // pokud není zakomentované používat servo
  
// DS3231 (RTC na adrese 0x68)
#define USE_DS3231    // pokud není zakomentované používat DS3231
#define DS_ADDRESS  0x68 
uint8_t SLEEP_PER = 2;  // nastavení intervalu spánku v minutách pro alarm v DS3231  

// Měření napájení pomocí A/D
#define USE_AD          // pokud není zakomentované používat měření napětí baterie pomocí AD
#define MIN_BATT    5.0 // minimální napětí z baterií
float volt;             // napětí z baterií

// Časovač v loop
unsigned long previousMillis =    0; // pomocná
const long interval =            100; // interval časovače

//*******************************************************************
#ifdef DEBUG    
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   
#else
  #define DPRINT(...)     
  #define DPRINTLN(...)   
#endif

#ifdef USE_SERVO 
  #include <SoftwareServo.h> // knihovna pro servo
  SoftwareServo servo;
#endif

#ifdef USE_DS3231 
  #include <Wire.h>          // knihovna pro I2C
  #include <ds3231.h>        // knihovna pro DS3231
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

void set_next_alarm(void){
    struct ts t;
    unsigned char wakeup_min;
    DS3231_get(&t);
    // calculate the minute when the next alarm will be triggered
    wakeup_min = (t.min / SLEEP_PER + 1) * SLEEP_PER;
    if (wakeup_min > 59) wakeup_min -= 60; 
    // flags define what calendar component to be checked against the current time in order
    // to trigger the alarm
    // A2M2 (minutes) (0 to enable, 1 to disable)
    // A2M3 (hour)    (0 to enable, 1 to disable) 
    // A2M4 (day)     (0 to enable, 1 to disable)
    // DY/DT          (dayofweek == 1/dayofmonth == 0)
    uint8_t flags[4] = { 0, 1, 1, 1 };
    // set Alarm2. only the minute is set since we ignore the hour and day component
    DS3231_set_a2(wakeup_min, 0, 0, flags);
    // activate Alarm2
    DS3231_set_creg(DS3231_CONTROL_INTCN | DS3231_CONTROL_A2IE);
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
  
  digitalWrite(PWR_ON_PIN,HIGH);      // podržet napájení (po uvolnění tlačítka
  
  digitalWrite(PWR_SERVO_ON_PIN,LOW); // vypnout napájení serva
  digitalWrite(SERVO_PIN,LOW);        // vypnout výstup pro PWM serva      
  digitalWrite(LED_LOWBAT_PIN,HIGH);  // zapnout LED

  wdt_enable(WDTO_2S);                // povolit watchdog ping 2s
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

  #ifdef USE_DS3231 
    #ifdef DEBUG
       DPRINT("RTC init...");
    #endif   
    Wire.begin();                           // aktivace I2C 
    Wire.beginTransmission(DS_ADDRESS);     // probuď zařízení na 0x68
    if(Wire.endTransmission()==0) {         // test zda byla odpověď ze zařízení na adrese 0x68 (RTC DS3231)
       DS3231_init(DS3231_CONTROL_INTCN);   // aktivace DS3231
       DS3231_clear_a2f();                  // smazat příznak alarmu a nastavit pin INT na vysokou impedanci
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
  
  #ifdef DEBUG
     DPRINT("DIP1-4 init...");
     DPRINTLN("OK");
     DPRINT("DIP1=");
     DPRINT(digitalRead(SW1_PIN));DPRINT(" DIP2=");DPRINT(digitalRead(SW2_PIN));DPRINT(" DIP3=");DPRINT(digitalRead(SW3_PIN));DPRINT(" DIP4=");DPRINTLN(digitalRead(SW4_PIN));
  #endif
  
  #ifdef USE_AD
    getBATT();
    if(volt<MIN_BATT) {
      #ifdef DEBUG
         DPRINTLN("Chyba - napeti baterie <5V!"); // pro výpis na UART
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
       delay(1000);
       getBATT();
       wdt_reset();                        // vynulovat časovač pro wdt
    }//konec while
    
  #endif // AD

  #ifdef USE_SERVO
     #ifdef DEBUG
        DPRINT("Servo init...");
     #endif
     pinMode(PWR_SERVO_ON_PIN,OUTPUT);      // napájení serva výstupní pin
     digitalWrite(PWR_SERVO_ON_PIN, HIGH);  // zapnout napájení serva
     delay(100);
     servo.attach(SERVO_PIN);               // aktivovat servo
     #ifdef DEBUG
        DPRINTLN("OK");
     #endif
  #endif  
  
  #ifdef USE_DS3231 
    #ifdef DEBUG
       DPRINT("RTC nastavuji interval...");
    #endif
//    set_next_alarm();                       // nastavit kdy se probudi CPU podle DIP
    #ifdef DEBUG
       DPRINTLN("OK");
    #endif
  #endif
      
  digitalWrite(LED_LOWBAT_PIN,LOW);         // vypnout LED
  wdt_reset();                        // vynulovat časovač pro wdt
}//konec setup

int test=0; // pro test níže v loop

void loop(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) { // časovač
    previousMillis = currentMillis;
// test serva, blikání LED, měření baterie
    if(test==1 or test==90 or test==180) getBATT();  
    digitalWrite(LED_LOWBAT_PIN,!digitalRead(LED_LOWBAT_PIN)); // blikame LED
    if(test<180) test++;
    if(test==180) {
      set_next_alarm();                      // nastavit kdy se probudi CPU podle DIP
      #ifdef DEBUG
        DPRINT("Vypinam!");
        delay(1000);
        wdt_reset();                        // vynulovat časovač pro wdt
      #endif
      digitalWrite(SERVO_PIN,LOW);           // vypnout výstup pro PWM serva
      digitalWrite(PWR_SERVO_ON_PIN,LOW);    // vypnout napájení serva
      digitalWrite(PWR_ON_PIN,LOW);          // vypnout napájení CPU
      while(1); // čekáme na reset od wdt
    }
  }//konec if
    
  #ifdef USE_SERVO
     SoftwareServo::refresh();           // obnova pro servo
     servo.write(test);
  #endif
  wdt_reset();                        // vynulovat časovač pro wdt
}// konec loop
