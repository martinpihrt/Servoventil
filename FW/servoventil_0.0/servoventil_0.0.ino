// záložka cti_me!
#define FW    "0.0.0"  // aktuální verze FW 

// přiřazení pinů na procesoru ATTINY 1634
//#define SCK_SCL_PIN      12 // PC1 pin 16 pro info  (SCK je na stejném vodiči jako SCL)
//#define MOSI_SDA_PIN     16 // PB1 pin 20 pro info  (MOSI je na stejném vodiči jako SDA)
//#define RX_PIN           1  // PA7 pin 2  pro info
//#define TX_PIN           0  // PB0 pin 1  pro info
#define PWR_SERVO_ON_PIN   2  // PA6 pin 3  spíná napájení serva
#define SERVO_PIN          3  // PA5 pin 4  výstup pro servo PWM
#define PWR_ON_PIN         4  // PA4 pin 5  spíná samodržení napájení procesoru
#define LOW_BAT_PIN        5  // PA3 pin 6  vstup z baterie, že je málé napájení baterie
#define LED_LOWBAT_PIN     6  // PA2 pin 7  výstup pro LED, že je málé napájení baterie
#define SW1_PIN            7  // PA1 pin 8  vstup DIP přepínač 1
#define SW2_PIN            8  // PA0 pin 9  vstup DIP přepínač 2
#define SW3_PIN            9  // PC5 pin 12 vstup DIP přepínač 3
#define SW4_PIN            10 // PC4 pin 13 vstup DIP přepínač 4

// UART
#define USE_UART      // pokud není zakomentované používat sériovou linku (pro zakázání zakomentuj//)
#define BAUD    9600  // výchozí rychlost sériové linky 9600bd
#define DEBUG         // pokud není zakomentované používat na sériovou linku pomocné výpisy (pro zakázání zakomentuj//)

// SERVO
#define USE_SERVO     // pokud není zakomentované používat servo

// DS3231 (RTC na adrese 0x68)
#define USE_DS3231    // pokud není zakomentované používat DS3231

// Měření napájení pomocí A/D
#define USE_AD        // pokud není zakomentované používat měření napětí baterie pomocí AD
#define VZORKY  10    // počet vzorků měření na AD


int sum = 0;          // suma ze vzorků 
unsigned char sample_count = 0; // aktuální vzorek (pomocná pro čítač)
float voltage = 0.0;  // výsledné napětí


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


void CheckSerial(){ // vzdálené nastavení serva
  static int value = 0;
  if (Serial.available()) {
    char ch = Serial.read();
    switch(ch) {
      case 'A':
        digitalWrite(PWR_SERVO_ON_PIN, HIGH);  // zapnout napájení serva
        servo.attach(SERVO_PIN);               // aktivovat servo
        #ifdef DEBUG
           DPRINTLN("UART Servo pripojeno...");
        #endif 
        break;
      case 'D':
        digitalWrite(PWR_SERVO_ON_PIN, LOW);  // vypnout napájení serva
        #ifdef DEBUG
           DPRINTLN("UART Servo odpojeno...");
        #endif
        break;
      case '0' ... '9':
        value=(ch-'0')*20;                   // znak 0-9 vynásob 20 -> pozici lze nastavit 0,20,40,60,80,100,120,140,160,180
        servo.write(value);
        #ifdef DEBUG
           DPRINT("UART Servo: ");
           DPRINTLN(value);
        #endif
      break;
    }
  }
}// konec void

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
  digitalWrite(LED_LOWBAT_PIN,LOW);   // vypnout LED
  
  
  #ifdef USE_UART
     Serial.begin(BAUD);              // rychlost linky 
     delay(1000);                     // počkáme na UART
  #endif
  
  #ifdef DEBUG
     DPRINT("FW:");
     DPRINT(FW);                      // verze FW
     DPRINT(" ");
     DPRINT(BUILDTM_DAY);             // den
     DPRINT(".");
     DPRINT(BUILDTM_MONTH);           // měsíc
     DPRINT(".");
     DPRINT(BUILDTM_YEAR);            // rok
     DPRINT(" ");
     DPRINTLN(BUILDTIME);             // čas kompilace  
  #endif  

  #ifdef USE_SERVO
     DPRINT("Servo init...");
     pinMode(PWR_SERVO_ON_PIN,OUTPUT);      // napájení serva výstupní pin
     digitalWrite(PWR_SERVO_ON_PIN, HIGH);  // zapnout napájení serva
     servo.attach(SERVO_PIN);               // aktivovat servo
     DPRINTLN("OK");
  #endif

  #ifdef USE_DS3231 
    DPRINT("RTC init...");
    Wire.begin();                           // aktivace I2C 
    DS3231_init(DS3231_CONTROL_INTCN);      // aktivace DS3231
    DS3231_clear_a2f();                     // smazat příznak alarmu a nastavit pin INT na vysokou impedanci
    DPRINTLN("OK");
  #endif

  #ifdef USE_AD
    DPRINT("Ubatt AD init...");
    while (sample_count < VZORKY) {
        int ad;
        ad = analogRead(LOW_BAT_PIN);
        sum += ad;
        DPRINT("AD");
        DPRINT(sample_count);
        DPRINT("=");
        DPRINTLN(ad);
        sample_count++;
        delay(10);
    }//konec while
    voltage = ((float)sum / (float)VZORKY * 3.3) / 1024.0; // výpočet napětí
    // 3.3 je referenční napětí AD
    DPRINT(voltage * 1.2);
    // 1.2 dle děliče
    DPRINTLN(" V");
    sample_count = 0;
    sum = 0;
    while(voltage<5.0){ // je malé napětí baterie
       digitalWrite(LED_LOWBAT_PIN,HIGH);   // zapnout LED
       delay(100);
       digitalWrite(LED_LOWBAT_PIN,LOW);    // vypnout LED
       delay(1000);
    }
    DPRINTLN("OK");
  #endif

  #ifdef DEBUG
     DPRINTLN("INIT OK");      
  #endif
}//konec setup

void loop() {

  #ifdef USE_SERVO
     CheckSerial();                    // kontroluj zda je něco na serialu
     SoftwareServo::refresh();         // obnova PWM pro servo
  #endif
}// konec loop
