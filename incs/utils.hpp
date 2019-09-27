#ifndef UTILS_HPP
#define UTILS_HPP
#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <json/json.h>
#include <map>
#include <any>

namespace libwshook::utils {

  typedef void (*hook_t)();
  typedef std::map<std::string, hook_t> hooks_t;

  std::string encode64(std::string const &val);
  std::string lower(std::string const &s);

} // namespace libwshook::utils

#endif /* !UTILS_HPP */
