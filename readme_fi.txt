
              **************************************************
              **                UFO2000 0.3.XX                **
              **             aka X-COM: Gladiators            **
              **        http://ufo2000.sourceforge.net/       **
              **                Finnish Manual                **
              **************************************************

             Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
             Copyright (C)      2002  UFO2000 Development Team

				Suomennos: bunie

					***

UFO2000 pelin kehitystä on jatkettu noin vuoden kestäneen tauon jälkeen.
Nyt UFO2000 on kuitenkin palannut ryhmän nimeltä "UFO2000 Development Team"
tukemana. Alkuperäisellä tekijällä Alexander Ivanovilla ei ole aikaa, eikä
halua tehdä koko työtä yksin. Yksityisen henkiön olisi erittäin hankalaa
säilyttää moinen projekti omissa käsissään. Lisäksi peli on täysin
ilmainen eikä kukaan ansaitse siitä senttiäkään. Lisätietoja varten lue UFO2000
kansiossa sijaitseva COPYING tiedosto.

Versioilla 0.3.20:stä eteenpäin ei ole vain yhtä tekijää. Itseasiassa muutamat
ihmiset ovat lahjoittaneet kirjoittamiansa ohjelmointikoodeja tähän versioon
(alkuperäinen tekijä Alexander Ivanov mukaan luettuna).

Kuka tahansa voi ottaa osaa UFO2000 pelin kehittämiseen. Kaikenlainen apu olisi
arvostettua. Vieraile http://sourceforge.net/projects/ufo2000/ ja lisää kommentteja,
ideoita, virheilmoituksia ja päivityksiä sinne.

Päätavoitteet tälle versio haaralle (0.3.xx) ovat:
- Korjata kaikki tunnetut virheilmoitukset.
- Siivota ja kommentoida lähteet.

ufo2000 kehitysryhmän jäsenet:
- Alexander Ivanov tunnettu myös nimellä 'sanami'
- Serge Semashko tunnettu myös nimellä 'ssvb'
- Alexander Sabourenkov tunnettu myös nimellä 'lxnt'
- ®¬ª  tunnettu myös nimellä 'tavy'

CHANGELOG tiedostosta voit löytää täydelliset muutos listaukset.

Tämän tiedoston on tarkastanut ja parannellut Jeremy Walker, tunnettu myös nimellä
Kuroshi X.

===============
Pelistä
===============

UFO2000 on moninpelattava muunnelma Microprosen pelin X-COM: UFO Defense:n
(UFO: Enemy Unknown) taktisesta osasta.

Se tarvitsee X-COM pelitiedostot kansioista:
   GEODATA GEOGRAPH MAPS SOUND TERRAIN UFOGRAPH UFOINTRO UNITS

Windows versio on laadittu VC6:lla ja tarvitsee DirectX 6:n tai uudemman toimiakseen.

===============
Kartan Muokkain
===============

Käytä hiiren oikeaa painiketta valitaksesi ruudun jonka haluat korvata ja valinta
ikkuna ilmestyy. Sen jälkeen valitse haluamasi 10x10 ruudun tyyli.

F2          Tallenna
F3          Avaa
ESC         Poistu


===============
Liittymis Keskustelu
===============

Käynnistettyäsi palvelimen, mutta ennen kuin siirryt tehtävän suunnitteluun tämä on
näkymä jonka tulet näkemään. Yhden pelaajista on aloitettava palvelimena ja toisen
liityjänä.

F4          Uudelleenpiirrä kuva
F10         Aloita peli
ESC         Kumoa


===============
Tehtävän Suunnittelu
===============

Seuraava vaihe Liittymis Keskustelun jälkeen on tehtävän suunnittelu. Täällä voit
valita sotilaita, määrätä heidän aloitus asemansa ja muokata heidän aloitus
tietoja/aseitansa pitämällä CTRL näppäintä painettuna ja sitten napsauttamalla
valitsemaasi sotilasta (katso alla oleva Yksikön Muokkain osio).

