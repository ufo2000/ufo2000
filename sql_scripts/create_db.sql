drop table ufo2000_users;
drop table ufo2000_sequences;
drop table ufo2000_games;
drop table ufo2000_game_packets;

CREATE TABLE ufo2000_users 
(name text primary key,password text);

CREATE TABLE ufo2000_sequences
(name text primary key,seq_val integer);

CREATE TABLE ufo2000_games 
(id integer primary key, 
 player1 text,
 player2 text,
 state integer,
 last_packet_to_pl1 integer,
 last_packet_to_plr2 integer,
 is_finished text,
 errors text,
 result integer --0-not finished, 1-player1 has won, 2-player2 has won, 3-draw
);

insert into ufo2000_sequences values ('ufo2000_games',0);

CREATE TABLE ufo2000_game_packets
(game integer,
 id integer,
 sender integer, -- 1-player1, 2-player2
 date text,
 primary key(game,id));
