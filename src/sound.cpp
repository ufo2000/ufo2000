#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <cstring>

#include <expat.h>
#include "allegro.h"

#include "sound.h"


struct sound_sym_maptable_t {
    SoundSym_e_t symcode;
    const char * const sym;
};

/** static mapping between enum values and sym attribute values */
static const sound_sym_maptable_t KNOWN_SYMS[] = {
    
{ SS_UNKNOWN, "unknown" }, /* unknown samples */
{ SS_UNUSED, "unused" },  /* empty, broken or otherwise unused samples */

/* interface */
{ SS_WINDOW_OPEN_1, "window-open-1" },
{ SS_WINDOW_OPEN_2, "window-open-2" },
{ SS_WINDOW_CLOSE_1, "window-close-1" },
{ SS_WINDOW_CLOSE_2, "window-close-2" },
{ SS_BUTTON_PUSH_1, "button-push-1" },
{ SS_BUTTON_PUSH_2, "button-push-2" },

/* inventory */
{ SS_CLIP_LOAD, "clip-load" },
{ SS_CLIP_EJECT, "clip-eject" },
{ SS_ITEM_GET, "item-get" },
{ SS_ITEM_PUT, "item-put" },
{ SS_ITEM_DROP, "item-drop" },

/* conventional weapons */
{ SS_CV_PISTOL_SHOT, "cv-pistol-shot" },
{ SS_CV_RIFLE_SHOT, "cv-rifle-shot" },
{ SS_CV_BULLET_BOUNCE, "cv-bullet-bounce" },
{ SS_CV_BULLET_HIT, "cv-bullet-hit" },
{ SS_CV_HWP_SHOT, "cv-HWP-shot" },
{ SS_CV_CRAFT_SHOT, "cv-craft-shot" },
{ SS_CV_HEAVY_CANNON_SHOT, "cv-heavy-cannon-shot" },
{ SS_CV_AUTO_CANNON_SHOT, "cv-auto-cannon-shot" },
{ SS_CV_CANNON_AP_HIT, "cv-cannon-AP-hit" },
{ SS_CV_CANNON_HE_HIT, "cv-cannon-HE-hit" },
{ SS_CV_CANNON_IN_HIT, "cv-cannon-IN-hit" },

/* melee weapons */
{ SS_KNIFE_PUNCH, "knife-punch" },
{ SS_KNIFE_THROW, "knife-throw" },
{ SS_KNIFE_HIT, "knife-hit" },
{ SS_KASTET_PUNCH, "kastet-punch" },
{ SS_STUNROD_HIT, "stunrod-hit" },
{ SS_BLUNT_HIT, "blunt-hit" },

/* rocket launchers */
{ SS_RL_LAUNCH, "rl-launch" },
{ SS_HWP_LAUNCH, "HWP-launch" },
{ SS_HWP_BLASTER_LAUNCH, "HWP-blaster-launch" },
{ SS_ALIEN_SMALL_LAUNCH, "alien-small-launch" },
{ SS_ALIEN_BLASTER_LAUNCH, "alien-blaster-launch" },
{ SS_SMALL_ROCKET_HIT, "small-rocket-hit" },
{ SS_HE_ROCKET_HIT, "HE-rocket-hit" },
{ SS_IN_ROCKET_HIT, "IN-rocket-hit" },
{ SS_HWP_ROCKET_HIT, "HWP-rocket-hit" },
{ SS_HWP_BLASTER_HIT, "HWP-blaster-hit" },
{ SS_ALIEN_SMALL_HIT, "alien-small-hit" },
{ SS_ALIEN_BLASTER_HIT, "alien-blaster-hit" },

/* laser weapons */
{ SS_LASER_PISTOL_SHOT, "laser-pistol-shot" },
{ SS_LASER_RIFLE_SHOT, "laser-rifle-shot" },
{ SS_LASER_HEAVY_SHOT, "laser-heavy-shot" },
{ SS_LASER_HWP_SHOT, "laser-HWP-shot" },
{ SS_LASER_CANNON_SHOT, "laser-cannon-shot" },
{ SS_LASER_CRAFT_SHOT, "laser-craft-shot" },

/* plasma weapons */
{ SS_PLASMA_PISTOL_SHOT, "plasma-pistol-shot" },
{ SS_PLASMA_RIFLE_SHOT, "plasma-rifle-shot" },
{ SS_PLASMA_HEAVY_SHOT, "plasma-heavy-shot" },
{ SS_PLASMA_HWP_SHOT, "plasma-HWP-shot" },
{ SS_PLASMA_CYBERDISK_SHOT, "plasma-cyberdisk-shot" },
{ SS_PLASMA_SECTOPOD_SHOT, "plasma-sectopod-shot" },
{ SS_PLASMA_CANNON_SHOT, "plasma-cannon-shot" },
{ SS_PLASMA_CRAFT_SHOT, "plasma-craft-shot" },
{ SS_PLASMA_HIT, "plasma-hit" },

/* grenades */
{ SS_GRENADE_FALL, "grenade-fall" },
{ SS_CV_GRENADE_BANG, "cv-grenade-bang" },
{ SS_SMOKE_GRENADE_BANG, "smoke-grenade-bang" },
{ SS_HIGH_EXPLOSIVE_BANG, "high-explosive-bang" },
{ SS_PROXY_GRENADE_BANG, "proxy-grenade-bang" },
{ SS_ALIEN_GRENADE_BANG, "alien-grenade-bang" },

/* movement */
{ SS_STEP_HUMAN, "step-human" },
{ SS_STEP_HUMAN_METAL, "step-human-metal" },
{ SS_STEP_SECTOID, "step-sectoid" },
{ SS_STEP_SECTOID_METAL, "step-sectoid-metal" },
{ SS_STEP_MUTON, "step-muton" },
{ SS_STEP_MUTON_METAL, "step-muton-metal" },
{ SS_STEP_ETHEREAL, "step-ethereal" },
{ SS_STEP_ETHEREAL_METAL, "step-ethereal-metal" },
{ SS_STEP_REAPER, "step-reaper" },
{ SS_STEP_REAPER_METAL, "step-reaper-metal" },
{ SS_STEP_SNAKE, "step-snake" },
{ SS_STEP_HOVER, "step-hover" },
{ SS_STEP_ALIEN_HOVER, "step-alien-hover" },
{ SS_DOOR_CLICK, "door-click" },
{ SS_ALIEN_EXT_DOOR, "alien-ext-door" },
{ SS_ALIEN_INT_DOOR, "alien-int-door" },

/* growls */
{ SS_MALE_WOUND, "male-wound" },
{ SS_MALE_DEATH, "male-death" },
{ SS_FEMALE_WOUND, "female-wound" },
{ SS_FEMALE_DEATH, "female-death" },
{ SS_HWP_DEATH, "HWP-death" },
{ SS_SECTOID_WOUND, "sectoid-wound" },
{ SS_SECTOID_DEATH, "sectoid-death" },
{ SS_MUTON_WOUND, "muton-wound" },
{ SS_MUTON_DEATH, "muton-death" },
{ SS_SILACOID_WOUND, "silacoid-wound" },
{ SS_SILACOID_DEATH, "silacoid-death" },
{ SS_CELATID_WOUND, "celatid-wound" },
{ SS_CELATID_DEATH, "celatid-death" },
{ SS_FLOATER_WOUND, "floater-wound" },
{ SS_FLOATER_DEATH, "floater-death" },
{ SS_REAPER_WOUND, "reaper-wound" },
{ SS_REAPER_DEATH, "reaper-death" },
{ SS_SNAKEMAN_WOUND, "snakeman-wound" },
{ SS_SNAKEMAN_DEATH, "snakeman-death" },
{ SS_CHRYSSALID_WOUND, "chryssalid-wound" },
{ SS_CHRYSSALID_DEATH, "chryssalid-death" },
{ SS_ETHEREAL_WOUND, "ethereal-wound" },
{ SS_ETHEREAL_DEATH, "ethereal-death" },


{ SS_TRAILER__SYM, "SS_TRAILER__SYM" }
};

