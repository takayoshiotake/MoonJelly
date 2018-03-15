//
//  MJWorkQueue2Test.cpp
//  MoonJellySample
//
//  Created by OTAKE Takayoshi on 2018/03/16.
//

#include <iostream>

#include "MoonJelly/MJWorkQueue2.hpp"

void MJWorkQueue2Test() {
    std::cout << "Test: " << __FUNCTION__ << std::endl;
    
    MoonJelly::MJWorkQueue2 queue;
    auto start = std::chrono::system_clock::now();
    auto record_time = [&]() {
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() << ": ";
    };
    
    queue.async([&](auto & self) {
        record_time();
        std::cout << "Hello, Async!" << std::endl;
    }, 1.0);
    
    queue.async([&](auto & self) {
        record_time();
        std::cout << "1" << std::endl;
    }, 3.0);
    auto & item = queue.async([&](auto & self) {
        record_time();
        std::cout << "2" << std::endl;
    });
    item.cancel();
    try {
        item.wait();
    }
    catch (std::runtime_error const & e) {
        record_time();
        std::cout << "Error: 2: " << e.what() << std::endl;
    }
    queue.async([&](auto & self) {
        record_time();
        std::cout << "3" << std::endl;
    }).wait();
    queue.async([&](auto & self) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        record_time();
        std::cout << "4" << std::endl;
    }, 0.2);
    queue.async([&](auto & self) {
        record_time();
        std::cout << "5" << std::endl;
    });
    
    record_time();
    std::cout << "Hello, World!\n";
    
    std::this_thread::sleep_for(std::chrono::milliseconds(3100));
    record_time();
}
