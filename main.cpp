#include "CommonIncludes.h"
#include "MemoryPool.h"
#include "MemoryPool.cpp"
#include <chrono>
#include <time.h>


int main()
{
	clock_t t;
	long double benchavg = 0;
	long double benchavg_new = 0;

	struct test
	{
		int int_arr[10];
		float f_arr[20];
		double d_arr[30];
	};

	MemoryPool<test> alloc_pool(1000);


	for (long long int j = 0; j < 100; j++) {
		t = clock();
		for (int i = 0; i < 100000; i++) {
			test* tt = static_cast<test*>(alloc_pool.malloc()); //allocation
		} 
		t = clock() - t;
		benchavg += (t / (j + 1)) - (benchavg / (j + 1));
	}

	std::cout << "Memory Pool Allocation: " << (benchavg * 1000) / CLOCKS_PER_SEC << std::endl;

	for (long long int j = 0; j < 100; j++) {
		test* t_arr[100001];
		t = clock();
		for (int i = 0; i < 100000; i++) {
			t_arr[i] = new test(); //allocation
		}
		t = clock() - t;
		benchavg += (t / (j + 1)) - (benchavg / (j + 1));
	}

	std::cout << "Heap Allocation: " << (benchavg * 1000) / CLOCKS_PER_SEC << std::endl;



	MemoryPool<test> dlloc_pool(1000);

	for (long long int j = 0; j < 100; j++) {
		t = clock();
		for (int i = 0; i < 100000; i++) {
			test* tt = static_cast<test*>(dlloc_pool.malloc()); //allocation
			dlloc_pool.free(tt); //dellocation
		}
		t = clock() - t;
		benchavg += (t / (j + 1)) - (benchavg / (j + 1));
	}

	std::cout << "Memory pool allocation & dellocation: " << (benchavg * 1000) / CLOCKS_PER_SEC << std::endl;

	for (long long int j = 0; j < 100; j++) {
		t = clock();
		for (int i = 0; i < 100000; i++) {
			test* tt = new test(); //allocation
		} //dellocation
		
		t = clock() - t;
		benchavg += (t / (j + 1)) - (benchavg / (j + 1));
	}

	std::cout << "Heap Allocation & dellocation: " << (benchavg * 1000) / CLOCKS_PER_SEC << std::endl;

	std::cin.get();
	return 0;
}
