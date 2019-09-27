#ifndef CONFIG_HPP
#define CONFIG_HPP
#pragma once

#include <string>

namespace libwshook {

  class config {
    public:
      std::string const _remote;
      std::string const _authToken;
      __attribute__ ((visibility ("default"))) config(std::string const &host, const int port, std::string const &access_key_id, std::string const &access_secret_key);
  };

} // namespace libwshook

#endif /* !CONFIG_HPP */
