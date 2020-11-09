/* assert */
#include <assert.h>

/* OpenMP API */
#include <omp.h>

/* EXIT_SUCCESS, rand */
#include <stdlib.h>

/* strtol */
#include <stdio.h>

static int
csort(unsigned const k,
      unsigned const n,
      unsigned const * const in,
      unsigned       * const out)
{
  double ts = omp_get_wtime();
  omp_set_dynamic(0);     // Explicitly disable dynamic teams
  omp_set_num_threads(1); // Use 4 threads for all consecutive parallel regions
  int num_threads;
  # pragma omp parallel
  num_threads = omp_get_num_threads();
  printf("number of threads = %d\n", num_threads);

  unsigned * const count = calloc(num_threads * k+1, sizeof(*count));
  if (NULL == count) {
    return -1;
  }
  /*
  printf("original count array:\n[");
  for (unsigned i = 0; i < n; i++) {
    printf("%u,",in[i]);
  }
  printf("]\n");
  */

  //double ts1 = omp_get_wtime();
  # pragma omp parallel for
  for (unsigned i = 0; i < n; i++) {
    count[in[i] * num_threads + omp_get_thread_num()]++;
  }
  //double te1 = omp_get_wtime();
  //printf("timer 1: %lf\n", te1 - ts1);
  /*
  printf("count after first loop:\n[");
  for (unsigned i = 0; i < num_threads*k +1; i++) {
    printf("%u,",count[i]);
  }
  printf("]\n");
  */

  //double ts2 = omp_get_wtime();
  unsigned total = 0;
  for (unsigned i = 0; i <= k * num_threads; i++) {
    unsigned const counti = count[i];
    count[i] = total;
    total += counti;
  }
  //double te2 = omp_get_wtime();
  //printf("timer 2: %lf\n", te2 - ts2);
  /*
  printf("count after second loop:\n[");
  for (unsigned i = 0; i < num_threads*k +1; i++) {
    printf("%u,",count[i]);
  }
  printf("]\n");
  */
  //double ts3 = omp_get_wtime();
  # pragma omp parallel for
  for (unsigned i = 0; i < n; i++) {
    out[count[in[i] * num_threads + omp_get_thread_num()]] = in[i];
    count[in[i] * num_threads + omp_get_thread_num()]++;
  }
  //double te3 = omp_get_wtime();
  //printf("timer 3: %lf\n", te3 - ts3);
  /*
  printf("count after final loop:\n[");
  for (unsigned i = 0; i <n; i++) {
    printf("%u,",out[i]);
  }
  printf("]\n");
  */
  free(count);
  double te = omp_get_wtime();
  printf("timer: %lf\n",te-ts);

  return 0;
}

int
main(int argc, char *argv[]) {
  /* Get array size from command line */
  unsigned n = strtol(argv[1], NULL, 10);

  /* Get key size from command line */
  unsigned k = strtol(argv[2], NULL, 10);

  /* Allocate memory */
  unsigned * const a = malloc(n * sizeof(*a));
  unsigned * const b = malloc(n * sizeof(*b));

  /* Populate with random values */
  for (unsigned i = 0; i < n; i++) {
    a[i] = rand() % (1u << k);
  }

  /* Sort array */
  int const ret = csort(1u << k, n, a, b);
  assert(0 == ret);

  /* Validate sorted array */
  for (unsigned i = 1; i < n; i++) {
    assert(b[i] >= b[i - 1]);
  }

  /* Free memory */
  free(a);
  free(b);

  return EXIT_SUCCESS;
}
