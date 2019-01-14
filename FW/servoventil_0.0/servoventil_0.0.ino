// záložka cti_me!
#define FW    "0.0-10.1.19"  // aktuální verze FW 

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


//*******************************************************************
#ifdef DEBUG    
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   
#else
  #define DPRINT(...)     
  #define DPRINTLN(...)   
#endif

#ifdef USE_SERVO 
  // TODO
#endif


void setup() { // nastavení
  pinMode(PWR_ON_PIN,OUTPUT);
  digitalWrite(PWR_ON_PIN,HIGH); 
  
  #ifdef USE_UART
     Serial.begin(BAUD);
  #endif
  #ifdef DEBUG
     DPRINT("FW:");
     DPRINTLN(FW);
  #endif  

  #ifdef USE_SERVO
     pinMode(PWR_SERVO_ON_PIN,OUTPUT);
     // TODO
  #endif

  #ifdef DEBUG
     DPRINTLN("INIT...OK");      
  #endif
}//konec setup

void loop() {

}// konec loop
