//
//  MJExceptionTest.cpp
//  MoonJellySample
//
//  Created by OTAKE Takayoshi on 2018/03/11.
//

#include <iostream>

#include "MoonJelly/MJException.hpp"

void MJExceptionTest() {
    std::cout << "Test: " << __FUNCTION__ << std::endl;
    
    try {
        try {
            throw MoonJelly::MJException("original", MoonJelly__make_stack_frame());
        }
        catch (MoonJelly::MJException & e) {
            std::cout << "catch: " << e.what() << std::endl;
            e.message = "overwritten";
            e.rethrow(MoonJelly__make_stack_frame());
        }
    }
    catch (MoonJelly::MJException & e) {
        std::cout << "catch: " << e.what() << std::endl;
    }
}