static SoundSym_e_t getSymCode(const char *const sym) {
    for (unsigned i = 0 ; i < sizeof(KNOWN_SYMS); i++) {
        if ( 0 == strcmp(sym, KNOWN_SYMS[i].sym) )
            return KNOWN_SYMS[i].symcode;
    }
    return SS_UNKNOWN;
}

static const char *getSymString(const SoundSym_e_t symcode) {
    for (unsigned i = 0 ; i < sizeof(KNOWN_SYMS); i++) {
        if ( symcode == KNOWN_SYMS[i].symcode)
            return KNOWN_SYMS[i].sym;
    }
    return NULL;
}


enum cat_file_type_e_t {
    CT_ERROR,
    CT_ORIGINAL,
    CT_CE,
    CT_UNKNOWN
};

/** used for loading original and CE sound files */
class soundFile {
    std::vector<SAMPLE *> samples;
    std::string fileName;
    
    void loadOrigCat(const std::string& buf, std::ostream& log, bool v = false);
    void loadCeCat(const std::string& buf, std::ostream& log, bool v = false);
    cat_file_type_e_t getFileType(const std::string& buf);
    
public:
    soundFile();
    ~soundFile();

    int loadFile(const char *filename, std::ostream& log, bool verbose = false);
    SAMPLE *fetchSample(int idx); 
};


