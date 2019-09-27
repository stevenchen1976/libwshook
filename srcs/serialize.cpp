#include "serialize.hpp"
#include <iostream>
#include <json/json.h>

namespace libwshook {

  Json::Value serialize::toJson(std::string const &msg) {
    Json::CharReaderBuilder builder;
    Json::CharReader *reader = builder.newCharReader();
    Json::Value json;
    std::string errors;

    reader->parse(msg.c_str(), msg.c_str() + msg.size(), &json, &errors);
    delete reader;

    return json;
  }

  std::string serialize::toString(Json::Value const &json) {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    return Json::writeString(builder, json);
  }

  std::string serialize::getKind(Json::Value const &json) {
    try {
      return json["kind"].asString();
    } catch (Json::RuntimeError const &e) {
      throw e;
    }
  }

} // namespace libwshook
