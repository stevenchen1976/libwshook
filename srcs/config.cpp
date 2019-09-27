#include "config.hpp"
#include "utils.hpp"
#include <iostream>

namespace libwshook {

  config::config(std::string const &host,
      const int port,
      std::string const &access_key_id,
      std::string const &access_secret_key) :
        _remote("ws://" + host + ":" +  std::to_string(port)),
        _authToken(utils::encode64(access_key_id + ":" + access_secret_key)) {
    }

} // namespace libwshook
