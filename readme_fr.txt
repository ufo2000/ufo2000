              **************************************************
              **                UFO2000 0.4.0                 **
              **             aka X-COM: Gladiators            **
              **        http://ufo2000.sourceforge.net/       **
              **              Manuel en français              **
              **************************************************

                                  $Rev: 324 $

             Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
             Copyright (C) 2002-2004  ufo2000 development team

		Traduit par Benjamin Power et JFarceur

Le développement de UFO2000 reprend après près d'un an d'inactivité.
UFO2000 est maintenant de retour et est amélioré grâce aux efforts
coopératifs de plusieurs développeurs, la "ufo2000 development team"
pour faire court. Pour plus d'informations sur qui a contribué à UFO2000,
lisez le fichier AUTHORS dans le répertoire UFO2000. Le premier auteur,
Alexander Ivanov, n'est pas impliqué en ce moment dans le processus de
développement, mais il a déjà fourni un énorme travail, donc merci
à lui! Sans son excellent travail ce jeu n'aurait jamais existé.

Ce jeu est un logiciel en distribution libre (GPL). Pour plus d'informations 
sur la licence, lisez le fichier COPYING, situé dans le répertoire
UFO2000.

N'importe qui peut prendre part dans le développement de UFO2000.
Toute aide serait appréciée. Veuillez visitez la page web
http://ufo2000.sourceforge.net/ où vous pouvez vous inscrire à
la liste de diffusion par courriel ("mailing list")de UFO2000 
et faire parvenir vos commentaires,idées, rapports de bugs et patchs.

Voir le fichier Changelog pour la liste complète de mises à jour.

===============
À Propos
===============

UFO2000 est un remake multijoueur de la partie tactique du jeu de
Microprose, X-COM: UFO Defense (UFO: Enemy Unknown).

Vous avez besoin des fichiers de données de la version originale de X-COM pour
pouvoir jouer à UFO2000. Le jeu peut aussi fonctionner avec les fichiers de
données de la démo de X-COM, mais vous ne pouvez dans ce cas que jouer sur la
carte 'City' et n'utiliser qu'un nombre limité d'unités. La possession de la
version complète de X-COM est donc vivement recommandée.

Le jeu compile et fonctionne sous les systèmes d'exploitation Windows,
Linux et FreeBSD. Le jeu peut probablement aussi marcher sous d'autres
systèmes semblables à UNIX. Si vous avez réussi à compiler et exécuter
UFO2000 sous un de ces derniers, veuillez nous en faire part.

De l'information plus détaillée sur l'installation de UFO2000 peut être trouvée
dans le fichier INSTALL depuis le répertoire UFO2000.

=========
Main Menu (Menu Principal)
=========

Après avoir lancé le jeu, vous verrez un certain nombres d'options dans le menu:

   Connect to internet server (Se connecter au serveur internet)

      Le jeu peut se connecter à un serveur sur internet ou sur un réseau
      local (pour le démarrer de votre ordinateur, lancez l'exécutable
      'ufo2000-srv' inclus dans la distribution de ufo2000). Après avoir
      sélectionné cette option du menu, vous verrez un dialogue de connexion.
      Vous devez entrer une adresse de serveur, lxnt.info est utilisé par défaut
      et est pour le moment le serveur officiel de ufo2000 sur internet. Si vous
      voulez jouer en réseau local, vous devez commencer un serveur ufo2000 sur
      un ordinateur et entrer son adresse IP pour s'y connecter depuis ufo2000.
      Les deux autres champs requis sont login et password. Ils sont utilisés
      pour identifier un joueur et garder des statistiques
      (http://ufo2000.lxnt.info/results.php en ce moment). Le nom de login
      doit être un nom inutilisé par les autres joueurs et le mot de passe doit
      avoir au moins 6 caractères. Après un login ou enregistrement effectué,
      vous verrez l'écran de chat du serveur internet.

   Start hotseat game (Démarrer un jeu hotseat)

      Commence le jeu sur un seul ordinateur (pas de réseau nécessaire). Les
      joueurs alternent à la fin leur tour. Après avoir cliqué cette option
      du menu, vous allez directement à l'écran de planification de mission.

   Load saved game (Charger une partie sauvegardée)

      Charge une partie sauvegardée au préalable. Ne marche que pour les
      parties hotseat.

   Quit (Quitter)

      Pas de commentaires ;)

