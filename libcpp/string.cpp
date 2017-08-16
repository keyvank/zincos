#include "libcpp/string.h"
#include "kernel/util.h"

string::string() : m_length(0), m_reserved(1), m_data(nullptr) {
  this->m_data = new char_t[this->m_reserved];
  this->m_data[this->m_length] = '\0';
}

string::string(char_t const p_char) : m_length(1), m_reserved(2), m_data(nullptr) {
  this->m_data = new char_t[this->m_reserved];
  this->m_data[0] = p_char;
  this->m_data[this->m_length] = '\0';
}

char_t const &string::operator[](size_t const p_index) const {
  return this->m_data[p_index];
}

char_t &string::operator[](size_t const p_index) {
  return this->m_data[p_index];
}

char_t string::pop_char() {
  char_t pop = this->m_data[--this->m_length];
  this->m_data[this->m_length] = '\0';
  return pop;
}

char_t string::peek_char() {
  char_t peek = this->m_data[0];
  memory_copy(reinterpret_cast<u8_t *>(this->m_data) + 1, reinterpret_cast<u8_t *>(this->m_data), this->m_length);
  this->m_length--;
  return peek;
}

void string::clear() {
  delete[] this->m_data;
  this->m_reserved = 1;
  this->m_length = 0;
  this->m_data = new char_t[this->m_reserved];
  this->m_data[this->m_length] = '\0';
}

string::~string() {
  delete[] this->m_data;
}

string::string(const string &p_string) : m_length(p_string.m_length), m_reserved(p_string.m_length + 1), m_data(nullptr) {
  this->m_data = new char_t[m_reserved];
  memory_copy(reinterpret_cast<u8_t *>(p_string.m_data), reinterpret_cast<u8_t *>(this->m_data), sizeof(char_t) * p_string.m_length);
  this->m_data[this->m_length] = '\0';
}
string::string(string &&p_string) : m_length(p_string.m_length), m_reserved(p_string.m_reserved), m_data(p_string.m_data) {
  p_string.m_data = nullptr;
}
string &string::operator=(const string &p_string) {
  delete[] this->m_data;
  this->m_length = p_string.m_length;
  this->m_reserved = p_string.m_length + 1;
  this->m_data = new char_t[m_reserved];
  memory_copy(reinterpret_cast<u8_t *>(p_string.m_data), reinterpret_cast<u8_t *>(this->m_data), sizeof(char_t) * p_string.m_length);
  this->m_data[this->m_length] = '\0';
  return *this;
}
string &string::operator=(string &&p_string) {
  delete[] this->m_data;
  this->m_length = p_string.m_length;
  this->m_reserved = p_string.m_reserved;
  this->m_data = p_string.m_data;
  p_string.m_data = nullptr;
  return *this;
}

string &string::operator+=(string const & p_string) {
  if(this->m_length + p_string.m_length >= this->m_reserved) {
    this->m_reserved = (this->m_length + p_string.m_length) * 2;
    char_t *new_location = new char_t[m_reserved];
    memory_copy(reinterpret_cast<u8_t *>(this->m_data), reinterpret_cast<u8_t *>(new_location), this->m_length);
    delete[] this->m_data;
    this->m_data = new_location;
  }
  memory_copy(reinterpret_cast<u8_t *>(p_string.m_data), reinterpret_cast<u8_t *>(this->m_data) + this->m_length, p_string.m_length);
  this->m_length = this->m_length + p_string.m_length;
  this->m_data[this->m_length] = '\0';
  return *this;
}

string::string(char_t const * const p_string) : string() {
  char_t const * curr = p_string;
  while(*curr) {
    *this += *curr;
    curr++;
  }
}
