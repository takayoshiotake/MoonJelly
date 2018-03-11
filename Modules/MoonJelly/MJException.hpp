//
//  MJException.hpp
//  MoonJelly
//
//  Created by OTAKE Takayoshi on 2018/03/11.
//

#pragma once

#include <exception>
#include <string>
#include <vector>
#include <sstream>
#include <thread>

#define MoonJelly__make_stack_frame() MoonJelly::make_stack_frame(__FILE__, __LINE__)

namespace MoonJelly {
    
    struct MJStackFrame {
        char const * file;
        int line;
        std::thread::id thread_id;
    };
    
    struct MJException: public std::exception {
        std::string message;
        std::vector<MJStackFrame> stack_frames;
        
        explicit MJException(std::string const & message, MJStackFrame const & stack_frame) noexcept
        : message(message) {
            stack_frames.push_back(stack_frame);
        }
        
        virtual char const * what() const noexcept override {
            static std::stringstream ss;
            ss.str("");
            ss.clear(std::stringstream::goodbit);
            
            ss << message;
            for (auto sf: stack_frames) {
                ss << std::endl << "  " << sf.thread_id << " at " << sf.file << "(" << sf.line << ")";
            }
            return ss.str().c_str();
        }
            
        [[noreturn]]
        void rethrow(MJStackFrame const & stack_frame) {
            stack_frames.push_back(stack_frame);
            throw *this;
        }
    };

    MJStackFrame make_stack_frame(char const * file, int line) {
        return { file, line, std::this_thread::get_id() };
    }
    
}
