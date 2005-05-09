
#include "stdafx.h"

#include "global.h"
#include "position.h"
#include "map.h"

/** returns the index of a cell in the map **/
int Position::index()
{
    return level() * map->width * 10 * map->height * 10 + column() * map->height * 10 + row();
}
