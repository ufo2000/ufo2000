/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2003  ufo2000 development team

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

#include "global.h"

#include "video.h"
#include "soldier.h"
#include "sound.h"
#include "map.h"
#include "multiplay.h"
#include "wind.h"
#include "explo.h"
#include "config.h"
#include "icon.h"
#include "colors.h"
#include "text.h"
#include "mouse.h"
#include "random.h"
#include "stats.h"

//uncomment to view some formulas results (reaction fire)
#define SHOW_DEBUG_INFO

//                  dirs   0  1  2   3   4   5  6  7
int Soldier::dir2ofs[8] = {1, 1, 0, -1, -1, -1, 0, 1};
//                    y  x     -1  0  1
char Soldier::ofs2dir[3][3] = {{5, 6, 7},
                               {4, 8, 0},
                               {3, 2, 1}};

IMPLEMENT_PERSISTENCE(Soldier, "Soldier");

void Soldier::initpck()
{
    Skin::initpck();
}

Soldier::Soldier(Platoon *platoon, int _NID)
{
    NID = _NID;
    z = -1; x = -1; y = -1;
    dir = 0; move_dir = 0; phase = 0; m_state = STAND;
    m_next = NULL; m_prev = NULL;
    m_stunned_body = NULL;
    m_platoon = platoon;
    m_skin = new Skin(this, md.SkinType, md.fFemale);

    m_bullet = new Bullet(this);
    m_place[P_SHL_RIGHT] = new Place( 16,  40,  2, 1);
    m_place[P_SHL_LEFT]  = new Place(112,  40,  2, 1);
    m_place[P_ARM_RIGHT] = new Place(  0,  64,  2, 3);
    m_place[P_ARM_LEFT]  = new Place(128,  64,  2, 3);
    m_place[P_LEG_RIGHT] = new Place(  0, 120,  2, 1);
    m_place[P_LEG_LEFT]  = new Place(128, 120,  2, 1);
    m_place[P_BACK]      = new Place(192,  40,  3, 3);
    m_place[P_BELT]      = new Place(192, 104,  4, 2);
    m_place[P_MAP]       = new Place(  0, 152, 20 * 10, 3);      //as map
    m_p_map_allocated = true;

    curway = -1; waylen = 0;
    FIRE_num = 0;
    MOVED = 0;
    m_reaction_chances = 0;

    m_ReserveTimeMode = RESERVE_FREE;

    memset(&md, 0, sizeof(md));
    memset(&id, 0, sizeof(id));
    memset(&ud, 0, sizeof(ud));
}

Soldier::Soldier(Platoon *platoon, int _NID, int _z, int _x, int _y, MANDATA *mdat, ITEMDATA *idat, DeployType dep_type,int32 vision_mask)
{
    NID = _NID; z = _z; x = _x; y = _y;
    dir = 0; move_dir = 0;
    m_vision_mask = vision_mask;
    
    //  Face the enemy depending on what the deployment is
    int ang, dest_col, dest_row;
    fixed ox, oy;
    switch (dep_type) {
        case DEP_LEFT:
            //  Face right on the minimap
            dir = 0;
            break;
        case DEP_RIGHT:
            //  Face left on the minimap
            dir = 4;
            break;
        case DEP_CENTER:
            //  Face from the center of the map
            dest_col = map->width * 10 - 1;
            dest_row = map->height * 10 - 1;
            ox = itofix(dest_col - x * 2);
            oy = itofix(dest_row - y * 2);
            if ((!ox) && (!oy)) {
                dir = 0;
            } else {
                ang = fixtoi(fatan2(oy, ox));
                if (ang < 0) ang = 256 + ang;
                ang = (ang + 16) % 256;
                dir = ang >> 5;
            }
            dir = DIR_REVERSE (dir);
            break;
        case DEP_SURROUND:
            //  Face to the center of the map
            dest_col = map->width * 10 - 1;
            dest_row = map->height * 10 - 1;
            ox = itofix(dest_col - x * 2);
            oy = itofix(dest_row - y * 2);
            if ((!ox) && (!oy)) {
                dir = 0;
            } else {
                ang = fixtoi(fatan2(oy, ox));
                if (ang < 0) ang = 256 + ang;
                ang = (ang + 16) % 256;
                dir = ang >> 5;
            }
            break;
    }

    phase = 0; m_state = STAND;
    m_next = NULL; m_prev = NULL;
    m_stunned_body = NULL;
    m_platoon = platoon;
    m_skin = new Skin(this, md.SkinType, md.fFemale);

    m_bullet = new Bullet(this);
    m_place[P_SHL_RIGHT] = new Place( 16,  40, 2, 1);
    m_place[P_SHL_LEFT]  = new Place(112,  40, 2, 1);
    m_place[P_ARM_RIGHT] = new Place(  0,  64, 2, 3);
    m_place[P_ARM_LEFT]  = new Place(128,  64, 2, 3);
    m_place[P_LEG_RIGHT] = new Place(  0, 120, 2, 1);
    m_place[P_LEG_LEFT]  = new Place(128, 120, 2, 1);
    m_place[P_BACK]      = new Place(192,  40, 3, 3);
    m_place[P_BELT]      = new Place(192, 104, 4, 2);
//    m_place[P_MAP] = map->place(z, x, y);      //!!
    m_p_map_allocated = false;

    g_map->set_man(z, x, y, this);

    curway = -1; waylen = 0;
    FIRE_num = 0;
    MOVED = 0;
    m_reaction_chances = 0;
    panicking = false;

    m_ReserveTimeMode = RESERVE_FREE;
    memcpy(&md, mdat, sizeof(md));
    memcpy(&id, idat, sizeof(id));

    initialize();
}


Soldier::~Soldier()
{
    delete m_bullet;
    m_bullet = NULL;
    for (int i = 0; i < NUMBER_OF_CARRIED_PLACES; i++) {
        delete m_place[i];
        m_place[i] = NULL;
    }
    if (m_p_map_allocated) delete m_place[P_MAP];
    m_place[P_MAP] = NULL;

    //closegame() and kill
    if (map != NULL && x != -1 && y != -1 && z != -1)
        map->set_man(z, x, y, NULL);
}

void Soldier::initialize()
{
    process_MANDATA();
    process_ITEMDATA();

    ud.CurTU = ud.MaxTU;
    ud.CurHealth = ud.MaxHealth;
    ud.CurEnergy = ud.MaxEnergy;
    ud.CurReactions = md.Reactions;
    ud.CurStrength = ud.MaxStrength;
    ud.Morale = 100;
    ud.CurFAccuracy = ud.MaxFA;
    ud.CurTAccuracy = ud.MaxTA;

    ud.CurFront = ud.MaxFront;
    ud.CurLeft = ud.MaxLeft;
    ud.CurRight = ud.MaxRight;
    ud.CurRear = ud.MaxRear;
    ud.CurUnder = ud.MaxUnder;
}

void Soldier::process_MANDATA()
{
    m_skin->update(md.SkinType, md.fFemale);

    memset(&ud, 0, sizeof(ud));

    if (md.TimeUnits < 50) md.TimeUnits = 50;
    if (md.TimeUnits > 80) md.TimeUnits = 80;
    if (md.Health < 50) md.Health = 50;
    if (md.Health > 80) md.Health = 80;
    if (md.Firing < 50) md.Firing = 50;
    if (md.Firing > 80) md.Firing = 80;
    if (md.Throwing < 50) md.Throwing = 50;
    if (md.Throwing > 80) md.Throwing = 80;
    if (md.Stamina < 50) md.Stamina = 50;
    if (md.Stamina > 80) md.Stamina = 80;
    if (md.Strength < 25) md.Strength = 25;
    if (md.Strength > 40) md.Strength = 40;
    if (md.Reactions < 50) md.Reactions = 50;
    if (md.Reactions > 80) md.Reactions = 80;

    if (md.TimeUnits + md.Health + md.Firing + md.Throwing + md.Stamina + (md.Strength * 2) + md.Reactions > MAXPOINTS) {
        md.TimeUnits = 50;
        md.Health    = 50;
        md.Firing    = 50;
        md.Throwing  = 50;
        md.Stamina   = 50;
        md.Reactions = 50;
        md.Strength  = 25;
    }

    char *p = strchr(md.Name, '_');
    while (p) {
        *p = ' ';
        p = strchr(md.Name, '_');
    }

    strcpy(ud.Name, md.Name);
    ud.MaxTU = md.TimeUnits;
    ud.MaxHealth = md.Health;
    ud.MaxStrength = md.Strength;
    ud.MaxEnergy = md.Stamina;

    md.SkinType = m_skin->get_type();
    md.fFemale  = m_skin->get_fFemale();

    ud.MaxFA = md.Firing;
    ud.MaxTA = md.Throwing;
    ud.Morale = 100;
    ud.HeadWound = 0;
    ud.TorsoWound = 0;
    ud.RArmWound = 0;
    ud.LArmWound = 0;
    ud.RLegWound = 0;
    ud.LLegWound = 0;

    // Set the armour values based on the skin.
    ud.MaxFront = m_skin->get_armour_value(0);
    ud.MaxLeft = m_skin->get_armour_value(1);
    ud.MaxRight = m_skin->get_armour_value(2);
    ud.MaxRear = m_skin->get_armour_value(3);
    ud.MaxUnder = m_skin->get_armour_value(4);

    ud.CurTU = ud.MaxTU;
    ud.CurHealth = ud.MaxHealth;
    ud.CurEnergy = ud.MaxEnergy;
    ud.CurReactions = md.Reactions;
    ud.CurStrength = ud.MaxStrength;
    ud.Morale = 100;
    ud.CurFAccuracy = ud.MaxFA;
    ud.CurTAccuracy = ud.MaxTA;

    ud.CurFront = ud.MaxFront;
    ud.CurLeft = ud.MaxLeft;
    ud.CurRight = ud.MaxRight;
    ud.CurRear = ud.MaxRear;
    ud.CurUnder = ud.MaxUnder;
}

void Soldier::process_ITEMDATA()
{
    destroy_all_items();

    Item * it = NULL;
    for (int i = 0; i < id.num; i++) {
        if (id.place[i] == 0xFF) //clip
        {
            ASSERT(it != NULL);
            Item *clip = new Item(intel_uint32(id.item_type[i]));
            int v = it->loadclip(clip);
            ASSERT(v);
        } else {
            Place *pp = place(id.place[i]);
            ASSERT(pp != NULL);
            it = new Item(intel_uint32(id.item_type[i]));
            int v = pp->put(it, id.x[i], id.y[i]);
            ASSERT(v);
        }
    }
}

void Soldier::build_ITEMDATA()
{
    memset(&id, 0, sizeof(id));

    for (int i = 0; i < NUMBER_OF_PLACES; i++) {
        m_place[i]->build_ITEMDATA(i, &id);
    }
}

static char *place_name_id[11] = {
    "RIGHT SHOULDER",
    "LEFT SHOULDER",
    "RIGHT HAND",
    "LEFT HAND",
    "RIGHT LEG",
    "LEFT LEG",
    "BACK PACK",
    "BELT",
    "GROUND",
    "ARMOURY",
    "COMMON POOL"
};

void Soldier::reset_stats()
{
    md.TimeUnits = 0;
    md.Health = 0;
    md.Stamina = 0;
    md.Reactions = 0;
    md.Strength = 0;
    md.Firing = 0;
    md.Throwing = 0;
    
    for (int i = 0; i < NUMBER_OF_PLACES; i++) {
        m_place[i]->destroy_all_items();
    }
}

/**
 * Set unit stat attribute
 *
 * @todo better and more flexible attributes list handling needed
 */
bool Soldier::set_attribute(const char *attribute_name, int value)
{
    if (strcmp(attribute_name, "TimeUnits") == 0) {
        md.TimeUnits = value;
    } else if (strcmp(attribute_name, "Health") == 0) {
        md.Health = value;
    } else if (strcmp(attribute_name, "Stamina") == 0) {
        md.Stamina = value;
    } else if (strcmp(attribute_name, "Reactions") == 0) {
        md.Reactions = value;
    } else if (strcmp(attribute_name, "Strength") == 0) {
        md.Strength = value;
    } else if (strcmp(attribute_name, "Firing") == 0) {
        md.Firing = value;
    } else if (strcmp(attribute_name, "Throwing") == 0) {
        md.Throwing = value;
    } else {
        return false;
    }
    process_MANDATA();
    return true;
}

