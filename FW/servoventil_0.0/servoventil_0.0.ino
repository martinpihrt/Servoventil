// záložka cti_me!
#define FW    "0.0-10.1.19"  // aktuální verze FW 

// přiřazení pinů na procesoru ATTINY4313
//#define SCL_PIN             // PB7 pin 19 pro info
//#define SDA_PIN             // PB5 pin 17 pro info
//#define RX_PIN              // PD0 pin 2  pro info
//#define TX_PIN              // PD1 pin 3  pro info
#define PWR_ON_PIN            // PD6 pin 11 spíná samodržení napájení procesoru
#define PWR_SERVO_ON_PIN      // PD5 pin 9  spíná napájení serva
#define SERVO_PIN             // PD4 pin 8  výstup pro servo
#define LOW_BAT_PIN           // PD3 pin 7  vstup z baterie, že je málé napájení baterie
#define LED_LOWBAT_PIN        // PB4 pin 16 výstup pro LED, že je málé napájení baterie
#define SW1_PIN               // PB3 pin 15 vstup DIP přepínač 1
#define SW2_PIN               // PB2 pin 14 vstup DIP přepínač 2
#define SW3_PIN               // PB1 pin 13 vstup DIP přepínač 3
#define SW4_PIN               // PB0 pin 12 vstup DIP přepínač 4

// UART
#define USE_UART      // pokud není zakomentované používat sériovou linku (pro zakázání zakomentuj//)
#define BAUD    9600  // výchozí rychlost sériové linky 9600bd
#define DEBUG         // pokud není zakomentované používat na sériovou linku pomocné výpisy (pro zakázání zakomentuj//)

// SERVO
#define USE_SERVO     // pokud není zakomentované používat servo


//*******************************************************************
#ifdef DEBUG     // pro pomocné výpisy   
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   
#else
  #define DPRINT(...)     
  #define DPRINTLN(...)   
#endif

/*
#ifdef USE_SERVO // pokud je použito servo
  #include <Servo.h>
  Servo servo;
#endif
*/

void setup() { // nastavení
  //pinMode(PWR_ON_PIN,OUTPUT);
  //digitalWrite(PWR_ON_PIN,HIGH); // podržet napájení
  
  #ifdef USE_UART
     Serial.begin(BAUD);
  #endif
  #ifdef DEBUG
     DPRINT("FW:");
     DPRINTLN(FW);
  #endif  
  /*
  #ifdef USE_SERVO
     pinMode(PWR_SERVO_ON_PIN, OUTPUT);
     servo.attach(SERVO_PIN);
     //servo.detach(SERVO_PIN); // odpojit PWM serva
     //servo.write(0); // 0-180
  #endif
*/
  #ifdef DEBUG
     DPRINTLN("INIT...OK");      
  #endif
}//konec setup

void loop() {

}// konec loop
