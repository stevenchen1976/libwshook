#ifndef SERIALIZABLE_HPP
#define SERIALIZABLE_HPP
#pragma once

#include <json/json.h>

namespace libwshook {

  class serialize {
    public:
      __attribute__ ((visibility ("default"))) serialize(void) = default;
      Json::Value toJson(std::string const &msg);
      std::string toString(Json::Value const &json);
      std::string getKind(Json::Value const &);
  };

} // namespace libwshook

#endif /* !SERIALIZABLE_HPP */
