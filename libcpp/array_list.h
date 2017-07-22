#pragma once

#include "cpu/types.h"
#include <libcpp/list.h>

template <typename T>
class array_list : public list<T> {
  private:
    T *m_data;
  public:
    T *data;
    array_list();
    array_list(size_t const p_size);
    T const &operator[](size_t const p_index) const;
    T &operator[](size_t const p_index);
};

template <typename T>
array_list<T>::array_list() {
}

template <typename T>
array_list<T>::array_list(size_t const p_init_size) {
}

template <typename T>
T const &array_list<T>::operator[](size_t const p_index) const {
  return this->m_data[p_index];
}

template <typename T>
T &array_list<T>::operator[](size_t const p_index) {
  return this->m_data[p_index];
}