bool Soldier::set_name(const char *newname)
{
    if (strlen(newname) + 1 > sizeof(ud.Name)) return false;
    strcpy(ud.Name, newname);
    strcpy(md.Name, newname);
    return true;
}

bool Soldier::set_skin_info(int skin_type, int female, int appearance)
{
    md.SkinType = skin_type;
    md.fFemale = female;
    md.Appearance = appearance;
    process_MANDATA();
    return true;
}

Place *Soldier::find_place(const char *place_name)
{
    for (int i = 0; i < (int)(sizeof(place_name_id) / sizeof(place_name_id[0])); i++)
        if (strcmp(place_name_id[i], place_name) == 0)
            return m_place[i];
    return NULL;
}

void Soldier::save_to_string(std::string &str)
{
    str.clear();
    
    std::string inv;
    
    for (int i = 0; i < NUMBER_OF_PLACES; i++) {
        inv += "[\"" + lua_escape_string(place_name_id[i]) + "\"] = {\n";
        std::string place_str;
        m_place[i]->save_to_string(place_str);
        inv += indent(place_str);
        inv += "},\n";
    }
    
    char tmp[512];
    str += "Name = \"" + lua_escape_string(md.Name) + "\",\n";

    sprintf(tmp, "SkinType = %d,\n", md.SkinType); str += tmp;
    sprintf(tmp, "fFemale = %d,\n", md.fFemale); str += tmp;
    sprintf(tmp, "Appearance = %d,\n", md.Appearance); str += tmp;

    str += "Attributes = {\n";
    sprintf(tmp, "\tTimeUnits = %d,\n", md.TimeUnits); str += tmp;
    sprintf(tmp, "\tHealth = %d,\n", md.Health); str += tmp;
    sprintf(tmp, "\tStamina = %d,\n", md.Stamina); str += tmp;
    sprintf(tmp, "\tReactions = %d,\n", md.Reactions); str += tmp;
    sprintf(tmp, "\tStrength = %d,\n", md.Strength); str += tmp;
    sprintf(tmp, "\tFiring = %d,\n", md.Firing); str += tmp;
    sprintf(tmp, "\tThrowing = %d,\n", md.Throwing); str += tmp;
    str += "},\n";

    str += "Inventory = {\n";
    str += indent(inv);
    str += "},\n";
}

void Soldier::destroy_all_items()
{
    for (int i = 0; i < NUMBER_OF_PLACES; i++) {
        m_place[i]->destroy_all_items();
    }
}

/**
 * Calculate point-cost of soldiers attributes and armor
 */ 
int Soldier::calc_mandata_cost(MANDATA _md)
{
    // TUs with stamina as they determine how far you go in a turn.
    // TUs twice as important because they get used up twice as fast.
    // Reactions is doubled because it means doing damage in enemy's turn!
    // Strength is two for one points wise, so double the value that's in there.
    // Accuracy averaged into one value.
    int p = (((_md.TimeUnits * 2) + _md.Stamina) / 2) +
            _md.Health +
            (_md.Reactions * 2) +
            ((_md.Firing + _md.Throwing) / 2) +
            (_md.Strength * 2);
            
    p += Skin::get_armour_cost(_md.SkinType, _md.fFemale);

    return p;
}

int Soldier::calc_full_ammunition_cost()
{
    int p = calc_mandata_cost(md);
    
    std::vector<Item *> items;
    get_inventory_list(items);
    
    for (int i = 0; i < (int)items.size(); i++) {
        p += items[i]->get_cost();
    }
    return p;
}

void Soldier::build_items_stats(char *buf, int &len)
{
    for (int i = 0; i < NUMBER_OF_PLACES; i++) {
        m_place[i]->build_items_stats(buf, len);
    }
}

   

/**
 * Get list of pointer to all the items owned by this soldier
 */
int Soldier::get_inventory_list(std::vector<Item *> &items)
{
    items.clear();
    for (int i = 0; i < NUMBER_OF_PLACES; i++) {
        m_place[i]->get_items_list(items);
    }
    return items.size();
}

/**
 * Restore minimal amount of parameters - even for Watch mode
 */
void Soldier::restore_moved()
{
    MOVED = 0;
}

/**
 * Restore full amount of parameters - at the beginning of the turn
 */
void Soldier::restore()
{
    restore_moved();
    // Percent of TUs: the lesser of 100% or ((strength / weight) * 100%).
    ud.CurTU = (count_weight() > ud.CurStrength) ? (ud.MaxTU * ud.CurStrength / count_weight()) : ud.MaxTU;
    //ud.CurHealth = ud.MaxHealth;
    //ud.CurStrength = ud.MaxStrength;
    // Regain 33% of energy per turn. Should this be 43%?
    ud.CurEnergy = ((ud.CurEnergy + (ud.MaxEnergy / 3)) >= ud.MaxEnergy) ? ud.MaxEnergy : ud.CurEnergy + (ud.MaxEnergy / 3);
    //ud.Morale = 100;
    //ud.CurFAccuracy = ud.MaxFA;
    //ud.CurTAccuracy = ud.MaxTA;

    panicking = false;

    if (ud.CurStun > 0) // Do we have stun damage?
    {
        int i = 5; //(int)randval(1, 11);

        // Are we currently stunned?
        bool was_stunned = is_stunned();
        unsigned char was_stun = ud.CurStun;

        // Reduce stun damage
        if (ud.CurStun > i)
            ud.CurStun -= i;
        else
            ud.CurStun = 0;

        // This means we should wake up, so find the body!
        if (was_stunned && !is_stunned()) {
            int z0 = -1, x0 = -1, y0 = -1, found = 0;
            Place *target = m_stunned_body->get_place();
            for (z0 = 0; z0 < map->level; z0++) {
                for (x0 = 0; x0 < map->width*10; x0++) {
                    for (y0 = 0; y0 < map->height*10; y0++) {
                        if (target == map->place(z0, x0, y0))
                        {
                            found = 1;
                            break;
                        }
                        if (found)
                            break;
                    }
                    if (found)
                        break;
                }
                if (found)
                    break;
            }
            // Body found on map (not in someone's backback) 
            // and also nobody is standing at this place.
            if (found && g_map->man(z0, x0, y0) == NULL) {
                z = z0;
                x = x0;
                y = y0;
                target->destroy(m_stunned_body);
                m_stunned_body = NULL;
                g_map->set_man(z0, x0, y0, this); // Get back into action.
//                g_map->update_vision_matrix(this);
                m_state = STAND;
                phase = 0;
            } else ud.CurStun = was_stun; // Otherwise don't wake up yet.
        }
    }
}

/**
 * Calculate actual height at which soldier resides on his (z, x, y) tile
 */
int Soldier::calc_z()
{
    int res = 0;
    if ((z > 0) && map->mcd(z, x, y, 0)->No_Floor && map->isStairs(z - 1, x, y)) {
        res += CELL_SCR_Z + map->mcd(z - 1, x, y, 3)->T_Level;
    } else {
        res += map->mcd(z, x, y, 0)->T_Level;
        res += map->mcd(z, x, y, 3)->T_Level;
    }
    return res;
}

/**
 * Draw soldiers inventory
 */
// See also: Inventory::draw(), Editor::show()
void Soldier::draw_inventory(BITMAP *dest)
{
    showspk(dest);

    text_mode(-1);
    textout(dest, large, md.Name,  0,  0, COLOR_LT_OLIVE);

// New: display weight and health:
    int wht     = count_weight();
    int max_wht = md.Strength;
    int color   = max_wht < wht ? COLOR_RED03 : COLOR_GRAY02; 
    textprintf(dest, g_small_font, 0, 20, color, _("Equipment weight: %d/%2d"), wht, max_wht);

    color = COLOR_ORANGE;
    if (ud.CurTU < 20)              // $$$ Todo: use required(25) and havetime()
        color = COLOR_WHITE1;   // no time left for snapshot or grenade-throw
    if (ud.CurEnergy < 7)
        color = COLOR_LT_BLUE;  // low energy - only 3 steps remain
    if (ud.CurEnergy < 2)
        color = COLOR_BLUE;     // not enough energy for a single step
    textout(dest,    g_small_font, _("TUs>"), 245, 25, COLOR_LT_OLIVE);
    textprintf(dest, g_small_font,            276, 25, color, "%d", ud.CurTU);

    color = COLOR_ORANGE;
    if ((ud.CurFront < ud.MaxFront) ||  // Armor damaged
        (ud.CurLeft  < ud.MaxLeft)  ||
        (ud.CurRight < ud.MaxRight) ||
        (ud.CurRear  < ud.MaxRear)  ||
        (ud.CurUnder < ud.MaxUnder)) 
        color = COLOR_GRAY03; 
    if ((ud.CurFront == 0) ||           // one side of armor is gone
        (ud.CurLeft  == 0) ||
        (ud.CurRight == 0) ||
        (ud.CurRear  == 0) ||
        (ud.CurUnder == 0))
        color = COLOR_WHITE; 
    if (ud.CurHealth < ud.MaxHealth)    // Soldier wounded
        color = COLOR_RED03; 
    int fw = ud.HeadWound + ud.TorsoWound + ud.RArmWound +
             ud.LArmWound + ud.RLegWound  + ud.LLegWound;   
    if (fw > 0) 
        color = COLOR_RED07;        // Fatal Wounds
    textout(dest,    g_small_font, _("Health>"), 245, 34, COLOR_LT_OLIVE);
    textprintf(dest, g_small_font,               276, 34, color, "%d", ud.CurHealth); 
//
    for (int i = 0; i < NUMBER_OF_CARRIED_PLACES; i++)
        m_place[i]->drawgrid(dest, i);

    map->place(z, x, y)->drawgrid(dest, P_MAP);
}

inline int graycol(int c)
{
    return makecol(c, c, c);
}

/**
 * Display soldiers attributes, with numbers and barcharts 
 */
void Soldier::draw_unibord(int abs_pos, int posx, int posy)
{
    int fw = ud.HeadWound + ud.TorsoWound + ud.RArmWound +
             ud.LArmWound + ud.RLegWound  + ud.LLegWound;   // Fatal Wounds

    const int row_num = 17, col_num = 9;
    FONT *name_f = large, *row_f = g_small_font;

    struct {
        char *str;
        int cur;
        int max;
        int col;
    }
    param[row_num] = {
        { (char*)_("TIME UNITS"),        ud.CurTU,        ud.MaxTU,      68},
        { (char*)_("ENERGY"),            ud.CurEnergy,    ud.MaxEnergy, 148},
        { (char*)_("HEALTH"),            ud.CurHealth,    ud.MaxHealth,  36},
        { (char*)_("FATAL WOUNDS"),      fw,              fw,            36},
        { (char*)_("BRAVERY"),           md.Bravery,      md.Bravery,   196},
        { (char*)_("MORALE"),            ud.Morale,       100,          197},
        { (char*)_("REACTIONS"),         ud.CurReactions, md.Reactions,  20},
        { (char*)_("FIRING ACCURACY"),   eff_FAccuracy(), ud.MaxFA,     132},
        { (char*)_("THROWING ACCURACY"), TAccuracy(100),  ud.MaxTA,     100},
        { (char*)_("STRENGTH"),          ud.MaxStrength,  md.Strength,   52},
        {NULL, 0, 0, 0},
        {NULL, 0, 0, 0},
        { (char*)_("FRONT ARMOUR"),      ud.CurFront,     ud.MaxFront,   87},
        { (char*)_("LEFT ARMOUR"),       ud.CurLeft,      ud.MaxLeft,    87},
        { (char*)_("RIGHT ARMOUR"),      ud.CurRight,     ud.MaxRight,   87},
        { (char*)_("REAR ARMOUR"),       ud.CurRear,      ud.MaxRear,    87},
        { (char*)_("UNDER ARMOUR"),      ud.CurUnder,     ud.MaxUnder,   87}
    };
    
    int width = 320;
    
    int name_h = text_height(name_f) + 6, row_h = text_height(row_f) + 3;
    int height = name_h + 8 + row_h * row_num;
    
    BITMAP *temp = create_bitmap(width, height);
    clear_to_color(temp, makecol(255, 255, 255));

    int c = 128;
    rect(temp, 0, 0, width - 1, height - 1, graycol(c));
    for (int i = 1; i < width - 1; i++) {
        for (int j = 1; j < name_h - 1; j++) {
            c = 64 + (i * 64 / (width - 2)) + (j * 64 / (name_h - 2));
            putpixel(temp, i, j, graycol(c));
        }
    }
    textout_centre(temp, name_f, md.Name, width / 2, 4, COLOR_GREEN04);

    int sx[col_num + 1] = {1, 150, 180, 200, 220, 240, 260, 280, 300, width - 1};
    for (int i = 2; i < col_num; i++)
        printsmall_center_x(temp, sx[i] + 1, name_h, COLOR_GREEN04, (i - 2) * 20);

    int s = 0;
    line(temp, 1, name_h + 6, width - 2, name_h + 6, graycol(192));
    for (int i = 0; i < 17; i++) {
        for (int j = 0; j < col_num; j++)
            rectfill(temp, sx[j], name_h + 6 + row_h * i + 1, sx[j + 1] - 1, name_h + 6 + row_h * (i + 1), graycol(255 - (16 * (s++ % 2))));
            
        if (param[i].str != NULL) {
            textout_right(temp, row_f, param[i].str, sx[1] - 2, name_h + 6 + row_h * i + 3, COLOR_GREEN00);
            textprintf_centre(temp, font, 165, name_h + 6 + row_h * i + 3, COLOR_RED02, "%d", param[i].cur);

            rect(temp, sx[2], name_h + 6 + row_h * i + 3, sx[2] + param[i].max, name_h + 6 + row_h * i + 10, xcom1_color(param[i].col));
            if (param[i].max)
                line(temp, sx[2], name_h + 6 + row_h * i + 4, sx[2], name_h + 6 + row_h * i + 9, xcom1_color(param[i].col - 4));
            if (param[i].cur)
                rectfill(temp, sx[2], name_h + 6 + row_h * i + 4, sx[2] + param[i].cur - 1, name_h + 6 + row_h * i + 9, xcom1_color(param[i].col - 4));

            // special case for the health bar
            if (i == 2) // draw stun damage
                if (ud.CurStun > 0)
                {
                    if (ud.CurStun < ud.CurHealth)
                        rectfill(temp, sx[2], name_h + 6 + row_h * i + 4, sx[2] + ud.CurStun - 1, name_h + 6 + row_h * i + 9, COLOR_WHITE1);
                    else
                        rectfill(temp, 170, name_h + 6 + row_h * i + 4, sx[2] + ud.CurHealth - 1, name_h + 6 + row_h * i + 9, COLOR_WHITE1);
                }
        }
    }
    
    set_trans_blender(0, 0, 0, 192);
    if (abs_pos)
        draw_trans_sprite(screen2, temp, posx, posy);
    else
        draw_trans_sprite(screen2, temp, (posx - width) / 2, (posy - height) / 2);
    destroy_bitmap(temp);
}

