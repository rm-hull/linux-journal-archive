/* 
 * Garbage Collection test program
 * Author: G. Insolvibile
 * This program is part of 
 * "Garbage Collection in C programs",
 * published in 2003 by Linux Journal
 */

#include <stdio.h>

#include "gctest.h"

extern opts_t options;
extern test_ops_t *ops;


/* Add a new node to the list */
list_t *add_node(list_t *head, unsigned int key) {
  list_t *newnode;

  newnode = ops->alloc(sizeof(list_t));
  if (!newnode) {
	printf("Fatal!\n");
	return NULL;
  }
  newnode->waste = ops->alloc_atomic(options
									 .elemsize);
  if (!newnode->waste) {
	printf("Fatal!\n");
	return NULL;
  }

  newnode->key = key;
  newnode->next = head;

  return newnode;
}


/* Create a list of 'n' random elements */
list_t *create_list(int n) {
  list_t *head = NULL;
  int i;
 
  for (i=0; i<n; i++) {
	if (!(head = add_node(head, rand()))) {
	  printf("Out of memory.\n");
	  test_report(NULL);
	  printf("Test aborted for memory exhaustion\n");
	  exit(0);
	}
  }

  return head;
}

/* Destroy the list */
void destroy_list(list_t *head) {
  list_t *ptr = head, *next;
  
  while (ptr) {
	next = ptr->next;
	ptr->next = NULL;
	if (options.gcfree) {
	  GC_free(ptr->waste);
	  GC_free(ptr);
	} else {
	  ops->free(ptr->waste);
	  ops->free(ptr);
	}
	ptr->waste = NULL;
	ptr = next;
  }

}
