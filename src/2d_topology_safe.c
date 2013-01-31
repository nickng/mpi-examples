/**
 * Example code for 2D grid (mesh) of processes.
 *
 * Adds process ID with neighbour's process ID (by exchanging with mesh neighbours)
 *
 * Uses a 2D virtual topology.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <mpi.h>

int main(int argc, char *argv[])
{
  MPI_Comm mesh_comm;
  MPI_Request request[4];

  int rank, size;

  // Topology settings.
  int ndimens = 2; // 2D grid
  int dimens[2];
  int wrap[2] = {0, 0}; // Wraparound (row=yes,col=yes)
  int reorder = 0;

  int dir[2] = {0, 1}; // 1st dimension, 2d dimension
  int displ = 1;

  // Rank
  int prev_rank, next_rank, above_rank, below_rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  dimens[0] = (int)sqrt((double)size);
  dimens[1] = (int)sqrt((double)size);


  /* ... Define virtual topology and connections ... */

  MPI_Cart_create(MPI_COMM_WORLD, ndimens, dimens, wrap, reorder, &mesh_comm);
  MPI_Cart_shift(mesh_comm, dir[0], displ, &prev_rank, &next_rank);
  MPI_Cart_shift(mesh_comm, dir[1], displ, &above_rank, &below_rank);

  /* ... End define virtual topology and connections ... */

#ifdef __DEBUG__
  /* ... Correct way of extract coordination from process rank */
  int mesh_rank;
  int coords[2];
  MPI_Comm_rank(mesh_comm, &mesh_rank);
  MPI_Cart_coords(mesh_comm, mesh_rank , ndimens, coords);
  fprintf(stderr, "Rank %d: %3d (%d, %d) Neighbours: (%d, %d, %d, %d)\n",
      rank,
      mesh_rank,
      coords[0], coords[1],
      above_rank, next_rank, below_rank, prev_rank);
#endif

  MPI_Barrier(mesh_comm);

  int sendbuf[4], recvbuf[4]; // 4 directions
  int result[size];

  // Fill initial buffer.
  sendbuf[0] = sendbuf[1] = sendbuf[2] = sendbuf[3] = rank;
  memset(result, 0, sizeof(int) * size);

  //
  // if a rank is negative then it is invalid and will get unexpected results
  //  but it won't block
  //

  if (above_rank >= 0) {
    MPI_Isend(&sendbuf[0], 1, MPI_INT, above_rank, 0, mesh_comm, &request[0]);
  }
  if (next_rank >= 0) {
    MPI_Isend(&sendbuf[1], 1, MPI_INT, next_rank, 0, mesh_comm, &request[1]);
  }
  if (below_rank >= 0) {
    MPI_Isend(&sendbuf[2], 1, MPI_INT, below_rank, 0, mesh_comm, &request[2]);
  }
  if (prev_rank >= 0) {
    MPI_Isend(&sendbuf[3], 1, MPI_INT, prev_rank, 0, mesh_comm, &request[3]);
  }

  //
  // At this point all data is being Isend and Recv can happen
  //

  if (above_rank >= 0) {
    MPI_Recv(&recvbuf[0], 1, MPI_INT, above_rank, 0, mesh_comm, MPI_STATUS_IGNORE);
    MPI_Wait(&request[0], MPI_STATUS_IGNORE);
    result[rank] += recvbuf[0];
  }
  if (next_rank >= 0) {
    MPI_Recv(&recvbuf[1], 1, MPI_INT, next_rank, 0, mesh_comm, MPI_STATUS_IGNORE);
    MPI_Wait(&request[1], MPI_STATUS_IGNORE);
    result[rank] += recvbuf[1];
  }
  if (below_rank >= 0) {
    MPI_Recv(&recvbuf[2], 1, MPI_INT, below_rank, 0, mesh_comm, MPI_STATUS_IGNORE);
    MPI_Wait(&request[2], MPI_STATUS_IGNORE);
    result[rank] += recvbuf[2];
  }
  if (prev_rank >= 0) {
    MPI_Recv(&recvbuf[3], 1, MPI_INT, prev_rank, 0, mesh_comm, MPI_STATUS_IGNORE);
    MPI_Wait(&request[3], MPI_STATUS_IGNORE);
    result[rank] += recvbuf[3];
  }

  MPI_Allgather(&result[rank], 1, MPI_INT, result, 1, MPI_INT, mesh_comm);

  if (rank == 0) {
    int i;
    for (i=0; i<size; i++) {
      if (i%dimens[1] == 0) printf("\n");
      printf("%3d", result[i]);
    }
    printf("\n");
  }

  MPI_Finalize();

  return EXIT_SUCCESS;
}
