//http://jakascorner.com/blog/2016/05/omp-monte-carlo-pi.html

#include <iostream>
#include <omp.h>
#include <chrono>

using namespace std;

void parallel();
void sequential();

int main()
{
	cout << "Monte Carlo method of generating Pi value\n";

	auto start = chrono::high_resolution_clock::now();

	parallel();

	auto stop = chrono::high_resolution_clock::now();
	auto time_par = chrono::duration_cast<chrono::milliseconds>(stop - start);
	cout << "Parallel :" << time_par.count() << " milliseconds" << endl;
	cout << "----------------------" << endl;

	start = chrono::high_resolution_clock::now();

	sequential();

	stop = chrono::high_resolution_clock::now();
	auto time_seq = chrono::duration_cast<chrono::milliseconds>(stop - start);
	cout << "Sequential :" << time_seq.count() << " milliseconds" << endl;
	cout << "----------------------";

	float speedup = (float)time_seq.count() / (float)time_par.count();
	cout << "\nSpeedup : " << speedup << endl;
	cout << "----------------------";
}

void sequential()
{
	const int n = 10000;
	int numIn = 0;
	for (int i = 0; i < n; i++)
	{
		double x = (double)i / n;
		for (int j = 0; j < n; j++)
		{
			double y = (double)j / n;
			if (x * x + y * y < 1) numIn++;
		}
	}

	cout << "Pi : " << (double)4 * numIn / n / n << endl;
}

void parallel()
{
	const int n = 10000;
	const int NUM_THREADS = 8;
	int numIn = 0;

#pragma omp parallel for reduction(+:numIn) num_threads(NUM_THREADS)
	for (int i = 0; i < n; i++) {
		double x = (double)i / n;
		for (int j = 0; j < n; j++) {
			double y = (double)j / n;
			if (x * x + y * y < 1) numIn++;
		}
	}
	cout <<"Pi : "<< (double)4*numIn/n/n << endl;

}