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
#include "global.h"
#include <stdio.h>
#include <vector>
#include <string>
#include "pfxopen.h"

/**
 * Table needed to check data files integrity
 */
struct { const char *mName; unsigned long mCrc32; } DataFilesCrc32[] =
{
    { "./geodata.dat", 0x00000000 },
    { "./soldier.dat", 0x00000000 },
    { "./armoury.set", 0x00000000 },
    { "./items.dat", 0x00000000 },
    { "./ufo2000.ini", 0x00000000 },
    { "./ufo2000.dat", 0x8EA4C104 },
    { "./keyboard.dat", 0xC796755E },
    { "./geodata/biglets.dat", 0x00000000 },
    { "./geodata/loftemps.dat", 0x7B354479 },
    { "./geodata/obdata.dat", 0x040A286A },
    { "./geodata/palettes.dat", 0x928554F0 },
    { "./geodata/scang.dat", 0x26C1BD1B },
    { "./geodata/smallset.dat", 0x00000000 },
    { "./geograph/back01.scr", 0xD071E645 },
    { "./geograph/back02.scr", 0xCA501299 },
    { "./geograph/back03.scr", 0x8098231A },
    { "./geograph/back09.scr", 0xF7A94FD1 },
    { "./geograph/back14.scr", 0x7AF63F1D },
    { "./ufograph/cursor.pck", 0x69DE75D6 },
    { "./ufograph/cursor.tab", 0xF5FCB568 },
    { "./ufograph/detblob.dat", 0x3569B6D3 },
    { "./ufograph/detbord.pck", 0xB1D72063 },
    { "./ufograph/detbord2.pck", 0xDE760A78 },
    { "./ufograph/hit.pck", 0x29C07717 },
    { "./ufograph/hit.tab", 0x4FF3EE8F },
    { "./ufograph/icons.pck", 0xD407EF95 },
    { "./ufograph/inticon.pck", 0x0BD62FDF },
    { "./ufograph/inticon.tab", 0x2FE8023E },
    { "./ufograph/man_0f0.spk", 0x9B75B5F8 },
    { "./ufograph/man_0f1.spk", 0xB199957A },
    { "./ufograph/man_0f2.spk", 0x8ED17367 },
    { "./ufograph/man_0f3.spk", 0x740442DF },
    { "./ufograph/man_0m0.spk", 0x18B23EF7 },
    { "./ufograph/man_0m1.spk", 0x1B90E3B2 },
    { "./ufograph/man_0m2.spk", 0x17F8E53B },
    { "./ufograph/man_0m3.spk", 0xBC57FC2E },
    { "./ufograph/man_1f0.spk", 0xF87390C0 },
    { "./ufograph/man_1f1.spk", 0x02CB471C },
    { "./ufograph/man_1f2.spk", 0x7E91A5E0 },
    { "./ufograph/man_1f3.spk", 0xF050BD04 },
    { "./ufograph/man_1m0.spk", 0xD919B6A5 },
    { "./ufograph/man_1m1.spk", 0x3AD7592B },
    { "./ufograph/man_1m2.spk", 0x2A0840E1 },
    { "./ufograph/man_1m3.spk", 0x47D7ACCB },
    { "./ufograph/man_2.spk", 0x890C3076 },
    { "./ufograph/man_3.spk", 0xA8BB9C04 },
    { "./ufograph/medibits.dat", 0x00DEF9F9 },
    { "./ufograph/medibord.pck", 0x871634C4 },
    { "./ufograph/scanbord.pck", 0xE3F32D10 },
    { "./ufograph/smoke.pck", 0x65CB2E8E },
    { "./ufograph/smoke.tab", 0xEE8E1054 },
    { "./ufograph/spicons.dat", 0xC71706D7 },
    { "./ufograph/tac00.scr", 0x226E61A0 },
    { "./ufograph/tac01.scr", 0xBBB1E67F },
    { "./ufograph/unibord.pck", 0x94576591 },
    { "./ufograph/x1.pck", 0x6CB3F43D },
    { "./ufograph/x1.tab", 0x606994D3 },
    { "./ufointro/pict2.lbm", 0x783E3C9D },
    { "./ufointro/pict4.lbm", 0x8178ADD1 },
    { "./ufointro/pict5.lbm", 0xA8543964 },
    { "./units/bigobs.pck", 0xBB5FB17D },
    { "./units/bigobs.tab", 0x422CC4A5 },
    { "./units/celatid.pck", 0x99C5D952 },
    { "./units/celatid.tab", 0xB450CFCB },
    { "./units/chrys.pck", 0xCA40A89D },
    { "./units/chrys.tab", 0xE5EA7925 },
    { "./units/civf.pck", 0xBA9955B6 },
    { "./units/civf.tab", 0x449DC508 },
    { "./units/civm.pck", 0x0E4AE803 },
    { "./units/civm.tab", 0x863CBE43 },
    { "./units/cyber.pck", 0xFC0BF3F5 },
    { "./units/cyber.tab", 0xFF00CFF0 },
    { "./units/ethereal.pck", 0x2E750A7C },
    { "./units/ethereal.tab", 0xE8227E28 },
    { "./units/floater.pck", 0xF195F43D },
    { "./units/floater.tab", 0xC3B2729A },
    { "./units/floorob.pck", 0xDF863538 },
    { "./units/floorob.tab", 0x833AC878 },
    { "./units/handob.pck", 0x3EE61F8E },
    { "./units/handob.tab", 0x6EFF9E62 },
    { "./units/muton.pck", 0xD7E169A3 },
    { "./units/muton.tab", 0x91FC80DE },
    { "./units/sectoid.pck", 0xF60F22DC },
    { "./units/sectoid.tab", 0x6831E47A },
    { "./units/silacoid.pck", 0xC6263195 },
    { "./units/silacoid.tab", 0x668C146F },
    { "./units/snakeman.pck", 0x729DC125 },
    { "./units/snakeman.tab", 0x497B94D8 },
    { "./units/tanks.pck", 0x5F3EFF2A },
    { "./units/tanks.tab", 0x4558D63B },
    { "./units/xcom_0.pck", 0x8681EF39 },
    { "./units/xcom_0.tab", 0x68930DB8 },
    { "./units/xcom_1.pck", 0xF03FB73B },
    { "./units/xcom_1.tab", 0xA3839667 },
    { "./units/xcom_2.pck", 0xC49CCBE5 },
    { "./units/xcom_2.tab", 0x528354F7 },
    { "./units/x_reap.pck", 0x6EF04CD0 },
    { "./units/x_reap.tab", 0x0469A2D9 },
    { "./units/x_rob.pck", 0x68F1F014 },
    { "./units/x_rob.tab", 0x63DA3656 },
    { "./units/zombie.pck", 0xF05C3A0B },
    { "./units/zombie.tab", 0x88323A4E }
};

