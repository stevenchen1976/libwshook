#ifndef WS_HPP
#define WS_HPP
#pragma once

#include "config.hpp"
#include "serialize.hpp"
#include "utils.hpp"
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <json/json.h>

namespace libwshook {

  class ws : public serialize {

    using client = websocketpp::client<websocketpp::config::asio_client>;
    using message_ptr = websocketpp::config::asio_client::message_type::ptr;

    config const _config;
    client _c;
    websocketpp::connection_hdl _hdl;

    __attribute__ ((visibility ("default"))) void initWebsocketClient();

    void connect();
    void reconnect();

    void handleInfo(std::string const &message);
    void handlePull(Json::Value &&json);
    void handlePing(Json::Value &&json);

    void pong(std::string &&instanceId);

    void on_open(client* c, websocketpp::connection_hdl const &hdl);
    void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr const &msg);
    void on_close(websocketpp::connection_hdl const &hdl);
    void on_fail(websocketpp::connection_hdl const &hdl);

    protected:
      utils::hooks_t _hooks;

    public:
      template<typename... Args>
        explicit ws(Args&&... args) : _config(std::forward<Args>(args)...) {
          try {
            initWebsocketClient();
          } catch (...) {
            std::cerr << "Cannot construct ws" << std::endl;
            throw;
          }
        }

      template <typename A, typename B>
      __attribute__ ((visibility ("default")))
        void push(A &&instanceId, B &&metrics) {
          Json::Value message(Json::nullValue);

          message["kind"] = "PUSH";
          message["instance_id"] = std::forward<A &&>(instanceId);
          message["metrics"] = std::forward<B &&>(metrics);
          _c.send(_hdl, toString(message), websocketpp::frame::opcode::text);
        }


      template <typename A, typename B, typename C = Json::Value>
      __attribute__ ((visibility ("default")))
        void abort(A &&instanceId, B &&reason, C &&errors = Json::arrayValue) {
          std::cerr << "[LIBWSHOOK] Abort (from " + std::forward<A>(instanceId) + "): " + std::forward<B>(reason) << ". Errors: " << errors << std::endl;
          Json::Value message(Json::nullValue);

          message["kind"] = "ERROR";
          message["instance_id"] = std::forward<A &&>(instanceId);
          message["message"] = std::forward<B &&>(reason);
          message["errors"] = std::forward<C &&>(errors);
          _c.send(_hdl, toString(message), websocketpp::frame::opcode::text);
        }

      __attribute__ ((visibility ("default"))) void run();

  };

} // namespace libwshook

#endif /* !WS_HPP */
