
              **************************************************
              **                UFO2000 0.4.0                 **
              **             aka X-COM: Gladiators            **
              **        http://ufo2000.sourceforge.net/       **
              **       Deutsches Handbuch / German Manual     **
              **************************************************

                                  $Rev: 444 $

             Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
             Copyright (C) 2002-2004  ufo2000 development team

Die Entwicklung von UFO2000 schreitet nun, nach einem Jahr der Inaktivität, 
wieder voran.
Durch die gemeinsamen Anstrengungen von mehreren Entwicklern, kurz dem 
"ufo2000 development team", wird UFO2000 nun weiter programmiert und verbessert.

In der Datei AUTHORS im Verzeichnis UFO2000 finden Sie Informationen 
zu den Personen, die zu diesem Projekt beigetragen haben.

Der ursprüngliche Autor, Alexander Ivanov, ist zur Zeit nicht an der 
Entwicklung beteiligt, aber er hat bereits eine grossartige Vorarbeit 
geleistet, vielen Dank dafür !
Ohne seine Initiative wäre dieses Spiel nicht entstanden.

Dieses Spiel ist freie Software (GPL). Nähere Informationen zur Lizenz
stehen in der Datei COPYING, die sich im Verzeichnis UFO2000 befindet. 

Jeder kann sich an der Entwicklung von UFO2000 beteiligen, 
jede Hilfe ist willkommen.
Besuchen Sie dazu die Webseite http://ufo2000.sourceforge.net.
Dort können Sie die ufo2000-Mailingliste abonnieren 
und Kommentare, Ideen, Bugreports und Patches einsenden.

Die Liste der Änderungen von Version zu Version
finden Sie in der Datei "ChangeLog".

=========
Allgemein
=========

UFO2000 ist ein Remake des taktischen Teils des Spiels "X-COM: UFO Defense"
(bzw. "UFO: Enemy Unknown") von Microprose. 
Gespielt wird per Netzwerk (Internet bzw. lokales Netzwerk) gegen 
einen anderen Spieler.
Es ist auch ein "Hotseat"-Spiel mit nur einem Rechner möglich.

Es werden Dateien von einem X-COM Originalspiel benötigt (Grafiken etc.).

Das Spiel funktioniert eingeschränkt auch mit einer Demoversion von X-COM.
Dann stehen allerdings nur die "City"-Karte und nur bestimmte Einheiten zur 
Verfügung.  Die Verwendung einer Vollverson von X-COM wird daher empfohlen.

Zusätzlich können auch Daten von "X-COM: Terror from the deep"
verwendet werden, bzw. von der entsprechenden Demoversion.

Die Demoversionen stehen zum Download zur Verfügung auf:
ftp://ftp.microprose.com/pub/mps-online/x-com/xcomdemo.zip
ftp://ftp.microprose.com/pub/mps-online/demos/terror.zip

Das Spiel läuft unter den Betriebssystemen Windows, Linux und FreeBSD.
Vermutlich funktioniert es auch unter anderen UNIX-systemen.
Falls es Ihnen gelingt, UFO2000 auf einem anderen System zu compilieren 
und zum laufen zu bringen, bitten wir um Rückmeldung.

Detailierte Informationen zum installieren von UFO2000 befinden sich in der
Datei INSTALL im UFO2000 Verzeichnis.


=========
Programme
=========

ufo2000-srv.exe - Server: für ein Spiel im lokalen Netzwerk oder per Internet.
                  Der Server läuft nach dem Start im Hintergrund und
                  erledigt die Kommunikation zwischen den Clients.

ufo2000.exe     - Client: mit diesem Programm wird gespielt.

=========
Main Menu - Hauptmenü
=========

Nach dem starten von UFO2000.exe wird ein Menü mit den folgenden Optionen 
angezeigt:


   Connect to internet server 

      Verbindung zu einem UFO2000-Server im LAN oder Internet aufnehmen.
      Es wird ein Dialog angezeigt zur Eingabe von Server-Adresse, Login 
      und Passwort.

      Als Server-Adresse ist voreingestellt "lxnt.info", dies ist z.Zt. 
      der offizielle ufo2000-server im Internet. 
      Bei einem Spiel im LAN muss auf einem der erreichbaren Rechner 
      das Serverprogramm ufo2000-srv.exe laufen. 
      Als Server-Adresse wird die IP-Adresse dieses Rechners 
      eingetragen (z.B. "192.168.1.2" ).

      Mit Login-Name und Passwort wird der Spieler identifiziert.
      Alle Spieler sollten unterschiedliche Login-Namen verwenden,
      Passworte sollten mindestens 6 Zeichen lang sein.
      Bei Spielen auf lxnt.info wird auch eine Statistik der Spiele geführt,
      siehe http://ufo2000.lxnt.info/results.php.

      Nach erfolgreichem Login gelangt man in den Chat-Modus und kann dort mit 
      anderen Spielern Kontakt aufnehmen und sie zu einem Spiel herausfordern.


   Start hotseat game 

      Startet UFO2000 auf einem einzelnen Computer für ein "Hotseat"-Spiel,
      dazu wird kein Netzwerk benötigt.
      "Hotseat" = "heisser Stuhl" bedeutet, dass beide Spieler den selben PC 
      benutzen und abwechselnd Ihre Züge eingeben, während der andere jeweils 
      wegschaut.
      Dieser Menüpunkt führt direkt zum Missions-Planer.


   Load saved game 

      Laden eines gespeicherten Spiels, nur für lokale Hotseat-Spiele.


   Quit

      Programm beenden


