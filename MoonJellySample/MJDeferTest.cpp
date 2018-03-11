//
//  MJDeferTest.cpp
//  MoonJellySample
//
//  Created by OTAKE Takayoshi on 2018/03/11.
//

#include <iostream>
#include <sstream>

#include "MoonJelly/MJDefer.hpp"

void MJDeferTest() {
    std::cout << "Test: " << __FUNCTION__ << std::endl;
    
    std::stringstream ss;
    {
        auto defer = MoonJelly::make_defer([&]() {
            ss << "1";
        });
        {
            auto defer_a = MoonJelly::make_defer([&]() {
                ss << "2";
            });
            auto defer_b = MoonJelly::make_defer([&]() {
                ss << "3";
            });
            ss << "4";
        }
        ss << "5";
    }
    std::cout << ss.str() << ": " << (ss.str().compare("43251") == 0 ? "passed" : "failed") << std::endl;
}
