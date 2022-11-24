#ifndef VECTOR_H
#define VECTOR_H

#include <cassert>
#include <cinttypes>
#include <cstdlib>
#include <iostream>

template <class T>
class vector {
   private:
    T *data;
    uint8_t n;

   public:
    vector() {
        data = (T *)malloc(N * N * sizeof(T));
        n = 0;
    }

    /* ~vector() { free(data); } */

    inline T &get(int i) {
        assert(i < n);
        return data[i];
    }

    inline void push(T t) { data[n++] = t; }

    inline T remove(uint8_t i) {
        T entry = data[i];
        data[i] = data[--n];
        return entry;
    }

    inline uint8_t size() { return n; }

    inline bool is_empty() { return !n; }

    inline T get_random_entry() {
        assert(n != 0);
        return data[rand() % n];
    }
};

#endif
