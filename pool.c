/////////////////////////////////////////////////////////////////////////////
// INTEGRITY INSTRUCTIONS

// Explicitly state the level of collaboration on this question
// Examples:
//   * I discussed ideas with classmate(s) [include name(s)]
//   * I worked together with classmate(s) in the lab [include name(s)]
//   * Classmate [include name] helped me debug my code
//   * I consulted website [include url]
//   * None
// A "None" indicates you completed this question entirely by yourself
// (or with assistance from course staff)
/////////////////////////////////////////////////////////////////////////////
// INTEGRITY STATEMENT:
// I received help from the following sources:

// None

// Name: Xiaoyun Liu
// login ID: x263yu
/////////////////////////////////////////////////////////////////////////////

#include "cs136-trace.h"
#include "pool.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

struct llnode {
  char *address;
  int length;
};

struct pool {
  int size;
  int active;
  int maxlen;
  char *data;
  struct llnode *trace;
};


struct pool *pool_create(int size) {
  assert(size > 0);
  struct pool *p = malloc(sizeof(struct pool));
  p->active = 0;
  p->maxlen = 1;
  p->size = size;
  p->data = malloc(size * sizeof(char));
  p->trace = malloc(p->maxlen * sizeof(struct llnode));
  return p;
}


bool pool_destroy(struct pool *p) {
  assert(p);
  if (p->active   == 0) {
    free(p->data);
    free(p->trace);
    free(p);
    return true;
  } else {
    return false;
  }
}


char *pool_alloc(struct pool *p, int size) {
  assert(p);
  assert(size > 0);
  int pos = -1;
  char *addr;
  if (p->active == 0) {
    if (p->size >= size) {
      p->trace[0].address = p->data;
      p->trace[0].length = size;
      p->active += 1;
      return p->data;
    } else {
      return NULL;
    }
  }
  if (p->active == p->maxlen) {
    p->maxlen *= 2;
    p->trace  = realloc(p->trace, p->maxlen * sizeof(struct llnode));
  }
  if (p->trace[0].address - p->data >= size) {
    addr = p->data;
    p->active++;
    for (int i = p->active - 1; i > 0; i--) {
      p->trace[i].address = p->trace[i - 1].address;
      p->trace[i].length = p->trace[i - 1].length;
    }
    p->trace[0].address = addr;
    p->trace[0].length = size;
    return addr;
  } 
  for (int i = 0; i < p->active - 1; i++) {
    if (p->trace[i + 1].address - 
        (p->trace[i].address + p->trace[i].length) >= size) {
      pos = i + 1;
      addr = p->trace[i].address + p->trace[i].length;
      break;
    }
  } 
  if (pos == -1 && p->data + p->size - (p->trace[p->active - 1].address 
                              + p->trace[p->active - 1].length) >= size) {
    addr = p->trace[p->active - 1].address 
      + p->trace[p->active - 1].length;
    p->trace[p->active].address = addr;
    p->trace[p->active].length = size;
    p->active++;
    return addr;
  }
  if (pos == -1) return NULL;
  for (int i = p->active; i > pos; i--) {
    p->trace[i].address = p->trace[i - 1].address;
    p->trace[i].length = p->trace[i - 1].length;
  }
  p->trace[pos].address = addr;
  p->trace[pos].length = size;
  p->active++;
  return addr;
}


bool pool_free(struct pool *p, char *addr) {
  assert(p);
  int pos = -1;
  for (int i = 0; i < p->active; i++) {
    if (p->trace[i].address == addr) {
      pos = i;
    } 
  }
  if (pos == -1) {
    return false;
  }
  for (int i = pos; i < p->active - 1; i++) {
    p->trace[i].address = p->trace[i + 1].address;
    p->trace[i].length = p->trace[i + 1].length;
  }
  p->active--;
  return true;
}


char *pool_realloc(struct pool *p, char *addr, int size) {
  assert(p);
  assert(size > 0);
  int pos = -1;
  char *old_addr;
  int len = 0;
  if (p->active == 0) {
    return NULL;
  }
  for (int i = 0;i < p->active - 1; i++) {
    if (p->trace[i].address == addr) {
      if (size <= p->trace[i + 1].address - p->trace[i].address) {
        p->trace[i].length = size;
        return p->trace[i].address;
      } else {
        pos = i;
        old_addr = p->trace[i].address;
        len = p->trace[i].length;
        break;
      }
    }
  }
  if (p->trace[p->active - 1].address == addr) {
    pos = p->active - 1;
    old_addr = p->trace[pos].address;
    len = p->trace[pos].length;
    if (size <= p->data + p->size - p->trace[pos].address) {
        p->trace[pos].length = size;
        return p->trace[pos].address;
    }
  }
  if (pos == -1) return NULL;
  char *new_addr = pool_alloc(p, size);
  if (new_addr == NULL) return NULL;
   for (int i = 0; i < len; i++) {
    new_addr[i] = old_addr[i];
  } 
  pool_free(p, old_addr);
  return new_addr;
}


void pool_print_active(struct pool *p) {
  assert(p);
  if (p->active == 0) {
    printf("active: none\n");
    return;
  }
  printf("active: ");
  for (int i = 0;i < p->active - 1; i++) {
    printf("%ld [%d], ", p->trace[i].address - p->data, 
           p->trace[i].length);
  }
  printf("%ld [%d]\n", 
         p->trace[p->active - 1].address - p->data, 
         p->trace[p->active - 1].length);
}


void pool_print_available(struct pool *p) {
  assert(p);
  int sum = 0;
  bool first_time = true;
  if (p->active == 0) {
    printf("available: %d [%d]\n", 0, p->size);
    return;
  }
  for(int i = 0; i < p->active; i++) {
    sum += p->trace[i].length;
  }
  if (sum == p->size) {
    printf("available: none\n");
    return;
  }
  printf("available: ");
  if (p->trace[0].address - p->data > 0) {
    printf("%d [%ld]", 0, p->trace[0].address - p->data);
    first_time = false;
  }
  for(int i = 0; i < p->active - 1; i++) {
    if (p->trace[i].address + p->trace[i].length 
        < p->trace[i + 1].address) {
      if (first_time) {
        printf("%ld [%ld]", 
               p->trace[i].address + p->trace[i].length - p->data,
               p->trace[i + 1].address - 
               (p->trace[i].address + p->trace[i].length));
        first_time = false;
      } else {
        printf(", %ld [%ld]", 
               p->trace[i].address + p->trace[i].length - p->data,
               p->trace[i + 1].address - 
               (p->trace[i].address + p->trace[i].length));
      }
    }
  }
  if (p->data + p->size - (p->trace[p->active - 1].address 
                           + p->trace[p->active - 1].length) > 0) {
    if (first_time) {
      printf("%ld [%ld]\n", p->trace[p->active - 1].address 
           + p->trace[p->active - 1].length - p->data, 
           p->data + p->size - (p->trace[p->active - 1].address 
                                + p->trace[p->active - 1].length));
      return;
    } else {
      printf(", %ld [%ld]\n", p->trace[p->active - 1].address 
           + p->trace[p->active - 1].length - p->data, 
           p->data + p->size - (p->trace[p->active - 1].address 
                                + p->trace[p->active - 1].length));
      return;
    }
  } else {
    printf("\n");
  }
}
