
              **************************************************
              **                UFO2000 0.4.0                 **
              **             aka X-COM: Gladiators            **
              **        http://ufo2000.sourceforge.net/       **
              **              Manual en Castellano            **
              **************************************************

             Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
             Copyright (C) 2002-2004  ufo2000 development team

El desarrollo de UFO2000 continua tras cerca de un año de inactividad. 
Ahora UFO2000 ha regresado y con la ayuda de nuevos desarrolladores, 
"ufo2000 development team" para acortar. Para la información 
sobre quién ha contribuído a UFO2000, lea el fichero AUTHORS en el directorio
UFO2000. El autor original, Alexander Ivanov, actualmente no está 
involucrado en el proceso de desarrollo, pero ha creado un excelente trabajo,
y le damos las gracias! Sin su excelente trabajo, este juego jamás podría 
haber existido.

Este juego es software libre (GPL). Para más información sobre la licencia 
de uso, lea el fichero COPYING, que se encontrará en el directorio de UFO2000. 

Cualquier persona puede formar parte en el desarrollo de UFO2000 development.
Toda ayuda será bien recibida. Por favor, visite la página: http://ufo2000.sourceforge.net/.
Ahí podrá subscribirse a la lista de correo de ufo2000 y poder comunicar 
sus ideas, fallos y parches.

Vea el fichero ChangeLog para ver la lista completa de cambio.


===============
Acerca de
===============

UFO2000 es un remake multijugador de la parte táctica del juego de Microprose,
 X-COM: UFO Defense (UFO: Enemy Unknown).

Necesitarás tener instalados el X-COM original para jugar a UFO2000. Podrá funcionar
con la versión demo de X-COM, pero tan solo podrás jugar en los escenarios tipo
ciudad, y utilizar una cantidad limitada de unidades. Se recomienda en cualquier caso
tener la versión completa de X-COM.

El juego compila y funciona en Windows, Linux el sistema operativo FreeBSD.
Probablemente el juego funcionará en otros sistemas como UNIX. Si es capaz 
de compilar y ejecutar UFO2000 en uno de ellos, por favor, haganoslo saber.

Más detalles referentes a la compilación de UFO2000 puede encontrarse en el fichero
INSTALL en el directorio de UFO2000.

==============
MENU DE INICIO
==============

Tras iniciar el juego, aparecerán varias opciones:

  Conectarse a un servidor de internet

	El juego puede conectarse a un servidor de internet o a una red de área local
	(para iniciar tu propio servidor, ejecuta ufo2000-srv incluído en la instalación)
	Tras elegir esta opción, aparecerá conectarse al servidor ("connect to server").
	Hay que introducir la dirección; lnxt.info se utiliza por defecto, es actualmente
	el servidor oficial de ufo2000 en internet. Si quieres jugar en una red local, tendrías
	que iniciar el servidor en un ordenador y su dirección ip con lo conectes desde otro
	ordenador.
	Los otros dos campos son nombre de usuario (login) y contraseña (password). Se utilizan
	para identificar al usuario para las estadísticas (http://ufo2000.lxnt.info/results.php,
	en la actualidad.) El nombre de usuario debe tener una longitud mínima de 6 caracteres.
	Tras conectarse correctamente, aparecerá la ventana de chat.

  Iniciar juego Hotseat

	Inicia una parta en un solo ordenador. (no requiere una red) El jugador debe alternar el juego
	cada turno. Tras hacer click en esta opción, aparecerá directamente el planificador de misiones.

  Cargar partida grabada
	Carga una partida grabada. Tan sólo funciona en partidas hotseat.

  Quit

	Sin comentarios ;)

===============
Chat
===============

Tras conectarse al servidor de ufo2000, verás una consola de chat y una lista de jugadores
conectados al servidor. El estado de cada jugador está determinado por el color de su nombre:

Blanco - ese es tu propio nombre
Gris - disponible para hablar
Amarillo - has retado a éste jugador
Verde - puedes aceptar un reto de éste jugador
Rojo - está ocupado jugando con alguie.

Para retar a un jugador, tan solo haz click en su nombre (se volverá amarillo.) Si alguien
tiene el nombre en verde, significa que te ha retado. Tras hacer click en su nombnre, el juego
se iniciará y la pantalla de chat será reemplazada por el planificador de misiones.


===============
Planificador de Misiones
===============

Aquí podrá elegir sus soldados, determinar las ubicaciones de inicio, y editar 
sus estadísticas y armamento pulsando la tecla de CTRL izquierda 
y haciendo click a la vez en el soldado elegido. (vea más abajo la sección Editor de Unidades)

Para ubicar las unidades, presione el botón izquierdo del ratón en el nombre de la unidad
y confirme el punto de inicio en el mapa haciendo nuevamente click. Para des-seleccionar
el soldado, haga click con el botón derecho del ratón en el nombre.


Información sobre las unidades del jugador:
   1) "total men points=..." La suma totales de parámetros (TU, Vida,.. )
      de las unidades elegidas.
   2) Numero de armas de cada tipo por las unidades seleccionadas.


Opciones de Partida (Match settings):

 1) Nombre del escenario elegido:
	Click para elegir el escenario y usar las opciones del escenario
 2) Nombre del mapa elegido:
	Click para elegir el terreno y el tamaño del mapa o para generar/cargar
	un nuevo mapa.
 3) Reglas ("Game Rules"):
	Click para especificar las reglas, como límite de turnos, tiempo, etc.

