#pragma once

#include "cpu/types.h"

class string {
  private:

  public:
    size_t m_length;
    size_t m_reserved;
    char_t *m_data;
    string();
    string(char_t const p_char);
    string(char_t const * const p_string);
    string(const string &);
    string(string &&);
    string &operator=(const string &);
    string &operator=(string &&);
    ~string();

    char_t const &operator[](size_t const p_index) const;
    char_t &operator[](size_t const p_index);

    string &operator+=(string const & p_string);

    char_t pop_char();
    char_t peek_char();
    void clear();
    inline size_t get_length() const;
};

inline size_t string::get_length() const {
  return this->m_length;
}
