#pragma once

#include "cpu/types.h"
#include "kernel/heap.h"
#include "kernel/util.h"

class string {
  private:
    size_t m_length;
    size_t m_reserved;
  public:
    char_t *m_data;
    string();
    string(const string &) = delete;
    string(string &&) = delete;
    string &operator=(const string &) = delete;
    string &operator=(string &&) = delete;
    ~string();

    char_t const &operator[](size_t const p_index) const;
    char_t &operator[](size_t const p_index);

    void put_char(char_t const p_char);
    char_t pop_char();
    char_t peek_char();
    void clear();
    inline size_t get_length() const;
};

inline size_t string::get_length() const {
  return this->m_length;
}