Asettaaksesi yksikkösi paina vasenta hiirenpainiketta hänen nimensä yllä ja osoita
valitsemaasi aloitus asemaa kartalta. Paina uudelleen vahvistaaksesi. Poistaaksesi
sotilaan valinnan napsautta hiiren oikeaa painiketta hänen nimensä yläpuolella.

Tietoa pelaaja yksiköistä:
   1) "total men points=..." (kokonais mies pisteet) Valitun yksikön parametrien
      summa (Aika Yksiköt(TU), Terveys...(Health...)).
   2) Jokaisen tyyppisten aseiden määrä valituille yksiköille.

Luodaksesi uuden kartan napsauta "NEW" ja lataaksesi vanhan paina "LOAD". Jos olet
säätänyt karttasi koon ufo2000.ini tiedostosta, mutta sinulla on silti vanha
karttasi paina "NEW" tai lataa haluamasi kartta.

Lopetettuasi suunnittelun napsauta oman puolesi (palvelin(server)/liityjä(client))
"SEND" sanaa lähettääksesi omat pelaajatietosi vastapelaajalle. Sen jälkeen "SEND"
muuttuu vihreäksi. Aloittaakseen pelin pelaajien on napsautettava "START" sitten 
kun kaikki "SEND" vaihtoehdot ovat vihreinä. Peli alkaa vasta kun kaikki
nämä vaihtoehdot ovat muuttuneet vihreiksi. Sotilaittesi säätäminen tai kartan
vaihtaminen muuttaa kaikki sanat takaisin punaisiksi.

Kaksi pelaajaa samalla tietokoneella -pelityypissä voit painaa F10 "START" toiminnon
käyttämistä varten ("SEND" tekstisi on silti oltava vihreänä).

Palataksesi päävalikkoon paina ESC.


===============
Yksikön Muokkain
===============

Tänne pääset yksikön sijoitus ruudusta. Pidä CTRL painettuna ja napsauta sotilasta
jota haluat muokata.

Täällä voit säätää sotilaan parametrejä ja varustusta. Sotilailla käytössä olevat
parametrit ovat: Aika Yksiköt, Terveys, sekä Ampumis- ja Heittämistarkkuus. Muut
parametrit, kuten Voima ovat poissa käytöstä, koska emme tiedä miten ne toimivat
sisällä pelissä. Jos vielä sen jälkeen kun olet järjestänyt varustuksesi haluat
muokata tietojasi uudelleen, niin napsauta oikealla sijaitsevaa taulukkoa.

Jokainen parametri on rajoitettu välille >= 50 ja <= 80. Näiden parametrien summan
on oltava <= 240 jokaista yksikköä kohden. Yksiköiden aseiden tyypeille ja määrälle
ei ole olemassa rajoituksia, joten Tehtävän Suunnittelu vaiheen aikana on
suositeltavaa, että katsot vihollisesi aseistusta.

F2          Tallenna
F3          Avaa

Vasen CTRL   Näyttää esineen vahinko tilan (muokkaimessa -- maksimi arvo), tämä
Vasen ALT    toimii myös tavaraluettelo ruudussa.


===============
Pelin Hallinta
===============

Kaikki yksiköiden toiminnot, kuten UFO1:ssä ovat hallittavissa hiirellä. Voit
lähettää ja vastaanottaa viestejä toisille pelaajille/toisilta pelaajilta pelin
aikana kirjoittamalla viestisi näppäimistölläsi ja painamalla enter.

Näitä näppäimiä käytetään pelin hallintaan:

F1          Päällä/Pois päältä  Pakettejen näyttäminen.
F4          Uudelleenpiirrä kuva.
F5          Uudelleenkäynnistä
F10         Kokonäyttö/Ikkuna tila.
ESC         Poistu päävalikkoon.
ALT-X       Nopea poistuminen OS:iin.

Kursorit &  Vaihda kartta ruudun kokoa.
Näppäinpaneeli +/-

