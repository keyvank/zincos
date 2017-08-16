#pragma once

#include "cpu/types.h"
#include "libcpp/linked_list.h"
#include "libcpp/string.h"

class object {
  friend class kernel;
  friend class vfs;
private:
  object *m_parent;
  string m_name;
  linked_list<object *> m_entries;
public:
  object(object * const p_parent, string const &p_name);
  object(object const &) = delete;
  object(object &&) = delete;
  object &operator=(object const &) = delete;
  object &operator=(object &&) = delete;
  ~object();
};

class vfs {
  friend class kernel;
private:
  object m_root;
public:
  vfs();
  vfs(vfs const &) = delete;
  vfs(vfs &&) = delete;
  vfs &operator=(vfs const &) = delete;
  vfs &operator=(vfs &&) = delete;
  ~vfs();
};
