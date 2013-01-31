/**
 * Example code for disjoint group of processes.
 *
 * Create two groups of processes by MPI_Group
 * decided by process rank odd/even.
 */

#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

int main(int argc, char *argv[])
{
  MPI_Comm grp_comm;
  MPI_Group world_grp;
  MPI_Group grp;

  int rank, size;
  int *grpmbrs_even, *grpmbrs_odd;
  int grpmbr_even_idx, grpmbr_odd_idx;
  int i;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  grpmbrs_even = (int *)calloc(size/2, sizeof(int));
  grpmbrs_odd  = (int *)calloc(size/2, sizeof(int));

  MPI_Comm_group(MPI_COMM_WORLD, &world_grp); // For everyone.

  /* ... Calculate group members  ... */
  /* ... Shared between all procs ... */

  grpmbr_even_idx = 0;
  grpmbr_odd_idx  = 0;
  for (i=0; i<size; ++i) {
    // Not rank because this will be iterated and run by all procs
    if (i%2 == 0) {
      grpmbrs_even[grpmbr_even_idx++] = i;
    } else {
      grpmbrs_odd[grpmbr_odd_idx++] = i;
    }
  }

  /* ... End calculate group members ... */

  // All processes run this once (for their own group) only.
  if (rank%2 == 0) {
    printf("Rank %d setting up for group even\n", rank);
    MPI_Group_incl(world_grp, size/2, grpmbrs_even, &grp);
  } else {
    printf("Rank %d setting up for group odd\n", rank);
    MPI_Group_incl(world_grp, size/2, grpmbrs_odd, &grp);
  }

  MPI_Comm_create(MPI_COMM_WORLD, grp, &grp_comm);
  MPI_Group_free(&world_grp);
  MPI_Group_free(&grp);

  int result = 0;
  MPI_Allreduce(&rank, &result, 1, MPI_INT, MPI_SUM, grp_comm);

  printf("Rank %d, group %d, result %d\n", rank, rank%2, result);

  MPI_Finalize();

  free(grpmbrs_even);
  free(grpmbrs_odd);

  return EXIT_SUCCESS;
}
