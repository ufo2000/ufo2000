drop table ufo2000_users;
drop table ufo2000_sequences;
drop table ufo2000_games;
drop table ufo2000_game_packets;
drop table ufo2000_game_players;

CREATE TABLE ufo2000_users 
(name text primary key,password text);

CREATE TABLE ufo2000_sequences
(name text primary key,seq_val integer);

CREATE TABLE ufo2000_games 
(id integer primary key, 
 last_received_packed integer,
 is_finished text, -- 'Y'/'N'
 errors text,
 result integer --0-not finished, 1-player1 has won, 2-player2 has won, 3-draw
);

CREATE TABLE ufo2000_game_players
(game integer, 
 player text,
 last_sended_packet integer,
 position integer, -- 1 - the player goes first
 primary key(game, player)
);


insert into ufo2000_sequences values ('ufo2000_games',0);

CREATE TABLE ufo2000_game_packets
(game integer,
 id integer,
 sender integer, -- 1-player1, 2-player2
 date text,
 command text,
 primary key(game,id));
