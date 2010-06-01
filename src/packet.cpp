/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002       ufo2000 development team

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "stdafx.h"

#include "server_protocol.h"
#include "global.h"
#include "packet.h"
#include "multiplay.h"
#include "units.h"

char *Packet::strCommand[COMMAND_NUM] = {
    "NOTE", "QUIT", "TURN", "DOOR", "POSE", "PRIM", "UNLO", "LOAD",
    "TAKE", "DROP", "MOVE", "FACE", "TACT", "ADUN", "PDSZ", "PDAT",
    "SEUN", "DEUN", "MDAT", "STRT", "REST", "TLIM", "TCRC", "ELEV",
    "SCEN", "RULE", "OPTN", "PANI", "MORL", "RTIM", "P2ST", "EQLS",
    "DRAW", "IRND", "RSTP", "VSRC", "EQCH"
};

Packet::Packet()
{
    reset();
}

void Packet::reset()
{
    cur = 0; size = 0;
    memset(data, 0, sizeof(data));
}

extern Units local;
extern int build_crc();
extern int GAMELOOP;

void Packet::create(Command cmd)
{
    reset();
    assert (local.Position >= 1 && local.Position <= 2);
    int len = sprintf(data + size, "_Xcom_%d_%05d_%s_", local.Position, local.packet_num, strCommand[(int)cmd]);
    g_current_packet_num=local.packet_num;
    g_current_packet_pos=local.Position;
    local.packet_num++;
    size += len;
    
    if(GAMELOOP && net->gametype == GAME_TYPE_INTERNET_SERVER)
    {
        // send debug info to the server (crc)
        char debug_info[1000];
        sprintf(debug_info, "%d_%05d_%d", g_current_packet_pos, g_current_packet_num, build_crc());
        net->m_internet_server->send_packet(SRV_SAVE_DEBUG_INFO, std::string(debug_info));
    }
}

//01234567890123
//23_Xcom_TURN_
//        8 2
//^      ^  ^
//pkt - buf
Command Packet::command(char *buf, int buf_size)
{
    memset(data, 0, sizeof(data));
    cur = 0;
    if (strstr(buf, "_Xmes_") != NULL) {
        strcpy(data, strstr(buf, "_Xmes_") + 6);
        return CMD_MESSAGE;
    }

    if (strstr(buf, "_Xcom_") != NULL)
        for (int i = 0; i < COMMAND_NUM; i++) {
            char *xcom = strstr(buf, "_Xcom_");
            char *pkt = NULL;
            Position = xcom [strlen("_Xcom_")] - '0';
            
            //get packet number from the packet
            char packet_num[100];
            strncpy(packet_num, xcom + strlen("_Xcom_") + 2, 5);
            packet_num[5]=0;
            g_current_packet_num = atol(packet_num);
            g_current_packet_pos = Position;

            if (!memcmp(xcom + strlen("_Xcom_") + 8, strCommand[i], strlen(strCommand[i])))
                pkt = strstr(buf, strCommand[i]);

            if (pkt != NULL) {
                int data_start = pkt + strlen(strCommand[i]) + 1 - buf;
                memcpy(data, buf + data_start, buf_size - data_start);
                size = buf_size - data_start;
                return (Command)i;
            }
        }
    return CMD_NONE;
}

Packet &Packet::operator<<(const std::string &x)
{
    for (int i = 0; i < (int)x.size(); i++) {
        if (x[i] == '_' || x[i] == '\\') {
            data[size++] = '\\';
            data[size++] = x[i];
        } else if (x[i] == '\n') {
            data[size++] = '\\';
            data[size++] = 'n';
        } else if (x[i] < 0x20) {
            sprintf(&data[size], "\\%02X", (uint8)x[i]);
            size += 3;
        } else {
            data[size++] = x[i];
        }
    }
    data[size++] = '_';
    return *this;
}

Packet &Packet::operator>>(std::string &x)
{
    x = "";

    while (data[cur] != '_') {
        if (data[cur] == '\\') {
            cur++;
            if (data[cur] == '\\') {
                x.append("\\");
                cur++;
            } else if (data[cur] == '_') {
                x.append("_");
                cur++;
            } else if (data[cur] == 'n') {
                x.append("\n");
                cur++;
            } else {
                int c;
                sscanf(&data[cur], "%2X", &c);
                x += (char)c;
                cur += 2;
            }
        } else {
            x += data[cur++];
        }
    }
    cur++;
    return *this;
}

Packet &Packet::operator<<(int i)
{
    int len = sprintf(data + size, "%d_", i);
    size += len;
    return *this;
}

Packet &Packet::operator>>(int &i)
{
    sscanf(data + cur, "%d_", &i);

    int len = 0;
    while (data[cur + len] != '_')
        len++;

    cur += len + 1;
    return *this;
}

Packet &Packet::operator<<(uint32 i)
{
    unsigned long x = i;
    int len = sprintf(data + size, "%08lX_", x);
    size += len;
    return *this;
}

Packet &Packet::operator>>(uint32 &i)
{
    unsigned long x;
    sscanf(data + cur, "%lX_", &x);
    i = (uint32)x;

    int len = 0;
    while (data[cur + len] != '_')
        len++;

    cur += len + 1;
    return *this;
}

BQ::BQ(int sz) {
}

BQ::~BQ() {
}

void BQ::put(const std::string &str)
{
    bq.push_back(str);
}

int BQ::get(std::string &str)
{
    if (bq.empty()) return 0;

    str = bq.front();
    bq.pop_front();
    return 1;
}
