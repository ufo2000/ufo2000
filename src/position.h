#ifndef POSITION_H
#define POSITION_H

#include "global.h" 
 
 /**
  * Position class for storing level, row, and column of map positions.
  */ 
class Position
{
private:
    int32 m_pos;

public:
    Position() {m_pos = -1;}
    Position( int32 pos) {m_pos = pos;}    
    Position(const Position& pos) {m_pos = pos.m_pos;}        
    Position(int lev, int col, int row) {m_pos = (lev << 16) | (col << 8) | row;}
    int level() {return m_pos >> 16;}
    int column() {return (m_pos >> 8) & 255;}
    int row() {return m_pos & 255;}
    int Position::index();
    int32 position() {return m_pos;}
    Position operator= (const Position pos){ m_pos = pos.m_pos; return pos; }
    bool operator== (const Position& pos){ return m_pos == pos.m_pos; }
    bool operator!= (const Position& pos){ return m_pos != pos.m_pos; }
};
#endif
