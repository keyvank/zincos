#pragma once

#include "cpu/types.h"

class string {
  private:
    size_t m_length;
    size_t m_reserved;
  public:
    char_t *m_data;
    string();
    string(char_t const * const p_string);
    string(const string &);
    string(string &&);
    string &operator=(const string &);
    string &operator=(string &&);
    string &operator=(char_t const * const p_string);
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
