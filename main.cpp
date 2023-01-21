#include "CommonIncludes.h"
#include "MemoryPool.h"
#include "MemoryPool.cpp"
#include <mutex>
#include <thread>
#include <chrono>


int main()
{
	
	
	struct test
	{
		test(std::string name = "this is a very big string which should consume lot of memory yayayayayyayayyayayyayayyayay")
		{
			this->name = name;
		}
		std::string name;
	};

	struct test_pool
	{
		
		test_pool(std::string name = "this is a very big string which should consume lot of memory yayayayayyayayyayayyayayyayay")
		{
			this->name = name;
		}
		std::string name;

		void* operator new(size_t size)
		{
			return mpool.malloc();
		}
	};

	MemoryPool<test_pool> mpool(1000);

	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 10000; i++)
	{
		test* mem = static_cast<test*>(mpool.malloc());
		//test* t1 = new test();
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << time.count() << std::endl;

	//std::cout << mpool1 << std::endl;
	//std::cout << mpool2 << std::endl;
	//mpool1.freeBlocks(5);

	std::cin.get();
	//auto start = std::chrono::high_resolution_clock::now();
	//std::thread t1(func);
	//std::thread t2(func);
	//t1.join();
	//t2.join();
	//auto end = std::chrono::high_resolution_clock::now();
	//auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	//std::cout << "Time taken: " << time << std::endl;
	//std::cout << x << std::endl;
	return 0;
}
