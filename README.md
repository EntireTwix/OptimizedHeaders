# OptimizedHeaders
a repo with a goal of well optimized header files. While I put attention towards optimization by default, this repo is for header files that are explicitly performance focused

fast_mat.hpp
-----
STACK based matrix data structure, some functions are constexpr which is benificial when applicable

mat.hpp
-----
HEAP based matrix data structure, it is widely applicable to many usecases

tpool.hpp
-----
a lightweight LOCKING thread pool, with INDIVIDUAL QUEUES per thread. It works best with synthetic/well distributed workloads. Extremely Fast

stpool.hpp
-----
a lightweight LOCK-FREE SHARED QUEUE thread pool, utilizing atomic_queue.h from user from https://github.com/max0x7ba. This thread pool is faster then tpool.h if the distribution of work is nonuniform, otherwise its on average 15-35% slower (for synthetic workloads).

utils.hpp
-----
a small set of utility functions
* ConfirmContexpr (consteval for versions less then C++20)
* CopyFast, a meta-function parameter that passes as const refference or copy based on whats fastest for way to pass as value for that type
* CombineBits, variadically combines any arguments into an array of chars
* File Reading, functions that parse files with simple delimenator formatting
* Split, string splitting function for parsing
* JSON I/O in the form of Save/Load for nlohmann/json repo
