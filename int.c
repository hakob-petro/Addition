#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

double f(double x)
{
	return sqrt(4.0 - x * x);
}

double integral(int a, int b, double h)
{
	int S = (f(b * h) + f(a * h));
	for (int i = a + 1; i < b; i++) 
	{
		S += f(i * h);
	}
	return S * h;
}
		
double answer = 0.0;
# define interval 2.0

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Input: %s number of intervals.\n", argv[0]);
		return 1;
	}
	MPI_Init(&argc, &argv);
	int N = atoi(argv[1]);	
	int rank, numtasks, i;
	double h = interval / N; // Шаг
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Номер текущего потока
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks); //Число потоков

	
       

	double t = MPI_Wtime();
	double result = 0;
	// Раздаём задания процессам
	int a = rank * (N / numtasks);
	int b;
	if (rank != numtasks - 1)
	{
 		b = (rank + 1) * (N / numtasks);
	}
	else
	{
		b = N;
	}
	result = integral(a, b, h); 
	// Передаём всё нулёвому процессу (адрес сообщения, число пересылаемого, тип, 	
	// номер процесса получателя, индентификатор сообщения, коммутатор
	if (rank != 0)
	{
		MPI_Send(&result, 1, MPI_DOUBLE, 0, rank, MPI_COMM_WORLD);
	}
	if (rank == 0)
	{
		for (i = 1; i < numtasks; i++)
		{
			double message;
			MPI_Recv(&message, 1, MPI_DOUBLE, i , i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			answer += message;
		}
		printf( "Answer:%lf\n", answer);
	}
		t = MPI_Wtime() - t;

	if (rank == 0)
	{
		printf( "Time: %f, Number of processes: %d\n", t, numtasks);
	}	
	MPI_Finalize();
	return 0;

}

