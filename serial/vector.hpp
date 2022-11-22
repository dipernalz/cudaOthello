#ifndef VECTOR_H
#define VECTOR_H

#include <cassert>
#include <cinttypes>
#include <cstdlib>

template <class T>
class vector {
   private:
    T *data;
    uint8_t n;

   public:
    vector() {
        data = new T[N * N];
        n = 0;
    }

    ~vector() { delete[] data; }

    inline void push(T t) { data[n++] = t; }

    inline bool is_empty() { return !n; }

    inline T get_random_entry() {
        assert(n != 0);
        return data[rand() % n];
    }
};

#endif
