              **************************************************
              **                UFO2000 0.3.XX                **
              **             aka X-COM: Gladiators            **
              **        http://ufo2000.sourceforge.net/       **
              **                 Polish Manual                **
              **************************************************

             Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
             Copyright (C)      2002  UFO2000 Development Team

                     T≥umaczenie: dolny & Sir-Torpeda 

                                       ***

Prace nad UFO2000 sπ kontynuowane po ponad roku zastoju. UFO2000 powrÛci≥o, wspierane przez grupÍ "UFO2000 Development Team".
Oryginalny autor, Alexander Ivanov, nie ma ani czasu, ani chÍci by zajmowaÊ siÍ ca≥ym projektem samemu. Naleøy dodaÊ, øe gra jest
kompletnie darmowa (na licencji GPL) i nikt nie zarobi≥ na niej ani centa. Dla informacji przeczytaj plik COPYING, znajdujπcy siÍ
w folderze z UFO2000. 

Wersje od 0.3.20 nie majπ jednego autora. Kilka osÛb dorzuci≥o swoje kawa≥ki kodu do tej wersji 
(pierwotny autor - Alexander Ivanov jest wúrÛd nich). Kaødy moøe wziπúÊ udzia≥ w tworzeniu UFO2000.
Doceniamy kaødπ pomoc. Odwiedü http://sourceforge.net/projects/ufo2000/ i oficjalne forum: http://www.ufo2000.prv.pl
i zamieúÊ tam swoje komentarze, pomys≥y, raporty o b≥Ídach, w≥asne patche. 

Podstawowe zadania dla najbliøszych wersji (0.3.xx) to: 
- wyeliminowanie wszystkich znanych b≥ÍdÛw. 
- uporzπdkowanie i skomentowanie ürÛde≥ 

Cz≥onkowie UFO2000 Development Team: 
- Alexander Ivanov aka 'sanami' 
- Serge Semashko aka 'ssvb' 
- Alexander Sabourenkov aka 'lxnt' 
- ê&&™  aka 'tavy' 

Zobacz plik CHANGELOG by przeúledziÊ historiÍ zmian. 

Ten plik zosta≥ dodany przez Jeremiego Walkera, aka Kuroshi X. 

=============== 
WstÍp 
=============== 

UFO2000 jest multiplayerowym rimejkiem taktycznej czÍúci gry X-COM: UFO Defense (UFO: Enemy Unknown) wyprodukowanej przez firmÍ Microprose. 

UFO2000 wymaga X-COM'owych plikÛw i folderÛw: 
GEODATA GEOGRAPH MAPS SOUND TERRAIN UFOGRAPH UFOINTRO UNITS 

Wersja Windowsowa skompilowana zosta≥a za pomocπ VC6 i wymaga DirectX 6 lub nowszego. Sπ takøe wersje DJGPP oraz Linux. 


=============== 
Edytor Map 
=============== 

Aby zamieniÊ dany obszar mapy uøyj prawego przycisku myszy, po jego naciúniÍciu pojawi siÍ menu z kawa≥kami terenu do wyboru. 

F2 Zapis mapy 
F3 Za≥adowanie mapy 
ESC Koniec 


=============== 
Connect Chat 
=============== 

Po odpaleniu serwera, przed przejúciem do ekranu planowania misji pojawi siÍ ten ekran. Jeden gracz musi za≥oøyÊ grÍ,
drugi (klient) musi do≥πczyÊ by gra mog≥a siÍ rozpoczπÊ. 

F4 Odúwieøenie ekranu 
F10 Start gry 
ESC Anulowanie 


=============== 
Planowanie Misji 
=============== 

NastÍpnym ekranem, zaraz po Connect Chat, jest ekran Planowania Misji. 
Tutaj moøesz wybieraÊ øo≥nierzy, ustawiaÊ ich pozycje startowe i zmieniaÊ ich statystyki/stan uzbrojenia,
poprzez przytrzymanie lewego CTRL i klikniÍcie na imiÍ wybranego øo≥nierza (wiÍcej szczegÛ≥Ûw w sekcji zatytu≥owanej "Edytor Jednostek"). 

