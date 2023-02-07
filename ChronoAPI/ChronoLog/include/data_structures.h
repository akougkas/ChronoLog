/*BSD 2-Clause License

Copyright (c) 2022, Scalable Computing Software Laboratory
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
//
// Created by kfeng on 3/30/22.
//

#ifndef CHRONOLOG_DATA_STRUCTURES_H
#define CHRONOLOG_DATA_STRUCTURES_H

#include <string>
#include <cstring>
#include <vector>
#include <cstdint>
#include <chrono>

typedef struct ChronoLogCharStruct {
private:
    char value[256];
    void Set(char* data_, size_t size) {
        snprintf(this->value, size+1, "%s", data_);
    }
    void Set(const std::string& data_) {
        snprintf(this->value, data_.length()+1, "%s", data_.c_str());
    }
public:
    ChronoLogCharStruct() {}

    ChronoLogCharStruct(const ChronoLogCharStruct &other) : ChronoLogCharStruct(other.value) {} /* copy constructor*/
    ChronoLogCharStruct(ChronoLogCharStruct &&other) : ChronoLogCharStruct(other.value) {} /* move constructor*/

    ChronoLogCharStruct(const char *data_) {
        snprintf(this->value, strlen(data_) + 1, "%s", data_);
    }

    ChronoLogCharStruct(std::string data_) : ChronoLogCharStruct(data_.c_str()) {}

    ChronoLogCharStruct(char *data_, size_t size) {
        snprintf(this->value, size, "%s", data_);
    }

    const char *c_str() const {
        return value;
    }

    std::string string() const {
        return std::string(value);
    }

    char* data() {
        return value;
    }

    size_t size() const {
        return strlen(value);
    }
    /**
   * Operators
   */
    ChronoLogCharStruct &operator=(const ChronoLogCharStruct &other) {
        strcpy(value,other.c_str());
        return *this;
    }
    /* equal operator for comparing two Chars. */
    bool operator==(const ChronoLogCharStruct &o) const {
        return strcmp(value, o.value) == 0;
    }
    ChronoLogCharStruct operator+(const ChronoLogCharStruct& o){
        std::string added=std::string(this->c_str())+std::string(o.c_str());
        return ChronoLogCharStruct(added);
    }
    ChronoLogCharStruct operator+(std::string &o)
    {
        std::string added=std::string(this->c_str())+o;
        return ChronoLogCharStruct(added);
    }
    ChronoLogCharStruct& operator+=(const ChronoLogCharStruct& rhs){
        std::string added=std::string(this->c_str())+std::string(rhs.c_str());
        Set(added);
        return *this;
    }
    bool operator>(const ChronoLogCharStruct &o) const {
        return  strcmp(this->value,o.c_str()) > 0;
    }
    bool operator>=(const ChronoLogCharStruct &o) const {
        return strcmp(this->value,o.c_str()) >= 0;
    }
    bool operator<(const ChronoLogCharStruct &o) const {
        return strcmp(this->value,o.c_str()) < 0;
    }
    bool operator<=(const ChronoLogCharStruct &o) const {
        return strcmp(this->value,o.c_str()) <= 0;
    }

} ChronoLogCharStruct;

namespace std {
    template<>
    struct hash<ChronoLogCharStruct> {
        size_t operator()(const ChronoLogCharStruct &k) const {
            std::string val(k.c_str());
            return std::hash<std::string>()(val);
        }
    };
}

#endif //CHRONOLOG_DATA_STRUCTURES_H
