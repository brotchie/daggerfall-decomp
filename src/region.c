/* region.c */

#include "dagger.h"

/* between the region.c and daedra.c runs: unit not certain */
int climate_at(int x, int z) { return climate_lookup(x, z); }
