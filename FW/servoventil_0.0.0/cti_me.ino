/* 
 * Pro podporu procesoru ATTINY 1634 je potřeba do Arduino IDE doplnit podporu z :http://drazzy.com/package_drazzy.com_index.json (https://github.com/SpenceKonde/ATTinyCore/blob/master/Installation.md)
 * Nastavení pojistek: 
   8 MHZ interní oscilátor
   ATTinyCore\hardware\avr\1.2.2/avrdude.conf -v -pattiny1634 -cstk500v2 -Pusb -e -Uefuse:w:0b11111110:m -Uhfuse:w:0b11010110:m -Ulfuse:w:0xE2:m 

   Verze FW:
   FW 0.0.0 Martin Pihrt - založení projektu
   FW x.x

výpis na serialu:

FW:0.0.0,18.2.2019,11:13:33
RTC init...OK
DIP1-4 init...OK
DIP1=0 DIP2=1 DIP3=0 DIP4=1
Baterie=5.86V
Servo init...OK
RTC nastavuji interval...OK
Baterie=5.78V
Baterie=5.85V
Vypinam! 
 */
