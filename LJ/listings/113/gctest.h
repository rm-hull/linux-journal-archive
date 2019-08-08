/* 
 * Garbage Collection test program
 * Author: G. Insolvibile
 * This program is part of 
 * "Garbage Collection in C programs",
 * published in 2003 by Linux Journal
 */

typedef struct {
  unsigned char gc:1;	/* Use Garbage Collection */
  unsigned char unlink:1;	/* Unlink list */
  unsigned char collect:1; /* Collect during loop */
  unsigned char incr:1;		/* Incremental GC */
  unsigned char verbose:1;	/* Verbose report */
  unsigned char graph:1;	/* Print graph info */
  unsigned char instr:1;	/* Collect time stats */
  unsigned char calloc:1; /* Use calloc() instead 
						   * of malloc() */
  unsigned char gcfree:1; /* Use GC_free() when 
						   * unlinking */
  unsigned int fdiv;	  /* Free space divisor */
  unsigned int elemsize;
  unsigned int nelem;
  unsigned int loops;
} opts_t;

typedef struct list {
  struct list *next;
  unsigned int key;
  unsigned char *waste;
} list_t;

typedef struct test_ops {
  void *(*alloc)(unsigned int);
  void *(*alloc_atomic)(unsigned int);
  void (*free)(void *);
  void (*print_info)();
  unsigned int (*heap_size)();
  void (*print_graph_info)();
} test_ops_t;

