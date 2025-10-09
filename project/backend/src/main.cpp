#include <iostream>
#include <memory>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>

#include "core/Network.hpp"
#include "core/Engine.hpp"
#include "core/Host.hpp"
#include "utils/JsonAdapter.hpp"

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

int main() {
    Network net;
    Engine engine(net);

    http_listener listener(U("http://0.0.0.0:8080"));

    // GET /status
    listener.support(methods::GET, [&](http_request request) {
        web::json::value resp;
        resp[U("status")] = web::json::value::string(U("ok"));
        request.reply(status_codes::OK, resp);
    });

    // POST handler
    listener.support(methods::POST, [&](http_request request) {
        auto path = request.relative_uri().path();

        if (path == U("/addNode")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    auto name = utility::conversions::to_utf8string(jv[U("name")].as_string());
                    auto ip = jv.has_field(U("ip"))
                        ? utility::conversions::to_utf8string(jv[U("ip")].as_string())
                        : std::string("127.0.0.1");

                    auto node = net.addNode<Host>(ip, 8080); // Use Host as example
                    // Note: Host constructor is (address, port), so adjust if needed

                    web::json::value resp;
                    resp[U("result")] = web::json::value::string(U("node added"));
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(
                        utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        } else if (path == U("/ping")) {
            request.extract_json().then([&](web::json::value jv) {
                try {
                    std::string src = utility::conversions::to_utf8string(jv[U("src")].as_string());
                    std::string dst = utility::conversions::to_utf8string(jv[U("dst")].as_string());
                    std::vector<std::string> pathOut;
                    bool ok = engine.ping(src, dst, pathOut);

                    web::json::value resp;
                    resp[U("success")] = web::json::value::boolean(ok);
                    web::json::value arr = web::json::value::array();
                    for (size_t i = 0; i < pathOut.size(); ++i)
                        arr[i] = web::json::value::string(
                            utility::conversions::to_string_t(pathOut[i]));

                    resp[U("path")] = arr;
                    request.reply(status_codes::OK, resp);

                } catch (const std::exception& e) {
                    web::json::value resp;
                    resp[U("error")] = web::json::value::string(
                        utility::conversions::to_string_t(e.what()));
                    request.reply(status_codes::BadRequest, resp);
                }
            }).wait();

        } else {
            web::json::value resp;
            resp[U("error")] = web::json::value::string(U("unknown endpoint"));
            request.reply(status_codes::NotFound, resp);
        }
    });

    try {
        listener.open().wait();
        std::cout << "Server running at http://0.0.0.0:8080" << std::endl;
        std::cin.get();
        listener.close().wait();
    } catch (const std::exception& e) {
        std::cerr << "Listener error: " << e.what() << std::endl;
    }

    return 0;
}
