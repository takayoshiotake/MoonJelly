//
//  main.cpp
//  MoonJellySample
//
//  Created by OTAKE Takayoshi on 2018/03/11.
//

#include <iostream>

int main(int argc, const char * argv[]) {
    extern void MJDeferTest();
    MJDeferTest();
    
    std::cout << std::endl;
    
    extern void MJWorkQueueTest();
    MJWorkQueueTest();
    
    return 0;
}
