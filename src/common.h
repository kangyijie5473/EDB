//
// Created by kang on 19-5-11.
//

#ifndef SRC_COMMON_H
#define SRC_COMMON_H

#include <cstdint>
#include <string>
#include <sstream>

const int C_OK = 0;
const int C_ERR = -1;
const int NOT_ATTACH = -1;
const char INT_ins = (char)0xcc;
struct Frame {
    char pc[19];
    uint64_t offset;
    char name[64];
    char rbp[19]; // "0x123456789abcdefg\0"
    char rsp[19];
};
template <typename T>
std::string getString(T n)
{
    std::stringstream newstr;
    newstr << n;
    return newstr.str();
}
#endif //SRC_COMMON_H
