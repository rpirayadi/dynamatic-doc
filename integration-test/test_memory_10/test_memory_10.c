#include "test_memory_10.h"
#include "../integration_utils.h"
#include <stdlib.h>

void test_memory_10(int a[N], int n) {
  for (int i = 0; i < n; i++)
    a[i] = a[i] + a[i + 1] + 5;
}

int main(void) {
  inout_int_t a[N];
  srand(13);
  for (unsigned j = 0; j < N; ++j)
    a[j] = rand() % 100;

  CALL_KERNEL(test_memory_10, a, N);
  return 0;
}