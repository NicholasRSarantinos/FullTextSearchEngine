#include "stdafx.h"
#include <stdlib.h>  /* for malloc, free */
#include <string.h>  /* for memcmp, memmove */

#ifndef STEMMER_CODE
#define STEMMER_CODE

struct stemmer;

extern struct stemmer * create_stemmer(void);
extern void free_stemmer(struct stemmer * z);

extern int stem(struct stemmer * z, char * b, int k);

#endif