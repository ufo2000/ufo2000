/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
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

#ifndef SOLDIER_H
#define SOLDIER_H
#include "pck.h"
#include "spk.h"
#include "bullet.h"
#include "item.h"
#include "place.h"
#include "skin.h"
#include "position.h"

struct UNITDATA
{
    unsigned char CurTU;        //!< The number of TUs the unit has currently
    unsigned char CurHealth;    //!< Current Health
    unsigned char CurStun;      //!< The stun level: if this is higher than CurHealth, they're stunned.
    unsigned char CurEnergy;    //!< Current Energy
    unsigned char CurReactions; //!< Current Reaction
    unsigned char CurStrength;  //!< Current Strength
    unsigned char CurFront;     //!< Current armor on the front
    unsigned char CurLeft;      //!< Current armor at the left side
    unsigned char CurRight;     //!< Current armor at the right side
    unsigned char CurRear;      //!< Current armor on the rear
    unsigned char CurUnder;     //!< Current armor at the bottom
    unsigned char CurFAccuracy; //!< current Firing Accuracy (might go down when wounded)
    unsigned char CurTAccuracy; //!< current Throwing Accuracy
    unsigned char MaxTU;        //!< The maximum number of TUs the unit has
    unsigned char MaxHealth;    //!< Maximum Health
    unsigned char MaxEnergy;    //!< Maximum Energy
    unsigned char MaxStrength;  //!< Maximum Strength
    unsigned char MaxFront;     //!< Maximum armor on the front
    unsigned char MaxLeft;      //!< Maximum armor at the left side
    unsigned char MaxRight;     //!< Maximum armor at the right side
    unsigned char MaxRear;      //!< Maximum armor on the rear
    unsigned char MaxUnder;     //!< Maximum armor at the bottom
    unsigned char MaxFA;        // ?
    unsigned char MaxTA;        // ?
    
    unsigned char Morale;
    unsigned char HeadWound;    //!< Number of fatal wounds in the head.
    unsigned char TorsoWound;
    unsigned char RArmWound;    //!< Right arm wounds
    unsigned char LArmWound;
    unsigned char RLegWound;
    unsigned char LLegWound;
    char          Name[26];     //!< The unit name!!
};

#define P_SHL_RIGHT       0
#define P_SHL_LEFT        1
#define P_ARM_RIGHT       2
#define P_ARM_LEFT        3
#define P_LEG_RIGHT       4
#define P_LEG_LEFT        5
#define P_BACK            6
#define P_BELT            7
#define NUMBER_OF_CARRIED_PLACES 8
#define P_MAP             8
#define P_ARMOURY         9
#define NUMBER_OF_PLACES  9
// Like ground, but accessible to all soldiers in mission-planner:
#define P_COMMON_POOL    10

//! maximum number of points a soldier can have
#define MAXPOINTS (7*60)

#define AUTO   0
#define SNAP   1
#define AIMED  2
#define APUNCH 0
#define ATHROW 1

#define DAMAGEDIR_FRONT      0
#define DAMAGEDIR_FRONTLEFT  1
#define DAMAGEDIR_LEFT       2
#define DAMAGEDIR_REARLEFT   3
#define DAMAGEDIR_REAR       4
#define DAMAGEDIR_REARRIGHT  5
#define DAMAGEDIR_RIGHT      6
#define DAMAGEDIR_FRONTRIGHT 7
#define DAMAGEDIR_UNDER      8

#define HEAL_COST	8
#define HEAL_TYPE_HEALTH 0
#define HEAL_TYPE_STUN 1
#define HEAL_TYPE_ENERGY 2
#define HEAL_TYPE_MORALE 3

enum State { SIT = 0, STAND, MARCH, FALL, LIE };

/**
 * A class which describes unit with all his equipment, health status and
 * position on map 
 *
 * @ingroup battlescape
 */
class Soldier: public persist::BaseObject
{
    DECLARE_PERSISTENCE(Soldier);
private:
    friend class Map;
    friend class Platoon;
    friend class Inventory;
    friend class Icon;
    friend class Item;
    friend class Editor;
    friend class Skin;
public:
    MANDATA md;
    ITEMDATA id;
    int z, x, y;
private:
    UNITDATA ud;

    static int dir2ofs[8];
    static char ofs2dir[3][3];

    Soldier *m_next, *m_prev;
    Bullet *m_bullet;
    Place  *m_place[NUMBER_OF_PLACES]; //8 - for internal editor use
    Platoon *m_platoon;
    Skin *m_skin;
    bool m_p_map_allocated;
    unsigned char m_ReserveTimeMode;

