//
// Created by Bugen Zhao on 7/13/20.
//

#include <include/random.hh>
#include <include/types.h>

namespace random {
    static int next = 1018;

    uint16_t rand() {
        next = next * 1103515245 + 12345;
        return static_cast<uint16_t>(next / 65536);
    }
}
