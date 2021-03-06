#pragma once

#include "cpu/types.h"
#include "kernel/heap.h"
#include "kernel/util.h"
#include "libcpp/list.h"

template <typename T>
class linked_list : public list<T> {
  private:
    struct node { // nested class
        T value;
        node *next;
    };
    node *m_first;
    size_t m_size;
  public:
    linked_list();
    T const &operator[](size_t const p_index) const;
    T &operator[](size_t const p_index);
    ~linked_list();

    void add(T const &p_value);
    void remove(size_t const p_index);
    inline size_t get_size() const;
};

template <typename T>
linked_list<T>::linked_list() : m_first(NULL), m_size(0) {

}

template <typename T>
T const &linked_list<T>::operator[](size_t const p_index) const {
  node *current = this->m_first;
  for(size_t i = 0; i < p_index; i++)
    current = current->next;
  return current->value;
}

template <typename T>
T &linked_list<T>::operator[](size_t const p_index) {
  node *current = this->m_first;
  for(size_t i = 0; i < p_index; i++)
    current = current->next;
  return current->value;
}

template <typename T>
void linked_list<T>::remove(size_t const p_index) {
  if(p_index > 0) {
    node *current = this->m_first;
    for(size_t i = 0; i < p_index - 1; i++)
      current = current->next;
    node *next = current->next->next;
    delete current->next;
    current->next = next;
  } else {
    node *next = this->m_first->next;
    delete this->m_first;
    this->m_first = next;
  }
  this->m_size--;
}


template <typename T>
void linked_list<T>::add(T const &p_value) {
  if(this->m_first == NULL) {
    this->m_first = new node;
    this->m_first->value = p_value;
    this->m_first->next = NULL;
  } else {
    node *current = this->m_first;
    while(current->next)
      current = current->next;
    current->next = new node;
    current->next->value = p_value;
    current->next->next = NULL;
  }
  this->m_size++;
}

template <typename T>
inline size_t linked_list<T>::get_size() const {
  return this->m_size;
}

template <typename T>
linked_list<T>::~linked_list() {
  node *current = this->m_first;
  node *temp;
  while(current) {
    temp = current;
    current = current->next;
    delete temp;
  }
}