    int NID;
    int dir;    //!< direction of the face
    int move_dir;    //!< direction of the movement
    int phase;
    State m_state;	//!< Movement state

//! Each soldier in a platoon has a unique vision mask
//! This bit is written to the index vision matrix when a soldier can see that cell
    int32 m_vision_mask;            

//! Information about seen enemies. 
    int32 m_visible_enemies;

//! This flag is set after soldier has moved. 
//! Needed for correct work of 'switch to next soldier'-button
    int MOVED;

//! If we get stunned, this points to the corpse.
    Item *m_stunned_body;

//! The number of shots to be done and aiming point.
//! Soldier cannot be saved before all shots have been processed.
    int FIRE_num;
    int FIRE_z, FIRE_x, FIRE_y;

//! Number of chances for reaction fire enemy soldiers get
    int m_reaction_chances; 

//! Information for pathfinding. It does not need to be saved and can be built
//! by calling wayto function. If waylen != 0 this soldier is currently moving
    char way[100];
    int curway, waylen;

    bool panicking;

    void berserk_fire();
    int calc_z();
    void standard_aiming(int za, int xa, int ya);
    void precise_aiming(int za, int xa, int ya);    

public:
    //! Flag that sets a unit as stunned when the game starts (I.E. same square as another unit on game start)
    bool stun_on_init;
    //! set all stats to minimum and remove all inventory
    void reset_stats();
    //! set attribute value (when initializing soldier stats)
    bool set_attribute(const char *attribute_name, int value);
    //! set name for soldier
    bool set_name(const char *newname);
    //! set skin information
    bool set_skin_info(int skin_type, int female, int appearance);
    //! get pointer to body part by name
    Place *find_place(const char *place_name);
    
    //! get name of a soldier
    const char *get_name() { return ud.Name; }

    //! get number of items in equipment and a list of pointers
    int get_inventory_list(std::vector<Item *> &items);

    static void initpck();
    static void freepck() { Skin::freepck(); }

    Soldier(Platoon *platoon, int _NID);
    Soldier(Platoon *platoon, int _NID, int _z, int _x, int _y, MANDATA *sdat, ITEMDATA *idat, DeployType dep_type,int32 vision_mask);
    virtual ~Soldier();

    void initialize();

    void set_reserve_type(int type); //used to set restime from icon
    int get_reserve_type() {return m_ReserveTimeMode;};

    void process_MANDATA();
    void show_MANDATA(int gx, int gy, int gcol);
    void process_ITEMDATA();
    void build_ITEMDATA();
    void build_items_stats(char *buf, int &len);

    Platoon *get_platoon() {return m_platoon;}
    int get_NID() {return NID;}

    void restore_moved();
    void restore();
    int move(int ISLOCAL);

    void draw() { m_skin->draw(); }
    void draw_inventory(BITMAP *dest);
    void draw_unibord(int abs_pos, int posx, int posy);
    void draw_selector(int select_y);
    void showspk(BITMAP *dest);
    //void showspk() { showspk(screen2); }
    void drawinfo(int x, int y);
    void draw_stats(BITMAP* bitmap, int x, int y, bool selected);
    void draw_bullet_way();
    //void draw_enemy_seen(int select_y);

    void turnto(int destdir);
    void wayto(int dest_lev, int dest_col, int dest_row);
    void finish_march(int ISLOCAL);
    bool use_elevator(int dz);
    void break_march();
    bool faceto(int dest_col, int dest_row);
    int dirto(int src_col, int src_row, int dest_col, int dest_row);
    int open_door();
    int change_pose();
    int standup();
    int prime_grenade(int iplace, int delay_time, int req_time);
    int unload_ammo(Item *it);
    int load_ammo(int iplace, int srcplace, Item *&it);
    void die();
    void stun();
    void fall_stun(); //Used when a unit falls on this one. Stuns the unit for the round without affecting armor.

    void apply_accuracy(REAL &fi, REAL &te);
    void apply_throwing_accuracy(REAL &fi, REAL &te, int weight);
    int required(int pertime);
        int eff_FAccuracy();
    int FAccuracy(int peraccur, int TWOHAND);
    int TAccuracy(int peraccur);

    void try_shoot();
    void try_reaction_shot(Soldier *the_target);
    void shoot(int tlev, int tcol, int trow, int ISLOCAL);
    int assign_target(Action action, int iplace);
    int do_target_action(int z0, int x0, int y0, int zd, int xd, int yd, Action action, int iplace);
    int thru(int z0, int x0, int y0, int zd, int xd, int yd, int iplace, int req_time);
    int beam(int z0, int x0, int y0, int zd, int xd, int yd, int iplace, int req_time);
    int fire(int z0, int x0, int y0, int zd, int xd, int yd, int iplace, int req_time);
    int punch(int z0, int x0, int y0, int zd, int xd, int yd, int iplace, int req_time);
    int do_heal(int z0, int x0, int y0, int zd, int xd, int yd, int iplace, int req_time);
	int aimedthrow(int z0, int x0, int y0, int zd, int xd, int yd, int iplace, int req_time);

    int check_for_hit(int _z, int _x, int _y);
    
	void heal_wounds(int, int);
	void heal_energy_stun(int, int, int, int);
	void heal_morale(int, int);
	
	void apply_hit(int sniper, int _z, int _x, int _y, int _type, int _hitdir);
    int do_armour_check(int &pierce, int damdir);
    void apply_wound(int hitloc);
    void hit(int sniper, int pierce, int type, int hitdir, int dam_dev = 50);
    void explo_hit(int sniper, int pierce, int type, int hitdir);      //silent