/**
 * Draw yellow arrow above selected friendly soldier
 *
 * @param select_y vertical offset, to make the arrow move up & down
 */
void Soldier::draw_selector(int select_y)
{
    if (ismoving()) return;

    int num = 0;
    num ^= (m_state == SIT);
    num ^= (map->place(z, x, y)->item() != NULL) * 2;
    
    int sx = map->x + CELL_SCR_X * x + CELL_SCR_X * y + 16 - selector[num]->w / 2;
    int sy = map->y - (x + 1) * CELL_SCR_Y + CELL_SCR_Y * y - CELL_SCR_Z * z - 20 - selector[num]->h;

    draw_alpha_sprite(screen2, selector[num], sx, sy - (select_y / 2) + calc_z());
}

void Soldier::turnto(int destdir)
{
    int a = destdir - dir;
    if (a != 0) {
        if (a > 0) {
            if (a <= 4) {
                dir++;
            } else {
                dir--;
            }
        } else {
            if (a > -4) {
                dir--;
            } else {
                dir++;
            }
        }
        if (dir < 0) dir = 7;
        if (dir > 7) dir = 0;
    }
}

int Soldier::ismoving()
{
    if ((m_state == MARCH) || (m_state == FALL) || (!m_bullet->ready()))
        return true;

    if ((z != -1) && ((FIRE_num > 0) || (m_reaction_chances > 0) || (curway != -1) || (waylen != 0)))
            return true;
        
    return false;
}

int Soldier::move(int ISLOCAL)
{
    if (m_stunned_body != NULL || z == -1) return 0; // auto-return 0 if stunned

    if ((z > 0) && map->mcd(z, x, y, 0)->No_Floor && !can_fly()) {
        if (!map->isStairs(z - 1, x, y)) {
            map->set_man(z, x, y, NULL);
            z--;
            map->set_man(z, x, y, this);
            if (this == sel_man) {
                //map->center(this);
                map->sel_lev = z;
            }
        }
    }
    if (map->isStairs(z, x, y)) {
        map->set_man(z, x, y, NULL);
        z++;
        map->set_man(z, x, y, this);
        //map->sel_lev = z;
        if (this == sel_man) {
            //map->center(this);
            map->sel_lev = z;
        }
    }

//    map->set_man(z, x, y, this);
//    m_place[P_MAP] = map->place(z, x, y);

    if (m_state == FALL) {
        //textprintf(screen, font, 1, 100, 1, "phase=%d", phase);
        if (phase < 2 * 3) {
            phase++;
        } else {
            phase = 0;
            m_state = LIE;
            return 0;
        }
    }

    // Vertical movements
    if (m_state == MARCH) {
        if(move_dir > DIR_NULL) {
            int tu_cost;
            int zd, xd, yd;
            map->step_dest(z, x, y, move_dir, can_fly(), zd, xd, yd, tu_cost, panicking);
            if (time_reserve(tu_cost, ISLOCAL) != OK)
                finish_march(ISLOCAL);
            else
            {
                map->set_man(z, x, y, NULL);
                z = zd;
                x = xd;
                y = yd;
                map->set_man(z, x, y, this);
                spend_time(tu_cost, 1);
//                m_place[P_MAP] = map->place(z, x, y);

                if (this == sel_man) {
                    //map->center(this);
                    map->sel_lev = z;
                }

//                map->update_vision_matrix(this);
                
                curway++;
                if (curway >= waylen) {
                    move_dir = dir;
                    finish_march(ISLOCAL);
                } else {
                    if (time_reserve(walktime(way[curway]), ISLOCAL) != OK)
                        finish_march(ISLOCAL);
                    else
                        move_dir = way[curway];
                        if( way[curway] < 8)
                            dir = way[curway];
                }
            }
            return 1;
        }


        if ((phase == 3 || phase == 7) && platoon_local->is_visible(z, x, y) && !is_flying()) {
            // Make some step sounds (twice per movement from one cell to another)
            switch (md.SkinType) {
                case S_SECTOID:
                    soundSystem::getInstance()->play(SS_STEP_SECTOID, 128);
                    break;
                case S_MUTON:
                    soundSystem::getInstance()->play(SS_STEP_MUTON, 128);
                    break;
                default:
                    soundSystem::getInstance()->play(SS_STEP_HUMAN, 128);
                    break;
            }
        }
        phase++;

        if (phase == 4) {
            // We are exactly in the middle between map cells
            map->set_man(z, x, y, NULL);
            x += DIR_DELTA_X(dir);
            y += DIR_DELTA_Y(dir);
            
            // If we're moving along a diagonal, use 1.5 times the cost, as 
            // in the original game itself. Please note that walktime(-1) 
            // returns the time of a horizontal move, whereas walktime(dir)
            // factors in the diagonal move multiplier.
            int time = walktime(-1);
            if (DIR_DIAGONAL(dir)) time = time * 3 / 2;
            if (panicking) time = time * 3 / 4;
            spend_time(time, 1);
            
            map->set_man(z, x, y, this);
//            m_place[P_MAP] = map->place(z, x, y);

//            map->update_vision_matrix(this);
        }

        if (phase >= 8) {
            // We have just come to another map cell
            phase = 0;
            
            // Check for proximity grenades
            if (map->check_mine(z, x, y)) {
                finish_march(ISLOCAL);
                if (!is_active()) return 0;
            }

            // Check for reaction fire.
            if (ISLOCAL) {
                platoon_remote->soldier_moved(this);
                if (platoon_remote->check_reaction_fire(this)) {
                    // In other words, if more than 0 shots were fired.
                    finish_march(ISLOCAL);
                }
            } else {
                platoon_local->soldier_moved(this);
            }
            
            if (m_state == MARCH) {
                // We haven't stopped because of proximity mines and reaction 
                // fire, so continue marching until we reach final destination
                curway++;
                if (curway >= waylen) {
                    finish_march(ISLOCAL);
                } else {
                    if (time_reserve(walktime(way[curway]), ISLOCAL) != OK)
                        finish_march(ISLOCAL);
                    else {
                        move_dir = way[curway];
                        if( way[curway] < 8)
                            dir = way[curway];
                    }
                }
            }
        }
    }

    if ((m_state == STAND) || (m_state == SIT)) {
        // If we performed some action before, lets give enemy a chance for reaction fire
        int reaction_chances = m_reaction_chances;
        m_reaction_chances = 0;
        if (!ISLOCAL) reaction_chances = 0;

        while (reaction_chances > 0 && nomoves()) {
            reaction_chances--;
            if (platoon_remote->check_reaction_fire(this))
                break;
        }

        m_reaction_chances = reaction_chances;

        if (curway != -1) {
            move_dir = way[curway];
            if (dir == way[curway] || way[curway] >= DIR_NULL) {
                if (waylen == 0) {
                    curway = -1;
                } else {
                    m_state = MARCH;
                    phase = 0;
                }
            } else {
                turnto(way[curway]);
            }

            g_map->update_vision_matrix(this);
        } else {
            if (FIRE_num && m_bullet->ready()) {
                FIRE_num--;
                // The ISLOCAL bit is for reaction fire...
                shoot(FIRE_z, FIRE_x, FIRE_y, ISLOCAL);
            }
        }
    }
    return 1;
}

int Soldier::tus_reserved(std::string *error)
{
    int time = 0;

    Item *it = rhand_item();
    if(!it) it = lhand_item();  //no item in right hand - use item in left hand
    
    if(it) {                    //if soldier has got at least one item in hands
        switch(m_ReserveTimeMode) {
            case RESERVE_FREE:
            break;
        
            case RESERVE_SNAP:
            if (it->obdata_accuracy(1)) {
                time = required(it->obdata_time(1));
                if (error) *error = _("Time units are reserved for snap shot.");
            }
            break;
        
            case RESERVE_AIM:
            if (it->obdata_accuracy(2)) {
                time = required(it->obdata_time(2));
                if (error) *error = _("Time units are reserved for aimed shot.");
            }
            break;
        
            case RESERVE_AUTO:
            if (it->obdata_accuracy(0)) {
                time = required(it->obdata_time(0)) * it->obdata_autoShots();
                if (error) *error = _("Time units are reserved for auto shot.");
            }
            break;
        }
        
        if(it->is_grenade() && it->delay_time() > 0 && m_ReserveTimeMode != RESERVE_FREE) {
            time = required(25);
            if (error) *error = _("Time units are reserved for grenade throw.");
        }
    }
    
    return time;
}

/**
 * Test if soldier has reserved time for shooting.
 * Returns true if he has enough time for the next action.
 */
int Soldier::time_reserve(int walk_time, int ISLOCAL, int use_energy)
{
    if(!ISLOCAL)            // during enemy turn: don't check for reserved time
        return havetime(walk_time, use_energy);

    std::string error = "";
    int time = tus_reserved(&error);
    
    if((havetime(walk_time + time, 0) != OK) && (havetime(time, 0) == OK)) {
        if(error != "")
            g_console->printf(COLOR_SYS_INFO1, error.c_str());
        return havetime(walk_time + time, 0);
    } else {
        return havetime(walk_time, use_energy);
    }
}


void Soldier::wayto(int dest_lev, int dest_col, int dest_row)
{
    if ((dest_col != x) || (dest_row != y) || (dest_lev != z)) {


        curway = 1;
        waylen = map->pathfind(z, x, y, dest_lev, dest_col, dest_row, can_fly(), panicking, way);
        if (map->man(dest_lev, dest_col, dest_row) != NULL) {
            waylen--;
        }

        int ISLOCAL = platoon_local->belong(this);
        if ((waylen < 2) || (time_reserve(walktime(way[1]), ISLOCAL) != OK)) {
            curway = -1;
            waylen = 0;
        } else {
            //net->send_move(NID, dest_lev, dest_col, dest_row);
        }
    }
}

/**
 * Move soldier up or down a level.
 * Currently, this only works on gravlifts.
 */