====================
Internet Server Chat
====================

Sobald die Verbindung zum ufo2000-Server besteht, wird die Chat-
Konsole angezeigt. 
Die rechte Spalte zeigt eine Liste der anwesenden Spieler.

Die Farbe des Spielernamens zeigt den Status des Spielers an:

Weiss - der eigene Spielername
Grau  - bereit zum Chat
Gelb  - an diesen Spieler wurde eine Herausforderung gesendet
Grün  - Spieler hat Herausforderung akzeptiert, 
        bzw. selbst eine Herausforderung gesendet
Rot   - Spieler ist beschäftigt (spielt gerade mit jemand anderem)

Um einem anderen Spieler eine Herausforderung zu senden, 
einfach dessen Namen anklicken, der Name wird dann gelb.
Durch anklicken eines grünen Namens kann man eine Herausforderung annehmen,
und damit ein Spiel starten.  Dies führt zunächst zum Missions-Planer.


===============
Mission Planner - Missions-Planer
===============

Oben in der Mitte wird die aktuell ausgewählte Karte angezeigt,
links und rechts davon die Soldaten der beiden Spieler, 
unten in der Mitte die Optionen für das Spiel.

Einer der beiden Spieler kann hier die allgemeinen Daten zum Spiel festlegen, 
d.h. insbesondere die Karte auswählen (mittlere Box unterhalb der Karte).

Weiterhin kann jeder Spieler hier die Zusammensetzung seines Teams ändern
(Namensliste der Soldaten links bzw. rechts der Karte).

Spieldaten:
  Durch anklicken von "NEW" wird eine neue, zufällige Karte erzeugt.
  Mit "LOAD" kann eine vorbereitete Karte geladen werden.
  Zum erstellen von solchen Karten wird ein extra Karteneditor benötigt.

Teamdaten:
  Hier kann man seine Soldaten auswählen, Ihre Werte ändern, Waffen und 
  Ausrüstung wählen, und ihre Startpositionen auf der Karte festlegen.
  In den Einheiten-Editor gelangt man durch anklicken eines Soldaten-Namen,
  während man die linke CTRL-Taste festhält ( = "STRG-links" ).
  Nähere Einzelheiten siehe nächster Abschnitt "Einheiten-Editor".

Soldaten auf der Karte plazieren (nur im eigenen Bereich der Karte) :
  mit linker Maustaste Namen anklicken, dann Position auf Karte anklicken.
  Zur Bestätigung nochmals klicken.
  Ein rechter Mausklick auf einen Namen deselektiert den zugehörigen Soldaten.

Infos zu den Einheiten:
   1) "total points=..." Die Summe aller Parameters (TU, Gesundheit,.. )
      aller aufgestellten Einheiten.
   2) Anzahl der Waffen und Ausrüstungsgegenstände

Mit dem "SEND"-Button in der Box "Server" werden die fertigen Daten zum Server 
und von dort zum anderen Spieler übertragen, der Button wird daraufhin grün.

Sobald der andere Spieler seine Aufstellung per SEND-Button übertragen 
hat, wird auch dessen SEND-Button grün.
Wenn beide Send-Buttons grün sind, können die Spieler das Spiel 
starten durch anklicken ihrer jeweiligen "START"-Buttons.

Bei Änderungen nach einem SEND werden alle Statusanzeigen wieder rot.

Bei einem Hotseat-Spiel muss der "START"-Button gleich nach dem "SEND"-Button
betätigt werden, anschliessend ist der zweite Spieler an der Reihe mit seinen
Vorbereitungen.

Mit ESC gelangt man zurück ins Hauptmenü.


===========
Unit Editor - Einheiten-Editor
===========

Vom Missions-Planer aus erreicht man den Einheiten-Editor 
durch CTRL-Klick auf einen Soldatennamen.

Hier kann man Daten und Ausrüstung seiner Soldaten ändern.

Angezeigt werden
links : die "Beladung" des Soldaten für Rucksack, Taschen, Hände etc.,
        darunter "Ground" die Ablagefläche an seiner aktuellen Position.
rechts: die Daten und Werte des Soldaten (Chart mit Balkendiagrammen)
unten : die verfügbaren Waffen, Munition etc.

Man kann das Team seiner Soldaten komplett abspeichern und wieder laden:

F2          Save - Speichern des Teams
F3          Load - Laden

