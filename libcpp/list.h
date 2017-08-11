#pragma once

#include "cpu/types.h"

template <typename T>
class list {
  public:
    virtual void add(T const &p_value) = 0;
    virtual size_t get_size() const = 0;
    virtual T const &operator[](size_t const p_index) const = 0;
    virtual T &operator[](size_t const p_index) = 0;

    virtual ~list();
};

template <typename T>
list<T>::~list() { }
