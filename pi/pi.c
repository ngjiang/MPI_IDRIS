#include<stdio.h>
#include<mpi.h>
double f(double x){
	if(x>=0 && x<=1) return 4./(1+x*x);
	return 0.;
}
double x_i(long long i, long long N){
	double x;
	x = (double) i/N;
	return x;
}
long long myAtoll(char *a){
	long long ll;
	int i;
	for(i=0, ll=0; a[i]; ll+=a[i++]-'0')ll*=10;
	return ll;
}
int main(int argc, char *argv[]){
	long long N, i, head, tail;
	int n, rank;
	double dx, x, x0, x1, part, sum;
	double ti, tf;

	if(argc > 1) N = myAtoll(argv[1]);
	else N = 1000;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &n);
	
	ti = MPI_Wtime();

	//printf("Process %d: argc = %d, last: %s, N = %lld\n", rank, argc, argv[argc - 1], N);

	head = (N+n-1)/n * rank;
	tail = (N+n-1)/n * (rank+1);
	if(tail > N) tail = N;

	sum = 0.;
	part = 0.;
	for(i = head, x0 = x_i(i,N); i < tail; i++, x0 = x1){
		x1 = x_i(i+1,N);
		part += f((x0+x1)/2.) * (x1-x0);
	}

	printf("Process %2d: int_[%5.3lf, %5.3lf] f(x) dx = %10.8lf.\n", rank, x_i(head,N), x_i(tail,N), part);

	MPI_Reduce(&part, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if(!rank) printf("pi = %0.8lf\n", sum);
	
	tf = MPI_Wtime();
	if(!rank) printf("Calculation time: %5.3lf s.\n", tf-ti);

	MPI_Finalize();
	return 0;
}
