#include <stdint.h>
#include <stdio.h>

#include "../src/transitions/b3s23_asymmetric.h"

int log_all_top_mid_next() {
  uint64_t top = 0;
  uint64_t mid = 4;
  uint64_t next_expected = 4;
  int width = 8;
  for (uint64_t bot = 0; bot < (1 << 8); ++bot) {
    int next_raw = get_transition(top, mid, bot, width);
    int next_actual = postprocess_transition(next_raw, width);
    bool is_valid = is_valid_transition(top, mid, bot, next_raw, width);
    fprintf(stderr,
            "[Transition] width: %d, top: %x, mid: %x, bot: %x, next_raw: %x, "
            "next_actual: %x, next_expected: %x, is_valid: %d"
            "\n",
            width, top, mid, bot, next_raw, next_actual, next_expected,
            is_valid);
    if (next_expected == next_actual && is_valid) {
      fprintf(stderr, "Success!\n");
    }
  }
  return 1;
}

int main(void) {
  int status = log_all_top_mid_next();
  return 0;
}