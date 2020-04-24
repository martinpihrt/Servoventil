/* 
 * Pro podporu procesoru ATTINY 1634 je potřeba do Arduino IDE doplnit podporu z :http://drazzy.com/package_drazzy.com_index.json (https://github.com/SpenceKonde/ATTinyCore/blob/master/Installation.md)
 * Nastavení pojistek: 
   8 MHZ interní oscilátor
   ATTinyCore\hardware\avr\1.2.2/avrdude.conf -v -pattiny1634 -cstk500v2 -Pusb -e -Uefuse:w:0b11111110:m -Uhfuse:w:0b11010110:m -Ulfuse:w:0xE2:m 
                                                                                            0xFE                   0xD6                   0xE2   
   Verze FW:
 * FW 1.0.0 Martin Pihrt 
   Uvedení do provozu:
   Po připojení napájení čeká zařízení na stisk tlačítka. V tomto režimu neodebírá žádný proud z baterií (oba MOSFET tranzistory jsou udržovány uzavřené přes rezistory z kladného pólu). Časovač RTC ještě nebyl nastaven a má vloženu záložní baterii. Po stisknutí tlačítka na minimálně 1 sec se uzemní řídící vstup (gate) MOSFET tranzistoru, ten se otevře a přivede napájecí napětí 6V na stabilizátor 3,3V. Za stabilizátorem je připojen procesor ATTINY 1634, který provede toto:
   a) sepne pomocný tranzistor, který podrží sepnutý (gate) u MOSFETu i po uvolnění tlačítka.
   *b) TODO změří napětí baterie (na AD vstupu procesoru je napětí z baterií 0-6V sníženo rezistorovým děličem na rozsah 0-3,3V). Napětí baterie se pohybuje (naprázdno) plně nabitého článku 1,4 V až napětí vybitého článku 1,0 V. Napětí pod 5V lze považovat za vybitý stav. Na AD vstupu naměříme hodnotu napětí (po zmenšení děličem) 2,6V. Pokud je baterie vybitá bliká LED dioda. RTC běží pouze z baterie a když je čas alarmu tak RTC přes svůj vnitřní tranzistor (otevřený kolektor) uzemní MOSFET napájení, který sepne napájení pro stabilizátor a CPU. 
   c) přečte nastavené DIP spínače, sepne na určitý čas servo (napájení serva přes MOSFET a generuje signál pro odpovýdající směr a rychlost serva na určitou dobu) -> ventil se otevře. Následně vypne napájení serva a čeká nastavenou dobu (dle DIP spínače 1-7 minut). Po nastavené době opět sepne napájení serva a ventil uzavře.
   e) nastaví čas (alarm) do RTC pro příští probuzení zařízení (cyklus 12, nebo 24 hodin).
   f) vypne veškeré napájení (běží pouze RTC z baterie).
   g) po nastavené době (DIP pozice 4: 12/24 hodin) RTC probudí CPU a opět se provede otevření ventilu na určitou dobu...

   DIP spínač má následující význam
   BCD (dvojkový) kód pro nastvení doby otevření ventilu 1-7 minut.
   pozice 1 -> ON=1 minuta
   pozice 2 -> ON=2 minuty
   pozice 3 -> ON=4 minuty
   výsledný čas dle pozice 1-3 se sčítá. Příklad: čas otevření ventilu chceme 3 minuty -> DIP pozice 1 = ON, pozice 2 = ON, pozice 3 = OFF
   pozice 4 -> ON= 12 hodin, OFF= 24 hodin
 
 * FW 0.0.0 Martin Pihrt  
   založení projektu
   FW x.x
 */
