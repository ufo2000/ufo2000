#include "pfxopen.h"
#include <string.h>
#include <allegro.h>
#include <stdlib.h>
#include <sys/stat.h>

//#include <unistd.h>

const char *origfiles_prefix;
const char *ownfiles_prefix;
const char *gametemp_prefix;
const char *runtemp_prefix;

static char *get_prefixed_name(const char *pathname, const char *pfx) {
	char *tmpbuf;
	int pfxlen, namelen;

	namelen = strlen(pathname);
	if (pfx != NULL) {
		pfxlen = strlen(pfx);
	} else {
		pfxlen = 0;
	}
	tmpbuf = (char *) malloc(namelen + pfxlen + 1);

	if (tmpbuf == NULL) {
		return NULL;
	}

	if (pfx != NULL) {
		memmove(tmpbuf, pfx, pfxlen);
	}
	memmove(tmpbuf + pfxlen, pathname, namelen);
	*(tmpbuf + pfxlen + namelen) = '\0';

	return tmpbuf;
}
int open_prefixed(const char *pathname, int flags, const char *pfx) {
	char *pname;
	int rval;

	pname = get_prefixed_name(pathname, pfx);
	if (pname == NULL) {
		return -1;
	}
	if (flags & O_CREAT) {
		rval = open(pname, flags, S_IRUSR | S_IWUSR);
	} else {
		rval = open(pname, flags);
	}

	free(pname);

	return rval;
}

FILE *fopen_prefixed(const char *pathname, const char *flags, const char *pfx) {
	char *pname;
	FILE *rval;

	pname = get_prefixed_name(pathname, pfx);
	if (pname == NULL) {
		return NULL;
	}

	rval = fopen(pname, flags);

	free(pname);

	return rval;
}

DATAFILE *loaddata_prefixed(const char *pathname, const char *pfx) {
	char *pname;
	DATAFILE *rval;

	pname = get_prefixed_name(pathname, pfx);
	if (pname == NULL) {
		return NULL;
	}

	rval = load_datafile(pname);

	free(pname);

	return rval;
}

BITMAP *loadbitmap_prefixed(const char *filename, RGB *pal, const char *pfx) {
	char *pname;
	BITMAP *rval;

	pname = get_prefixed_name(filename, pfx);
	if (pname == NULL) {
		return NULL;
	}

	rval = load_bitmap(pname, pal);

	free(pname);

	return rval;
}