/*
XCOM Golden (collectors? not sure) edition sound/sample*.cat file format

First block: offset/length table.
For each sample two int32 values: offset and sample length.
Table length can be determined by the first value, which is effectively the
length of the table.

At each offset there is:
1. byte, number of 'data' bytes that follow
2. 'data' bytes of unknown meaning,
then length bytes in RIFF WAVE format. All samples are :
(the format is somewhat inconsistent).

RIFF (little-endian) data, WAVE audio, Microsoft PCM, 8 bit, mono 11025 Hz

according to 'file' utility.

there are some short samples, each has length = 206 and 2 data bytes.
All other samples have 3 data bytes.

This program usage:

extract_samples <path to sample file> [<some arg>]

If <some arg> is given (actual value is ignored), then all samples
are extracted into current directory with names sample_%d.wav,
where %d is sample's number in .cat file.

*/


/* pack_*() converted to work with std::string ref */

static short buf_igetw(const std::string& inbuf, unsigned *curp) {
    short rval;
    if (*curp + 2 > inbuf.size())
        return EOF;
    
/* TODO: fixup endianness. */    
    inbuf.copy((char *)&rval, *curp, 2);
    *curp += 2;
    
    return rval;
}

static int buf_igetl(const std::string& inbuf, unsigned *curp) {
    int rval;
    if (*curp + 4 > inbuf.size() )
        return EOF;
    
/* TODO: fixup endianness. */
    inbuf.copy((char *)&rval, *curp, 4);
    *curp += 4;
    
    return rval;
}

static int buf_getc(const std::string& inbuf, unsigned *curp) {
    char rval;
    if (*curp + 1 > inbuf.size())
        return EOF;
    
    rval = inbuf.at(*curp);
    *curp += 1;
    
    return rval;
}

static int buf_fread(void *buf, unsigned len, const std::string& inbuf, unsigned *curp) 
{
    if (*curp + len > inbuf.size())
        return EOF;
    inbuf.copy((char *)buf, *curp, len);
    *curp += len;
    return len;
}

/* allegro's loadwav converted to operate on std::string ref. */

