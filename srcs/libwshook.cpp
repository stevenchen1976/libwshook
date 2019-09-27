#include "libwshook.hpp"
#include "ws.hpp"
#include <iostream>

namespace libwshook {

  void wshook::hook(std::string const &kind, utils::hook_t f) {
    std::cout << "[LIBSHOOK] Hooking " + kind + " function\n";
    _hooks[utils::lower(kind)] = f;
  }

} // namespace libwshook