bool Soldier::use_elevator(int dz)
// Todo: Flying armor
{
    // Check map borders and available time units
    int ISLOCAL = platoon_local->belong(this);
    if (z + dz >= map->level || z + dz < 0 || (time_reserve(10, ISLOCAL, false) != OK))
        return false;
    // Check that the soldier is standing on elevator and can use it
    if (!map->mcd(z, x, y, 0)->Gravlift || !map->mcd(z + dz, x, y, 0)->Gravlift)
        return false;
    // Check that nobody blocks the way
    if (map->man(z + dz, x, y))
        return false;
    
    // Spend time units
    spend_time(10);
    m_reaction_chances++;
    
    // Change map location
    map->set_man(z, x, y, NULL);
    z += dz;
    map->set_man(z, x, y, this);
    
    // Sent action to remote player
    if (platoon_local->belong(this))
        net->send_use_elevator(NID, dz);

//    map->update_vision_matrix(this);
    map->cell_visibility_changed(z, x, y);
    return true;
}

void Soldier::finish_march(int ISLOCAL)
{
    if (is_active())
        m_state = STAND;
    curway = -1;
    waylen = 0;
    if (ISLOCAL)
        net->send_move(NID, z, x, y);     //!!!!!!!!!!!!!!!
}


void Soldier::break_march()
{
    //phase = 0;
    //curway = -1;
    waylen = curway - 1;
}


int Soldier::dirto(int src_col, int src_row, int dest_col, int dest_row)
{
    fixed ox = itofix(dest_col - src_col);
    fixed oy = itofix(dest_row - src_row);
    int ang = fixtoi(fatan2(oy, ox));
    if (ang < 0) ang = 256 + ang;
    ang = (ang + 16) % 256;
    return ang >> 5;
}


bool Soldier::faceto(int dest_col, int dest_row)
{
    fixed ox = itofix(dest_col - x);
    fixed oy = itofix(dest_row - y);
    if ((!ox) && (!oy)) return true; // No turning required
    int ang = fixtoi(fatan2(oy, ox));
    if (ang < 0) ang = 256 + ang;
    ang = (ang + 16) % 256;
    //text_mode(-1); textprintf(screen, font, 1, 1, 1, "ang = %d", ang); readkey();

    int nturns = (ang >> 5) - dir;
    if (nturns == 0)
        return true;

    if (nturns < 0) nturns = -nturns;
    if (nturns > 4)
        nturns = 8 - nturns;
    int ISLOCAL = platoon_local->belong(this);
    if (time_reserve(nturns, ISLOCAL, 0) == OK) {
        spend_time(nturns);
        net->send_face(NID, dest_col, dest_row);
        way[0] = ang >> 5; curway = 0; waylen = 0;
        map->update_vision_matrix(this);
        return true;
    } else
        return false;
}

/**
 * Calculate a random value between min and max
 */
static double randval(double min, double max)
{
    double std = (double)rand() / (double)(RAND_MAX - 1);
    return min + std * (max - min);
}

#define HITLOC_HEAD     0
#define HITLOC_TORSO    1
#define HITLOC_LEFTARM  2
#define HITLOC_RIGHTARM 3
#define HITLOC_LEFTLEG  4
#define HITLOC_RIGHTLEG 5

/**
 * Check if a shot goes thru the armor
 */
// I can't believe that pierce is passed by reference here.
// But, how else am I going to do this?
int Soldier::do_armour_check(int &pierce, int damdir)
{
    int hitloc;
    unsigned char *armor;
  
    // It is still a bit broken, but less than before.
    // The problem is that right now there are 8 directions and 4 armor sections
    // Dividing diagonal hits equaly between neighbouring sections was more of
    // a problem than solution.
    // The right way is to use either 4 or 16 directions.
    switch (damdir) {
        case DAMAGEDIR_FRONT:
            armor = &ud.CurFront;
            hitloc = HITLOC_TORSO;
            break;
        case DAMAGEDIR_FRONTLEFT:
            armor = &ud.CurFront;
            hitloc = HITLOC_TORSO;
            break;
        case DAMAGEDIR_LEFT:
            armor = &ud.CurLeft;
            hitloc = HITLOC_LEFTARM;
            break;
        case DAMAGEDIR_REARLEFT:
            armor = &ud.CurLeft;
            hitloc = HITLOC_LEFTLEG;
            break;
        case DAMAGEDIR_REAR:
            armor = &ud.CurRear;
            hitloc = HITLOC_TORSO;
            break;
        case DAMAGEDIR_REARRIGHT:
            armor = &ud.CurRight;
            hitloc = HITLOC_RIGHTLEG;
            break;
        case DAMAGEDIR_RIGHT:
            armor = &ud.CurRight;
            hitloc = HITLOC_RIGHTARM;
            break;
        case DAMAGEDIR_FRONTRIGHT:
            armor = &ud.CurFront;
            hitloc = HITLOC_TORSO;
            break;
        case DAMAGEDIR_UNDER:
        default:
            armor = &ud.CurUnder;
            hitloc = HITLOC_TORSO;
    }
    
    if (*armor >= pierce) {
        *armor -= pierce;
        return -1;
    }

    pierce -= *armor;
    *armor = 0;
  
    return hitloc;
}

void Soldier::apply_wound(int hitloc)
{
    // This SHOULD apply from 1-3 wounds, but random values can't be transmitted at the moment.
    switch(hitloc)
    {
    case HITLOC_HEAD:
        ud.HeadWound++;
        break;
    case HITLOC_TORSO:
    default:
        ud.TorsoWound++;
        break;
    case HITLOC_LEFTARM:
        ud.LArmWound++;
        break;
    case HITLOC_RIGHTARM:
        ud.RArmWound++;
        break;
    case HITLOC_LEFTLEG:
        ud.LLegWound++;
        break;
    case HITLOC_RIGHTLEG:
        ud.RLegWound++;
        break;
    }
    
    change_morale(-10);
    
    return;
}

void Soldier::hit(int sniper, int pierce, int type, int hitdir, int dam_dev)
{
    // TODO look at this closer
    int damagedir = (dir + (hitdir + 4)) % 8; // Becomes DAMAGEDIR_*, except DAMAGEDIR_UNDER...
    int hitloc;

    // Currently just randomizing the damage to be from 0.5 to 1.5 of
    // the table value, NOT 0.0 to 2.0 as it was in X-Com.
    if (dam_dev > 0)
        pierce = (int) cur_random->getUniform(pierce * (1.0 - (dam_dev / 100.0)), pierce * (1.0 + (dam_dev / 100.0)));

    // Give credit to the sniper for inflicting damage if it's not stun damage.
    if (sniper && (type != DT_STUN))
    {
        StatEntry *stat = platoon_local->get_stats()->get_stat_for_SID(sniper);
        if (!stat) stat = platoon_remote->get_stats()->get_stat_for_SID(sniper);
        if (stat) stat->inc_damage_inflicted(pierce);
        
        //morale penalty for friendly fire
        if (m_platoon->findman(sniper)) {
            change_morale(-10);
            m_platoon->findman(sniper)->change_morale(-20);
        }
    }
    
    // Record that we took damage.
    this->m_platoon->get_stats()->get_stat_for_SID(NID)->inc_damage_taken(pierce);

    if ((hitloc = do_armour_check(pierce, damagedir)) == -1) return; // Can't pierce the armour.

    if (type == DT_STUN) {
        ud.CurStun += pierce;
        if (is_stunned()) {
            m_state = FALL;
            phase = 0;
        }
        return;
    }

    if (ud.CurHealth <= pierce) // ud.CurHealth is unsigned
    {
        if (sniper)
        {
            // Credit the sniper for getting a kill
            StatEntry *stat = platoon_local->get_stats()->get_stat_for_SID(sniper);
            if (!stat) stat = platoon_remote->get_stats()->get_stat_for_SID(sniper);
            if (stat) stat->inc_kills();
            
            // Change the morale of enemy squad
            /*if (platoon_local->belong(this))
                platoon_remote->change_morale(10, false);
            else if (platoon_remote->belong(this))
                platoon_local->change_morale(10, false);*/
                
            // Change the morale of the sniper
            /*if (m_platoon->findman(sniper) != NULL)
                m_platoon->findman(sniper)->change_morale(-20);
            else {
                Soldier *snp = platoon_local->findman(sniper);
                if (snp != NULL)
                    snp->change_morale(10);
                else {
                    snp = platoon_remote->findman(sniper);
                    if (snp != NULL)
                        snp->change_morale(10);
                }
            }*/
        }
        // Record that we died
        this->get_platoon()->get_stats()->get_stat_for_SID(NID)->set_dead(1);
        ud.CurHealth = 0;
        if (m_state != FALL)
        {
            switch (md.SkinType)
            {
                case S_SECTOID:
                    soundSystem::getInstance()->play(SS_SECTOID_DEATH);
                    break;
                case S_MUTON:
                    soundSystem::getInstance()->play(SS_MUTON_DEATH);
                    break;
                default:
                    if (md.fFemale == 1)
                        soundSystem::getInstance()->play(SS_FEMALE_DEATH);
                    else
                        soundSystem::getInstance()->play(SS_MALE_DEATH);
            }
            m_state = FALL;
            phase = 0;
        }
    } else {
        apply_wound(hitloc);
        ud.CurHealth -= pierce;
        switch (md.SkinType)
        {
            case S_SECTOID:
                soundSystem::getInstance()->play(SS_SECTOID_WOUND);
                break;
            case S_MUTON:
                soundSystem::getInstance()->play(SS_MUTON_WOUND);
                break;
            default:
                if (md.fFemale == 1)
                    soundSystem::getInstance()->play(SS_FEMALE_WOUND);
                else
                    soundSystem::getInstance()->play(SS_MALE_WOUND);
        }

        if (is_stunned())
        {
            m_state = FALL;
            phase = 0;
        }
    }
}

void Soldier::explo_hit(int sniper, int pierce, int type, int hitdir) //silent
{
    // TODO del this
    // int damagedir = (dir + (hitdir + 4)) % 8; // Becomes DAMAGEDIR_*, except DAMAGEDIR_UNDER...
    // int damagedir = hitdir;
  
    int hitloc;

    // Give credit to the sniper for inflicting damage if it's not stun damage.
    if (sniper && (type != DT_STUN))
    {
        StatEntry *stat = platoon_local->get_stats()->get_stat_for_SID(sniper);
        if (!stat) stat = platoon_remote->get_stats()->get_stat_for_SID(sniper);
    // Todo: to avoid counting the same kill several times 
    // (e.g. when several High-Explosives go off at once),
    // we should check if victim is still alive / only credit remaining damage
        if (stat) stat->inc_damage_inflicted(pierce);
    }
    
    // Record that we took damage.
    this->m_platoon->get_stats()->get_stat_for_SID(NID)->inc_damage_taken(pierce);

    if (type != DT_STUN)
        damage_items(pierce / 3);
    // Items are OUTSIDE the armour, after all; 
    // but in order not to leave a soldier without his items, damage is seriously reduced
  
    // TODO del this
    // If minimal range, hit under armour. Otherwise, hit armour normally.
    //if ((dist < 2) && ((hitloc = do_armour_check(pierce, DAMAGEDIR_UNDER)) == -1)) return;
    //else if ((dist >= 2) && (hitloc = do_armour_check(pierce, damagedir)) == -1) return;
  
    if ((hitloc = do_armour_check(pierce, hitdir)) == -1) return;
    
    if (type == DT_STUN) // Did we get stunned?
    {
        ud.CurStun += pierce;
        if (is_stunned())
        {
            m_state = FALL;
            phase = 0;
        }
        return;
    }

    if (ud.CurHealth <= pierce) // ud.CurHealth is unsigned
    {
        // Credit the sniper for getting a kill
        if (sniper)
        {
            StatEntry *stat = platoon_local->get_stats()->get_stat_for_SID(sniper);
            if (!stat) stat = platoon_remote->get_stats()->get_stat_for_SID(sniper);
            if (stat) stat->inc_kills();
        }
        // Record that we died
        this->get_platoon()->get_stats()->get_stat_for_SID(NID)->set_dead(1);
        ud.CurHealth = 0;
        if (m_state != FALL)
        {
            switch(md.SkinType)
            {
                case S_SECTOID:
                    soundSystem::getInstance()->play(SS_SECTOID_DEATH);
                    break;
                case S_MUTON:
                    soundSystem::getInstance()->play(SS_MUTON_DEATH);
                    break;
                default:
                    if (md.fFemale == 1)
                        soundSystem::getInstance()->play(SS_FEMALE_DEATH);
                    else
                        soundSystem::getInstance()->play(SS_MALE_DEATH);
            }
            m_state = FALL;
            phase = 0;
        }
    }
    else
    {
        apply_wound(hitloc);
        ud.CurHealth -= pierce;

        if (is_stunned())
        {
            m_state = FALL;
            phase = 0;
        }
    }
}


