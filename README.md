# OptimizedHeaders (LEGACY)
a repo with a goal of well optimized header files. While I put attention towards optimization by default, this repo is for header files that are explicitly performance focused

fast_mat.hpp
-----
STACK based matrix data structure, all constexpr

mat.hpp
-----
HEAP based matrix data structure, it is widely applicable to many usecases

tpool.hpp
-----
a simple LOCKING thread pool, with INDIVIDUAL QUEUES per thread. It works best with synthetic/well distributed workloads. Extremely Fast

utils.hpp
-----
a small set of utility functions
* ConfirmContexpr (consteval for versions less then C++20)
* CopyFast, a meta-function parameter that passes as const refference or copy based on whats fastest for way to pass as value for that type
