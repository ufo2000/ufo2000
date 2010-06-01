
              **************************************************
              **                UFO2000 0.4.0                 **
              **        http://ufo2000.sourceforge.net/       **
              **               Manual Português               **
              **************************************************

                                  $Rev: 439 $

             Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
             Copyright (C) 2002-2004  ufo2000 development team

O desenvolvimento do UFO2000 continua depois de cerca de um ano de
inactividade. Agora o UFO2000 está de volta e é melhorado pelos esforços
cooperativos de várias pessoas que o desenvolveram; a "ufo2000 development
team" para ser breve. Para informações sobre quem contribuiu para o UFO2000,
leia o ficheiro AUTHORS no directório do UFO2000. O autor original,
Alexander Ivanov, actualmente já não está envolvido no processo de
desenvolvimento, mas ele já fez um grande trabalho, muitos agradecimentos
para ele! Sem o seu excelente trabalho este jogo nunca teria existido.

Este jogo é software grátis (GPL). Para mais informações sobre a licença,
leia o ficheiro COPYING, localizado no seu directório do UFO2000.

Qualquer pessoa pode tomar parte no desenvolvimento do UFO2000; qualquer
ajuda é apreciada. Por favor visite o site http://ufo2000.sourceforge.net.
Lá pode inscrever-se na lista de correio do ufo2000 e apresentar os seus
comentários, ideias, falhas e actualizações.

Veja o ficheiro ChangeLog para uma lista completa de mudanças.

===============
Acerca de
===============

O UFO2000 é um "remake" multijogador da parte táctica do jogo da
Microprose, X-COM: UFO Defense (UFO: Enemy Unknown).

Para jogar o UFO2000, necessita de ficheiros do X-COM original. O jogo
também funciona com ficheiros da demo do X-COM, mas apenas poderá jogar
no mapa 'Cidade' e usar um conjunto limitado de unidades. Ter uma versão
completa do X-COM é altamente recomendado.

O jogo compila e trabalha nos sistemas operativos Windows, Linux e FreeBSD.
Provavelmente o jogo também funcionará em outros sistemas UNIX. Se
conseguiu compilar e executar o UFO2000 com sucesso num deles, por favor
informe-nos.

Informação detalhada sobre instalar o UFO2000 pode ser encontrada no
ficheiro INSTALL no directório do UFO2000.

===============
Menu Principal
===============

Depois de iniciar o jogo, verá um número de opções do menu disponíveis:

   Connect to internet server (Ligar a um servidor de internet)

      O jogo pode-se ligar a um servidor na internet on numa rede local
      (para iniciar o servidor no seu computador, apenas execute o ficheiro
      'ufo2000-srv' incluido na distribuição do ufo2000). Depois de
      escolher esta opção, verá a janela "connect to server". Tem que
      introduzir o endereço do servidor; lxnt.info é o pré-definido, é
      actualmente o servidor oficial de ufo2000 na internet. Se quer jogar
      numa rede local, tem que iniciar o servidor do ufo2000 num computador,
      e introduzir o seu endereço IP para se ligar a ele a partir de um
      ufo2000 noutro computador.
     Os outros dois campos necessários são login e palavra-passe. Eles são
      usados para identificar o utilizador para recolher algumas estatísticas
      do jogo (http://ufo2000.lxnt.info/results.php, actualmente). Login
      deve ser qualquer nome não usado pelos outros jogadores e palavra-
      -passe deve ter no mínimo 6 caracteres. Depois de login e registo
      com sucesso, verá o ecrã de chat do servidor de internet.

   Start hotseat game (Iniciar jogo "hotseat")

      Inicia o jogo num só computador (rede não é necessária). O jogador
      actual muda depois de cada turno. Depois de clicar neste item, será
      movido directamente para o ecrã do planeador de missões.

   Load saved game (Carregar jogo guardado)

      Carrega um jogo previamente guardado. Funciona apenas para jogos
      "hotseat".

   Quit (Sair)

      Sem comentários ;)

================================
Conversa do Servidor de Internet
================================

Depois de ligar a um servidor ufo2000, verá uma consola de chat e uma
lista de jogares ligados ao servidor. O estao de cada jogador é
determinado pela cor do seu nome:

nome de jogador branco   - é o seu próprio nome
nome de jogador cinzento - disponível para conversa
nome de jogador amarelo  - enviou um desafio a este jogador
nome de jogador verde    - pode aceitar um desafio deste jogador
nome de jogador vermelho - o jgador está ocupado a jogar com alguém