void Soldier::die()
{   
    unlink();

    if (m_stunned_body == NULL) {
        // Ordinary death, drop all the posession and create corpse
        g_map->set_man(z, x, y, NULL);

        z = g_map->find_ground(z, x, y);
        for (int i = 0; i < NUMBER_OF_CARRIED_PLACES; i++)
            m_place[i]->dropall(z, x, y);

        const char *ctype;
        if (md.SkinType == S_XCOM_0)
            ctype = "CORPSE";
        else if (md.SkinType == S_XCOM_1)
            ctype = "CORPSE & ARMOUR";
        else if ((md.SkinType == S_XCOM_2) || (md.SkinType == S_XCOM_3))
            ctype = "CORPSE & POWER SUIT";
        else if (md.SkinType == S_SECTOID)
            ctype = "Sectoid Corpse";
        else
            ctype = "Muton Corpse";

        Item *it = create_item(ctype);
        ASSERT(it);
        g_map->place(z, x, y)->put(it);
    } else {
        // A death of an already stunned soldier (because his body got hit 
        // or probably because of a fatal wound once it gets implemented)
        m_stunned_body = NULL;
    }

    //m_platoon->change_morale(-(100 / (m_platoon->num_of_men() + 1)), false);

    g_console->printf(COLOR_BLUE, _("%s killed."), md.Name);
    battle_report(_("killed: %s\n"), md.Name);
  // Todo: With-what-weapon, By-whom ?
}


void Soldier::stun()
// Todo: Make difference between 'real' corpses and stunned soldiers.
{
    ASSERT(m_stunned_body == NULL);

    g_map->set_man(z, x, y, NULL);

    z = g_map->find_ground(z, x, y);
    for (int i = 0; i < NUMBER_OF_CARRIED_PLACES; i++)
        m_place[i]->dropall(z, x, y);

    const char *ctype;
    if (md.SkinType == S_XCOM_0)
        ctype = "CORPSE";
    else if (md.SkinType == S_XCOM_1)
        ctype = "CORPSE & ARMOUR";
    else if ((md.SkinType == S_XCOM_2) || (md.SkinType == S_XCOM_3))
        ctype = "CORPSE & POWER SUIT";
    else if (md.SkinType == S_SECTOID)
        ctype = "Sectoid Corpse";
    else
        ctype = "Muton Corpse";

    m_stunned_body = create_item(ctype, this);
    ASSERT(m_stunned_body);
    g_map->place(z, x, y)->put(m_stunned_body);

    x = -1;
    y = -1;
    z = -1;

    curway = -1; waylen = 0;
    FIRE_num = 0;
    MOVED = 0;
    m_reaction_chances = 0;

    g_console->printf(COLOR_BLUE, _("%s stunned."), md.Name);
    battle_report(_("stunned: %s\n"), md.Name);
}


/**
 * Remove a soldier from the prev/next - chain
 */
void Soldier::unlink()
{
    if (m_prev != NULL) m_prev->m_next = m_next;
    if (m_next != NULL) m_next->m_prev = m_prev;
    if (m_platoon != NULL && m_platoon->man == this) m_platoon->man = m_next;
    m_prev = NULL; m_next = NULL; m_platoon = NULL;
}

/**
 * Show TUs needed to move item in hand to a place like belt, backpack etc.
 */ 
void Soldier::draw_deselect_times(BITMAP *dest, Item *sel_item, int sel_item_place)
{
    int time = 0;

    for (int i = 0; i < NUMBER_OF_CARRIED_PLACES; i++) {
        time = calctime(sel_item_place, i);
        if (sel_item != NULL && sel_item->obdata_reloadTime() &&
                (i == P_ARM_LEFT || i == P_ARM_RIGHT) && item(i) != NULL) {
            time += sel_item->obdata_reloadTime();
        }
        m_place[i]->draw_deselect_time(dest, i, time);
    }
    map->place(z, x, y)->draw_deselect_time(dest, P_MAP, calctime(sel_item_place, P_MAP));
}

void Soldier::damage_items(int damage)
{
    int place_def[8] = { 20, 20, 10, 10, 30, 30, 50, 40 }; // ?? 8: NUMBER_OF_CARRIED_PLACES

    for (int i = 0; i < 8; i++) {
        int def = damage * place_def[i] / 100;
        m_place[i]->damage_items(damage - def);
    }
}

/**
 * When soldier panics: 
 * action == 0 - run randomly;
 *        == 1 - "berserk" mode.
 */
void Soldier::panic(int action)
{   
    /*switch(action) {
        case 0:
        m_place[P_ARM_LEFT]->dropall(z, x, y);
        m_place[P_ARM_RIGHT]->dropall(z, x, y);
        //run for you life! - not implemented yet
        ud.CurTU = 0;
        g_console->printf(COLOR_ROSE, "%s has panicked.", md.Name);
        break;
        
        case 1:
        if(platoon_local->belong(this))     //shots will be sent to the remote player - so don't double them
            berserk_fire();              
        g_console->printf(COLOR_ROSE, "%s has gone berserk.", md.Name);
        break;
    }*/
    
    panicking = true;
    m_place[P_ARM_LEFT]->dropall(z, x, y);
    m_place[P_ARM_RIGHT]->dropall(z, x, y);

    change_morale(20);
    
    g_console->printf(COLOR_ROSE, _("%s has panicked."), md.Name);
    if (platoon_local->belong(this))
        g_console->printf(COLOR_ROSE, _("(can't access inventory but moves faster)"));
                                         
    battle_report( "%s: %s\n", _("Panicked"), md.Name);
}

void Soldier::berserk_fire()
{
    Item *it = rhand_item();
    int iplace = P_ARM_RIGHT;
    
    //no item in right hand or it cannot fire - use item in left hand
    if(!it || (!it->obdata_accuracy(0) && !it->obdata_accuracy(1) && !it->obdata_accuracy(2))) {
        it = lhand_item();
        iplace = P_ARM_LEFT;
    }
    
    //no item in both hands or neither of them can fire - do nothing    
    if(!it || (!it->obdata_accuracy(0) && !it->obdata_accuracy(1) && !it->obdata_accuracy(2)))
        return;
    
    target.item = it;
    target.place = iplace;
    
    FIRE_z = z * 12 + 6;
    FIRE_x = (x + (DIR_DELTA_X(dir) * 10)) * 16 + 8;
    FIRE_y = (y + (DIR_DELTA_Y(dir) * 10)) * 16 + 8;    
    
    if(it->obdata_accuracy(0)) {
        target.accur = FAccuracy(it->obdata_accuracy(0), it->obdata_twoHanded()) / 2;
        target.time = required(it->obdata_time(0)) * it->obdata_autoShots();
        target.action = AUTOSHOT;
    } else if(it->obdata_accuracy(1)) {
        target.accur = FAccuracy(it->obdata_accuracy(1), it->obdata_twoHanded()) / 2;
        target.time = required(it->obdata_time(1));
        target.action = SNAPSHOT;
    } else if(it->obdata_accuracy(2)) {
        target.accur = FAccuracy(it->obdata_accuracy(2), it->obdata_twoHanded()) / 2;
        target.time = required(it->obdata_time(2));
        target.action = AIMEDSHOT;
    }
    
    FIRE_num = ud.CurTU / target.time;
}

/**
 * Change value of morale, with bounds-check
 */
void Soldier::change_morale(int delta)
{
    int new_morale = ud.Morale + delta;
    
    //double the morale penalty if there are no team-mates near
    if (delta < 0 && m_platoon->dist_to_nearest(this) > 9)
        new_morale += delta;
    
    if (new_morale > 100)
        new_morale = 100;
    if (new_morale < 1)
        new_morale = 1;
        
    ud.Morale = new_morale;
}

//shl_right 0   //leg_left  5
//shl_left  1   //back      6
//arm_right 2   //belt      7
//arm_left  3   //map       8
//leg_right 4
/**
 * Calculate time for moving items between hand, belt, etc.
 */
// !! Relict from XCOM: loading ammo always uses fixed amount of time,
// !! e.g. opening backpack uses no time.
int Soldier::calctime(int src, int dst)
{
    if (src == dst)
        return 0;

    ASSERT((src >= 0) && (src < 9) &&
           (dst >= 0) && (dst < 9));

    if (((src == 2) && (dst == 3)) || ((src == 3) && (dst == 2)))
        return 4;
                           // shl    arm   leg   bk bl map
    static int tohand[]   = { 3,  3, 0, 0, 4, 4,  8, 4, 8};
    static int fromhand[] = {10, 10, 0, 0, 8, 8, 14, 8, 2};

    return tohand[src] + fromhand[dst];
}


Item *Soldier::select_item(int &i, int scr_x, int scr_y)
{
    for (i = 0; i < NUMBER_OF_PLACES; i++) {
        Item *it = m_place[i]->mselect(scr_x, scr_y);
        if (it != NULL) {
            return it;
        }
    }
    return NULL;
}


int Soldier::deselect_item(Item *&it, int it_place, int &req_time, int scr_x, int scr_y)
{
    for (int i = 0; i < NUMBER_OF_PLACES; i++) {
        req_time = calctime(it_place, i);
        int ISLOCAL = platoon_local->belong(this);
        if (time_reserve(req_time, ISLOCAL, 0) == OK) {
            if (m_place[i]->mdeselect(it, scr_x, scr_y)) {
                spend_time(req_time);
                return i;
            }
        }
    }
    return -1;
}


Soldier *Soldier::nextman()
{
    if (m_next != NULL) return m_next;
    Soldier *s = this;
    while (s->m_prev != NULL) {
        s = s->m_prev;
    }
    return s;
}


Soldier *Soldier::prevman()
{
    if (m_prev != NULL) return m_prev;
    Soldier *s = this;
    while (s->m_next != NULL) {
        s = s->m_next;
    }
    return s;
}


Item *Soldier::item_under_mouse(int ipl, int scr_x, int scr_y)
{
    if ((ipl >= 0) && (ipl <= 7))
        return m_place[ipl]->item_under_mouse(scr_x, scr_y);
    return NULL;
}

/**
 * Test if soldier has Item in hand or one of his pockets.
 */
int Soldier::haveitem(Item *it)
{
    for (int i = 0; i < NUMBER_OF_CARRIED_PLACES; i++)
        if (m_place[i]->isthere(it))
            return 1;
    return 0;
}

Place *Soldier::find_item(Item *it, int &lev, int &col, int &row)
{
    for (int i = 0; i < NUMBER_OF_CARRIED_PLACES; i++)
        if (m_place[i]->isthere(it)) {
            lev = z; col = x; row = y;
            return m_place[i];
        }
    return NULL;
}

int Soldier::find_place_coords(Place *pl, int &lev, int &col, int &row)
{
    for (int i = 0; i < NUMBER_OF_CARRIED_PLACES; i++)
        if (m_place[i] == pl) {
            lev = z; col = x; row = y;
            return 1;
        }
    return 0;
}


int Soldier::place(Place *pl)
{
    for (int i = 0; i < NUMBER_OF_PLACES; i++)
        if (pl == m_place[i])
            return i;
    return -1;
}


int Soldier::open_door()
// Todo: Different sounds for different types of doors
{
    if (havetime(6) == OK) {
        if (map->open_door(z, x, y, dir)) {
            soundSystem::getInstance()->play(SS_DOOR_CLICK);
            spend_time(6);
            if (platoon_local->belong(this)) platoon_remote->check_reaction_fire(this); //local soldier opens door - so let's check reaction fire
            net->send_open_door(NID);
            return 1;
        }
    }
    return 0;
}

/**
 * Change between standing and sitting/kneeling.
 * Return 1 on success, 0 on failure.
 */
int Soldier::change_pose()
{
    ASSERT((m_state == SIT) || (m_state == STAND));

    int ISLOCAL = platoon_local->belong(this);
    if (m_state == SIT) {
        if (time_reserve(8, ISLOCAL, 0) != OK) return 0;
        m_state = STAND;
        spend_time(8);
    } else {
        if (time_reserve(4, ISLOCAL, 0) != OK) return 0;
        m_state = SIT;
        spend_time(4);
    }
    m_reaction_chances++;
    net->send_change_pose(NID);
    return 1;
}

