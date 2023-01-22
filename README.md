# MemoryPool
This is a memory pool which can be used as a simple allocator for quick allocation/dellocation of large amount of objects.  
The performance boost over heap allocation becomes larger as the number of objects to be allocated increases.  
Memory Pool is instantiated in the following manner:
`MemoryPool<type> poolName(pool_size);`  
To allocate memory: `poolName.malloc()`  
To free a specific chunk: `poolName.free(chunkPointer)`  
