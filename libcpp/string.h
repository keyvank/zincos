#pragma once

#include "cpu/types.h"
#include "kernel/heap.h"
#include "kernel/util.h"

class string {
  private:
    heap &m_heap;

    size_t m_length;
    size_t m_reserved;
  public:
    char_t *m_data;
    string(heap &m_heap);
    string(const string &) = delete;
    string(string &&) = delete;
    string &operator=(const string &) = delete;
    string &operator=(string &&) = delete;
    ~string();

    char_t const &operator[](size_t const p_index) const;
    char_t &operator[](size_t const p_index);

    void put_char(char_t const p_char);
    char_t pop_char();
    void clear();
    inline size_t get_length() const;
};

string::string(heap &p_heap) : m_heap(p_heap), m_length(0), m_reserved(1), m_data(nullptr) {
  m_data = reinterpret_cast<char_t *>(m_heap.allocate(sizeof(char_t) * m_reserved));
  m_data[0] = '\0';
}

char_t const &string::operator[](size_t const p_index) const {
  return this->m_data[p_index];
}

char_t &string::operator[](size_t const p_index) {
  return this->m_data[p_index];
}

void string::put_char(char_t const p_char) {
  this->m_data[this->m_length++] = p_char;
  if(this->m_length >= this->m_reserved) {
    this->m_reserved *= 2;
    addr_t new_location = this->m_heap.allocate(this->m_reserved * sizeof(char_t));
    memory_copy(reinterpret_cast<u8_t *>(this->m_data), reinterpret_cast<u8_t *>(new_location), sizeof(char_t) * this->m_length);
    this->m_heap.free(this->m_data);
    this->m_data = reinterpret_cast<char_t *>(new_location);
  }
  this->m_data[this->m_length] = '\0';
}

char_t string::pop_char() {
  char_t pop = this->m_data[--this->m_length];
  this->m_data[this->m_length] = '\0';
  return pop;
}

void string::clear() {
  this->m_heap.free(this->m_data);
  m_reserved = 1;
  m_length = 0;
  m_data = reinterpret_cast<char_t *>(m_heap.allocate(sizeof(char_t) * m_reserved));
  m_data[0] = '\0';
}

inline size_t string::get_length() const {
  return this->m_length;
}

string::~string() {
  this->m_heap.free(this->m_data);
}
