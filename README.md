# Pool-ADT
This project manages a "pool" of memory that is used to provide dynamic, resizable character arrays.
A pool is a consecutive block of memory. Portions of that block (sub-blocks) are provided to
clients via the pool_alloc function (similar to how malloc works). When finished with the
memory, the client must return it to the pool by calling pool_free.
A pool may only be initially allocated on the heap by a single malloc within pool_create. It is
freed by a single free within pool_destroy (and no reallocs). All addresses returned from
successful calls to pool_alloc (and pool_realloc) must be addressess from within that pool.
In addition to the large pool itself, the ADT must use a data structure to maintain the pool and keep track of which areas of the pool
have been allocated and which areas are still free.
