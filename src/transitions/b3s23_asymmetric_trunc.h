#include <inttypes.h>
#include <stdbool.h>

static inline bool is_valid_transition(uint64_t top, uint64_t mid, uint64_t bot,
                                       uint64_t next_raw, int width) {
  // In order for a transition to be valid,
  // the next generation should be contained inside the same width boundary.
  // And in B3/S23, if and only if all three bits at a boundary is on,
  // it causes the birth of a cell outside the boundary.
  const uint64_t BITMASK_OF_BOUNDARY_BITS = (((uint64_t)1) << (width - 1)) | 1;
  return (top & mid & bot & BITMASK_OF_BOUNDARY_BITS) == 0;
}

static inline uint64_t postprocess_transition(uint64_t next_raw, int width) {
  return next_raw;
}

static inline uint64_t get_transition(uint64_t top, uint64_t mid, uint64_t bot,
                                      int width) {
  // Get the sum of a column by summing each bit from top/mid/bot
  uint64_t mid_col_sum_b0 = top ^ mid ^ bot;
  uint64_t mid_col_sum_b1 = (top & mid) | (top & bot) | (mid & bot);

  // Shift the sum
  uint64_t shl_col_sum_b0 = mid_col_sum_b0 << 1;
  uint64_t shl_col_sum_b1 = mid_col_sum_b1 << 1;
  uint64_t shr_col_sum_b0 = mid_col_sum_b0 >> 1;
  uint64_t shr_col_sum_b1 = mid_col_sum_b1 >> 1;

  // Add the non-shifted and shifted column sums to get the sum of a 3x3 grid.
  // Sum the 2^0s bit first
  uint64_t grid_sum_b0 = shl_col_sum_b0 ^ mid_col_sum_b0 ^ shr_col_sum_b0;
  uint64_t grid_sum_b1_carry = (shl_col_sum_b0 & mid_col_sum_b0) |
                               (shl_col_sum_b0 & shr_col_sum_b0) |
                               (mid_col_sum_b0 & shr_col_sum_b0);
  // Sum the 2^1s bits (3 of them) and the carry bit just made
  // Sum pairs first
  uint64_t grid_sum_b1_0 = grid_sum_b1_carry ^ mid_col_sum_b1;
  uint64_t grid_sum_b2_0 = grid_sum_b1_carry & mid_col_sum_b1;
  uint64_t grid_sum_b1_1 = shl_col_sum_b1 ^ shr_col_sum_b1;
  uint64_t grid_sum_b2_1 = shl_col_sum_b1 & shr_col_sum_b1;
  // And sum the summed pairs
  uint64_t grid_sum_b1 = grid_sum_b1_0 ^ grid_sum_b1_1;
  uint64_t grid_sum_b2_carry = grid_sum_b1_0 & grid_sum_b1_1;
  uint64_t grid_sum_b2 = grid_sum_b2_0 ^ grid_sum_b2_1 ^ grid_sum_b2_carry;
  uint64_t grid_sum_b3 = (grid_sum_b2_0 & grid_sum_b2_1) |
                         (grid_sum_b2_0 & grid_sum_b2_carry) |
                         (grid_sum_b2_1 & grid_sum_b2_carry);

  // In Conway's game of life, a cell is alive next generation
  // Either when the grid sum is 3 (B3, S2)
  // or when the grid sum is 4 and the cell is alive this genereation (S3)
  uint64_t grid_sum_eq_3 =
      grid_sum_b0 & grid_sum_b1 & (~grid_sum_b2) & (~grid_sum_b3);
  uint64_t grid_sum_eq_4 =
      (~grid_sum_b0) & (~grid_sum_b1) & grid_sum_b2 & (~grid_sum_b3);

  // We're going to check only `width` bits so mask off the irrelevant ones.
  uint64_t result =
      (grid_sum_eq_3 | (grid_sum_eq_4 & mid)) & ((1 << width) - 1);
  return result;
}