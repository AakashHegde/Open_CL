#include <iostream>
#include <omp.h>
#include <chrono>
#include <algorithm>


using namespace std;

void bitonic_sort(int* seq, int length, bool flag);
void generate_seq(int* seq, int len);

int main()
{
	int* seq, N = 1024 * 64;
	//cout << "Enter number of elements : ";
	//cin >> N;
	seq = (int*)malloc(N * sizeof(int));
	generate_seq(seq, N);
	//for (int i = 0; i < N; ++i)
	//	cout << seq[i] << " ";
	//cout << endl;

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
	auto time_par = chrono::duration_cast<chrono::milliseconds>(stop - start);
	cout << "Parallel :" << time_par.count() << " milliseconds" << endl;

	cout << "\n----------------------" << endl;
	if (is_sorted(seq, seq + N))
		cout << "Array is checked and confirmed SORTED by in-built function is_sorted()" << endl;

	generate_seq(seq, N);
	start = chrono::high_resolution_clock::now();

	sort(seq, seq + N);

	stop = chrono::high_resolution_clock::now();
	auto time_seq = chrono::duration_cast<chrono::milliseconds>(stop - start);
	cout << "\n\nSequential :" << time_seq.count() << " milliseconds" << endl;
	cout << "\n----------------------";

	float speedup = (float)time_seq.count() / (float)time_par.count();
	cout << "\nSpeedup : " << speedup << endl;
	cout << "----------------------";

}

void generate_seq(int* seq, int len)
{
	for (int i = 0; i < len; ++i)
	{
		seq[i] = rand();
	}
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
