#include "CommonIncludes.h"
#include "MemoryPool.h"
#include <iostream>

template<typename size_type>
MemoryPool<size_type>::MemoryPool(uint32_t chunksPerBlock) :
	m_chunksPerBlock(chunksPerBlock),
	p_freeHead(nullptr),
	m_chunksInUse(0)
{
	if (chunksPerBlock <= 0)
		throw std::invalid_argument("Chunks per blocks must be positive");
}

//move 
template<typename size_type>
MemoryPool<size_type>::MemoryPool(MemoryPool<size_type>&& other) noexcept :
	m_chunksPerBlock(std::exchange(other.m_chunksPerBlock, 0)),
	m_chunksInUse(std::exchange(other.m_chunksInUse, 0)),
	p_freeHead(std::exchange(other.p_freeHead, nullptr)),
	m_blockList(std::move(other.m_blockList))
{

#ifdef _DEBUG
	std::cout << "Move constructor called" << std::endl;
	for (auto it : other.m_blockList)
	{
		std::cout << &it << ", ";
	}
#endif // !_DEBUG

}

//copy and swap idiom
template<typename size_type>
MemoryPool<size_type>& MemoryPool<size_type>::operator=(MemoryPool<size_type> other)
{
	swap(other);
	return *this;
}

template<typename size_type>
MemoryPool<size_type>::~MemoryPool()
{
	for (auto block : m_blockList)
	{
		std::free(block);
	}
}

template<typename size_type>
void* MemoryPool<size_type>::malloc()
{
	if (p_freeHead == nullptr)
		p_freeHead = allocateBlock();
	SMemoryChunk* chunk = p_freeHead;
	p_freeHead = p_freeHead->m_next;
	m_chunksInUse++;

#ifdef _DEBUG
	std::cout << "Handed out:: chunk #" << chunk->m_index << " of block #" << chunk->m_parentBlock << std::endl;
#endif // !_DEBUG

	return chunk;
}

template<typename size_type>
void MemoryPool<size_type>::free(void* pChunk)
{
	SMemoryChunk* chunk = reinterpret_cast<SMemoryChunk*>(pChunk);
	m_chunksInUse--;
	chunk->m_parentBlock = std::floor(m_chunksInUse / m_blockList.size());
	chunk->m_next = p_freeHead;
	p_freeHead = chunk;

#ifdef _DEBUG
	std::cout << "Added to pool:: chunk #" << chunk->m_index << " of block #" << chunk->m_parentBlock << std::endl;
#endif // !_DEBUG

}

template<typename size_type>
void MemoryPool<size_type>::freeBlocks(uint32_t blocks)
{
	if (blocks < 0)
		throw std::invalid_argument("Number of blocks must positive");

	blocks = std::min(blocks, (uint32_t)m_blockList.size());

#ifdef _DEBUG
	std::cout << "Free " << blocks << " blocks. Previous head : " << p_freeHead << " | ";
#endif // !_DEBUG

	uint32_t i = blocks;
	while (i)
	{
		while (p_freeHead && p_freeHead->m_parentBlock == m_blockList.size() - 1)
			p_freeHead = p_freeHead->m_next;
		std::free(m_blockList.back());
		m_blockList.pop_back();
		i--;
	}

#ifdef _DEBUG
	std::cout << "Current head : " << p_freeHead << std::endl;
#endif // !_DEBUG

	SMemoryChunk::total_chunks -= blocks * m_chunksPerBlock;
}

template<typename size_type>
bool MemoryPool<size_type>::empty() const
{
	return (m_chunksInUse == 0);
}

template<typename size_type>
uint32_t MemoryPool<size_type>::allocatedChunks() const
{
	return m_chunksInUse;
}

template<typename size_type>
uint32_t MemoryPool<size_type>::allocatedBlocks() const
{
	return m_blockList.size();
}

template<typename size_type>
uint32_t MemoryPool<size_type>::chunksPerBlock() const
{
	return m_chunksPerBlock;
}

template<typename size_type>
std::size_t MemoryPool<size_type>::chunkSize() const
{
	return sizeof(size_type);
}

template<typename size_type>
std::size_t MemoryPool<size_type>::blockSize() const
{
	return m_chunksPerBlock * sizeof(size_type);
}

template<typename size_type>
void MemoryPool<size_type>::reserveBlocks(uint32_t blocks)
{
	m_blockList.reserve(blocks);
	while (blocks > m_blockList.size())
		p_freeHead = allocateBlock(p_freeHead);

#ifdef _DEBUG
	std::cout << "Reserved blocks. Free head moved to : " << p_freeHead << std::endl;
#endif // !_DEBUG

}

template<typename size_type>
MemoryPool<size_type>::SMemoryChunk* MemoryPool<size_type>::allocateBlock(SMemoryChunk* tail)
{
	SMemoryChunk* block = reinterpret_cast<SMemoryChunk*>(std::malloc(sizeof(SMemoryChunk) * m_chunksPerBlock));
	if (block == nullptr)
		throw std::bad_alloc();
	m_blockList.push_back(block);

#ifdef _DEBUG
	std::cout << "Allocated new block #" << m_blockList.size() - 1 << std::endl;
#endif // !_DEBUG

	SMemoryChunk* chunk = block;
	chunk->m_parentBlock = m_blockList.size() - 1;

	for (uint32_t i = 0; i < m_chunksPerBlock - 1; ++i)
	{
		new(chunk) SMemoryChunk(chunk + 1, m_blockList.size() - 1);
		chunk = chunk->m_next;
	}
	new(chunk) SMemoryChunk(tail, m_blockList.size() - 1);
	return block;
}

template<typename size_type>
void MemoryPool<size_type>::swap(MemoryPool<size_type>& other)
{
	std::swap(m_chunksPerBlock, other.m_chunksPerBlock);
	std::swap(m_chunksInUse, other.m_chunksInUse);
	std::swap(p_freeHead, other.p_freeHead);
	std::swap(m_blockList, other.m_blockList);
}

template<typename size_type>
std::ostream& operator<< (std::ostream& os, const MemoryPool<size_type>& pool)
{
	os << "Chunk size: " << sizeof(size_type) << ",  Chunks in use: " << pool.m_chunksInUse << ", Chunks per block: " <<
		pool.m_chunksPerBlock << ", Free head: " << pool.p_freeHead << ", Block list size: " << pool.m_blockList.size() <<
		std::endl << "Block pointers as follows: ";
	for (auto it : pool.m_blockList)
		os << &it << ", ";
	return os;
}
