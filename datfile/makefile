# Compile misc. resources (icons, color-palette...) into ufo2000.dat
#
all:
	dat -l  ../ufo2000.dat
# Stop if dat.exe from allegro-tools is missing
	rm -f ../ufo2000.dat
	dat -a ../ufo2000.dat gamepal.bmp -t PAL
	dat -a ../ufo2000.dat menupal.bmp -t PAL
	dat -a ../ufo2000.dat tftdpal.bmp -t PAL
	dat -a ../ufo2000.dat fonts/unifont_8x8.txt -t FONT
	dat ../ufo2000.dat UNIFONT_8X8_TXT NAME=UNIFONT_8X8
	dat -a ../ufo2000.dat fonts/biglets.cyr -t DATA
	dat -a ../ufo2000.dat fonts/smallset.cyr -t DATA
	dat -a ../ufo2000.dat graphics/mouse.bmp -t BMP
	dat -a ../ufo2000.dat graphics/mouse2.bmp -t BMP
	dat -a ../ufo2000.dat graphics/select.bmp -t BMP 
	dat -a ../ufo2000.dat graphics/select2.bmp -t BMP
	dat -a ../ufo2000.dat graphics/select3.bmp -t BMP
	dat -a ../ufo2000.dat graphics/select4.bmp -t BMP
	dat -a ../ufo2000.dat graphics/kastet.bmp -t BMP
	dat -a ../ufo2000.dat graphics/knife.bmp -t BMP
	dat -h ../ufo2000.h -p DAT_ ../ufo2000.dat
#
	echo; echo "Don't forget to update crc in filecheck.lua !"
#.
