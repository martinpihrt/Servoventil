/* 
 * Pro podporu procesoru ATTINY 1634 je potřeba do Arduino IDE doplnit podporu z :http://drazzy.com/package_drazzy.com_index.json (https://github.com/SpenceKonde/ATTinyCore/blob/master/Installation.md)
 * Nastavení pojistek: 
   8 MHZ interní oscilátor
   ATTinyCore\hardware\avr\1.2.2/avrdude.conf -v -pattiny1634 -cstk500v2 -Pusb -e -Uefuse:w:0b11111110:m -Uhfuse:w:0b11010110:m -Ulfuse:w:0xE2:m 

   Verze FW:
   FW 0.0 Martin Pihrt - založení projektu
   FW x.x
 */
