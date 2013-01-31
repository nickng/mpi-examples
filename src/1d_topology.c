/**
 * Example code for 1D grid (ring) of processes.
 *
 * Uses a 1D virtual topology.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>

int main(int argc, char *argv[])
{
  MPI_Comm ring_comm;
  MPI_Request request;

  int rank, size;

  // Topology settings.
  int ndimens = 1; // 1D grid
  int wrap    = 1; // Wraparound (ring)
  int reorder = 0;

  int dir   = 0;
  int displ = 1;

  // Rank
  int prev_rank, next_rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /* ... Define virtual topology and connections ... */

  MPI_Cart_create(MPI_COMM_WORLD, ndimens, &size, &wrap, reorder, &ring_comm);
  MPI_Cart_shift(ring_comm, dir, displ, &prev_rank, &next_rank);

  /* ... End define virtual topology and connections ... */

  int pipe;
  int sendbuf, recvbuf;
  int result = 0;

  // Fill initial buffer.
  memcpy(&sendbuf, &rank, sizeof(int));

  // Piping content through ring.
  for (pipe=0; pipe<size; pipe++) {
    MPI_Isend(&sendbuf, 1, MPI_INT, next_rank, pipe, ring_comm, &request);
    MPI_Recv(&recvbuf, 1, MPI_INT, prev_rank, pipe, ring_comm, MPI_STATUS_IGNORE);

#ifdef __DEBUG__
    int ring_rank;
    MPI_Comm_rank(ring_comm, &ring_rank);
    fprintf(stderr, "Rank %d: Isend %d --> %d\n", rank, ring_rank, next_rank);
    fprintf(stderr, "Rank %d: Recv  %d --> %d\n", rank, prev_rank, ring_rank);
#endif

    result += recvbuf;

    MPI_Wait(&request, MPI_STATUS_IGNORE);
    memcpy(&sendbuf, &recvbuf, sizeof(int));
  }

  printf("Rank %d, result %d\n", rank, result);

  MPI_Finalize();

  return EXIT_SUCCESS;
}