Para generar un mapa nuevo, haga click en "NEW", para cargar pulse en "LOAD". Si tiene 
adjustado el tamaño del mapa en el fichero de ufo2000.ini file, pero aún tiene su antiguo mapa 
presiona "NEW" o cargue el mapa que deseé.

Tras terminar, haga click en su parte (cliente/servidor) "SEND" 
para emitir sus datos locales al otro jugador. "SEND" entonces aparecerá en verde. 
Para iniciar la partida, los jugadores deben hacer click en "START" tras tener todas
las opciones de "SEND" en verde. El juego solo empezará cuando estas opciones estén en
verde. Adjustar sus jugadores, o cambiar de mapa volverá a colorear todas las palabras en rojo

Para volver al menú principal, pulse ESC.


===============
Editor de Unidades
===============

Puede acceder a esta opcion desde la pantalla de Despliegue. Mantenga pulsado CTRL 
y haga click en el soldado que desee editar.

Ahora podrá editar los parámetros del soldado. Los parámetros que puede cambiar son:
Unidades de Tiempo, Salud, Disparo, y Lanzamiento, Precisión. 
Otros parámetros como Fuerza están bloquedados porque no sabemos cómo funcionan 
en el juego. Si, tras ordenar su equipamiento, desea editar las estadísticas de nuevo, 
haga click en la parte derecha.

Cada parámetro está restringido a ser >= 50 y <=80. La suma 
de estos valores tiene un cierto límite en cada unidad, así que no puedes maximizar
todos los parámetros. Puedes grabar y cargar varias configuraciones de unidades
con las siguientes teclas:

F2          Grabar
F3          Cargar

===============
Controles de Juego
===============

Todas las acciones, como en X-COM, se realizan mediante el ratón. Es posible
enviar y recibir mensajes del otro jugador durante el juego,
tan solo escriba su mensaje con el teclado, y pulse intro.

Estas son las teclas utilizadas en el juego:

F10         Pantalla Completa/Modo Ventana

ESC         Salir al Menu principal

Cursores &   cambiar el tamaño del mapa de la pantalla
Teclado Numérico +/-  

F1          Activar/Desactivar  Muestra paquetes Raw
F4          Redibuja la pantalla
F5          Reiniciar


ALT-X       Escape rápido al SO


CTRL Derecho Cambio entre dos mapas de teclado elegidos como teclado primario (primay_keyboard)
F9	     y secundario en el archivo ufo2000.ini

CTRL Derecho + 'b', 'd', 'e', 'p' or 'r' Cambia el mapa de teclas: Bielorruso, Alemán,
        Inglés, Polaco y Ruso (otros lenguajes pueden ser elegidos
        en el fichero ufo2000.ini)

CTRL Izquierdo   Muestra la ruta de la unidad elegida hacia donde se encuentra el curso mostrando
                 el coste de TU.En modo punto de mira, mostrará la trayectoria de sus disparos
  y objetos arrojables. En la pantalla de inventario, mostrará el valor de daño.
  (ALT GR)

SHIFT Izquierdo  Permite puntería precisa si se mantiene hasta pulsar el botón de disparo
  con el botón derecho del ratón.

SHIFT Izquierdo  + teclas cursor Mueve la pantalla del mapa

INTRO       Enviar mensaje
BORRAR      elimina el ultimo caracter en el mensaje

F12         hace una captura de pantalla como "snapshot.pcx"


===============
Fichero INI
("ufo2000.ini")
===============

(Asteriscos indican notas mostradas al final de la sección. '*1')

[General]
port=2000            #IP Port (no para la versión Windows)
server=10.0.0.2      #Dirección IP por defecto del Servidor (no para la versión de Windows)
width=500            #Ancho de la pantalla de Batalla
height=280           #Altura de la pantalla de Batalla
map_width=4          #   "NEW" tamaño del mapa al pulsar en NEW
map_height=4         #   anchura y altura no deben exceder de 36 !
sound=yes            #si - soundos fx activados, no - desactivados
speed_unit = 30      #Velocidad de la unidad *1
speed_bullet = 30    #Como lo de arriba, pero para proyectiles.
speed_mapscroll = 30 #Como lo de arriba, pero para el scroll del mapa.
mapscroll = 10       #

[Editor]
platoon_size=8       #numero de soldados en el fichero "soldier.dat" *2
last_map_name = c:\ufo2000\geoscape.dat  #Nombre del último mapa
last_unit_name = team_ru.units           #nombre de la última unidad

[Flags]
F_FULLSCREEN = 0  # Poner a 1 para iniciar el juego a pantalla completa
F_FILECHECK = 1   # Poner a 0 para desactivar el chequeo de la integridad de ficheros al iniciar
F_SELECTENEMY = 1 # Poner a 0 para desactivar marcas azules en soldados enemnigos

*1: Puede ser ajustado en juego, en el menú de opciones.
*2: El formato del fichero "soldier.dat" es el mismo que el de
UFO1, así que puede usar cualquier "soldier.dat" de cualquier juego. No obstante, debe 
ajustar platoon_size (tamaño del pelotón) en ufo2000.ini (e.g. numero de soldados 
en "soldier.dat").

--FIN DEL FICHERO--