Links-CTRL und Links-ALT:
            Zustands-Anzeige für alle Gegenstände (Schaden bzw. Munitionsvorrat)
            durch einen eingeblendeten Balken (grün=ok) bzw. Zahlenwert.
            Dies funktioniert auch in der Inventar-Anzeige während des Spiels.

ESC         Einheiten-Editor verlassen



Werte der Soldaten:

Time Units - Zeiteinheiten für Bewegung, Schiessen und Aktionen
Stamina    - Ausdauer
Health     - Gesundheit = Hitpoints
Reactions  - Chance für Reaktions-Schüsse in der gegnerischen Phase
Firing     - Zielgenauigkeit mit Schusswaffen
Throwing   - Zielgenauigkeit beim Werfen
Strength   - Stärke = maximale Beladung

Die anderen Parameter (Bravery, PSI etc.) werden z.Zt. nicht verwendet.

Ein Klick auf den Chart mit den Werten öffnet den Werte-Dialog.
Dort können der Name geändert werden, sowie Panzerungstyp ("Skin") 
und die Werte per Mausklick eingestellt werden.

Alle Werte müssen im Bereich zwischen 50 un 80 liegen.
Punkte für Stärke zählen doppelt, hier ist der Bereich 25 bis 40.
Es gibt ein Limit für die Gesamtsumme der Werte, man kann daher 
nicht alle Werte auf Maximum setzen.

Für die Bewaffnung gibt es kein festes Limit, die Spieler können aber
entsprechende Vereinbarungen treffen.


==========
Kontrollen während des Spiels
==========

Die Bedienung erfolgt, wie in X-COM, hauptsächlich mit der Maus:

linker  Mausklick: Aktions-Buttons oder Waffe anklicken, 
                   zur Cursor-Position laufen / schiessen
rechter Mausklick: Soldat in Richtung zum Cursor drehen, Tür öffnen


Tastenbefehle:

Man kann Textnachrichten von/an den anderen Spieler senden/empfangen.
Dazu einfach den Text per Tastatur eingeben. Abschicken mit der ENTER-Taste.

ENTER       Chat-Text senden
BACKSPACE   letztes Zeichen des Chat-Textes löschen

F10         Umschaltung Vollbild / Fenster-Modus

F12         Bildschirmfoto in Datei "snapshot_XX.pcx" abspeichern

ESC         Zurück zum Hauptmenü


Cursor  
            Grösse der Kartenanzeige auf dem Bildschirm ändern

Keypad +/- 
            Musik-Lautstärke regeln

F9          Umschaltung zwischen Tastenbelegung primary_keyboard und 
            secondary_keyboard, wie eingestellt in Datei ufo2000.ini 

Rechts-CTRL + 'b', 'd', 'e', 'p' or 'r' 
            Umschaltung zwischen Tastenbelegung für Belarusian, Deutsch, 
            Englisch, Polnisch und Russisch.
            (andere Sprachen können in der Datei ufo2000.ini gewählt werden)

Links-CTRL  Einblendung zusätzlicher Daten:
  Bewegung: Anzeige des Weges des aktiven Soldaten zur Cursor-Position,
            mit TU-Kosten.
  Waffe   : Anzeige der Schusslinie bzw. Wurfbahn.
  Inventar: Zustands-Anzeige für alle Gegenstände (Schaden bzw. Munitionsvorrat)

Links-SHIFT Erlaubt genaueres Zielen durch Anvisieren eines bestimmten Teils
            des Zielfeldes.
            Dazu muss diese Taste festhalten werden, während man
            mit der linken Maustaste den Schuss auslöst.
            Es erscheint dann ein Menü zur Auswahl des genauen Zielpunkts.


===============
INI file
("ufo2000.ini")
===============

(Markierung mit Stern, z.B. "*1": siehe Anmerkungen unten)

[General]
width=500            #Battle Screen width
height=280           #Battle Screen height
map_width=4          #   "NEW" map size
map_height=4         #   map_width*map_height <= 36 !
sound=yes            #yes - sound fx on, no - off

speed_unit = 30      #Sets unit speed. *1
speed_bullet = 30    #Same as above, but for game projectiles.
speed_mapscroll = 30 #Same as above, but for map scrolling.
mapscroll = 10       #

[Editor]
platoon_size=8       #number of soldiers in "soldier.dat" file *2
last_map_name = c:\ufo2000\geoscape.dat  #last map name
last_unit_name = team_ru.units           #last unit name

[Flags]
F_FULLSCREEN = 0    # Set to 1 to start the game in fullscreen mode
F_SELECTENEMY = 1   # Set to 0 to disable blue markers above enemy soldiers


*1: Kann während des Spiels im Options-Menü geändert werden.

*2: Das Format der Datei " "soldier.dat" ist das gleiche wie im Spiel 
    X-COM: UFO Defense, es können also solche Dateien z.B. von gespeicherten 
    Spielständen verwendet werden.
 Allerdings muss hier in ufo2000.ini der Parameter "platoon_size" 
 entsprechend der Anzahl von Soldaten eingetragen werden.

--ENDE--
