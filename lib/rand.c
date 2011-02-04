/*
  rand() implementation taken directly from:

  `Some portable very-long-periodrandom number generators',
  Computers In Physics, Vol. 8, No.1, Jan/Feb 1994

  A copy of which was obtained at:
    http://www.famaf.unc.edu.ar/~kisbye/mys/cip08117.pdf

*/

static unsigned long x=521288629, y=362436069, z=16163801, c=1, n=1131199209;

/* This is not meant to be a good seed, just one that initialises a guaranteed sequence */
void srand(unsigned int seed) {
	x=521288629;
	y=362436069;
	z=16163801 ^ seed;
	c=1;
	n=1131199209;
}

unsigned long rand(void) {
	long int s;
	if (y > x+c) { s = y - (x+c); c = 0; }
	else { s=y-(x+c)-18; c=1; }
	x = y;
	y = z;
	return (z=s) + (n-69069*n+1013904243);
}