/**
 * Try to change pose from kneeling to standing.
 * Return 1 on success, 0 on failure.
 */
int Soldier::standup()
{
    if (m_state == STAND)
        return 1;
    else if (m_state == SIT)
        return change_pose();
    else
        return 0;
}

int Soldier::prime_grenade(int iplace, int delay_time, int req_time)
{
    ASSERT((iplace == P_ARM_RIGHT) || (iplace == P_ARM_LEFT));

    if (havetime(req_time) == OK) {
        Item * it = item(iplace);
        ASSERT(it != NULL);

        elist->add(this, it, delay_time);

        spend_time(req_time);
        m_reaction_chances += req_time / 4;
        net->send_prime_grenade(NID, iplace, delay_time, req_time);
        return 1;
    }
    return 0;
}


/**
 * Try to unload clip from item.  
 * Check if it is a weapon, hands are free, soldier has enough time, etc.
 * Return 1 on success, 0 on failure.
 */
int Soldier::unload_ammo(Item * it)
{
    if ((it == NULL) || (!it->haveclip()))
        return 0;
    if ((rhand_item() != NULL && rhand_item() != it) || 
        (lhand_item() != NULL && lhand_item() != it))
        return 0;

    int ISLOCAL = platoon_local->belong(this);
    if (it->roundsremain() > 0) {
        if (time_reserve(8, ISLOCAL, false) == OK) {
            putitem(it, P_ARM_RIGHT);
            putitem(it->unload(), P_ARM_LEFT);
            spend_time(8);
                
            net->send_unload_ammo(NID);
            return 1;
        } else return 0;
    } else {
        if (time_reserve(10, ISLOCAL, false) == OK) {
            putitem(it, P_ARM_RIGHT);
            putitem(it->unload(), P_MAP);
            spend_time(10);           
            net->send_unload_ammo(NID);
            return 1;
        } else return 0;
    }
}

/**
 * Try to load item into iplace,
 * test if soldier has enough time, item is ammo of correct type, etc.
 * Return 1 on success, 0 on failure.
 */
int Soldier::load_ammo(int iplace, int srcplace, Item *&it)
{
    if (it == NULL)
        return 0;
                                                        
    int time = it->obdata_reloadTime() + calctime(srcplace, iplace);
    int ISLOCAL = platoon_local->belong(this);
    if (time_reserve(time, ISLOCAL, false) != OK) return 0;

    Item *gun = item(iplace);
    if (gun == NULL)
        return 0;
    if (!gun->loadclip(it))
        return 0;

    spend_time(time);
    net->send_load_ammo(NID, iplace, srcplace);
    return 1;
}

/**
 * Function that decrements soldier time units and energy for some action.
 * The soldier must have enough time units and energy before calling 
 * this function.
 * 
 * @param tm          time required to perform an action
 * @param use_energy  flag which shows whether the action requires energy 
 *                    to perform
 */
void Soldier::spend_time(int tm, int use_energy)
{
    ASSERT(ud.CurTU >= tm);
    ud.CurTU -= tm;
    if (use_energy) {
        ASSERT(ud.CurEnergy >= (tm / 2));
        ud.CurEnergy        -= (tm / 2);
    }

    if (FLAGS & F_ENDLESS_TU) {
        if (ud.CurTU     < 32) ud.CurTU = ud.MaxTU;
        if (ud.CurEnergy < 16) ud.CurEnergy = ud.MaxEnergy;
    }
}

/**
 * Function that checks if the soldier has time units and energy required
 * to do something. For actions that require energy, use_energy should be 1.
 *
 * @param ntime       time required to perform an action
 * @param use_energy  flag which shows whether the action requires energy 
 *                    to perform
 * @result            Error code from GameErrorCodes.
 * @sa                GameErrorCodes
 */
int Soldier::havetime(int ntime, int use_energy)
{
    if (use_energy)
        if (ud.CurEnergy < (ntime / 2))
            return ERR_NO_ENERGY;
    if (ud.CurTU < ntime)
        return ERR_NO_TUS;
    return OK;
}

/**
 * Function that returns the time needed to move from current location to 
 * specified direction.
 *
 * @param _dir  walk direction (-1 in the case when the time to get to 
 *              the current map location is needed)
 */
int Soldier::walktime(int _dir)
{
    int dz = z, dx = x, dy = y;
    if (_dir != -1) {
        dx += DIR_DELTA_X(_dir);
        dy += DIR_DELTA_Y(_dir);
    }
    int time_of_dst = map->walk_time(dz, dx, dy);

    if (_dir != -1 && DIR_DIAGONAL(_dir))
        time_of_dst = time_of_dst * 3 / 2; // Diagonal move multiplier.
    // Only used with havetime(). Actual movement calls walktime(-1).

    return time_of_dst;
}


/**
 * Calculate how many TUs are required for an action 
 * that needs a percentage of the MaxTU.
 * (e.g. Autoshot needs 30% of max. TU)
 */
// ud.MaxTU  100%
//   x         ntime
int Soldier::required(int pertime)
{
    return (ud.MaxTU * pertime) / 100;
}

/**
 * Calculate effective firing accuracy, 
 * with wound/stun/morale/physical condition modifiers applied.
 * For displaying in unit-stats-screen, e.g. Soldier::draw_unibord()
 */
int Soldier::eff_FAccuracy()
{
    int ac = ud.CurFAccuracy;
    int penalty_health = 0;
    if (ud.MaxHealth != 0) penalty_health = (ac * (ud.MaxHealth - ud.CurHealth)) / ud.MaxHealth / 2;
    int penalty_morale = (ac * (100 - ud.Morale)) / 100 / 2;
    int penalty_fly = is_flying() ? ac * 20 / 100 : 0;
    ac -= penalty_health + penalty_morale + penalty_fly;
    return ac < 0 ? 0 : ac;
}

/**
 * Calculate firing accuracy.
 * Accuracy gets better for sitting/kneeling, 
 * and for using a two-handed weapon with an empty free hand.
 */
int Soldier::FAccuracy(int peraccur, int TWOHAND)
{
    int ac = eff_FAccuracy();

    if (TWOHAND) {
        if ((rhand_item() != NULL) && (lhand_item() != NULL))
            ac -= ac / 3;
    }

    if (m_state == SIT) ac += ac / 10;

    double weapon_delta  = 1. / (double)(peraccur * peraccur);
    double soldier_delta = 1. / (double)(ac * ac);

    return static_cast<int>(sqrt(2. / (weapon_delta + soldier_delta)));
}

/**
 * Calculate throwing accuracy
 */
int Soldier::TAccuracy(int peraccur)
{
    int ac = (ud.CurTAccuracy * peraccur) / 100;
    if (ud.MaxHealth != 0) ac = (ac * ud.CurHealth) / ud.MaxHealth;
    return ac;
}

/**
 * Calculate random deviation for a shot
 * based on firing accuracy of soldier.
 */
void Soldier::apply_accuracy(REAL & fi, REAL & te)
{
    REAL TE_STEP = (PI * 2.0 / (double)(cfg_get_base_accuracy()));
    REAL FI_STEP = (PI * 0.5 / (double)(cfg_get_base_accuracy()));

//  double acc = 100. * 100. / (double)(target.accur * target.accur);
    double acc = 100. / (double)(target.accur);
//  Make acc tend to 0.0 while target.accur tends to 100.
    if (acc > 1.20) acc -= 1.0;
//  However, if accuracy is very good, some chance to miss should be left.
    else acc /= 6.0;

    te += cur_random->getNormal(TE_STEP * acc);
    fi += cur_random->getNormal(FI_STEP * acc);
}

/**
 * Calculate random deviation for a grenade-throw 
 * based on throwing accuracy of soldier.
 */
void Soldier::apply_throwing_accuracy(REAL &fi, REAL &te, int weight)
{
    ASSERT(weight > 0);
    REAL TE_STEP = (PI /  8 / 30.0);
    REAL FI_STEP = (PI / 32 / 30.0);

    REAL rand_te, rand_fi, rand_range;
    rand_range = 100.0 - target.accur;
    if (rand_range < 1.0) rand_range = 1.0;
    rand_te = cur_random->getUniform(-rand_range * 0.5, rand_range * 0.5);
    rand_fi = cur_random->getUniform(0, rand_range) - rand_range / weight;
    te += TE_STEP * rand_te;
    fi += FI_STEP * rand_fi;
}


int Soldier::check_for_hit(int _z, int _x, int _y)
{
    if (m_state == FALL || m_state == LIE) return 0;

    int lev = _z / 12;
    int col = _x / 16;
    int row = _y / 16;

    if ((lev == z) && (col == x) && (row == y)) {
        //return 1;
        lev = _z % 12;
        col = _x % 16;
        row = _y % 16;
        int s = 0;
        if (m_state == SIT)
            s = 1;

        //lev -= map->mcd(z, x, y, 0)->T_Level; //!!!
        //lev -= map->mcd(z, x, y, 3)->T_Level;
        //if (lev < 0) lev = 0;

        if (m_skin->check_for_hit(s, dir, lev, col, 15 - row))
            return 1;
    }

    return 0;
}


void Soldier::apply_hit(int sniper, int _z, int _x, int _y, int _wtype, int _hitdir)
{
    if (check_for_hit(_z, _x, _y)) {    
        hit(sniper, Item::obdata_damage(_wtype), Item::obdata_damageType(_wtype), _hitdir, Item::obdata_dDeviation(_wtype));
    }
}

/**
 * Aiming in the center of the map cell or soldier, if any
 */
void Soldier::standard_aiming(int za, int xa, int ya)
{
    FIRE_z = za * 12;
    FIRE_x = xa * 16 + 8;
    FIRE_y = ya * 16 + 8;
    Soldier *s = map->man(za, xa, ya);
//  Currently dependent informally on body_txt array.
    if (s) FIRE_z += s->calc_z() + (s->m_state == SIT) ? 4 : 7;
    else FIRE_z += 6;
//  TODO when different races have different shapes:
//  Aim directly in the centre of the creature (soldier):
//  if (s) {
//      FIRE_z += s->calc_z() + s->ud.UnitBottom + (s->ud.UnitHeight >> 1);
//      g_console->printf(COLOR_SYS_INFO1, "%d %d %d\n", (int) FIRE_z, (int) s->ud.UnitBottom, (int) s->ud.UnitHeight);
//  } else FIRE_z += 6;
}

/**
 * Aiming at the precise height (Left-Shift key)
 */
void Soldier::precise_aiming(int za, int xa, int ya)
{
    BITMAP *bmp = map->create_lof_bitmap(za, xa, ya);
    BITMAP *bmp_back = create_bitmap(bmp->w, bmp->h);
    int mx = mouse_x, my = mouse_y;
    blit(screen, bmp_back, mx, my, 0, 0,  bmp->w, bmp->h);
    blit(bmp,    screen,    0, 0, mx, my, bmp->w, bmp->h);
    int z = 8, x = 8, y = 8;

    int mouse_leftr = 0, mouse_rightr = 0;
    if (!(mouse_b & 1)) mouse_leftr   = 1;
    if (!(mouse_b & 2)) mouse_rightr  = 1;

    MouseRange temp_mouse_range(mx + 1, my + 1, mx + bmp->w -1, my + bmp->h - 1);
    show_mouse(screen);
    text_mode(0);
    while (!keypressed()) {
        if (CHANGE) {
            int sx =     (mouse_x - mx) / 20;
            int sy = 2 - (mouse_y - my) / 20;
            z = sx * 3 + sy;

            x = (mouse_y - my) % 20 - 3; if (x < 0) x = 0; if (x > 15) x = 15;
            y = (mouse_x - mx) % 20 - 3; if (y < 0) y = 0; if (y > 15) y = 15;

            //textprintf(screen, font, mx, my-10, 1, "z=%02d x=%02d y=%02d", z, x, y);
            //textprintf(screen, font, mx, my-10, 1, "sx=%d sy=%d", sx, sy);
            CHANGE = 0;
        }

        if ((mouse_b & 1) && (mouse_leftr)) { //left
            mouse_leftr = 0;

            break;
        }

        if ((mouse_b & 2) && (mouse_rightr)) { //right
            mouse_rightr = 0;
            z = 8;
            x = 8;
            y = 6;
            break;
        }

        if (!(mouse_b & 1)) mouse_leftr  = 1;
        if (!(mouse_b & 2)) mouse_rightr = 1;
    }

    FIRE_z = za * 12 + z;
    FIRE_x = xa * 16 + x;
    FIRE_y = ya * 16 + y;

    show_mouse(NULL);
    blit(bmp_back, screen, 0, 0, mx, my, bmp->w, bmp->h);
    destroy_bitmap(bmp_back);
    destroy_bitmap(bmp);
}

