#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include "mutex"
#include "CommonIncludes.h"

/*
Also, implement this to be thread safe.
*/

static const size_t DEFAULT_POOL_SIZE = 1024;
static const size_t DEFAULT_NEW_ALLOCATION_SIZE = DEFAULT_POOL_SIZE;

template<typename size_type = std::byte>
class MemoryPool
{
public:
	MemoryPool() = delete;

	explicit MemoryPool(uint32_t chunksPerBlock = DEFAULT_POOL_SIZE);

	MemoryPool(const MemoryPool&) = delete;

	MemoryPool(MemoryPool&& other) noexcept;

	MemoryPool& operator=(MemoryPool other);

	~MemoryPool();

	void* malloc();

	void free(void* pChunk);
	void freeBlocks(uint32_t blocks);

	bool empty() const;

	uint32_t allocatedChunks() const;
	uint32_t allocatedBlocks() const;

	uint32_t chunksPerBlock() const;

	std::size_t chunkSize() const;
	std::size_t blockSize() const;

	void reserveBlocks(uint32_t blocks);

private:
	struct SMemoryChunk
	{
		SMemoryChunk(SMemoryChunk* next = nullptr, uint32_t parentBlock = 0) : m_next(next), m_parentBlock(parentBlock)
		{
#ifdef _DEBUG
			std::cout << "Create chunk:: #" << m_index << " of block #" << m_parentBlock << std::endl;
#endif // !_DEBUG

		}

		union
		{
			struct
			{
				SMemoryChunk* m_next;
				uint32_t m_parentBlock;
			};
			std::byte m_data[sizeof(size_type)];
		};

		uint32_t m_index = ++total_chunks;
		inline static uint32_t total_chunks = 0;
	};
	std::vector<SMemoryChunk*> m_blockList;
	uint32_t m_chunksPerBlock;
	SMemoryChunk* p_freeHead;
	uint32_t m_chunksInUse;

	SMemoryChunk* allocateBlock(SMemoryChunk* tail = nullptr);
	void swap(MemoryPool& other);
	friend std::ostream& operator<< <size_type> (std::ostream& os, const MemoryPool<size_type>& pool);

};


#endif