Oikea CTRL  Vaihda rus ja lat välillä jos päällä INI:ssä.

Vasen CTRL  Näytä valitun miehen reitti kursori ruutuun kuluvien Aika Yksiköiden
	    	kanssa. Kohteenvalinta tilassa näyttää laukaustesi ja heitettävien
		esineiden lentoradan. Tavaraluettelo ruudussä näyttää esineiden
		vahinko arvot. (ALT vaihtoehtoinen)

Vasen SHIFT Pohjassa pidettynä vasemmalla hiirenpainikkeella ampumisen jälkeen
		sallii tarkan tähtäämisen.

ENTER	    lähetä viesti
BKSP        poista viimeinen kirjain viestissä

F11   Huomautus tila päällä/pois päältä, jota käytetään tarkastamaan yhteyden tila.
      Lähettää "NOTE" paketin toiseen tietokoneeseen joka sekunti ja vastaanottaa
	vastauksen. Jos paketti ei tule läpi sekuntien laskin käynnistyy aloittaen
	viimeisestä vastaanotetusta paketista ja näytettynä tieto (Info) ikkunan
	oikeassa ylä kulmassa. Jos pakko tyydyttää rauhaton modeemi komento
	terminaalissa, kuten "atdp123456", on parempi kääntää Huomautus tila jatkossa
	pois päältä.

F12         tallenna näkymä "snapshot.pcx" tiedostoon


===============
INI tiedosto
("ufo2000.ini")
===============

(Tähtimerkkien selitykset ovat listattuna tämän osion jälkeen. Esim. '*1')

[General]
port=2000            #IP Portti (ei Windows versiolle)
server=10.0.0.2      #Palvelimen oletus IP osoite (ei Windows Versiolle)
width=500            #Taistelu Ruudun leveys
height=280           #Taistelu Ruudun pituus
map_width=4          #   "UUSI" kartan koko
map_height=4         #   map_width*map_height <= 36 !
russian=yes          #no poista käytöstä CTRL rus/lat,  yes - päällä
bloodymenu=no        #no - ei verisiä valikoita, yes - kyllä!
sound=yes            #yes - äänitehosteet päällä, no - pois päältä
comport=2            #   modeemin COM portti (ei Windows versioille)
                     #   1 - COM1, 2 - COM2, 3 - COM3, ...
digvoices=16         # ^2  (2, 4, 8, ...)  äänien määrä tehosteissa
modules=no           #   ei käytössä
modvoices=8          #      "
modname=templsun.xm  #      "
modvolume=125        #      "
speed_unit = 30      #Säätää yksikön nopeuden. *1
speed_bullet = 30    #Sama kuin yllä, mutta ammuksille.
speed_mapscroll = 30 #Sama kuin yllä, mutta kartan kelaukselle.
mapscroll = 10       #

[Editor]
platoon_size=8       #sotilaiden määrä "soldier.dat" tiedostossa *2
last_map_name = c:\ufo2000\geoscape.dat  #viimeisen kartan nimi
last_unit_name = team_ru.units           #viimeisen yksikön nimi

[Flags]
F_FULLSCREEN = 0  # Aseta 1 käynnistääksesi pelin kokonäytöllä.
F_FILECHECK = 1   # Aseta 0 poistaaksesi data tiedostojen eheyden tarkistuksen
                  # käynnistyksen yhteydessä
F_SELECTENEMY = 1 # Aseta 0 poistaaksesi siniset merkinnät vihollis
                  # sotilaiden yllä

*1: Voidaan säätää sisällä pelissä asetukset valikosta.
*2: "soldier.dat" tiedoston formaatti on sama kuin UFO1:ssä, joten voit käyttää
mitä tahansa "soldier.dat" tiedostoja pelistä. Sinun on kuitenkin kirjoitettava
vastaava platoon_size ufo2000.ini tiedostoon. (esim. sotilaiden määrä
"soldier.dat" tiedostossa).

--TIEDOSTON LOPPU--