static SAMPLE *wav2sample(const std::string& inbuf, std::ostream& log, bool verbose = false)
{
    char buffer[25];
    int i;
    int length, len;
    int freq = 22050;
    int bits = 8;
    int channels = 1;
    signed short s;
    SAMPLE *spl = NULL;
    
    unsigned curp = 0;
    
    buf_fread(buffer, 12, inbuf, &curp);          /* check RIFF header */
    if (memcmp(buffer, "RIFF", 4) || memcmp(buffer+8, "WAVE", 4)) {
        if (verbose)
            log<<"Not RIFF WAVE.\n";
        curp = 0xFFFFFFFF; // hack around goto. 
    }

    while (inbuf.size() > curp ) {
        if (buf_fread(buffer, 4, inbuf, &curp) != 4) {
            if (verbose)
                log<<"Can't read chunk ID at offset "<<curp<<std::endl;
            break;
        }

        length = buf_igetl(inbuf, &curp);          /* read chunk length */

        if (memcmp(buffer, "fmt ", 4) == 0) {
            i = buf_igetw(inbuf, &curp);            /* should be 1 for PCM data */
            length -= 2;
            if (i != 1) {
                if (verbose)
                    log<<"Not PCM? (i = 0x"
                       <<std::hex<<std::setw(4)<<i<<")\n"<<std::dec;
                break;
            }
            
            channels = buf_igetw(inbuf, &curp);     /* mono or stereo data */
            length -= 2;
            if ((channels != 1) && (channels != 2)) {
                if (verbose)
                    log<<"Bogus num of channels: "<<channels<<"\n";
                break;
            }
            
            freq = buf_igetl(inbuf, &curp);         /* sample frequency */
            length -= 4;
            
            buf_igetl(inbuf, &curp);                /* skip six bytes */
            buf_igetw(inbuf, &curp);
            length -= 6;
            
            bits = buf_igetw(inbuf, &curp);         /* 8 or 16 bit data? */
            length -= 2;
            if ((bits != 8) && (bits != 16)) {
                if (verbose)
                    log<<"\nBogus sample size: "<<bits<<"\n";
                break;
            }
            if (verbose)
                log<<"Format: "<<channels<<" channel(s), "
                   <<" bits, "<<bits<<" Hz"<<freq<<"\n";
        }
        else if (memcmp(buffer, "data", 4) == 0) {
            /* printf("Data: %d bytes\n", length); */
            len = length / channels;
            
            if (bits == 16)
            len /= 2;
            
            spl = create_sample(bits, ((channels == 2) ? TRUE : FALSE), freq, len);
            
            if (spl) {
                *allegro_errno = 0;
                if (bits == 8) {
                    if (buf_fread(spl->data, length, inbuf, &curp) != length) {
                        if (verbose)
                            log<<"Can't read "<<length<<" bytes of data ("
                               <<(inbuf.size() - curp)<<" left)\n", 
                        *allegro_errno = 1;
                    }
                }
                else {
                   for (i=0; i<len*channels; i++) {
                        s = buf_igetw(inbuf, &curp);
                        ((signed short *)(spl->data))[i] = s^0x8000;
                   }
                }
                
                length = 0;
                
                if (*allegro_errno) {
                    if (verbose)
                        printf("allegro_errno: %d.\n", *allegro_errno);
                   destroy_sample(spl);
                   spl = NULL;
                }
            }
        }

        while (length > 0) {             /* skip the remainder of the chunk */
            if (buf_getc(inbuf, &curp) == EOF)
                break;
            length--;
        }
    }

    return spl;
}
/* Original UFO samples have values from 0 to 0x3f. 
This suggests that the game had 3 volume levels, which were achieved 
by shifting the sample value 0, 1 or 2 positions.
This code unconditionally shifts the sample 2 positions to achieve maximum
volume. */

