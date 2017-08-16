#include "cpu/types.h"
#include "kernel/vfs.h"

object::object(object * const p_parent, string const &p_name) : m_parent(p_parent), m_name(p_name), m_entries() {

}

object::~object() {
}

vfs::vfs() : m_root(nullptr, "") {

}

vfs::~vfs() {
}
