#include <iostream>
#include <tpool.hpp>
#include <CUDA/mat.hpp>

#include <ratio>
#include <chrono>

int main()
{
    double g_res, c_res;
    int sum;
    ThreadPool pool;
    pool.Start();

    for (int j = 0; j < 1000; ++j)
    {

        pool.AddTask([&g_res, &c_res, &sum]() {
            for (int i = 1;; i += 10)
            {
                fMat a(1, i);
                fMat b(i, 1);

                using namespace std::chrono;

                {
                    high_resolution_clock::time_point t1 = high_resolution_clock::now();
                    a.DotBench(b, true);
                    high_resolution_clock::time_point t2 = high_resolution_clock::now();
                    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
                    g_res = time_span.count();
                }

                {
                    high_resolution_clock::time_point t1 = high_resolution_clock::now();
                    a.DotBench(b, false);
                    high_resolution_clock::time_point t2 = high_resolution_clock::now();
                    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

                    c_res = time_span.count();
                }
                if (g_res < c_res)
                {
                    sum += i;
                    break;
                }
            }
        });
    }
    while (pool.Jobs())
        ;

    std::cout << sum / 500 << '\n';
    return 0;
}
