#ifndef IOPY_SETTINGS_H
#define IOPY_SETTINGS_H

// Change settings by editing here

// Width of the row, maximum 16
#define IOPY_PRIVATE_ROW_WIDTH 8;

// The output database filename
static const char* const IOPY_DB_FILENAME = "./iopy_b3s23_a_w8.db";

// The transition util functions to include.
#include "transitions/b3s23_asymmetric.h"

// Main code

#include <stdbool.h>
#include <stdint.h>

// Derived variables
static const int IOPY_ROW_WIDTH = IOPY_PRIVATE_ROW_WIDTH;
static const uint64_t IOPY_ROW_N = ((uint64_t)1) << IOPY_PRIVATE_ROW_WIDTH;

// Type check for transitions
static inline uint64_t get_transition(uint64_t top, uint64_t mid, uint64_t bot,
                                      int width);
static inline uint64_t postprocess_transition(uint64_t next_raw, int width);
static inline bool is_valid_transition(uint64_t top, uint64_t mid, uint64_t bot,
                                       uint64_t next_raw, int width);

#endif