SAMPLE *raw2sample(const std::string& buf) {
    SAMPLE *spl = create_sample(8, 0, 11025, buf.size());      
/* above returns NULL if len == 0 */
    if (spl == NULL)
        return NULL;
    
/* copy data */
    buf.copy(static_cast<char *>(spl->data), buf.size());
    
/* adjust volume */
    for (unsigned i = 0; i < buf.size(); i++) 
        (static_cast<unsigned char *>(spl->data))[i] = 
            (static_cast<unsigned char *>(spl->data))[i] << 2;
    
    return spl;
}

/* soundFile implementation */

soundFile::soundFile() { }

soundFile::~soundFile() { }

int soundFile::loadFile(const char *fname, std::ostream& log, bool verbose) {
    std::ifstream ifs(fname);
    
    if (!ifs)
        return -1;

    std::stringstream sbuf;
    sbuf<<ifs.rdbuf();
    
    switch(getFileType(sbuf.str())) {
        case CT_ORIGINAL:
            loadOrigCat(sbuf.str(), log, verbose);
            break;
        case CT_CE:
            loadCeCat(sbuf.str(), log, verbose);
            break;
        default:
            log<<"Unknown file type\n";
            return -1;
    }
    
    return 0;
    
}

SAMPLE * soundFile::fetchSample(int idx) {
//    return samples.at(idx);
    return samples[idx];
}


/** try to determine if this is an CE file or not. */
cat_file_type_e_t soundFile::getFileType(const std::string& buf) {
    int i;
    int offs;
    const int head_len = 10;
    char sbuf[head_len];

    buf.copy((char *)&offs, 4, 0); /* TODO: will break on big-endian arch. */
    buf.copy(sbuf, head_len, offs);
    
/* 
 * Assuming this is CE file, scan first head_len bytes for RIFF sequence.
 * Declare original file otherwise.  
 *
 */
    
    for (i=0; i<head_len - 4; i++) 
        if ( 0 == strcmp(sbuf+i, "RIFF"))
            return CT_CE; /* gotcha, this is CE file */
    
    return CT_ORIGINAL; 
}

/* load Gold Edition .cat file */
void soundFile::loadCeCat(const std::string& buf, std::ostream& log, bool verbose) {
    int i, count, nsamples;

    buf.copy((char *)&nsamples, 4, 0); 
/* TODO: fixup endianness. */
    nsamples /= 8;
    
    int offsets[nsamples + 1];
    int lengths[nsamples + 1];
    
    count = 0;
    do {
        buf.copy((char *)&(offsets[count]), 4, 4 * (2 * count ));
        buf.copy((char *)&(lengths[count]), 4, 4 * (2 * count +1));
/* TODO: fixup endianness. */
        count++;
    } while(count < nsamples);
    
    offsets[count] = buf.size();
    lengths[count] = 0;

    std::ios::fmtflags oldFlags = log.flags();

    for (i=0; i<nsamples; i++) {
        SAMPLE *smpl;
        int datbytes;

        /* seems like the first datbyte is actually datbytes length: 
           it is always 1 or 2, and the same number of datbytes follow. */
        if (verbose) 
            log<<"#"<<std::setw(2)<<std::setfill(' ')<<std::dec<<i
               <<": "<<std::setw(6)<<(lengths[i])
               <<" bytes from 0x"
               <<std::hex<<std::setfill('0')<<std::setw(5)<<(offsets[i])
               <<", data bytes:";
            
        if (lengths[i] > 0) {
            datbytes= buf.at(offsets[i]);
            
            if (verbose) {
                log<<std::setw(2);
                for (int di=offsets[i] + 1; di<offsets[i] + datbytes + 1; di++)
                    log<<" 0x"<< (static_cast<unsigned int>(buf.at(di)) & 0xFF);
                log<<"\n";
            }
    
            datbytes++; // include datlen
        
            if ( verbose && (lengths[i] + offsets[i] + datbytes != offsets[i+1])) 
                log<<" !!! warning: inconsistency: "
                       "next offset should be 0x"
                   <<(lengths[i] + offsets[i] + datbytes)
                   <<", is 0x"<<offsets[i+1]<<std::endl; 

            std::string chunk = buf.substr(offsets[i] + datbytes,lengths[i]);
            smpl = wav2sample(chunk, log, verbose);
            
            if (verbose && (smpl == NULL))
                log<<"Error reading wave.\n";
            
        } else {
            if (verbose)
                log<<" none.\n";
            smpl = NULL;
        }
        samples.push_back(smpl);
    }
    
    if (verbose)
        log.flags(oldFlags);
}

