//
//  MJWorkQueueTest.cpp
//  MoonJellySample
//
//  Created by OTAKE Takayoshi on 2018/03/11.
//

#include <iostream>

#include "MoonJelly/MJWorkQueue.hpp"

void MJWorkQueueTest() {
    std::cout << "Test: " << __FUNCTION__ << std::endl;
    
    MoonJelly::MJWorkQueue queue;
    
    queue.async([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Hello, Async!" << std::endl;
    });
    
    queue.async([]() {
        std::cout << "1" << std::endl;
    });
    auto & item = queue.async([]() {
        std::cout << "2" << std::endl;
    });
    item.cancel();
    try {
        item.wait();
    }
    catch (std::runtime_error const & e) {
        std::cout << "Error: 2: " << e.what() << std::endl;
    }
    queue.async([]() {
        std::cout << "3" << std::endl;
    }).wait();
    queue.async([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "4" << std::endl;
    });
    queue.async([]() {
        std::cout << "5" << std::endl;
    }); // will be cancelled
    
    std::cout << "Hello, World!\n";
}