void Soldier::try_shoot()
{
    // Moving soldier cannot shoot
    if (ismoving()) return ;

    if ((z == map->sel_lev) && (x == map->sel_col) && (y == map->sel_row)) return ;

    if (FIRE_num != 0) return ;

    // In melee you have to be near your target
    if (target.action == PUNCH) {
        if ((z != map->sel_lev) || (abs(x - map->sel_col) > 1) || (abs(y - map->sel_row) > 1))
            return ;
    }

    if (target.action == AUTOSHOT)
        FIRE_num = target.item->obdata_autoShots();
    else
        FIRE_num = 1;

    if ((key[KEY_LSHIFT]) && (target.action != THROW)) precise_aiming(map->sel_lev, map->sel_col, map->sel_row);
    else standard_aiming(map->sel_lev, map->sel_col, map->sel_row);

    // Perform some checks to determine if we can keep targeting mode after 
    // this shot. So we need to check if TWO shots can be made (one right now 
    // and another one after left clicking mouse in a targeting mode again)

    if (havetime(target.time * 2 * FIRE_num) != OK) {
        TARGET = 0;
    }

    if (!target.item->is_laser() && !target.item->is_hand_to_hand() &&
            (target.item->roundsremain() < FIRE_num + 1)) {
        TARGET = 0;
    }

    if (target.action != SNAPSHOT && target.action != AIMEDSHOT &&
            target.action != AUTOSHOT && target.action != PUNCH) {
        TARGET = 0;
    }

    // Face to target
    if(!faceto(map->sel_col, map->sel_row)) {
        FIRE_num = 0;
    }
}

// Note the absence of map->sel_*, since the aim is to hit the target's position.
// So instead of using map->sel_*, we use the_target->*.
void Soldier::try_reaction_shot(Soldier *the_target)
{
    TARGET = 0;

    // Moving soldier cannot shoot
    if (ismoving()) return;

    if ((z == the_target->z) && (x == the_target->x) && (y == the_target->y)) return;

    if (FIRE_num != 0) return;

    // This shouldn't happen, but check anyways!
    if (target.action == PUNCH) {
        if ((z != the_target->z) || (abs(x - the_target->x) > 1) || (abs(y - the_target->y) > 1))
            return ;
    }

    if (target.action == AUTOSHOT)
        FIRE_num = target.item->obdata_autoShots();
    else
        FIRE_num = 1;

    standard_aiming(the_target->z, the_target->x, the_target->y);

    // Check that we can fire at least one shot using current settings

    if (havetime(target.time) != OK) {
        FIRE_num = 0;
        return;
    }

    if (!target.item->is_laser() && !target.item->is_hand_to_hand() &&
            (target.item->roundsremain() < FIRE_num)) {
        FIRE_num = 0;
        return;
    }

    if (target.action != SNAPSHOT && target.action != AIMEDSHOT &&
            target.action != AUTOSHOT && target.action != PUNCH) {
        FIRE_num = 0;
        return;
    }

    // Face to target
    faceto(the_target->x, the_target->y);
    return;
}

void Soldier::calc_bullet_start(int xs, int ys, int zs, int* xr, int* yr, int *zr)
{
    *zr = zs * 12 + 8; if (m_state == SIT) *zr -= 3;
    *xr = xs * 16 + 8 + DIR_DELTA_X(dir) * 4;
    *yr = ys * 16 + 8 + DIR_DELTA_Y(dir) * 4;
}

void Soldier::calc_shot_stat(int zd, int xd, int yd)
{
    int x0, y0, z0;
    calc_bullet_start(x, y, z, &x0, &y0, &z0);

    REAL ro = sqrt((double)((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0)));

    int shoot_cnt = 0;

    for(int cic = 0; cic < 1000; cic++)
    {
        REAL fi = acos((REAL)(zd - z0) / ro);
        REAL te = atan2((REAL)(yd - y0), (REAL)(xd - x0));
        apply_accuracy(fi, te);

        for(int i = 3; 1; i++) {
            int x = (int)(x0 + i * cos(te) * sin(fi));
            int y = (int)(y0 + i * sin(te) * sin(fi));
            int z = (int)(z0 + i * cos(fi));
    
            if (!map->inside(z, x, y))          break;
            if (!map->pass_lof_cell(z, x, y))   break;
            if (platoon_remote->check_for_hit(z, x, y) ||
                platoon_local->check_for_hit(z, x, y)) {
                    shoot_cnt++;
                    break;
                }
        }
    }
    g_console->printf(COLOR_SYS_INFO1, "ToShoot = %d%%", shoot_cnt / 10);
}

void Soldier::shoot(int zd, int xd, int yd, int ISLOCAL)
{
    if (key[KEY_LCONTROL] && (FLAGS & F_REACTINFO)) {
        calc_shot_stat(zd, xd, yd);
    } else {
        if (ISLOCAL)
            m_reaction_chances += (target.time / 4); // How many chances at a reaction shot do we get? TUs / 4.
        ASSERT(target.action != NONE);
        ASSERT(target.item != NULL);
        int x0, y0, z0, chk;
        calc_bullet_start(x, y, z, &x0, &y0, &z0);
        chk = do_target_action(z0, x0, y0, zd, xd, yd, target.action, target.place);
        if (chk == OK) net->send_target_action(NID, z0, x0, y0, zd, xd, yd, target.action, target.place);
        else report_game_error(chk);
    }
}

int Soldier::assign_target(Action action, int iplace)
{
    Item *it = item(iplace);
    if (it == NULL) return ERR_NO_ITEM;
    switch (action)
    {
        case THROW:
            target.accur = 100;
            target.time = required(25);
            break;
        case SNAPSHOT:
            target.accur = FAccuracy(it->obdata_accuracy(SNAP), it->obdata_twoHanded());
            target.time = required(it->obdata_time(SNAP));
            break;
        case AIMEDSHOT:
            target.accur = FAccuracy(it->obdata_accuracy(AIMED), it->obdata_twoHanded());
            target.time = required(it->obdata_time(AIMED));
            break;
        case AUTOSHOT:
            target.accur = FAccuracy(it->obdata_accuracy(AUTO), it->obdata_twoHanded());
            target.time = required(it->obdata_time(AUTO));
            break;
        case PUNCH:
            target.accur = 100;
            target.time = required(it->obdata_useTime());
            if (!target.time) target.time = required(25); // some old melee weapons don't have "useTime" property
            break;
        case AIMEDTHROW:
            target.accur = TAccuracy(it->obdata_accuracy(ATHROW));
            target.time = required(50);
            break;
        default: ASSERT(false);
    }
    target.item = it;
    target.place = iplace;
    target.action = action;
    return OK;
}

int Soldier::do_target_action(int z0, int x0, int y0, int zd, int xd, int yd, Action action, int iplace)
{
    int chk;
    chk = assign_target(action, iplace);
    if (chk != OK) return chk;
    switch (action)
    {
        case THROW:
            chk = thru(z0, x0, y0, zd, xd, yd, target.place, target.time);
            break;
        case SNAPSHOT:
        case AIMEDSHOT:
        case AUTOSHOT:
            if (target.item->is_laser()) {
                chk = beam(z0, x0, y0, zd, xd, yd, target.place, target.time);
            } else {
                chk = fire(z0, x0, y0, zd, xd, yd, target.place, target.time);
            }
            break;
        case PUNCH:
            chk = punch(z0, x0, y0, zd, xd, yd, target.place, target.time);
            break;
        case AIMEDTHROW:
            chk = aimedthrow(z0, x0, y0, zd, xd, yd, target.place, target.time);
            break;
        default: ASSERT(false);
    }
    return chk;
}

int Soldier::punch(int z0, int x0, int y0, int zd, int xd, int yd, int iplace, int req_time)
{
    int chk;
    Item *it = item(iplace);
    if (it == NULL) return ERR_NO_ITEM;
    chk = havetime(req_time);
    if (chk != OK) return chk;

    REAL ro = sqrt((double)((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0)));
    REAL fi = acos((REAL)(zd - z0) / ro);
    REAL te = atan2((REAL)(yd - y0), (REAL)(xd - x0));
    apply_accuracy(fi, te);
    spend_time(req_time);
    m_bullet->punch(z0, x0, y0, fi, te, it->itemtype());
    return OK;
}


int Soldier::thru(int z0, int x0, int y0, int zd, int xd, int yd, int iplace, int req_time)
{
    int chk;
    Item *it = item(iplace);
    if (it == NULL) return ERR_NO_ITEM;
    chk = havetime(req_time);
    if (chk != OK) return chk;
    zd -= 8;
    REAL ro = sqrt((double)((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0)));
    int ro_real = m_bullet->calc_throw(z0, x0, y0, zd, xd, yd);
    if (ro_real > 18 * 16) {
        TARGET = 1; FIRE_num = 0;
        return ERR_DISTANCE;
    }

    REAL fi = acos((REAL)(zd - z0) / ro);
    REAL te = atan2((REAL)(yd - y0), (REAL)(xd - x0));
    REAL zA = sqrt(ro);
    apply_throwing_accuracy(fi, te, target.item->obdata_weight());
    spend_time(req_time);
    m_place[iplace]->set_item(NULL);
    m_bullet->thru(z0, x0, y0, ro, fi, te, zA, it);
    return OK;
}


int Soldier::aimedthrow(int z0, int x0, int y0, int zd, int xd, int yd, int iplace, int req_time)
{
    int chk;
    Item *it = item(iplace);
    if (it == NULL) return ERR_NO_ITEM;
    chk = havetime(req_time);
    if (chk != OK) return chk;

    REAL ro = sqrt((double)((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0)));
    REAL fi = acos((REAL)(zd - z0) / ro);
    REAL te = atan2((REAL)(yd - y0), (REAL)(xd - x0));
    apply_accuracy(fi, te);
    spend_time(req_time);
    m_place[iplace]->set_item(NULL);
    m_bullet->aimedthrow(z0, x0, y0, fi, te, it);
    return OK;
}


int Soldier::beam(int z0, int x0, int y0, int zd, int xd, int yd, int iplace, int req_time)
{
    int chk;
    Item *it = item(iplace);
    if (it == NULL) return ERR_NO_ITEM;
    chk = havetime(req_time);
    if (chk != OK) return chk;

    REAL ro = sqrt((double)((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0)));
    REAL fi = acos((REAL)(zd - z0) / ro);
    REAL te = atan2((REAL)(yd - y0), (REAL)(xd - x0));
    apply_accuracy(fi, te);
    spend_time(req_time);
    soundSystem::getInstance()->play(it->get_sound());
    m_bullet->beam(z0, x0, y0, fi, te, it->itemtype());
    return OK;
}


int Soldier::fire(int z0, int x0, int y0, int zd, int xd, int yd, int iplace, int req_time)
{
    int chk;
    Item *it = item(iplace);
    if ((it == NULL)) return ERR_NO_ITEM;
    if ((!it->haveclip()) || (it->roundsremain() < 1)) return ERR_NO_AMMO;
    chk = havetime(req_time);
    if (chk != OK) return chk;

    REAL ro = sqrt((double)((xd - x0) * (xd - x0) + (yd - y0) * (yd - y0) + (zd - z0) * (zd - z0)));
    REAL fi = acos((REAL)(zd - z0) / ro);
    REAL te = atan2((REAL)(yd - y0), (REAL)(xd - x0));
    apply_accuracy(fi, te);
    spend_time(req_time);
    soundSystem::getInstance()->play(it->get_sound());
    m_bullet->fire(z0, x0, y0, fi, te, it->cliptype());
    it->shot();
    return OK;
}


void Soldier::draw_bullet_way()
{
    if (TARGET && target.action == THROW)
        m_bullet->showthrow(z, x, y, map->sel_lev, map->sel_col, map->sel_row);
    else
        m_bullet->showline(z, x, y, map->sel_lev, map->sel_col, map->sel_row);
}