/* some data associated with the sample. */
static const int ORIG_SAMPLE_DATA_HEADER_LEN = 6;

/** Load original *.cat file */
void soundFile::loadOrigCat(const std::string& buf, std::ostream& log, bool verbose) {
    int i, count, nsamples;

    buf.copy((char *)&nsamples, 4, 0); 
/* TODO: fixup endianness. */
    nsamples /= 8;
    
    int offsets[nsamples + 1];
    int lengths[nsamples + 1];
    
    count = 0;
    do {
        buf.copy((char *)&(offsets[count]), 4, 4 * (2 * count ));
        buf.copy((char *)&(lengths[count]), 4, 4 * (2 * count +1));
/* TODO: fixup endianness. */
        count++;
    } while(count < nsamples);
    
    offsets[count] = buf.size();
    lengths[count] = 0;

    std::ios::fmtflags oldFlags = log.flags();
    
    for (i=0; i<nsamples; i++) {
        SAMPLE *smpl;

        if (verbose) 
            log<<"#"<<std::setw(2)<<std::setfill(' ')<<std::dec<<i
               <<": "<<std::setw(6)<<(lengths[i])
               <<" bytes from 0x"
               <<std::hex<<std::setfill('0')<<std::setw(5)<<(offsets[i])
               <<", data bytes:";
        
        if (lengths[i] > 0) {
            if (verbose) {
                log<<std::setw(2);
                for (int di=offsets[i]; 
                        di<offsets[i] + ORIG_SAMPLE_DATA_HEADER_LEN; di++)
                    log<<" 0x"<< (static_cast<unsigned int>(buf.at(di)) & 0xFF);
                log<<"\n";
                if (lengths[i] + offsets[i] != offsets[i+1]) 
                    log<<" !!! warning: inconsistency: "
                           "next offset should be 0x"
                       <<(lengths[i] + offsets[i])
                       <<", is 0x"<<offsets[i+1]<<std::endl; 
            }

            std::string chunk = 
                buf.substr(offsets[i] + ORIG_SAMPLE_DATA_HEADER_LEN, 
                            lengths[i] - ORIG_SAMPLE_DATA_HEADER_LEN);
            smpl = raw2sample(chunk);
            
            if (verbose && (smpl == NULL)) {
                log<<"Error reading raw."<<std::endl;
            }
        } else {
            if (verbose)
                log<<" none.\n";
            smpl = NULL;
        }
        samples.push_back(smpl);
    }
    if (verbose)
        log.flags(oldFlags);
    
}


/* Elements:

<sndmap> - one or more <file>, no attrs.
<file>  - one or more <sample>, attrs: name, md5sum (optional)
<sample> - empty, attrs: sym, priority (optional)

*/

/* XML parser state */
struct xmlp_baton_t {
    XML_Parser parser;
    soundSystem *zeSys;
    
    bool inSoundMap, inFile, inSample, skipThisFile; // parse state
    
    soundFile *current;
    int curSampleIndex; // index of current sample in current file
    SAMPLE *curSample;
    
    bool errorOccured, verbose;
    
    std::ostream *log;
};

/** helper function to get an attribute value of an element */
static XML_Char *h_getAttr(const XML_Char **atts, const char *name) 
{
    XML_Char **cur_p = (XML_Char **)atts, *a_name, *a_value;
    
    while (*cur_p != NULL) {
        a_name = *cur_p;
        cur_p++;
        a_value = *cur_p;
        cur_p++;
        if ( 0 == strcmp(a_name, name) )
            return a_value;
    }
    return NULL;
}

