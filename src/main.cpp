#include <iostream>

int main(int argc, char** argv) {
#ifdef HCS_VERSION
    const char* version = HCS_VERSION;
#else
    const char* version = "0.0.0";
#endif

    std::cout << "Head Create Script Compiler v" << version << std::endl;
    return 0;
}
