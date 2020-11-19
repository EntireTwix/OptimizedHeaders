#include <iostream>
#include <CUDA/mat.hpp>

#include <ratio>
#include <chrono>

int main()
{
    double g_res, c_res;
    int sum;
    for (int j = 0; j < 10; ++j)
    {
        for (int i = 1;; i += 10)
        {
            system("CLS");
            fMat a(1, i);
            fMat b(i, 1);

            std::cout << "elements " << i * 2 << '\n';

            using namespace std::chrono;

            std::cout << "With GPU\n";
            {
                high_resolution_clock::time_point t1 = high_resolution_clock::now();
                a.DotBench(b, true);
                high_resolution_clock::time_point t2 = high_resolution_clock::now();
                duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
                std::cout << "It took me " << time_span.count() << " seconds.";
                std::cout << std::endl;
                g_res = time_span.count();
            }

            std::cout << "With CPU\n";
            {
                high_resolution_clock::time_point t1 = high_resolution_clock::now();
                a.DotBench(b, false);
                high_resolution_clock::time_point t2 = high_resolution_clock::now();
                duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
                std::cout << "It took me " << time_span.count() << " seconds.";
                std::cout << std::endl;
                c_res = time_span.count();
            }
            if (g_res < c_res)
            {
                sum += i;
                break;
            }
        }
    }

    std::cout << sum / 5 << '\n';
    return 0;
}