By rozstawiÊ swoje jednostki, naciúnij lewy przycisk myszy na imieniu øo≥nierza, a potem nakieruj kursor na mapkÍ.
Ponownie naciúnij lewy przycisk myszy, zatwierdzajπc swÛj wybÛr. Jeúli chcesz zrezygnowaÊ z umieszczenia øo≥nierza na mapie,
naciúnij prawy przycisk myszy na jego imieniu (pÛüniej jednakøe bÍdzie moøna znÛw go wybraÊ i ustawiÊ). 

Informacje na temat jednostek gracza: 
1) "total men points=..." - suma statystyk (TU, Health,..) dla jednostek umieszczonych na mapie. 
2) Liczba i rodzaj wybranego uzbrojenia dla jednostek umieszczonych na mapie. 

Aby wygenerowaÊ nowπ mapÍ, naciúnij "NEW", jeúli chcesz wgraÊ zapisanπ wczeúniej mapkÍ, wybierz "LOAD".
Jeúli zosta≥a zmieniona wielkoúÊ mapy w ufo2000.ini, jednak ta siÍ nie zmieni≥a, wybierz "NEW" do wygenerowania w≥aúciwej,
bπdü wgraj nowπ (wedle w≥asnego wyboru). Gdy juø zakoÒczysz planowaÊ misjÍ, nacisnij "SEND" w okienku serwera,
bπdü klienta (zaleønie od tego, czy hostujesz gre, czy teø do≥πczasz siÍ do gry tworzonej przez kogoú).
S≥owo to zmieni wtedy kolor na zielony. Aby rozpoczπÊ rozgrywkÍ, gracze muszπ nacisnπÊ "START", gdy juø u kaødego "SEND" bÍdzie mia≥o
zielony kolor. Jakiekolwiek zmiany we w≥asnym oddziale, bπdü zmiana mapy, zmieniπ kolor s≥ow na czerwony (w takim przypadku trzeba po
prostu powtÛrzyÊ procedurÍ opisanπ w tym akapicie). Dla gry typu "hotseat" moøna nacisnπÊ F10, aby rozpoczπÊ rozgrywkÍ
(jednak "SEND" musi mieÊ zielony kolor). 

Aby powrÛciÊ do g≥Ûwnego menu, naciúnij ESC. 

=============== 
Edytor Jednostek 
=============== 

DostÍp do edytora jednostek moøna uzyskaÊ z ekranu Planowania Misji. Przytrzymaj CTRL i nacisnij na imiÍ øo≥nierza,
ktÛrego chcesz poddaÊ edycji. Moøesz tutaj zmieniÊ wyposaøenie øo≥nierza i jego niektÛre statystyki. ZmodyfikowaÊ moøemy wartoúÊ
parametrÛw: Time Units, Health, Firing i Throwing Accuracy. Inne, takie jak Strength, sπ wy≥πczone, poniewaø nie wiemy jaki wp≥yw majπ
na rozgrywkÍ. Edycja statystyk nastÍpuje po klikniÍciu lewym przyciskiem myszy na na dane znajdujπce siÍ po prawej stronie ekranu. 

Kaødy z parametrÛw musi mieÊ wartoúÊ wiÍkszπ, lub rÛwnπ 50 i mniejszπ, bπdü rÛwnπ 80. Suma czterech parametrÛw musi zamykaÊ siÍ w 240.
Nie ma jednak ograniczeÒ dla iloúci i rodzaju kaødej z broni dla Twoich jednostek, wiÍc podczas fazy Planowania Misji sugerowane jest
braÊ poprawkÍ na uzbrojenie przeciwnika. 

F2 Zapis oddzia≥u do pliku 
F3 Wgranie juø poprzednio zapisanego oddzia≥u 

Lewy CTRL; Lewy ALT - pokazuje maksymalnπ iloúÊ obraøeÒ, jakπ moøe zadaÊ broÒ - te klawisze dzia≥ajπ takøe podczas gry. 


=============== 
Klawiszologia 
=============== 

Wszystkie akcje, tak jak w UFO1, sπ wykonywane za pomocπ myszy. 
Moøesz wysy≥aÊ i otrzymywaÊ wiadomoúci do/od przeciwnika. Wystarczy wpisaÊ wiadomoúÊ i nacisnπÊ enter. 

