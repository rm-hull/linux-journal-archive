/* 
 * Garbage Collection test program
 * Author: G. Insolvibile
 * This program is part of 
 * "Garbage Collection in C programs",
 * published in 2003 by Linux Journal
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "gctest.h"

static struct option long_options[] = {
  {"gc", 0, 0, 'g'},
  {"trad", 0, 0, 't'},
  {"elemsize", 1, 0, 's'},
  {"nelem", 1, 0, 'n'},
  {"loops", 1, 0, 'l'},
  {"unlink", 0, 0, 'u'},
  {"gcoll", 0, 0, 'G'},
  {"incr", 0, 0, 'i'},
  {"fdiv", 1, 0, 'f'},
  {"verbose", 0, 0, 'v'},
  {"graph", 0, 0, 'x'},
  {"instrument", 0, 0, 'm'},
  {"calloc", 0, 0, 'c'},
  {"gcfree", 0, 0, 'F'},
  {0, 0, 0, 0}
};

#define OPTS_STRING "gts:n:l:uGif:xvmcF"

void usage() {
  printf("Usage: gctest -g | -t [options]\n"
		 " -g --gc		use garbage collection (default)\n"
		 " -t --trad	use traditional malloc/free\n"
		 " -s --elemsize <n> list element size (default 10000)\n"
		 " -n --nelem <n>    number of elements for each list (default 100)\n"
		 " -l --loops <n>   number of loops (default 20)\n"
		 " -v --verbose  report stats for each loop\n"
		 " -x --graph  print graph info for each loop\n"
		 " -m --instrument  collect malloc delay stats\n"
		 " -c --calloc  use calloc() instead of malloc() [trad only]\n"
		 "\n"
		 "GC specific options:\n"
		 " -u --unlink explicitly unlink list (default no)\n"
		 " -G --gcoll	perform GC during loop (default no)\n"
		 " -i --incr   enable incremental GC (default no)\n"
		 " -f --fdiv <n>   set GC free space divisor (default 3)\n"
		 " -F --gcfree  use GC_free() when unlinking the list\n");
}

int
parseopts(int argc, char **argv, opts_t *opts) {
  int c;
  int result = -1;

  /* Set defaults */
  opts->gc = 1;
  opts->unlink = 0;
  opts->collect = 0;
  opts->incr = 0;
  opts->instr = 0;
  opts->elemsize = 10000;
  opts->nelem = 100;
  opts->loops = 20;
  opts->calloc = 0;
  opts->gcfree = 0;

  while (1) {
	int this_option_optind = optind ? optind : 1;
	int option_index = 0;

	c = getopt_long (argc, argv, OPTS_STRING,
					 long_options, &option_index);
	if (c == -1)
	  break;

	switch (c) {
	  case 'g':
		opts->gc = 1;
		result = 0;
		break;

	  case 't':
		opts->gc = 0;
		result = 0;
		break;

	  case 's':
		opts->elemsize = atoi(optarg);
		break;

	  case 'n':
		opts->nelem = atoi(optarg);
		break;

	  case 'l':
		opts->loops = atoi(optarg);
		break;

	  case 'u':
		opts->unlink = 1;
		break;

	  case 'G':
		opts->collect = 1;
		break;

	  case 'i':
		opts->incr = 1;
		break;

	  case 'f':
		opts->fdiv = atoi(optarg);
		break;

	  case 'v':
		opts->verbose = 1;
		break;

	  case 'x':
		opts->graph = 1;
		break;

	  case 'm':
		opts->instr = 1;
		break;

	  case 'c':
		opts->calloc = 1;
		break;

	  case 'F':
		opts->gcfree = 1;
		break;

	  default:
		usage();
		return -1;
	}
  }

  if (result == -1)
	usage();

  return result;

}
