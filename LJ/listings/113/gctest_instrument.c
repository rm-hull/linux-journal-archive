/* 
 * Garbage Collection test program
 * Author: G. Insolvibile
 * This program is part of 
 * "Garbage Collection in C programs",
 * published in 2003 by Linux Journal
 */

#include <stdio.h>
#include <sys/time.h>
#include <math.h>

#include "gctest.h"

static void * (*origmalloc)(unsigned int);
static void * (*origmallocat)(unsigned int);

#define STATLEN 16384	/* must be a power of 2 */

static struct timeval measures[STATLEN];
static struct timeval start, end;

static unsigned int stat_idx = 0;

void *instr_malloc(unsigned int len) {
  void *tmp;

  gettimeofday(&start, NULL);
  tmp = origmalloc(len);
  gettimeofday(&end, NULL);
  timersub(&end, &start, &measures[stat_idx]);
  stat_idx = (stat_idx+1) & (STATLEN-1);

  return tmp;
}

void *instr_mallocat(unsigned int len) {
  void *tmp;
  
  gettimeofday(&start, NULL);
  tmp = origmallocat(len);
  gettimeofday(&end, NULL);
  timersub(&end, &start, &measures[stat_idx]);
  stat_idx = (stat_idx+1) & (STATLEN-1);

  return tmp;
}

void instrument_init(test_ops_t *ops) {

  origmalloc = ops->alloc;
  ops->alloc = instr_malloc;

  origmallocat = ops->alloc_atomic;
  ops->alloc_atomic = instr_mallocat;

}

void instrument_report() {

  int i;
  double delta, tavg=0.0, tvar=0.0, 
	tmin=HUGE, tmax=0.0;

  printf("\nTiming measurements results\n"
		 "---------------------------\n");
  printf("Collected %d allocation times\n", 
		 stat_idx);

  /* Only (stat_idx % STATLEN) valus are counted */
  for (i=0; i<stat_idx; i++) {
	delta = (double)measures[i].tv_sec 
	  + (double)measures[i].tv_usec/1000000.0;
	if (delta > tmax)
	  tmax = delta;
	else if (delta < tmin)
	  tmin = delta;
	tavg += delta;
  }
  tavg /= stat_idx;

  for (i=0; i<stat_idx; i++) {
	delta = (double)measures[i].tv_sec 
	  + (double)measures[i].tv_usec/1000000.0;
	tvar += (tavg-delta)*(tavg-delta);
  }
  tvar /= stat_idx;

  printf("Average, std dev, max, min delay: "
		 "%f %f %f %f [msec]\n", 
		 tavg*1000, sqrt(tvar)*1000, 
		 tmax*1000, tmin*1000);

}