static void h_recordXMLStreamPos(std::ostream& os, XML_Parser xp) {
    int line = XML_GetCurrentLineNumber(xp);
    int column = XML_GetCurrentColumnNumber(xp);
    os<<" line "<<line<<", column "<<column<<" ";
}

/** XML start element parser callback */
static void h_StartEl(void *userData,
                        const XML_Char *name,
                        const XML_Char **atts ) 
{
    xmlp_baton_t *b = static_cast <xmlp_baton_t *>(userData);
    
    if (b->errorOccured) // skip the rest if error detected.
        return;
    
    if ( 0 == strcmp(name, "soundmap") ) {
        b->inSoundMap = true;
        return;
    }
    
    if ( 0 == strcmp(name, "file") ) {
        XML_Char *fname;
        if (b->current != NULL)
            delete b->current;
        b->current = new soundFile();
        if (NULL == (fname = h_getAttr(atts, "name"))) {
            b->errorOccured = true;
            *b->log<<"No 'name' attribute in file element ";
            h_recordXMLStreamPos(*b->log, b->parser);
            
            return;
        }
        *b->log<<"Loading '"<<fname<<"'\n";
        if (b->current->loadFile(fname, *b->log, b->verbose) != 0) {
            b->skipThisFile = true;
            *b->log<<"can not load, skipping.\n";
            return;
        }
        b->inFile = true;
        return;
    }
    
    if ( 0 == strcmp(name, "sample") ) {
        XML_Char *sym;
        SAMPLE *sample;
        SoundSym_e_t soundSym;
        b->inSample = true;
        
        if (NULL == (sym = h_getAttr(atts, "sym"))) {
            b->errorOccured = true;
            *b->log<<"No 'sym' attribute in sample element";
            h_recordXMLStreamPos(*b->log, b->parser);
            return;
        }
        soundSym = getSymCode(sym);
        if ((soundSym == SS_UNKNOWN) || (soundSym == SS_UNUSED))
            return;
        if (NULL == (sample = b->current->fetchSample(b->curSampleIndex))) {
            b->errorOccured = true;
            *b->log<<"Extraneous <sample> sym='"<<sym<<"' ";
            h_recordXMLStreamPos(*b->log, b->parser);
            return;
        }
        b->zeSys->setSample(soundSym, sample);
        b->curSample = sample;
        return;
    }

    if ( 0 == strcmp(name, "alias") ) {
        XML_Char *sym;
        SoundSym_e_t soundSym;
        
        if (!b->inSample) {
            b->errorOccured = true;
            *b->log<<"alias element outside sample element";
            h_recordXMLStreamPos(*b->log, b->parser);
            return;
        }
        
        if (NULL == (sym = h_getAttr(atts, "sym"))) {
            b->errorOccured = true;
            *b->log<<"No 'sym' attribute in alias element";
            h_recordXMLStreamPos(*b->log, b->parser);
            return;
        }
        soundSym = getSymCode(sym);
        if ((soundSym == SS_UNKNOWN) || (soundSym == SS_UNUSED))
            return;
        b->zeSys->setSample(soundSym, b->curSample);
        return;
    }
    
    
    b->errorOccured = true;
    *b->log<<"Unknown element '"<<name<<"'"<<std::endl;
    h_recordXMLStreamPos(*b->log, b->parser);
}

/** XML end element parser callback */
static void h_EndEl(void *userData, const XML_Char *name) 
{
    xmlp_baton_t *b = static_cast <xmlp_baton_t *>(userData);
    
    if ( 0 == strcmp(name, "soundmap") ) {
        b->inSoundMap = false;
        return;
    }
    
    if ( 0 == strcmp(name, "file") ) {
        b->inFile = false;
        if (!b->skipThisFile)
            *b->log << "loaded ok.\n";
        else
            b->skipThisFile = false;
        b->curSampleIndex = 0;
        return;
    }
    
    if ( 0 == strcmp(name, "sample") ) {
        b->inSample = false;
        b->curSample = NULL;
        b->curSampleIndex++;
        return;
    }
}

