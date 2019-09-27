#include "ws.hpp"
#include "config.hpp"
#include "libwshook.hpp"
#include "utils.hpp"
#include <boost/exception/diagnostic_information.hpp>
#include <boost/exception_ptr.hpp>
#include <iostream>

namespace libwshook {

  void ws::initWebsocketClient() {
    try {
      // Disable logging
      _c.clear_access_channels(websocketpp::log::alevel::all);
#ifdef DEBUG
      // Set logging to be pretty verbose (everything except message payloads)
      _c.set_access_channels(websocketpp::log::alevel::all);
      _c.clear_access_channels(websocketpp::log::alevel::frame_payload);
#endif
      // Initialize ASIO
      _c.init_asio();
      // Register handlers
      _c.set_message_handler(websocketpp::lib::bind(&libwshook::ws::on_message, this, &_c, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
      _c.set_open_handler(websocketpp::lib::bind(&libwshook::ws::on_open, this, &_c, websocketpp::lib::placeholders::_1));
      _c.set_close_handler(websocketpp::lib::bind(&libwshook::ws::on_close, this, websocketpp::lib::placeholders::_1));
      _c.set_fail_handler(websocketpp::lib::bind(&libwshook::ws::on_fail, this, websocketpp::lib::placeholders::_1));
      // Create connection
      connect();
    } catch (websocketpp::exception const &e) {
      std::cerr << e.what() << std::endl;
    } catch (std::exception const &e) {
      std::cerr << e.what() << std::endl;
    } catch (...) {
      std::cerr << "Cannot init websocket" << std::endl;
      throw;
    }
  }

  void ws::connect() {
    websocketpp::lib::error_code ec;

    client::connection_ptr const con = _c.get_connection(_config._remote, ec);

    if (ec) {
      throw std::runtime_error("Could not create ws connection because: " + ec.message() + "\n");
    }
    con->append_header("Authorization", _config._authToken);
    _c.connect(con);
  }

  void ws::reconnect() {
    std::cerr << "[LIBWSHOOK] Reconnect in 3000 ms" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    connect();
  }

  void ws::run() {
    try {
      _c.run();
    } catch (websocketpp::exception const & e) {
      std::cout << e.what() << std::endl;
    }
  }

  void ws::on_message([[maybe_unused]] client* c, websocketpp::connection_hdl hdl, message_ptr const &msg) {
    _hdl = std::move(hdl);
    try {
      auto json = toJson(msg->get_payload());
      auto const kind = utils::lower(getKind(json));
      if (kind == "info") {
        handleInfo(json["message"].asString());
      } else if (kind == "pull") {
        handlePull(std::move(json));
      } else if (kind == "ping") {
        handlePing(std::move(json));
      } else {
        throw;
      }
    } catch (...) {
      std::cerr << "Cannot handle message : " << msg->get_payload() << std::endl;
    }
  }

  void ws::on_open([[maybe_unused]] client* c, [[maybe_unused]] websocketpp::connection_hdl const &hdl) {
    std::cout << "[LIBWSHOOK] Sensor connected" << std::endl;
  }

  void ws::on_close(websocketpp::connection_hdl const &hdl) {
    auto const con = _c.get_con_from_hdl(hdl);

    std::cerr << "[LIBWSHOOK] Connection closed ("
      << websocketpp::close::status::get_string(con->get_remote_close_code())
      << ") : "
      << con->get_remote_close_reason()
      << std::endl;
    reconnect();
  }

  void ws::on_fail(websocketpp::connection_hdl const &hdl) {
    auto const con = _c.get_con_from_hdl(hdl);

    std::cerr << "[LIBWSHOOK] Connection failed : "
      << con->get_ec().message()
      << std::endl;
    reconnect();
  }

  void ws::handleInfo(std::string const &message) {
    std::cout << "[INFO] " + message << std::endl;
  }

  void ws::handlePull(Json::Value &&json) {
    std::cout << "[LIBWSHOOK] Handle PULL" << std::endl;
    try {
      reinterpret_cast<void(*)(Json::Value const &)>(_hooks["pull"])(std::move(json));
    } catch (...) {
      std::cerr << "[LIBWSHOOK] Cannot handle PULL" << std::endl;
    }
  }

  void ws::handlePing(Json::Value &&json) {
    std::cout << "[LIBWSHOOK] Handle PING" << std::endl;
    try {
      pong(json["instance_id"].asString());
    } catch (...) {
      std::cerr << "[LIBWSHOOK] Cannot handle PING" << std::endl;
    }
  }

  void ws::pong(std::string &&instanceId) {
    Json::Value message(Json::nullValue);

    message["kind"] = "PONG";
    message["instance_id"] = std::forward<std::string &&>(instanceId);
    _c.send(_hdl, toString(message), websocketpp::frame::opcode::text);
  }

} // namespace libwshook
