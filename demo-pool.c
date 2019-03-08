#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pool.h"

/* 
   This is demo, showing how the pool functions behave similar
   to thier "original" counterparts
*/
 
int main(void) {
  struct pool *p = pool_create(100);
  
  char *string1 = pool_alloc(p, 11);
  strcpy(string1, "string one");
  
  char *string2 = pool_alloc(p, 11);
  strcpy(string2, "string two");
  
  string1 = pool_realloc(p, string1, 25);
  strcat(string1, " is now longer");
  
  assert(!strcmp(string1, "string one is now longer"));
  
  strcpy(string2, "uh oh.. an overflow of string two");
  
  assert(!strcmp(string1, "overflow of string two"));
  
  assert(pool_free(p, string1));
  assert(!pool_free(p, string1));
  assert(pool_free(p, string2));
  assert(pool_destroy(p));
  
  p = pool_create(3000);
  char *too_big = pool_alloc(p, 10000);
  assert(!too_big);
  char *s1 = pool_alloc(p, 1000);
  assert(s1);
  char *s2 = pool_alloc(p, 1000);
  assert(s2);
  char *s3 = pool_alloc(p, 1000);
  assert(s3);
  char *s4 = pool_alloc(p, 1000);
  assert(!s4);
  
  assert(pool_free(p,s1));
  pool_free(p,s1);
  pool_print_active(p);
  pool_print_available(p);
  assert(pool_free(p,s2));
  
  char *s5 = pool_realloc(p,s3,2000);
  assert(s5);
  assert(s5 ==s1);
  
  pool_print_active(p);
  pool_print_available(p);
  assert(pool_free(p,s5));
  
  pool_destroy(p);
}

