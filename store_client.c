/*
 * CSE 351 Lab 1b (Manipulating Bits in C)
 *
 * Name(s):  
 * NetID(s): 
 *
 * This is a file for managing a store of various aisles, represented by an
 * array of 64-bit integers. See aisle_manager.c for details on the aisle
 * layout and descriptions of the aisle functions that you may call here.
 *
 * Written by Porter Jones (pbjones@cs.washington.edu)
 */

#include <stddef.h>  // To be able to use NULL
#include "aisle_manager.h"
#include "store_client.h"
#include "store_util.h"

// Number of aisles in the store
#define NUM_AISLES 10

// Number of sections per aisle
#define SECTIONS_PER_AISLE 4

// Number of items in the stockroom (2^6 different id combinations)
#define NUM_ITEMS 64

// Global array of aisles in this store. Each unsigned long in the array
// represents one aisle.
unsigned long aisles[NUM_AISLES];

// Array used to stock items that can be used for later. The index of the array
// corresponds to the item id and the value at an index indicates how many of
// that particular item are in the stockroom.
int stockroom[NUM_ITEMS];


/* Starting from the first aisle, refill as many sections as possible using
 * items from the stockroom. A section can only be filled with items that match
 * the section's item id. Prioritizes and fills sections with lower addresses
 * first. Sections with lower addresses should be fully filled (if possible)
 * before moving onto the next section.
 */
#define NUM_SPACES 10
void refill_from_stockroom() {
  for (int i = 0; i < NUM_AISLES; i++) {
    unsigned long* cur_aisle = &aisles[i];
    for (int j = 0; j < SECTIONS_PER_AISLE; j++) {
      unsigned short section_id = get_id(cur_aisle, j);
      int items_needed = (NUM_SPACES - num_items(cur_aisle, j));
      if (stockroom[section_id] < items_needed) {items_needed = stockroom[section_id];}
      stockroom[section_id] -= items_needed;
      add_items(cur_aisle, j, items_needed);
    }
  }
}

/* Remove at most num items from sections with the given item id, starting with
 * sections with lower addresses, and return the total number of items removed.
 * Multiple sections can store items of the same item id. If there are not
 * enough items with the given item id in the aisles, first remove all the
 * items from the aisles possible and then use items in the stockroom of the
 * given item id to finish fulfilling an order. If the stockroom runs out of
 * items, you should remove as many items as possible.
 */
int fulfill_order(unsigned short id, int num) {
  int count = 0;
  for (int i = 0; i < NUM_AISLES; i++) {
    unsigned long* aisle = &aisles[i];
    for (int j = 0; j < SECTIONS_PER_AISLE; j++) {
      if (get_id(aisle, j) != id) {continue;}
      int items_needed = num_items(aisle, j);
      if (items_needed > num) {items_needed = num;}
      count += items_needed;
      num -= items_needed;
      remove_items(aisle, j, items_needed);
    }
  }
  int stock_needed = num;
  if (stockroom[id] < stock_needed) {
    stock_needed = stockroom[id];
  }
  stockroom[id] -= stock_needed;
  count += stock_needed;
  num -= stock_needed;
  return count;
}

/* Return a pointer to the first section in the aisles with the given item id
 * that has no items in it or NULL if no such section exists. Only consider
 * items stored in sections in the aisles (i.e., ignore anything in the
 * stockroom). Break ties by returning the section with the lowest address.
 */
unsigned short* empty_section_with_id(unsigned short id) {
  for (int i = 0; i < NUM_AISLES; i++) {
    unsigned long* aisle = &aisles[i];
    for (int j = 0; j < SECTIONS_PER_AISLE; j++) {
      if (get_id(aisle, j) != id) {continue;}
      if (num_items(aisle, j) == 0) {return (unsigned short*) aisle + j;}
    }
  }
  return NULL;
}

/* Return a pointer to the section with the most items in the store. Only
 * consider items stored in sections in the aisles (i.e., ignore anything in
 * the stockroom). Break ties by returning the section with the lowest address.
 */
unsigned short* section_with_most_items() {
  unsigned short* largest_section_ptr = (unsigned short*) &aisles[0];
  unsigned short largest_section_amt = 0;
  for (int i = 0; i < NUM_AISLES; i++) {
    unsigned long* aisle = &aisles[i];
    for (int j = 0; j < SECTIONS_PER_AISLE; j++) {
      unsigned short section_amt = num_items(aisle, j);
      if (section_amt > largest_section_amt) {
        largest_section_ptr = (unsigned short*) aisle + j;
        largest_section_amt = section_amt;
      }
      if (largest_section_amt == NUM_SPACES) {
        return largest_section_ptr;
      }
    }
  }
  return largest_section_ptr;
}
