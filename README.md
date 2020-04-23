# Servoventil
Servoventil pro zalévací systém řízený procesorem ATTINY 1634 napájený z 5 AA baterií (celkem 6V). Zařízení je postavené z SMD součástek jako finální produkt za co nejméně peněz. Schéma zapojení obsahuje pouze nejnutněší součástky a procesor, který není zbytečně velký (rozměrově a programově). Deska obsahuje DIP spínač pro nastavení časů.  Programovací konektor ICSP 2x5 pinů, LED signalizující vybité baterie, konektor pro připojení serva MG996R, startovací tlačítko, konektor pro sériovou linku (možnost komunikace s procesorem). Program pro procesor je napsán v prostředí Arduino IDE.  
Cena celého zařízení vychází bez serva na cca 225 Kč.

* Pohled na osazenou desku HW:1.0
<a href="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/final_1.0.jpg"><img src="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/final_1.0.jpg" width="100%"></a><br>
* Schéma zapojení HW:1.0
<a href="https://github.com/martinpihrt/Servoventil/blob/master/PCB/Eagle 1.0/schema.png"><img src="https://github.com/martinpihrt/Servoventil/blob/master//PCB/Eagle 1.0/schema.png" width="100%"></a><br>


# Funkce
## Uvedení do provozu
Po připojení napájení (5ks nabíjecí baterie AA 1,2V - celkem tedy 6-7V) čeká zařízení na stisk tlačítka. V tomto režimu neodebírá žádný proud z baterií (oba MOSFET tranzistory jsou udržovány uzavřené přes rezistory z kladného pólu). Časovač RTC ještě nebyl nastaven a má vloženu záložní baterii. Po stisknutí tlačítka na minimálně 1 sec se uzemní řídící vstup (gate) MOSFET tranzistoru, ten se otevře a přivede napájecí napětí 6V na stabilizátor 3,3V. Za stabilizátorem je připojen procesor ATTINY 1634, který provede toto:
* sepne pomocný tranzistor, který podrží sepnutý (gate) u MOSFETu i po uvolnění tlačítka.
* změří napětí baterie (na AD vstupu procesoru je napětí z baterií 0-6V sníženo rezistorovým děličem na rozsah 0-3,3V). Napětí baterie se pohybuje (naprázdno) plně nabitého článku 1,4 V až napětí vybitého článku 1,0 V. Napětí pod 5V lze považovat za vybitý stav. Na AD vstupu naměříme hodnotu napětí (po zmenšení děličem) 2,6V. Pokud je baterie vybitá bliká LED dioda. RTC běží pouze z baterie a když je čas alarmu tak RTC přes svůj vnitřní tranzistor (otevřený kolektor) uzemní MOSFET napájení, který sepne napájení pro stabilizátor a CPU. 
* přečte nastavené DIP spínače, sepne na určitý čas servo (napájení serva přes MOSFET a generuje signál pro odpovýdající směr a rychlost serva na určitou dobu) -> ventil se otevře. Následně vypne napájení serva a čeká nastavenou dobu (dle DIP spínače 1-7 minut). Po nastavené době opět sepne napájení serva a ventil uzavře.
* nastaví čas (alarm) do RTC pro příští probuzení zařízení (cyklus 12, nebo 24 hodin).
* vypne veškeré napájení (běží pouze RTC z baterie).
* po nastavené době (DIP pozice 4: 12/24 hodin) RTC probudí CPU a opět se provede otevření ventilu na určitou dobu...


## DIP spínač má následující význam
BCD (dvojkový) kód pro nastvení doby otevření ventilu 1-7 minut.
* pozice 1 -> ON=1 minuta
* pozice 2 -> ON=2 minuty
* pozice 3 -> ON=4 minuty
Výsledný čas dle pozice 1-3 se sčítá. Příklad: čas otevření ventilu chceme 3 minuty -> DIP pozice 1 = ON, pozice 2 = ON, pozice 3 = OFF

* pozice 4 -> ON= 12 hodin, OFF= 24 hodin


