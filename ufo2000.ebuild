# Copyright 1999-2003 Gentoo Technologies, Inc.
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit games

DESCRIPTION="free multiplayer remake of X-COM (UFO: Enemy Unknown)"
HOMEPAGE="http://ufo2000.sourceforge.net/"
SRC_URI="http://ufo2000.lxnt.info/files/${P}-src.tar.bz2
         ftp://ftp.microprose.com/pub/mps-online/x-com/xcomdemo.zip
         ftp://ftp.microprose.com/pub/mps-online/demos/terror.zip"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="x86"

DEPEND="virtual/glibc
	>=media-libs/allegro-4.0.0
	>=dev-lang/lua-5.0
	>=dev-games/hawknl-1.66
	dev-libs/expat"

src_unpack() {
	unpack ${P}-src.tar.bz2
	cd ${WORKDIR}/${P}/XCOMDEMO
	unpack xcomdemo.zip
	unzip XCOM.EXE -d ..
	rm XCOM.EXE
	cd ${WORKDIR}/${P}/TFTDDEMO
	unpack terror.zip
	unzip TFTD.ZIP
	rm TFTD.ZIP
	cd ${WORKDIR}
}

src_compile() {
	make DATA_DIR="${GAMES_DATADIR}/${PN}" OPTFLAGS="${CXXFLAGS}" || die "make failed"
}

src_install() {
	exeinto ${GAMES_BINDIR}
	doexe ufo2000
	dodir ${GAMES_DATADIR}/${PN}
	cp -R XCOMDEMO XCOM TFTDDEMO TFTD ${D}/${GAMES_DATADIR}/${PN}
	echo "You can copy data files from X-COM here" > ${D}/${GAMES_DATADIR}/${PN}/XCOM/readme.txt
	echo "You can copy data files from TFTD here" > ${D}/${GAMES_DATADIR}/${PN}/TFTD/readme.txt
	cp -R arts newmaps init-scripts ${D}/${GAMES_DATADIR}/${PN}
	cp ufo2000.dat keyboard.dat geodata.dat soldier.dat armoury.set items.dat ufo2000.ini soundmap.xml ${D}/${GAMES_DATADIR}/${PN}
	dodoc readme* INSTALL AUTHORS COPYING ChangeLog techinfo.txt
	prepgamesdirs
}

pkg_postinst() {
	games_pkg_postinst
	einfo "If you have a full version of X-COM, you can copy all the files"
	einfo "from the directory where you have it installed into"
	einfo "   ${GAMES_DATADIR}/${PN}/XCOM"
	echo
	einfo "This will allow you to use more terrain types and units."
	echo
}
