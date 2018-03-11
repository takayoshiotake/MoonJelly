//
//  MJDefer.h
//  MoonJelly
//
//  Created by OTAKE Takayoshi on 2018/03/11.
//

#pragma once

#include <functional>
#include <utility>

namespace MoonJelly {

    template <typename T>
    struct MJDefer {
        T defer_;
        
        explicit MJDefer(T && defer) noexcept
        : defer_(defer) {
        }
        
        MJDefer(MJDefer const &) = delete;
        MJDefer(MJDefer && obj)
        : defer_(obj.defer_) {
        }
        MJDefer & operator=(MJDefer const &) = delete;
//        MJDefer & operator=(MJDefer &&) = delete;
        
        ~MJDefer() noexcept {
            defer_();
        }
    };
    
    template <typename T>
    auto make_defer(T && defer) noexcept {
        return MJDefer<T>(std::move(defer));
    }

}