    int ismoving();
    int is_marching() { return (m_state == MARCH); }
    int havetime(int ntime, int use_energy = 0);
    int time_reserve(int walk_time, int ISLOCAL, int use_energy = 1);
    void spend_time(int tm, int use_energy = 0);
    int walktime(int _dir);
    int tus_reserved(std::string *error = NULL);
	int get_dir() { return dir; }
    State state() { return m_state; }

    void unlink();
    Soldier *nextman();
    Soldier *prevman();
    Soldier *next() { return m_next; }
    Soldier *prev() { return m_prev; }
    void set_next(Soldier *s) { m_next = s; }
    void set_prev(Soldier *s) { m_prev = s; }
    Bullet *bullet() { return m_bullet; }
    Skin *skin() { return m_skin; }

    void calc_bullet_start(int xs, int ys, int zs, int* xr, int* yr, int *zr); //calculates starting position for a bullet
        void calc_shot_stat(int zd, int xd, int yd);

    bool is_active()
    {
        return !is_dead() && !is_stunned() && z != -1;
    }

    bool is_dead()
    {
        return ud.CurHealth == 0;
    }

    bool is_stunned()
    {
        return ud.CurStun >= ud.CurHealth;
    }
    
    bool is_panicking()
    {
        return panicking;
    }

    Soldier *next_active_soldier()
    {
        Soldier *s = m_next;
        while (s != NULL) {
            if (s->is_active())
                return s;
            s = s->m_next;
        }
        return NULL;
    }

    Soldier *prev_active_soldier()
    {
        Soldier *s = m_prev;
        while (s != NULL) {
            if (s->is_active())
                return s;
            s = s->m_prev;
        }
        return NULL;
    }

    int32 get_vision_mask(){return m_vision_mask;}
    void set_visible_enemies(int32 visible_enemies) {m_visible_enemies=visible_enemies;}
    int32 get_visible_enemies() {return m_visible_enemies;}

    int check_reaction_fire(Soldier *the_target);
    int do_reaction_fire(Soldier *the_target, int place, int shot_type);

    int eot_save(char *txt);

    int count_weight();
    int has_forbidden_equipment();
	int has_twohanded_weapon();

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    static int calc_mandata_cost(MANDATA _md);
    int calc_ammunition_cost() { return calc_mandata_cost(md); }
    int calc_full_ammunition_cost();
    Item *item_under_mouse(int ipl, int scr_x, int scr_y);
    int calctime(int src, int dst);
    Item *select_item(int &pn, int scr_x, int scr_y);
    int deselect_item(Item *&it, int it_place, int &req_time, int scr_x, int scr_y);
    Place *find_item(Item *it, int &lev, int &col, int &row);
    int find_place_coords(Place *pl, int &lev, int &col, int &row);
    int haveitem(Item *it);

	/** Return item in left hand. */
    Item *lhand_item() { return m_place[P_ARM_LEFT]->item(); }
    /** Return item in right hand. */
    Item *rhand_item() { return m_place[P_ARM_RIGHT]->item(); }

	/** Return the item at the specific place.
		It stays in the place! */
    Item *item(int ip)
    {
        ASSERT((ip >= 0) && (ip < NUMBER_OF_PLACES));
        return m_place[ip]->item();
    }

	/** Return the item at the specific place and position.
		It stays in the place! */
    Item *item(int ip, int ix, int iy)
    {
        ASSERT((ip >= 0) && (ip < NUMBER_OF_PLACES));
        return m_place[ip]->item(ix, iy);
    }

	/** Remove and return item from place.
		@return The item that was there. It gets deleted from the place. */
    Item *getitem(int ip, int ix, int iy)
    {
        ASSERT((ip >= 0) && (ip < NUMBER_OF_PLACES));
        return m_place[ip]->get(ix, iy);
    }

	/** Put an item to a place, to the given position */
    int putitem(Item *it, int ip, int ix = -1, int iy = -1)
    {
        ASSERT((ip >= 0) && (ip < NUMBER_OF_PLACES));
        if (ix == -1 || iy == -1)
            return m_place[ip]->put(it);
        else
            return m_place[ip]->put(it, ix, iy);
    }
	
    int place(Place *place);
    
    /** return a place at given index. */
    Place *place(int ip)
    {
        ASSERT((ip >= 0) && (ip < NUMBER_OF_PLACES));
        return m_place[ip];
    }
    
    Item *get_stunned_body() { return m_stunned_body; }

    void destroy_all_items();
    void draw_deselect_times(BITMAP *dest, Item *sel_item, int sel_item_place);
    void damage_items(int damage);
    
    void panic(int action);
    void change_morale(int delta);

    void set_start_sit() {m_state=SIT;};
    
    void save_to_string(std::string &str);

    virtual bool Write(persist::Engine &archive) const;
    virtual bool Read(persist::Engine &archive);

    int can_fly();
    int is_flying();
};

#endif