Te klawisze sπ uøywane podczas gry: 

F1 On/Off Raw packets show 
F4 Odúwieøenie ekranu 
F5 Restart 
F10 ryb pe≥noekranowy/okienkowy 
ESC Wyjúcie do menu g≥Ûwnego 
ALT-X Szybkie wyjúcie z gry 

Kursory i +/- na klawiaturze numerycznej - zmiana wielkoúci ekranu gry 

Prawy CTRL - zmiana jÍzyka (jeúli uaktywnione w INI). 

Lewy CTRL - pokazuje drogÍ wybranej postaci do wybranej kratki z uwzglÍdnieniem kosztu w TU (Time Units - Jednostki Czasu). 

W trybie celowania pokazuje trajektoriÍ strza≥u lub rzutu. Na ekranie ekwipunku, pokazuje obraøenia zadawane przez przedmiot. 

Lewy SHIFT - pozwala na precyzyjne celowanie jeúli przytrzymany przed strza≥em. 

ENTER - wys≥anie wiadomoúci 

BACKSPACE - usuniÍcie ostatniego znaku w wiadomoúci 

F11 - Tryb powiadamiania w≥πczony/wy≥πczony. Uøywany do sprawdzania po≥πczenia. (Sends "NOTE" packet each second to remote computer and
receive the responce. If such packet do not come through, the counter of seconds activated starting from last packet received and shown
in top right corner of Info window. If it is necessary to fulfill ceaseless modem command in terminal, e.g. "atdp123456", better turn the
notifying mode off in advance.) 

F12 - Zrzut ekranu do pliku "snapshot.pcx" 

=============== 
Plik INI 
("ufo2000.ini") 
=============== 

[General] 
port=2000 # Port IP 
server=10.0.0.2 # Domyúlne IP serwera 
width=500 # SzerokoúÊ ekranu gry 
height=280 # WysokoúÊ ekranu gry 
map_width=4 # SzerokoúÊ mapy 
map_height=4 # WysokoúÊ mapy - map_width*map_height <= 36 ! 
russian=no # no wy≥πcza zmianÍ CTRL rus/lat, yes - w≥πcza 
bloodymenu=no # no - bloody menu wy≥., yes - w≥πczone! 
sound=yes # yes - düwiÍk w≥., no - wy≥πczony 
comport=2 # port COM modemu (nie dla wersji Windows) 
# 1 - COM1, 2 - COM2, 3 - COM3, ... 
digvoices=16 # ^2 (2, 4, 8, ...) iloúÊ g≥osÛw (number of voices for fx) 
modules=no # nie uøywane 
modvoices=8 # " 
modname=templsun.xm # " 
modvolume=125 # " 
speed_unit = 30 # Ustala szybkoúÊ jednostki. *1 
speed_bullet = 30 # SzybkoúÊ pociskÛw. 
speed_mapscroll = 30 # SzybkoúÊ scrollingu/przesuwu mapy. 
mapscroll = 10 # 

[Editor] 
platoon_size=8 # iloúÊ øo≥nierzy z "soldier.dat" *2 
last_map_name = c:\ufo2000\geoscape.dat # nazwa ostatniej mapy 
last_unit_name = team_ru.units # ostatnie nazwy jednostek 

[Flags] 
F_FULLSCREEN = 0 # Ustaw na 1 by uruchamiaÊ grÍ w trybie pe≥noekranowym 
F_FILECHECK = 1 # Ustaw na 0 by wy≥πczyÊ sprawdzanie integralnoúci danych na starcie 
F_SELECTENEMY = 1 # Ustaw na 0 by wy≥πczyÊ niebieskie markery nad g≥owami wroga. 

*1: Moøe byÊ zmienione w opcjach podczas gry. 
*2: Format pliku "soldier.dat" jest taki sam jak ten z UFO1, wieÊ moøesz uøyÊ "soldier.dat" z UFO. PamiÍtaj jednak wpisaÊ poprawny
platoon_size w ufo2000.ini (rÛwny liczbie øo≥nierzy w "soldier.dat"). 

--KONIEC PLIKU--
