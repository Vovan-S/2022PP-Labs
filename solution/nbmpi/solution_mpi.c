#include "header.h"

Parameters pars;
double c2;


int min(int a, int b) { return a < b ? a : b; }

int main(int argc, char **argv)
{
  int rank, number_of_processes;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  EdgeConditions edge;
  if (rank == 0)
  {
    // read parameters
    int err = read_from_stdin(&edge, &pars);
    if (err) {
      report_read_error(err);
      MPI_Abort(MPI_COMM_WORLD, err);
    }
  }

  MPI_Bcast(&pars, sizeof(pars), MPI_CHAR, 0, MPI_COMM_WORLD);
  // c squared = (tau * a / h)^2
  c2 = calculate_c2(&pars);

  if (rank != 0)
  {
    LOG("%d: received pars. M = %d, P = %d, a = %lf, tau = %lf, h = %lf\n",
        rank, pars.M, pars.P, pars.a, pars.tau, pars.h);
  }

  // every process work with at least two values
  if (number_of_processes >= pars.M - 2)
  {
    if (rank >= (pars.M - 2) / 2) 
    {
      MPI_Finalize();
      return 0;
    }
    number_of_processes = (pars.M - 2) / 2;
  }

  // fan out
  double *phi1_buffer = NULL, *phi2_buffer = NULL;
  int buffer_size = 0;
  int local_result_size = 0;
  if (rank == 0) 
  {
    // sending without edges
    buffer_size = pars.M - 2;
    phi1_buffer = edge.phi1 + 1;
    phi2_buffer = edge.phi2 + 1;
  }

  int doubles_in_part = (pars.M - 2) / number_of_processes;
  int start_k = 1;
  while((start_k << 1) < number_of_processes)
    start_k <<= 1;

  int TAG_PHI1 = 1, TAG_PHI2 = 2;
  MPI_Request request;
  for (int k = start_k; k > 0; k >>= 1)
  {
    for (int i = 0; i + k < number_of_processes; i += 2 * k)
    {
      int to_send = min(number_of_processes, i + 2 * k) - (i + k);
      int doubles_to_send = doubles_in_part * to_send;
      if (rank == i) 
      {
        // send last `doubles_to_send` values
        MPI_Isend(
            phi1_buffer + buffer_size - doubles_to_send,
            doubles_to_send, MPI_DOUBLE, i + k, TAG_PHI1, 
            MPI_COMM_WORLD, &request);
        MPI_Isend(
            phi2_buffer + buffer_size - doubles_to_send,
            doubles_to_send, MPI_DOUBLE, i + k, TAG_PHI2, 
            MPI_COMM_WORLD, &request);
        // "free" last `doubles_to_send` values
        buffer_size -= doubles_to_send;
      }
      if (rank == i + k) 
      {
        LOG("%d: about to receive %d from %d\n", i + k, to_send, i);
        phi1_buffer = malloc(doubles_to_send * sizeof(double));
        phi2_buffer = malloc(doubles_to_send * sizeof(double));
        MPI_Recv(phi1_buffer, doubles_to_send, MPI_DOUBLE, i,
                 TAG_PHI1, MPI_COMM_WORLD, &status);
        MPI_Recv(phi2_buffer, doubles_to_send, MPI_DOUBLE, i,
                 TAG_PHI2, MPI_COMM_WORLD, &status);
        local_result_size = buffer_size = doubles_to_send;
      }
    }
  }

  LOG("%d: my first is %lf, last is %lf\n", rank, phi1_buffer[0], phi1_buffer[buffer_size - 1]);


  // rank 0 processes x in range from 2 to its `buffer_size`, but 
  // allocates memory for global result
  int x_dim = rank == 0 ? pars.M : local_result_size;
  LOG("%d: my x_dim = %d, buffer_size = %d\n", rank, x_dim, buffer_size);
  double * local_result = malloc(x_dim * pars.P * sizeof(double));

  // for rank 0 to gather all the result
  double * global_result = 0;
  if (rank == 0)
  {
    global_result = local_result;
    // rank 0 writes in its buffer from 1 because it bypasses psi1
    local_result += pars.P;
  }

  // calculate u[1]
  for (int x = 0; x < buffer_size; x++)
    set_to_local_result(local_result, x_dim, x, 0, phi1_buffer[x]);
  // calculate u[2]
  calculate_row(rank, number_of_processes, local_result, buffer_size, 0,
                phi2_buffer, 1, &edge, calculate_second_step);
  // we do not need these buffers anymore
  if (rank == 0)
  {
    free(edge.phi1);
    free(edge.phi2);
    edge.phi1 = edge.phi2 = 0;
  }
  else
  {
    free(phi1_buffer);
    free(phi2_buffer);
  }
  // MPI_Finalize();
  // return 0;

  // for 3..P iterate
  for (int p = 2; p < pars.P; p++)
  {
    // TODO!
    calculate_row(rank, number_of_processes, local_result, buffer_size, 
      p - 1, local_result + p - 2, pars.P, &edge, iterate
    );
  }
  // fan in
  
  for (int k = 1; k < number_of_processes; k <<= 1)
  {
    for (int i = 0; i + k < number_of_processes; i += 2*k)
    {
      int to_receive = min(number_of_processes, i + 2 * k) - (i + k);
      int doubles_to_receive = to_receive * pars.P * doubles_in_part;
      if (rank == i) 
      {
        //recv
        LOG("%d: about to receive %d doubles from %d\n",
            rank, doubles_to_receive, i + k);
        MPI_Recv(
          local_result + buffer_size * pars.P, 
          doubles_to_receive, 
          MPI_DOUBLE,
          i + k,
          TAG,
          MPI_COMM_WORLD,
          &status);
        buffer_size += doubles_to_receive / pars.P;
      }
      if (rank == i + k)
      {
        // send
        LOG("%d: about to send %d doubles to %d\n",
            rank, doubles_to_receive, i);
        MPI_Send(
          local_result,
          doubles_to_receive,
          MPI_DOUBLE,
          i,
          TAG,
          MPI_COMM_WORLD
        );
      }
    }
  }

  // rank=0 write to stdin
  
  if (rank == 0)
  {
    // complete global result with edges
    for (int i = 0; i < pars.P; i++) 
    {
      set_to_local_result(global_result, x_dim, 0, i, edge.psi1[i]);
      set_to_local_result(global_result, x_dim, pars.M - 1, i, edge.psi2[i]);
    }
    // write to stdout
    write_to_std(global_result, &pars, 1);
    free(global_result);
    free_edge(&edge);
  }
  else 
  {
    free(local_result);
  }

  MPI_Finalize();
  return 0;
}

