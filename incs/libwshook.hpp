#ifndef LIBWSHOOK_HPP
#define LIBWSHOOK_HPP
#pragma once

#include "ws.hpp"
#include "utils.hpp"

namespace libwshook {


  class wshook : public ws {

    public:
      template<typename... Args>
      explicit wshook(Args&&... args) try : ws(std::forward<Args>(args)...) {} catch(...) { throw; }
      virtual ~wshook() = default;
      wshook(wshook const &) = delete;
      wshook& operator=(wshook const &) = delete;
      wshook(wshook &&) = delete;
      wshook& operator=(wshook &&) = delete;
      __attribute__ ((visibility ("default"))) void hook(std::string const &kind, utils::hook_t);
  };

} // namespace libwshook

#endif /* !LIBWSHOOK_HPP */