## Osazení desky spojů
Deska byla vyrobena u https://www.allpcb.com/.
* Pohled na desku TOP
<a href="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/pcb_top.jpg"><img src="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/pcb_top.jpg" width="100%"></a><br>
* Pohled na desku BOT
<a href="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/pcb_bot.jpg"><img src="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/pcb_bot.jpg" width="100%"></a><br>
První čím začneme je osazení SMD součástek z horní strany (TOP) a následně spodní (BOTTOM) strany. Po osazení SMD součástek zapájíme z horní strany THT (drátové) součástky - DIP spínače, držák baterie, napájecí konektor, řadové lišty. Před připojením napájení (baterie, nebo ICSP konektor) je potřeba desku pečlivě zkontrolovat na případné zkraty, nebo otočení součástek. Značení SMD rezistorů je následující: příklad text na rezistoru 1014 znamená 101 + 4 nuly tedy 101 0000 Ohmů, nebo 202 znamená 20 + 2 nuly tedy 20 00 Ohmů (2k).
Před připojením baterií (5ks AA) připojíme (pokud máme) napájecí zdroj s proudovou pojistkou, kterou nastavíme na 100 mA při 6V. Servo ani ICSP programátor není zatím k desce připojen. Obvod nemá v tuto chvíli ze zdroje odebírat žádný proud. Stiskneme a držíme tlačítko SW1 zmačknuté. Multimetrem (voltmetrem) změříme napětí na procesoru, RTC nebo stabilizátoru 3,3V. Napětí musí být cca 3,3V (při 6V na zdroji). Proud ze zdroje bude stále nepatrný (pár mA) - pouze se napájí procesor. Pokud se neobjevil problém (zkrat, velký podezřelý odběr proudu ze zdroje atd...) můžeme odpojit zdroj a zkusit připojit ICSP programátor. Nastavíme Arduino IDE na správný procesor a ICSP programátor. V tuto chvíli by již měl jít do procesoru nahrávat kód. Pokud připojíme na UART (sériovou linku) převodník, můžeme sledovat ladící výpisy co se aktuálně v procesoru odehrává. Protože je RTC obvod DS3231 jako samostatná součástka velmi drahý (okolo 200,-), tak ho odpájíme z China desky (ta se dá pořídit za 30,-).
* Pohled na osazenou desku TOP
<a href="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/top_os.jpg"><img src="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/top_os.jpg" width="100%"></a><br>
* Pohled na osazenou desku BOT
<a href="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/bot_os.jpg"><img src="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/bot_os.jpg" width="100%"></a><br>
* Připojení ICSP programátoru a USB-UART převodníku
<a href="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/top_icsp.jpg"><img src="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/top_icsp.jpg" width="100%"></a><br>
<a href="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/usb_uart.jpg"><img src="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/usb_uart.jpg" width="100%"></a><br>
* Deska RTC s DS3231 (před vypájením a po vypájení)
<a href="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/rtc_s.jpg"><img src="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/rtc_s.jpg" width="100%"></a><br>
<a href="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/rtc_bez.jpg"><img src="https://github.com/martinpihrt/Servoventil/blob/master/FOTO/rtc_bez.jpg" width="100%"></a><br>

## Přidání podpory pro procesor ATTINY 1634 do Arduino IDE
Spustíme nainstalované Arduino v našem počítači a otevřeme záložku "Soubor -> vlastnosti" a do správce dalších desek URL přidáme tento odkaz
http://drazzy.com/package_drazzy.com_index.json ze stránek [GitHub drazzy](https://github.com/SpenceKonde/ATTinyCore/blob/master/Installation.md). Následně potvrdíme tlačítkem "OK". Otevřeme v Arduinu záložku "Nástroje -> Vývojová deska -> Manažér desek" v otevřeném seznamu si vybereme "ATTinyCore by Spencle Konde" a přidáme do Arduina tlačítkem "instalace". V nastavení Arduino IDE je po těchto krocích možné využívat i jiné procesory než které jsou ve výchozí instalaci Arduina.


## Nahrání programu do desky - ICSP programátor
Připojíme do konektoru ICSP desky servoventilu náš programátor (pokud ho nemáme zakoupený, můžeme si ho vyrobit dle [tohoto návodu](https://pihrt.com/elektronika/390-programator-atmel-isp-usbasp)). Při připojení programátoru nepoužíváme napájení z baterií (baterie odpojíme)!


# Verze FW
* 1.0.0 výchozí verze programu 
* 0.0.0 výchozí verze programu - test všech periferií

# Verze HW
* 1.0 15.1.2019 výchozí verze desky 

# Licence
Creative Commons Attribution Share Alike 4.0
cc-by-sa-4.0

Na projektu se podílejí:
HW a FW Martin Pihrt - www.pihrt.com

# Aktuálně, TODO
## 23.4.2020
* Pracuji na verzi FW 1.0.0 (funkce dle popisu DIP a uvedení do provozu)

## 18.2.2019
* HW je otestován a plně funkční: pro test použit FW:0.0.0. Článek NiMh má 1,2 V. Napětí (naprázdno) plně nabitého článku je 1,4 V. Napětí vybitého článku je 1,0 V. Máme 5 článků -> napětí se bude pohybovat od 5V do 7V. Pod 5V je vybito LED blikne 2x krátce. Běží program - LED bliká rychle.

Výpis ze seriové linky (2 minuty probouzení)  
FW:0.0.0,18.2.2019,11:13:33  
RTC init...OK  
DIP1-4 init...OK  
DIP1=0 DIP2=1 DIP3=0 DIP4=1  
Baterie=5.86V  
Servo init...OK  
RTC nastavuji interval...OK  
Baterie=5.78V  
Baterie=5.85V  
Vypinam!  