====================
Internet Server Chat (Chat du serveur internet)
====================

Une fois connecté à un serveur ufo2000, vous verrez une console de chat
et une liste de joueurs connectés au même serveur. Le statut de chaque
joueur est déterminé par la couleur de son nom:

nom de joueur blanc - votre propre nom
nom de joueur gris  - disponible pour chat
nom de joueur jaune - vous avez envoyé un défi à ce joueur
nom de joueur vert  - vous pouvez accepté un défi de ce joueur
nom de joueur rouge - le joueur est occupé à défier une autre personne.

Pour défier un joueur, cliquez seulement sur son nom, qui deviendra jaune.
Si le nom de quelqu'un est vert, cela signifie qu'il vous a défié et
après avoir cliqué sur son nom, le jeu commencera et la console de chat
sera remplacée par l'écran de planification de mission.

Le joueur qui lance le défi (la première qui clique sur le nom de l'autre)
sera l'hôte de la partie, tandis que le second sera le client.
===============
Mission Planner (Planificateur de mission)
===============

Ici vous pouvez sélectionner vos soldats, choisir leurs emplacements de
départ et modifier leurs statistiques/armes de départ en maintenant
la touche CTRL(gauche) et en cliquant sur le soldat voulu (voir
la section sur l'Editeur d'Unités ci-dessous).

Pour placer vos unités cliquer avec le bouton gauche de la souris sur son
nom puis pointer sur la place désirée sur la carte. Cliquer de nouveau pour
confirmer. Pour désélectionner un soldat faites un clic droit sur son nom. 
Notez que vous ne pouvez placer vos soldats que dans 10 (dix) premières
cases de votre côté (celles du côté que vos noms de joueurs sont affichés).

Info sur les unités du joueur (affichée sous les noms de soldats):
    1) "total men points=..." (Somme totale de points) La somme totale des
       paramètres (TU [unité d'action], Health [vie],...)
    2) Nombre d'armes de son type pour les unités choisies.

Pour générer une nouvelle carte cliquer "NEW". La taille de la carte est 
ajustable dans le fichier ufo2000.ini. L'option "LOAD" (charger) ne fonctionne
pas pour le moment. Vous devrez vous contenter de cartes aléatoires.

Après avoir terminé vos plans cliquer sur le mot "SEND" de votre côté
(serveur/client) pour envoyer vos données à l'autre joueur. "SEND"
deviendra alors vert. Pour commencer la partie, les joueurs doivent
cliquer sur "START" après que tous les mots "SEND" soient verts. Le jeu
ne débutera que si toutes les options sont vertes. Modifier vos joueurs
ou changer la carte réinitialisera les options au rouge.

Notez que même en hotseat, les options send/start doivent être mises au vert.

Pour retourner au menu principal, appuyez sur ESC.

===============
Unit Editor     (Editeur d'Unités)
===============

Vous pouvez accéder à ce menu depuis l'écran de positionnements des unités.
Maintenez CTRL(gauche) et cliquer sur le soldat que vous voulez éditer.

En entrant dans l'éditeur vous pouvez modifier les paramètres de vos soldats
et son équipement. Les paramètres concernés pour vos soldats sont:

Name (Nom)			Le nom du soldat

Skin (Apparence)		L'apparence du soldat détermine non seulement
				l'image utilisée sur le jeu, mais aussi l'armure
				dont disposera le soldat. Comme l'armure risque 
				fortement de débalancer le jeu, il est conseillé
				aux joueurs de s'entendre sur le type d'armure
				utilisée pendant la partie.

Time Units (Unités de temps)	Déterminent surtout la distance maximale
				que le soldat peut parcourir pendant un tour.
  (TU's)			Toute action nécessite des unités de temps, mais
				la majorité de celles-ci consomment un pourcentage
				des unités de temps (toutes sauf le déplacement).
				Garder suffisamment d'unités de temps à la fin
				d'un tour permet au soldat de pouvoir faire un
				tir de réaction. Les unités de temps se rechargent
				au complet à chaque tour. 

Stamina/Energy (Endurance)	Détermine le niveau d'endurance à la course du
				soldat. Chaque pas d'un soldat coûte quelques
				points d'endurance (à l'instar des TU's), mais 
				celle-ci ne se regénère pas au complet au début
				d'un tour (au contraire des TU's). Si vous n'avez
				plus d'endurance, votre soldat ne peut plus marcher
				pendant le tour en cours.

Bravery (Courage)		Le courage n'est pas encore implémenté.

Reactions (Réactions)		Si votre soldat dispose suffisamment d'unités de 
				temps à la fin du tour, il a la chance de faire un 
				tir de réaction pendant le tour de l'ennemi, si un
				ennemi passait dans le champ de vision du soldat. 
				La probabilité qu'un tir de réaction ait lieu dépend 
				du nombre de TU's restants et du niveau de Réactions
				du soldat.

Firing (capacité de tir)	Plus cette valeur est haute, plus le soldat a de
				chances de toucher sa cible avec des armes à feu. 
				Notez qu'un soldat à genoux a un bonus à sa précision
				au tir.

Throwing (précision de lancer)	Plus cette valeur est haute, plus le soldat a de chances
				de lancer un objet (grenade ou autre) au bon endroit.

Strenght (Force)		L'équipement qu'un soldat peut porter dépend de la
				force dont il dispose. Le poids total de son équipement
				(WEIGHT) ne doit pas dépasser l'attribut de force du
				soldat, sinon une pénalité de TU's sera faite au début
				de chaque tour, de telle sorte que (force/poids = TU/TU_max).
				Notez qu'un point de force coûte deux points, au 
				contraire des autres statistiques.

D'autres paramètres sont désactivés parce que nous savons pas exactement comment ils 
fonctionnent dans le jeu (tels que le courage et le moral). 

Pour modifier l'équipement, cliquez sur l'objet que vous voulez et glissez le
dans l'inventaire du soldat. Notez que certaines armes nécessitent des munitions.
Pour charger les armes, déposer des munitions sur une arme dans l'équipement du 
soldat. Pour vérifier si elle est chargée, cliquez dessus, l'image des munitions
devrait apparaître à droite. 

Chaque paramètre est restreint à être >= 50 et <= 80 (la force est de 25 à 40).
La somme de ces paramètres a une certaine limite pour chaque unité,
donc vous ne pouvez pas mettre toutes les compétences au maximum. Il n'y a pas
de restrictionspour le type ou le nombre de chaque arme pour vos unités (sauf
pour ce qui est de la force des soldats),donc pendant la phase de Planification
de Mission il est conseillé de regarder la sélection adverse d'armes, et de
discuter du choix des armes et des armures, afin que la partie soit la plus
intéressante possible.

F2            Save (Sauvegarder l'équipe)
F3            Load (Charger une équipe existante)

CTRL Gauche   Montre le "item damage status", statut des dégâts d'un objet
ALT  Gauche   (dans l'éditeur -- valeur maxi), fonctionne aussi dans l'écran de
              l'inventaire pendant une partie.

===============
Game Controls   (Commandes de jeu)
===============

Toutes les actions des unités, comme dans X-COM, sont dirigées à l'aide de la
souris. Vous pouvez aussi envoyer et recevoir des messages à/de l'autre joueur
pendant la partie en tapant votre message au clavier puis en validant avec entrée.

Ces touches sont utilisées pour contrôler le jeu:

F10         Mode plein écran/fenêtrée
ESC         Retour au menu principal

Touches directionnelles   	Change la taille de l'écran de la carte
Clavier numérique +/-		Notez qu'en minimisant la taille de l'écran, vous aurez
				accès à une mini-carte à droite, qui affichent les
				unités et votre champ de vision.


CTRL Droit  Echange rapide entre les deux plans de clavier choisis dans le fichier
            ufo2000.ini avec primary_keyboard et secondary_keyboard .

CTRL Droit + 'b', 'd', 'e', 'p' or 'r' Echanger entre les plans de claviers biélorusse,
                     allemand, anglais, polonais et russe (d'autres langues
                     peuvent êtres choisies dans le fichier ufo2000.ini)

CTRL Gauche   Montre le chemin du soldat sélectionné à la cellule du pointeur avec le
              coût en TU. Dans le mode de visée, montre la trajectoire de vos tirs et de
              vos objets lancés. Dans l'inventaire, montre le "item damage value", statut
              des dégâts d'un objet (ou alternativement avec ALT).

SHIFT Gauche 	Permet une visée précise si maintenu pendant le mode visée, 
		avec une arme à feu.

ENTER       Envoye un message.
BKSP        Supprime dernier caractère d'un message.

F12         Sauvegarde la capture d'écran dans le fichier "snapshot.pcx"

PageUP/PageDOWN		Change "d'étage" plus rapidement.

===============
INI file
("ufo2000.ini")
===============
[System] = 
keyboard = us		Change la langue du clavier 
			(voir le fichier pour les choix disponibles)

# les 2 claviers alternatifs atteignables avec CTRL droit pendant le jeu
primary_keyboard = us
secondary_keyboard = ru

[General]
width = 640				#Largeur de l'écran de jeu au démarrage
height = 360				#Hauteur de l'écran de jeu au démarrage
map_width = 4				#Largeur des cartes générées au hasard
map_height = 4				#Hauteur des cartes générées au hasard
speed_unit = 30				#La vitesse de l'animation des soldats
speed_bullet = 30			#La vitesse de l'animation des balles	
speed_mapscroll = 30			#La vitesse de défilement de la carte en jeu
mapscroll = 10				

# La police d'écriture de la console doit être parmi celles-ci : default, xcom_small, xcom_large
consolefont = default

# La limite de temps par tour (valeur négative = temps illimité)
time_limit = -1

# résolution de l'écran désirée, si possible. Sinon 640x480 est utilisée
screen_x_res = 640
screen_y_res = 400

# Profondeur de couleur minimale désirée. Si impossible,
# des modes vidéo avec des profondeurs plus élevées sont utilisées.
color_depth = 16

# Fichiers musicaux utilisés pour les divers moment du jeu (.MIDI .XM .S3M .OGG .IT .MOD)
menu_music = $(ufo2000)/newmusic/WISDOM.XM	#menu d'ouverture du jeu
setup_music = $(ufo2000)/newmusic/mindbox.S3M	#écran de positionnement des soldats
editor_music = $(xcom)/sound/gmdefend.mid	#éditeur (non disponible)
combat_music = $(ufo2000)/newmusic/ATeX.OGG	#musique de combat
win_music = $(xcom)/sound/gmwin.mid		#musique de victoire
lose_music = $(ufo2000)/newmusic/areda.S3M	#musique de défaite
net_music1 = $(xcom)/sound/gmgeo1.mid		#musique sur le serveur internet
net_music2 = $(xcom)/sound/gmgeo2.mid		#musique alternative sur internet

# images d'arrière-plan
loading_image = $(ufo2000)/arts/text_back.jpg	#image au chargement du jeu
menu_image = $(ufo2000)/arts/menu.jpg		#image du menu principal
endturn_image = $(xcom)/ufograph/tac00.scr	#image de fin de tour
win_image = $(xcom)/geograph/back01.scr		#image de victoire
lose_image = $(xcom)/geograph/back02.scr	#image de défaite

[Server]
host = lxnt.info		#adresse du serveur
login = JFarceur		#nom de login
autologin = 1			#login automatique : 1 pour oui, 0 pour non
# http proxy settings (it must support CONNECT method to work)
# one of the following 3 values should be set: 
#   host:port (host name and port of http proxy)
#   auto (take value from http_proxy environment variable)
#   disabled (do not use http proxy)
http_proxy = disabled
password = 

[Editor]
platoon_size = 10		#taille de l'équipe (conseillé de laisser à 10)
last_map_name = geodata.dat	#la dernière carte utilisée
last_unit_name = team_ru.units	#la dernière équipe utilisée

[Flags]
F_FULLSCREEN = 0  # Plein écran au démarrage du jeu ( 1 pour oui , 0 pour non )
F_LARGEFONT = 0   # Configurez à 1 pour utiliser la police large de ufo pour les dialogues et la console
F_SMALLFONT = 0   # Configurez à 1 pour utiliser la police petite de ufo, annule F_LARGEFONT
F_SOUNDCHECK = 0  # Exécute un test des sons si configuré à 1
F_LOGTOSTDOUT = 0 # Copie les "init output" dans le fichier stdout (pour retracer des bugs)
F_PLANNERDBG = 1  # Permet de placer ses unités partout sur la carte. 