void set_to_local_result(double* arr, int x_dim, int m, int p, double value) 
{
  arr[m * pars.P + p] = value; 
}

double get_from_local_result(double* arr, int x_dim, int m, int p)
{
  return arr[m * pars.P + p];
}

void calculate_row(int rank, int np, double *buffer, int row_size, int row,
                   double *prev_row, int prev_row_dim, EdgeConditions * edge,
                   double(* func)(double, double, double, double))
{
  int TAG_GET_LEFT = 1, TAG_GET_RIGHT = 2;
  MPI_Request request_left, request_right, request_send;
  MPI_Status status;
  int left_rank = rank - 1, right_rank = (rank + 1) % np;
  double result_left = 0, result_right = 0; 
  // send and receive left side 
  // rank 0 sends psi2 and receives psi1
  if (rank == 0) 
  {
    MPI_Isend(edge->psi2 + row, 1, MPI_DOUBLE, np - 1,
              TAG_GET_RIGHT, MPI_COMM_WORLD, &request_send);
    result_left = edge->psi1[row];
  }
  else
  {
    double to_send = get_from_local_result(buffer, 0, 0, row);
    MPI_Isend(&to_send, 1, MPI_DOUBLE, 
              left_rank, TAG_GET_RIGHT, MPI_COMM_WORLD, &request_send);
    MPI_Irecv(&result_left, 1, MPI_DOUBLE, left_rank, TAG_GET_LEFT,
              MPI_COMM_WORLD, &request_left);
  }
  // send and receive right side
  double to_send = get_from_local_result(buffer, 0, row_size - 1, row);
  MPI_Isend(&to_send, 1, MPI_DOUBLE, right_rank, TAG_GET_LEFT,
            MPI_COMM_WORLD, &request_send);
  MPI_Irecv(&result_right, 1, MPI_DOUBLE, right_rank, TAG_GET_RIGHT,
            MPI_COMM_WORLD, &request_right);
  // calculate inner parts
  for (int m = 1; m < row_size - 1; m++)
  {
    set_to_local_result(buffer, 0, m, row + 1,
                        func(
                            get_from_local_result(buffer, 0, m, row),
                            get_from_local_result(buffer, 0, m - 1, row),
                            get_from_local_result(buffer, 0, m + 1, row),
                            prev_row[m*prev_row_dim]));
  }
  // wait for left edge
  if (rank != 0) MPI_Wait(&request_left, &status);
  // calculate left edge
  set_to_local_result(buffer, 0, 0, row + 1,
                      func(
                          get_from_local_result(buffer, 0, 0, row), 
                          result_left, 
                          get_from_local_result(buffer, 0, 1, row), 
                          prev_row[0]));
  // wait for right edge
  MPI_Wait(&request_right, &status);
  // calculate right edge
  set_to_local_result(buffer, 0, row_size - 1, row + 1,
                      func(
                          get_from_local_result(buffer, 0, row_size - 1, row), 
                          get_from_local_result(buffer, 0, row_size - 2, row), 
                          result_right, 
                          prev_row[(row_size - 1)*prev_row_dim]));
}

double iterate(double u11, double u12, double u10, double u01) 
{
  return calculate_iteration(u11, u01, u10, u12);
}