#pragma once

#include "cpu/types.h"
#include "kernel/heap.h"
#include "libcpp/list.h"

template <typename T>
class linked_list : public list<T> {
  private:
    struct node { // nested class
        T value;
        node *next;
    };
    heap &m_heap;
    node *m_first;
    size_t m_size;
  public:
    linked_list(heap &m_heap);
    T const &operator[](size_t const p_index) const;
    T &operator[](size_t const p_index);
    ~linked_list();

    void add(T const &p_value);
    size_t get_size() const;
};

template <typename T>
linked_list<T>::linked_list(heap &p_heap) : m_heap(p_heap), m_first(NULL), m_size(0) {

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
void linked_list<T>::add(T const &p_value) {
  if(this->m_first == NULL) {
    this->m_first = reinterpret_cast<node *>(this->m_heap.allocate(sizeof(node)));
    this->m_first->value = p_value;
    this->m_first->next = NULL;
  } else {
    node *current = this->m_first;
    while(current->next)
      current = current->next;
    current->next = reinterpret_cast<node *>(this->m_heap.allocate(sizeof(node)));
    current->next->value = p_value;
    current->next->next = NULL;
  }
  this->m_size++;
}

template <typename T>
size_t linked_list<T>::get_size() const {
  return this->m_size;
}

template <typename T>
linked_list<T>::~linked_list() {
  node *current = this->m_first;
  node *temp;
  while(current) {
    temp = current;
    current = current->next;
    this->m_heap.free(temp);
  }
}
