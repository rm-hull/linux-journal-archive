/* 
 * Garbage Collection test program
 * Author: G. Insolvibile
 * This program is part of 
 * "Garbage Collection in C programs",
 * published in 2003 by Linux Journal
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>
#include <malloc.h>
#include <gc.h>

#include "gctest.h"

test_ops_t *ops;

opts_t options;

extern void instrument_init(test_ops_t *ops);
extern void instrument_report();

void print_gc_info();
void ignore(void *);
unsigned int malloc_heapsize();
void print_graph_info_gc();
void print_graph_info_malloc();

/* Operations for traditional mode testing */
test_ops_t traditional = { malloc, malloc, free, 
						   malloc_stats, 
						   malloc_heapsize, 
						   print_graph_info_malloc};

/* Operations for GC mode testing */
test_ops_t garbagecoll = { GC_malloc, 
						   GC_malloc_atomic, 
						   ignore, print_gc_info, 
						   GC_get_heap_size, 
						   print_graph_info_gc };

void test_report(struct timeval *delta, 
				 unsigned int heap);


void print_gc_info() {
  printf("Number of bytes in the heap: %d\n", 
		 GC_get_heap_size());
  printf("Lower bound on free bytes in the heap:"
		 " %d\n", GC_get_free_bytes());
  printf("Number of bytes allocated since last "
		 "collection: %d\n",
		 GC_get_bytes_since_gc());
  printf("Total number of bytes allocated: %d\n", 
		 GC_get_total_bytes());
}

void print_malloc_info() {
  malloc_stats();
}

void print_graph_info_gc() {
  printf("== %d %d\n", GC_get_total_bytes(), 
		 GC_get_heap_size());
}

void print_graph_info_malloc() {
  struct mallinfo minfo;
  minfo = mallinfo();
  printf("== %d %d\n", minfo.uordblks, 
		 minfo.arena+minfo.hblkhd);
}

unsigned int malloc_heapsize() {
  struct mallinfo minfo;
  minfo = mallinfo();
  return minfo.arena + minfo.hblkhd;
}

void *calloc_wrap(unsigned int size) {
  return calloc(1, size);
}

void ignore(void *p) {}

void test_report(struct timeval *delta, 
				 unsigned int heapsize) {
  unsigned int listsize;
  unsigned long long overall;

  listsize = options.nelem * (options.elemsize
							  +sizeof(list_t));
  overall = options.loops * listsize;

  printf("\nTest conditions:\n----------------\n");
  if (options.gc)
	printf("Boehm-Demers Garbage Collector\n");
  else
	printf("Traditional malloc/free\n");
  printf("Loops    : %7d\nLength   : %7d nodes\n"
		 "Node size: %7d bytes\n",
		 options.loops, options.nelem, 
		 options.elemsize + sizeof(list_t));
  printf("List size    : %10d bytes\n"
		 "Overall alloc: %10lld bytes\n", 
		 listsize, overall);
  printf("Unlink during loop : %s\n", 
		 options.unlink?"yes":"no");
  if (options.gc) {
	printf("Collect during loop: %s\n", 
		   options.collect?"yes":"no");
	printf("Incremental GC     : %s\n", 
		   options.incr?"yes":"no");
	printf("Free space divisor : %d\n", 
		   GC_free_space_divisor);
  }

  printf("\nTest results:\n-------------\n");
  printf("Peak heap size %d bytes\n", heapsize);
  printf("Heap/overall_alloc ratio %f\n", 
		 (float)heapsize/(float)overall);
  printf("Heap/list_size ratio  %f\n", 
		 (float)heapsize/(float)listsize);
  if (delta)
	printf("Elapsed time %f sec\n", 
		   (float)delta->tv_sec
		   +(float)delta->tv_usec/1000000.0);

  if (options.instr)
	instrument_report();

}

int main(int argc, char **argv) {
  int heapsize, maxheap=0, n;
  struct timeval start, end, diff;
  char *hugewaste;

  options.fdiv = GC_free_space_divisor;
  if (parseopts(argc, argv, &options) == -1) {
	exit(0);
  }

  if (options.gc) {
	ops = &garbagecoll;
  } else {
	ops = &traditional;
  }

  if (options.incr)
	GC_enable_incremental();

  GC_free_space_divisor = options.fdiv;

  if (options.instr)
	instrument_init(ops);

  if (!options.gc && options.calloc) {
	ops->alloc = calloc_wrap;
  }
  
  gettimeofday(&start, NULL);
  for (n=0; n<options.loops; n++) {
	if (options.verbose)
	  printf("----------- run %d\n", n);
	if (options.unlink)
	  destroy_list(create_list(options.nelem));
	else
	  create_list(options.nelem);

	if (options.verbose)
	  ops->print_info();
	if (options.graph)
	  ops->print_graph_info();

	if (options.gc && options.collect)
	  GC_gcollect();

	heapsize = ops->heap_size();
	if (heapsize > maxheap) maxheap = heapsize;
	  
  }
  gettimeofday(&end, NULL);
  timersub(&end, &start, &diff);

  test_report(&diff, maxheap);

}