Para desafiar um jogador, basta clicar no nome dele (que fica amarelo).
Se o nome de alguém é verde, significa que ele o desafiou. Depois de
clicar no nome dele, o jogo começa e a consola de chat será
substituida pelo ecrã do planeador de missões.

====================
Planeador de Missões
====================

Aqui pode seleccionar soldados, determinar as suas posições inicias, e
editar as suas estatísticas/armas iniciais fixando o CTRL esquerdo e
clicando no soldado desejado (veja a secção Editor de Unidades abaixo).

Para colocar as suas unidades, clique no nome dele(a) com o botão direito
do rato e aponte para a posição inicial desejada no mapa. Clique de novo
para confirmar. Para deseleccionar o soldado clique com o botão direito
do rato no nome dele(a).

Informação sobre unidades:
   1) "total men points=..." Custo total de pontos para as unidades
   seleccionadas.
   2) Número de armas de cada tipo para as unidades seleccionadas.

Caixa "Match settings" (Definições de jogo):
   1)Nome do cenário seleccionado:
       Clique para escolher o cenário a ser usado e opções do cenário.
   2)Nome do mapa seleccionado:
       Clique para seleccionar o mapa e tamanho do mapa; e para gerar
       um mapa novo ou carregar um mapa existente.

Depois de completar os seus planos, clique no botão "SEND" (ENVIAR) do
seu lado para enviar os seus dados ao outro jogador.
Se os seus soldados selecionados cumprem os requísitos do jogo, "SEND" ficará
verde. Caso contrário verá uma mensagem sobre o erro na consola de chat.
Para iniciar o jogo, os jogadoes têm que clicar em "START" (INICIAR) depois
de todas as opções "SEND" estarem verdes. O jogo só começará quando todas
estas opções estiverem verdes. Ajustar os jogadores, mudar o mapa, cenário
ou regras do jogo vai reverter todas as opções para vermelho.

Para voltar ao menu principal, clique em ESC.

==================
Editor de Unidades
==================

Pode aceder a isto a partir do ecrã do Planeador de Missões. Fixe o
CTRL e clique no soldado que deseja editar.

Entrando nisto, pode editar os parâmetros do soldado e equipamento.
Parâmetros activados para soldados são: Stamina, Time Units, Health
(Saúde), Firing, Throwing Accuracy (Pontaria de Disparo e de Atirar) e
Reactions (Reacções). Outros parâmetros estão desactivados porque não
sabemos como funcionam no jogo. Se, depois de organizar o seu equipamento,
deseja editar os seus "stats" de novo, clique no gráfico da direita.

Cada parâmetro é limitado para ser >= 50 e <= 80. A soma de todos
estes parâmetros tem um certo limite para cada unidade, por isso não
pode definir todos os parâmetros no máximo. Pode guardar e carregar
várias configurações de unidades/equipas com estas teclas:

F2          Guardar
F3          Carregar

===================
Controlos de Jogo
===================

Todas as acções de unidades, como no X-COM, são controladas com o rato.
Pode enviar e receber mensagens para/de o outro jogador durante o jogo.
Apenas escreva a mensagem no teclado, depois clique em enter.

Estas teclas são usadas para controlo do jogo:

F10		Modo Ecrã Completo/Modo em Janela

ESC		Sair para o menu principal

Setas e		Mudar o tamanho do ecrã do mapa
Keypad +/-

CTRL Direito	Mudar rápido entre dois teclados seleccionados em primary_keyboard
F9          	e secondary_keyboard no ficheiro ufo2000.ini.

CTRL Direito + 'b', 'd', 'e', 'p' or 'r'
		Mudar entre teclado Bielorusso, Alemão, Inglês, Polaco e Russo
		(outros idiomas podem ser seleccionados no ficheiro ufo2000.ini)

CTRL Esquerdo	Mostrar rota do soldado seleccionado até o bloco seleccionado
		com custo de TU (Time Units).
		No modo de apontar, mostra a trajectória dos seus tiros e items
		atirados. No ecrã da inventoria, mostra valor de dano dos items.
		(ALT alternativo)

SHIFT Esquerdo	Permite um apontar preciso se fixado até depois de disparar
        	com o botão esquerdo do rato.

SHIFT Esquerdo + setas
		"Scroll" na janela do mapa

