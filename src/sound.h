#ifndef SOUND_H
#define SOUND_H

/* MUST be in sync with KNOWN_SYMS in sound.cpp*/
enum SoundSym_e_t {
    SS_UNKNOWN,
    SS_UNUSED,
/* interface */
    SS_WINDOW_OPEN_1,
    SS_WINDOW_OPEN_2,
    SS_WINDOW_CLOSE_1,
    SS_WINDOW_CLOSE_2,
    SS_BUTTON_PUSH_1,
    SS_BUTTON_PUSH_2,
/* inventory */
    SS_CLIP_LOAD,
    SS_CLIP_EJECT,
    SS_ITEM_GET,
    SS_ITEM_PUT,
    SS_ITEM_DROP,
/* Conventional weapons */
    SS_CV_PISTOL_SHOT,
    SS_CV_RIFLE_SHOT,
    SS_CV_BULLET_BOUNCE,
    SS_CV_BULLET_HIT,
    SS_CV_HWP_SHOT,
    SS_CV_CRAFT_SHOT,
    SS_CV_HEAVY_CANNON_SHOT,
    SS_CV_AUTO_CANNON_SHOT,
    SS_CV_CANNON_AP_HIT,
    SS_CV_CANNON_HE_HIT,
    SS_CV_CANNON_IN_HIT,
/* melee weapons */
    SS_KNIFE_PUNCH,
    SS_KNIFE_THROW,
    SS_KNIFE_HIT,
    SS_KASTET_PUNCH,
    SS_STUNROD_HIT,
    SS_BLUNT_HIT,
/* rocket launchers */
    SS_RL_LAUNCH,
    SS_HWP_LAUNCH,
    SS_HWP_BLASTER_LAUNCH,
    SS_ALIEN_SMALL_LAUNCH,
    SS_ALIEN_BLASTER_LAUNCH,
    SS_SMALL_ROCKET_HIT,
    SS_HE_ROCKET_HIT,
    SS_IN_ROCKET_HIT,
    SS_HWP_ROCKET_HIT,
    SS_HWP_BLASTER_HIT,
    SS_ALIEN_SMALL_HIT,
    SS_ALIEN_BLASTER_HIT,
/* laser weapons */
    SS_LASER_PISTOL_SHOT,
    SS_LASER_RIFLE_SHOT,
    SS_LASER_HEAVY_SHOT,
    SS_LASER_HWP_SHOT,
    SS_LASER_CANNON_SHOT,
    SS_LASER_CRAFT_SHOT,
/* plasma weapons */
    SS_PLASMA_PISTOL_SHOT,
    SS_PLASMA_RIFLE_SHOT,
    SS_PLASMA_HEAVY_SHOT,
    SS_PLASMA_HWP_SHOT,
    SS_PLASMA_CYBERDISK_SHOT,
    SS_PLASMA_SECTOPOD_SHOT,
    SS_PLASMA_CANNON_SHOT,
    SS_PLASMA_CRAFT_SHOT,
    SS_PLASMA_HIT,
/* grenades */
    SS_GRENADE_FALL,
    SS_CV_GRENADE_BANG,
    SS_SMOKE_GRENADE_BANG,
    SS_HIGH_EXPLOSIVE_BANG,
    SS_PROXY_GRENADE_BANG,
    SS_ALIEN_GRENADE_BANG,
/* movement */
    SS_STEP_HUMAN,
    SS_STEP_HUMAN_METAL,
    SS_STEP_SECTOID,
    SS_STEP_SECTOID_METAL,
    SS_STEP_MUTON,
    SS_STEP_MUTON_METAL,
    SS_STEP_ETHEREAL,
    SS_STEP_ETHEREAL_METAL,
    SS_STEP_REAPER,
    SS_STEP_REAPER_METAL,
    SS_STEP_HOVER,
    SS_STEP_ALIEN_HOVER,
    SS_STEP_SNAKE,
    SS_DOOR_CLICK,
    SS_ALIEN_EXT_DOOR,
    SS_ALIEN_INT_DOOR,
/* growls */
    SS_MALE_WOUND,
    SS_MALE_DEATH,
    SS_FEMALE_WOUND,
    SS_FEMALE_DEATH,
    SS_HWP_DEATH,
    SS_SECTOID_WOUND,
    SS_SECTOID_DEATH,
    SS_MUTON_WOUND,
    SS_MUTON_DEATH,
    SS_SILACOID_WOUND,
    SS_SILACOID_DEATH,
    SS_CELATID_WOUND,
    SS_CELATID_DEATH,
    SS_FLOATER_WOUND,
    SS_FLOATER_DEATH,
    SS_REAPER_WOUND,
    SS_REAPER_DEATH,
    SS_SNAKEMAN_WOUND,
    SS_SNAKEMAN_DEATH,
    SS_CHRYSSALID_WOUND,
    SS_CHRYSSALID_DEATH,
    SS_ETHEREAL_WOUND,
    SS_ETHEREAL_DEATH,


    SS_TRAILER__SYM // just so we need not worry about commas when adding above
};

SoundSym_e_t getSymCode(const char *const sym);

class soundSystem {
    static soundSystem *theInstance;
    std::vector<SAMPLE *> theSamples;
    bool soundInstalled;

protected:
    soundSystem();

public:

    static soundSystem *getInstance();

    int initialize(const std::string& buf, std::ostream *log, bool verbose = false);
    void shutdown();
    void play(SoundSym_e_t sndSym, int vol = 255, int pan = 127, int freq = 1000);

    void setSample(SoundSym_e_t sndSym, SAMPLE * sample);

    void getLoadedSyms(std::ostream *os);
    void playLoadedSamples(std::ostream *os);
};


#endif
