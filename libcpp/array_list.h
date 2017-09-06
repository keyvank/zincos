#pragma once

#include "cpu/types.h"
#include "kernel/heap.h"
#include "kernel/util.h"
#include "libcpp/list.h"

template <typename T>
class array_list : public list<T> {
  private:
    T *m_data;
    size_t m_size;
    size_t m_reserved;
  public:
    T *data;
    array_list();
    array_list(size_t const p_size);
    T const &operator[](size_t const p_index) const;
    T &operator[](size_t const p_index);
    ~array_list();
    void add(T const &p_value);
    bool contains(T const &p_value) const;
    s32_t find(T const &p_value) const;
    void remove(size_t const p_index);
    inline size_t get_size() const;
};

template <typename T>
array_list<T>::array_list() : m_data(NULL), m_size(0), m_reserved(1) {
  m_data = new T[this->m_reserved];
}

template <typename T>
array_list<T>::array_list(size_t const p_reserved) : m_data(NULL), m_size(0), m_reserved(p_reserved) {
  m_data = new T[this->m_reserved];
}

template <typename T>
T const &array_list<T>::operator[](size_t const p_index) const {
  return this->m_data[p_index];
}

template <typename T>
T &array_list<T>::operator[](size_t const p_index) {
  return this->m_data[p_index];
}

template <typename T>
void array_list<T>::add(T const &p_value) {
  //UNUSED(p_value);
  this->m_data[this->m_size++] = p_value;
  if(this->m_size >= this->m_reserved) {
    this->m_reserved *= 2;
    T *new_location = new T[this->m_reserved];
    memory_copy(reinterpret_cast<u8_t *>(this->m_data), reinterpret_cast<u8_t *>(new_location), sizeof(T) * this->m_size);
    delete[] this->m_data;
    this->m_data = new_location;
  }
}

template <typename T>
inline size_t array_list<T>::get_size() const {
  return this->m_size;
}

template <typename T>
bool array_list<T>::contains(T const &p_value) const {
  for(size_t i = 0; i < this->get_size(); i++) {
    if((*this)[i] == p_value) return true;
  }
  return false;
}

// Returns -1 when not found
template <typename T>
s32_t array_list<T>::find(T const &p_value) const {
  for(size_t i = 0; i < this->get_size(); i++) {
    if((*this)[i] == p_value) return i;
  }
  return -1;
}

// Fails silently
template <typename T>
void array_list<T>::remove(size_t const p_index) {
  if(p_index < this->m_size) {
    memory_copy(reinterpret_cast<u8_t *>(this->m_data + p_index + 1), reinterpret_cast<u8_t *>(this->m_data + p_index), sizeof(T) * (this->m_size - p_index - 1));
    this->m_size--;
  }
}

template <typename T>
array_list<T>::~array_list() {
  delete[] this->m_data;
}
