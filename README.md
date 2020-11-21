# OptimizedHeaders
a repo with a goal of well optimized header files. While I put attention towards optimization by default, this repo is for header files that are explicitly performance focused

fast_mat.h
-----
STACK based matrix data structure, some functions are constexpr which is benificial when applicable

mat.h
-----
HEAP based matrix data structure, it is widely applicable to many usecases

tpool.h
-----
a lightweight LOCKING thread pool, with INDIVIDUAL QUEUES per thread. It works best with synthetic/well distributed workloads

stpool.h
-----
a lightweight LOCK-FREE SHARED QUEUE thread pool, utilizing atomic_queue.h from user from https://github.com/max0x7ba. This thread pool is faster then tpool.h if the distribution of work is nonuniform, otherwise its on average 15-25% slower (for synthetic workloads)

CUDA
-----
CUDA variants of these headers
