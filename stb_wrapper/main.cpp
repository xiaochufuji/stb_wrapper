#include "stb_wrapper.h"
#include <chrono>  
#include <iostream> 

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    xiaochufuji::StbWrapper sw("go.png", xiaochufuji::RGB);
    sw.resize(1000, 1000);
    sw.rotate(90);
    sw.autoSave("go.jpg");

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "run: " << duration.count() << " ms" << std::endl;

    return 0;
}