/* soundSystem implementation */

soundSystem::soundSystem() { }
//~soundSystem::soundSystem() { }

int soundSystem::initialize(const std::string& xml, std::ostream *log, bool verbose) {
    xmlp_baton_t baton;
    std::stringstream *backup_log = NULL;
    
    if (log == NULL) { // install a proxy stream to steer clear of segfaults.
        backup_log = new std::stringstream;
        log = backup_log;
    }
    
    reserve_voices(4, -1);

    soundInstalled = false;
    
	if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) != 0) {
		*log<<"Error initialising sound system: "<<allegro_error<<std::endl;
        return -1;
    }
    soundInstalled = true;

    set_volume_per_voice(0);
    set_volume(255,255);

    theSamples.assign(sizeof(KNOWN_SYMS), NULL);

    baton.parser = XML_ParserCreate((XML_Char *)NULL);
    baton.zeSys = this;
    baton.current = NULL;
    baton.inSoundMap = false;
    baton.inFile = false;
    baton.errorOccured = false;
    baton.skipThisFile = false;
    baton.log = log;
    baton.verbose = verbose;
    baton.curSample = NULL;
    baton.curSampleIndex = 0;
    
    XML_SetElementHandler(baton.parser, h_StartEl, h_EndEl);
    XML_SetUserData(baton.parser, &baton);
    XML_Parse(baton.parser, xml.data(), xml.size(), 1);
    
    if (XML_GetErrorCode(baton.parser) != XML_ERROR_NONE) {
        *log<<"XML parse error: "<<XML_ErrorString(XML_GetErrorCode(baton.parser))
           <<" at line "<<XML_GetCurrentLineNumber(baton.parser)
           <<" col "<<XML_GetCurrentColumnNumber(baton.parser)<<std::endl;
        return -1;
    }
    
    XML_ParserFree(baton.parser);
    
    if (backup_log!= NULL)
        delete backup_log;
    
    return 0;
}

void soundSystem::shutdown() {
    if (soundInstalled) {
        remove_sound(); 
        soundInstalled = false;
    }
}

void soundSystem::setSample(SoundSym_e_t sndSym, SAMPLE * sample) {
    theSamples[sndSym] = sample;
}

void soundSystem::play(SoundSym_e_t sndSym, int vol, int pan, int freq) {
    if (!soundInstalled)
        return;
    
    SAMPLE *spl = theSamples[sndSym];
    
    if (spl == NULL)
        return;
    
    play_sample(spl, vol, pan, freq, 0);
}

soundSystem *soundSystem::theInstance = 0;

soundSystem *soundSystem::getInstance() {
    if (theInstance == 0) 
        theInstance = new soundSystem();
    
    return theInstance;
}

void soundSystem::getLoadedSyms(std::ostream *os) {
    for (unsigned i = 0; i != theSamples.size(); i++) 
        if (theSamples[i] != NULL)
            *os<<i<<": '"<<getSymString(static_cast<SoundSym_e_t> (i))<<"'\n";
}

void soundSystem::playLoadedSamples(std::ostream *os) {
    if (!soundInstalled)
        return;
    for (unsigned i = 0; i != theSamples.size(); i++) 
        if (theSamples[i] != NULL) {
            *os<<"Playing '"<<getSymString(static_cast<SoundSym_e_t> (i))<<"' (#"<<i
            <<", bits "<<(theSamples[i]->bits)
            <<" stereo "<<(theSamples[i]->stereo)
            <<" freq "<<(theSamples[i]->freq)
            <<" pri "<<(theSamples[i]->priority)
            <<" len "<<(theSamples[i]->len)<<")"<< std::endl;
            play(static_cast<SoundSym_e_t> (i));
            rest(1500); // sleep 1.5 secs to allow the sample to play.
        }
}