//////////////////////////////////////////////////////////////////////////////
/// Table for calculating crc32                                            ///
//////////////////////////////////////////////////////////////////////////////

unsigned long Crc32Table[256] =
{
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
    0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
    0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
    0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
    0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
    0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
    0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
    0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
    0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
    0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
    0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
    0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
    0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
    0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
    0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
    0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
    0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
    0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
    0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
    0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
    0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
    0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

unsigned long update_crc32(unsigned long InitCrc, const void *pInBuff, unsigned long InLen)
{
	unsigned char *_InBuff = (unsigned char *)pInBuff;
	if (InLen != 0) {
		InitCrc = ~ InitCrc;
		while (InLen-- != 0) {
			InitCrc = (InitCrc >> 8) ^ Crc32Table[((unsigned char) InitCrc) ^ (*(_InBuff++))];
		}
		InitCrc = ~InitCrc;
	}
	return InitCrc;
}

/**
 * Function that checks CRC32 of file
 *
 * @param filename name of the file
 * @param crc32    value of CRC32 that file must have
 * @return         true if there are no problems with checked file
 *                 
 * @note           If crc32 argument of function is 0 then only file 
 *                 existence is checked but file content is ignored
 */
bool check_file_crc32(const char *filename, unsigned long crc32)
{
	char buffer[1024];
	FILE *f = FOPEN_ORIG(filename, "rb");
	if (f == NULL) f = FOPEN_OWN(filename, "rb");
	
	if (f == NULL) return false; 
	if (crc32 == 0)	{
		fclose(f);
		return true;
	}
	unsigned long file_crc32 = 0;
	while (true) {
		long size = fread(buffer, 1, sizeof(buffer), f);
		if (size == 0 || ferror(f)) break;
		file_crc32 = update_crc32(file_crc32, buffer, size);
	}
	fclose(f);
	return file_crc32 == crc32;
}


bool get_corrupted_or_missing_files(std::vector<std::string> &result)
{
	std::vector<std::string>::size_type i;
	result.clear();
	for (i = 0; i < sizeof(DataFilesCrc32) / sizeof(DataFilesCrc32[0]); i++)
		if (!check_file_crc32(DataFilesCrc32[i].mName, DataFilesCrc32[i].mCrc32))
			result.push_back(DataFilesCrc32[i].mName);
	return !result.empty();
}