void Soldier::showspk(BITMAP *dest)
{
    // Why did these have "% 4" before? They prevented the flying suit from displaying.
    switch (md.SkinType) {
        case S_XCOM_0:
        case S_XCOM_1:
            Skin::m_spk[(md.SkinType - 1) % 4][md.fFemale][md.Appearance]->show(dest, 0, 0);
            break;
        case S_XCOM_2:
        case S_XCOM_3:
            Skin::m_spk[(md.SkinType - 1) % 4][0][0]->show(dest, 0, 0);
            break;
        case S_SECTOID:
            Skin::m_spk[4][0][0]->show(dest, 0, 0);
            break;
        case S_MUTON:
            Skin::m_spk[5][0][0]->show(dest, 0, 0);
            break;
        case S_CHAMELEON: {
            int stack_top = lua_gettop(L);
            lua_pushstring(L, "UnitsTable");
            lua_gettable(L, LUA_GLOBALSINDEX);
            ASSERT(lua_istable(L, -1));
            lua_pushnumber(L, md.Appearance);
            lua_gettable(L, -2);
            if (!lua_istable(L, -1)) {
                lua_pop(L, 1);
                lua_pushnumber(L, 0);
                lua_gettable(L, -2);
            }
            ASSERT(lua_istable(L, -1));
            lua_pushstring(L, "pInv");
            lua_gettable(L, -2);
            if (lua_islightuserdata(L, -1)) {
                ALPHA_SPRITE *spr = (ALPHA_SPRITE *)lua_touserdata(L, -1);
                ASSERT(spr);
                draw_alpha_sprite(dest, spr, 80 - (spr->w / 2), 100 - (spr->h / 2));
            }
            lua_settop(L, stack_top);
            break;
        }
        default:
            break;
    }
}

/**
 * Draw soldier-infos into the control-panel on the battlescape:
 * items in hand, current stats with barcharts 
 */
void Soldier::drawinfo(int x, int y)
{
    if (rhand_item() != NULL) {
        if (rhand_item()->clip() != NULL)
            icon->draw_item(I_RIGHT, rhand_item(), rhand_item()->roundsremain(), -1, false);
        else if (rhand_item()->is_grenade() && rhand_item()->delay_time() > 0)
            icon->draw_item(I_RIGHT, rhand_item(), -1, rhand_item()->delay_time() - 1, false);
        else if (rhand_item()->is_grenade() && rhand_item()->is_proximity_grenade() && rhand_item()->delay_time() < 0)
            icon->draw_item(I_RIGHT, rhand_item(), -1, -1, true);
        else
            icon->draw_item(I_RIGHT, rhand_item(), -1, -1, false);  
    }
    if (lhand_item() != NULL) {
        if (lhand_item()->clip() != NULL)
            icon->draw_item(I_LEFT, lhand_item(), lhand_item()->roundsremain(), -1, false);
        else if (lhand_item()->is_grenade() && lhand_item()->delay_time() > 0)
            icon->draw_item(I_LEFT, lhand_item(), -1, lhand_item()->delay_time() - 1, false);
        else if (lhand_item()->is_grenade() && lhand_item()->is_proximity_grenade() && lhand_item()->delay_time() < 0)
            icon->draw_item(I_LEFT, lhand_item(), -1, -1, true);
        else
            icon->draw_item(I_LEFT, lhand_item(), -1, -1, false);   
    }

    icon->draw_text(T_MAN_NAME, md.Name);

    icon->draw_attribute(A_TIME_UNITS, ud.CurTU,     ud.MaxTU);
    icon->draw_attribute(A_ENERGY,     ud.CurEnergy, ud.MaxEnergy);
    icon->draw_attribute(A_HEALTH,     ud.CurHealth, ud.MaxHealth);
    icon->draw_attribute(A_MORALE,     ud.Morale, 100);

    if (ud.CurStun > 0) // draw stun bar
    {
        if (ud.CurStun < ud.CurHealth)
            icon->draw_stun_bar(x, y, ud.CurStun,   ud.MaxHealth);
        else
            icon->draw_stun_bar(x, y, ud.CurHealth, ud.MaxHealth);
    }
}

void Soldier::draw_stats(BITMAP* bitmap, int x, int y, bool selected)
{   
    int dx = 15;
    int color = COLOR_WHITE;
    if(m_visible_enemies>0) color = COLOR_RED;
    if(selected) color = COLOR_BLUE02;
    textprintf(bitmap, g_small_font, x, y, color, "%s", md.Name); x += 70;
    textprintf(bitmap, g_small_font, x, y, COLOR_GREEN, "%d", ud.CurTU); x += dx;
    //textprintf(bitmap, g_small_font, x, y, COLOR_GREEN, "%d", m_vision_mask); x += dx;
    textprintf(bitmap, g_small_font, x, y, COLOR_RED, "%d", ud.CurHealth); x += dx;
    //textprintf(bitmap, g_small_font, x, y, COLOR_RED, "%d", m_visible_enemies); x += dx;
    textprintf(bitmap, g_small_font, x, y, COLOR_WHITE, "%d", ud.CurFront); x += dx;
    textprintf(bitmap, g_small_font, x, y, COLOR_WHITE, "%d", ud.CurLeft); x += dx;
    textprintf(bitmap, g_small_font, x, y, COLOR_WHITE, "%d", ud.CurRight); x += dx;
    textprintf(bitmap, g_small_font, x, y, COLOR_WHITE, "%d", ud.CurRear); x += dx;
    textprintf(bitmap, g_small_font, x, y, COLOR_WHITE, "%d", ud.CurUnder); x += dx;
}

#define AUTO   0
#define SNAP   1
#define AIMED  2

int Soldier::check_reaction_fire(Soldier *the_target)
{
    // Ok, check for reaction fire.
    // Compare the reaction figures.
    float total_reactions = ud.CurReactions;
    float tu_ratio; 
    if (the_target->ud.CurTU > 0) tu_ratio = (float)ud.CurTU / the_target->ud.CurTU; //needs for smooth interpolating
    else tu_ratio = 999;

    ASSERT(the_target->ud.CurReactions > 0); // Shouldn't happen, but...
    
    if (((float)ud.CurReactions / (float)the_target->ud.CurReactions) < total_reactions)
        total_reactions = ((float)ud.CurReactions / (float)the_target->ud.CurReactions);

    float r1 = total_reactions;
    total_reactions /= 2;
    float r2 = total_reactions;

    if (tu_ratio < 1) total_reactions *= tu_ratio;

    if(FLAGS & F_REACTINFO)
        g_console->printf(COLOR_SYS_INFO1, "ToReact = %5.4f (%5.4f, %5.4f) TU: %5.4f", total_reactions, r1, r2, tu_ratio);
    
    if (randval(0, 1) < total_reactions)
    {
        // We can make a reaction shot.
        // Try the weapon in right hand first
        for (int arm = P_ARM_RIGHT; arm <= P_ARM_LEFT; arm++) {
            switch (m_ReserveTimeMode) {
            case RESERVE_FREE:
                if (do_reaction_fire(the_target, arm, AUTO)) return 1;
                if (do_reaction_fire(the_target, arm, AIMED)) return 1;
                if (do_reaction_fire(the_target, arm, SNAP)) return 1;
                break;
            case RESERVE_SNAP:
                if (do_reaction_fire(the_target, arm, SNAP)) return 1;
                break;
            case RESERVE_AIM:
                if (do_reaction_fire(the_target, arm, AIMED)) return 1;
                if (do_reaction_fire(the_target, arm, SNAP)) return 1;
                break;
            case RESERVE_AUTO:
                if (do_reaction_fire(the_target, arm, AUTO)) return 1;
                if (do_reaction_fire(the_target, arm, SNAP)) return 1;
                break;
            }
        }

        // No weapon was ready to be fired.
        return 0;
    }
    return 0;
}

int Soldier::do_reaction_fire(Soldier *the_target, int place, int shot_type)
{
    Item *it = item(place);
    char *type_str = NULL;
    if (it == NULL) return 0; // no item in hand
    if (!it->obdata_isGun() && !it->is_laser()) return 0; // item is not a gun or laser
    if (!it->is_laser()     && !it->haveclip()) return 0; // gun with no clip

    int tus;

    // Can this item make this type of shot?
    if (!it->obdata_accuracy(shot_type)) return 0;

    // How many TUs do we use?
    tus = required(it->obdata_time(shot_type));
    if (shot_type == AUTO) tus *= it->obdata_autoShots();

    if (tus <= ud.CurTU) {
        // We have enough time to make the shot. Set up target.
        target.accur = FAccuracy(it->obdata_accuracy(shot_type), it->obdata_twoHanded());
        target.time = tus;
        switch (shot_type) {
            case AIMED:
                target.action = AIMEDSHOT;
                type_str = "AIMED shot";
                break;
            case AUTO:
                target.action = AUTOSHOT;
                type_str = "AUTO shot";
                break;
            case SNAP:
                target.action = SNAPSHOT;
                type_str = "SNAP shot";
                break;
        }
        target.item = it;
        target.place = place;
        try_reaction_shot(the_target);
        if (FIRE_num > 0) // If FIRE_num is set, we're firing shots, so...
        {
            if(FLAGS & F_REACTINFO) g_console->printf(COLOR_SYS_INFO1,"Reaction fire with %s!",type_str);
            return 1;
        }
    }

    // Nope.
    return 0;
}

/**
 * End-of-turn save
 */
int Soldier::eot_save(char *txt)
{
    int len = 0;
    len += sprintf(txt + len,
        "\r\n%s:\r\nNID=%d z=%d x=%d y=%d dir=%d state=%d ud.MaxTU=%d ud.MaxHealth=%d ud.MaxEnergy=%d ud.CurFAccuracy=%d ud.CurTAccuracy=%d ud.CurTU=%d ud.CurHealth=%d ud.CurEnergy=%d ud.Morale=%d\r\n",
        md.Name, NID, z, x, y, dir, (int)m_state, ud.MaxTU, ud.MaxHealth, ud.MaxEnergy, ud.CurFAccuracy, ud.CurTAccuracy, ud.CurTU, ud.CurHealth, ud.CurEnergy, ud.Morale);
    for (int i = 0; i < NUMBER_OF_CARRIED_PLACES; i++) {
        len += m_place[i]->eot_save(i, txt + len);
    }
    return len;
}

int Soldier::count_weight()
{
    int weight = 0;
    for (int i = 0; i < NUMBER_OF_CARRIED_PLACES; i++)
        weight += m_place[i]->count_weight();

    return weight;
}

int Soldier::has_forbidden_equipment()
{
    for (int i = 0; i < NUMBER_OF_PLACES; i++)
        if (m_place[i]->has_forbidden_equipment())
            return 1;
    return 0;
}

int Soldier::has_twohanded_weapon()
{
    std::vector<Item *> items;
    get_inventory_list(items);
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i]->obdata_twoHanded()) return 1;
    }
    return 0;
}

bool Soldier::Write(persist::Engine &archive) const
{
    PersistWriteBinary(archive, *this);

    PersistWriteObject(archive, m_next);
    PersistWriteObject(archive, m_prev);
    PersistWriteObject(archive, m_bullet);
    PersistWriteObject(archive, m_stunned_body);
    PersistWriteObject(archive, m_platoon);
    PersistWriteObject(archive, m_skin);

    for (int i = 0; i < NUMBER_OF_PLACES; i++)
        PersistWriteObject(archive, m_place[i]);

    return true;
}

bool Soldier::Read(persist::Engine &archive)
{
    PersistReadBinary(archive, *this);

    PersistReadObject(archive, m_next);
    PersistReadObject(archive, m_prev);
    PersistReadObject(archive, m_bullet);
    PersistReadObject(archive, m_stunned_body);
    PersistReadObject(archive, m_platoon);
    PersistReadObject(archive, m_skin);

    for (int i = 0; i < NUMBER_OF_PLACES; i++)
        PersistReadObject(archive, m_place[i]);

    return true;
}

void Soldier::set_reserve_type(int type)
{
    if(type == RESERVE_AUTO)
        if((rhand_item() && rhand_item()->obdata_accuracy(AUTO)) || (lhand_item() && lhand_item()->obdata_accuracy(AUTO)))
        {
            m_ReserveTimeMode = type;
            net->send_reserve_time(NID, type);
            return;
        }
        else return;
    if(type == RESERVE_FREE || rhand_item() || lhand_item())
    {
        m_ReserveTimeMode = type;
        net->send_reserve_time(NID, type);
    }
};

int Soldier::can_fly()
{
    return skin()->get_fFlying();
}

int Soldier::is_flying()
{
    return !map->support_for_feet(z, x, y) && can_fly();
}