ENTER		Enviar mensagem
BKSP		Remover último caractér na mensagem

F12		Guardar uma imagem do ecrã no ficheiro "snapshot.pcx" 

===============
Ficheiro INI
("ufo2000.ini")
===============
[System]
keyboard = us		Idioma do teclado 
			(veja o ficheiro INI para as opções possíveis)

# dois teclados alternativos para alternar entre eles usando a tecla CTRL direito
primary_keyboard = us
secondary_keyboard = ru

[General]
width = 640				#Comprimento do ecrã do mapa
height = 360				#Largura do ecrã do mapa
map_width = 4				#Comprimento do mapa gerado
map_height = 4				#Largura do mapa gerado
speed_unit = 30				#Velocidade de animação do movimento de unidades
speed_bullet = 30			#Velocidade de animação do movimento de balas
speed_mapscroll = 30			#Velocidade de "scrolling"
mapscroll = 10				

# tipo de letra da consola, tem que ter um destes valores : default, xcom_small, xcom_large
consolefont = default

# tempo limite de cada turno, em segundos (valor negativo - sem limite)
time_limit = -1

# resolução de ecrã preferida, se não for possível, 640x480 é utilizado
screen_x_res = 640			#Comprimento da resolução
screen_y_res = 400			#Largura da resolução

# definição de cores mínima preferida para o modo de vídeo, se não pode
# ser seleccionada, modos de vídeo com valores mais altos são utilizados
color_depth = 16

# volume da música (de 0 a 255)
music_volume = 255

# ficheiros para música de fundo (.MIDI .XM .S3M .OGG .IT .MOD)
menu_music = $(ufo2000)/newmusic/gmstory.ogg	#menu principal
setup_music = $(ufo2000)/newmusic/gmenbase.ogg	#planeador de missões
editor_music = $(ufo2000)/newmusic/gmdefend.ogg	#editor de unidades
combat1_music = $(ufo2000)/newmusic/gmstory.ogg	#música de combate
combat2_music = $(ufo2000)/newmusic/gmstory.ogg	#música de combate alternativa
win_music = $(ufo2000)/newmusic/gmlose.ogg	#música de vitória
lose_music = $(ufo2000)/newmusic/gmlose.ogg	#música de derrota
net_music1 = $(ufo2000)/newmusic/gmgeo.ogg	#música do ecrã de chat do servidor de internet
net_music2 = $(ufo2000)/newmusic/gmgeo.ogg	#música alternativa do ecrã de chat do servidor de internet

# imagens de fundo
loading_image = $(ufo2000)/arts/text_back.jpg	#imagem de carregamento do jogo
menu_image = $(ufo2000)/arts/menu.jpg		#imagem do menu principal
endturn_image = $(xcom)/ufograph/tac00.scr	#imagem de fim do turno
win_image = $(xcom)/geograph/back01.scr		#imagem de vitória
lose_image = $(xcom)/geograph/back02.scr	#imagem de derrota

[Server]
host = lxnt.info		#endereço do servidor
login = anonymous		#login do jogador
autologin = 0			#login automático (1 = sim, 0 = não)
# definições de proxy http (tem que suportar método CONNECT para funcionar)
# um destes 3 valores tem que ser definido:
#   host:port (nome do host e port da proxy http)
#   auto (retirar valor da variável de ambiente http_proxy)
#   disabled (não usar proxy http)
http_proxy = disabled
password = 			#palavra-passe do jogador

[Editor]
platoon_size = 10		#tamanho de cada equipa (máximo é 10)
last_map_name = geodata.dat	#o último mapa (ficheiro) utilizado
last_unit_name = team_ru.units	#a última equipa (ficheiro) utilizada

[Flags]
F_FULLSCREEN = 1		#Usar 1 para iniciar o jogo em modo ecrã completo
F_LARGEFONT = 0			#Usar 1 para usar o tipo de letra 'large' do ufo para janelas, consola e planeador
F_SMALLFONT = 0			#Usar 1 para usar o tipo de letra 'small'. Isto substitui a definição F_LARGEFONT
F_SOUNDCHECK = 0		#Executar verifição de sons no arranque se 1.
F_LOGTOSTDOUT = 0		#Copiar saída de init para stdout
F_PLANNERDBG = 0		#Permite colocar unidades (no planeador) em qualquer parte no mapa 

--FIM DO FICHEIRO--

