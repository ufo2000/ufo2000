drop view ufo2000_report_games_data;
drop view ufo2000_report_games_html;
drop view ufo2000_report_html;

create view ufo2000_report_games_data as
select p1.player pl1, p2.player pl2, g.result res
from ufo2000_games g, ufo2000_game_players p1, ufo2000_game_players p2
where g.id=p1.game and g.id=p2.game and p1.position=1 and p2.position=2;

create view ufo2000_report_games_html as
select "<br> <b> UFO2000 played games statistics table</b><br>" from dual
union all
select "<table border=1>" from dual
union all
select "<tr><td>"||"Player1"||"<td>"||"Player2"||"<td>"||"Result"||"<td>" from dual
union all
select "<tr><td>"||pl1||"<td>"||pl2||"<td>"||res||"<td>" from ufo2000_report_games_data
union all
select "</table>" from dual;

create view ufo2000_report_html as
select * from ufo2000_report_games_html;

