#include <iostream>
#include <omp.h>
#include <chrono>
#include <algorithm>


using namespace std;

void bitonic_sort(int* seq, int length, bool flag);
void generate_seq(int* seq, int len);
void parallelization_report(int* sizes, int n);

int main()
{
	int sizes[] = {pow(2,5), pow(2,7), pow(2,8), pow(2,10), pow(2,12), pow(2,15), pow(2,20)};
	//int sizes[] = { pow(2,4) };
	parallelization_report(sizes, sizeof(sizes)/sizeof(int));

}

void generate_seq(int* seq, int len)
{
	for (int i = 0; i < len; ++i)
	{
		seq[i] = rand();
	}
	//for (int i = 0; i < len; ++i)
	//	cout << seq[i] << " ";
	//cout << endl;
}

void swap(int* a, int* b)
{
	int t;
	t = *a;
	*a = *b;
	*b = t;
}


void bitonic_sort(int* seq, int length, bool flag)
{
	if (length < 2)
		return;
	int i, split_len = length / 2;
	//#pragma omp parallel for
	for (i = 0; i < split_len; ++i)
	{
		if (flag)
		{
			if (seq[i] > seq[i + split_len])
				swap(&seq[i], &seq[i + split_len]);
		}
		else
		{
			if (seq[i] < seq[i + split_len])
				swap(&seq[i], &seq[i + split_len]);
		}
	}
	bitonic_sort(seq, split_len, flag);
	bitonic_sort(seq + split_len, split_len, flag);
}


void parallelization_report(int* sizes, int n)
{

	cout << "\t\t\t||    SPEED-UP TABULATION    ||\n" << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	cout << "\tArray Size\tParallel\tSequential\tSpeed-up" << endl;
	cout << "-------------------------------------------------------------------------------" << endl;

	for(int x = 0; x < n; ++x)
	{
		float speedup;
		int N = sizes[x];
		int* seq = (int*)malloc(N * sizeof(int));
		generate_seq(seq, N);


		auto start = chrono::high_resolution_clock::now();

		int i, j;
		for (i = 2; i <= N; i = i * 2)	//length of largest division, cant parallelize
		{
#pragma omp parallel for
			for (j = 0; j < N; j = j + i)	//number of comparisions
			{
				bitonic_sort((seq + j), i, ((j / i) % 2 == 0));
			}
		}

		auto stop = chrono::high_resolution_clock::now();
		auto time_par_ms = chrono::duration_cast<chrono::milliseconds>(stop - start);
		auto time_par_us = chrono::duration_cast<chrono::microseconds>(stop - start);

		//print sorted array
		/*for (int i = 0; i < N; ++i)
		{
			cout << seq[i] << " ";
		}
		cout << endl;*/

		//check if sorted, else break
		/*if (!is_sorted(seq, seq + N))
		{
			cout << "Not sorted!" << endl;
			break;
		}*/

		generate_seq(seq, N);
		start = chrono::high_resolution_clock::now();

		sort(seq, seq + N);
		//for (i = 2; i <= N; i = i * 2)	
		//{
		//	for (j = 0; j < N; j = j + i)
		//	{
		//		bitonic_sort((seq + j), i, ((j / i) % 2 == 0));
		//	}
		//}

		stop = chrono::high_resolution_clock::now();
		auto time_seq_ms = chrono::duration_cast<chrono::milliseconds>(stop - start);
		auto time_seq_us = chrono::duration_cast<chrono::microseconds>(stop - start);

		if (time_par_ms.count()>2 && time_seq_ms.count()>2)
		{
			cout << "\t" << N << "\t\t" << time_par_ms.count() << "ms";
		}
		else
			cout << "\t" << N << "\t\t" << time_par_us.count() << "us";

		if (time_seq_ms.count()>2 && time_par_ms.count()>2)
		{
			cout << "\t\t" << time_seq_ms.count() << "ms";
			//speedup = (float)time_seq_ms.count() / (float)time_par_ms.count();
		}
		else
		{
			cout << "\t\t" << time_seq_us.count() << "us";
			//speedup = (float)time_seq_us.count() / (float)time_par_us.count();
		}

		speedup = (float)time_seq_us.count() / (float)time_par_us.count();
		cout << "\t\t" << speedup << endl;
		
		free(seq);
	}
}