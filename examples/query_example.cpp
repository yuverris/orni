#include "../src/orni_http_server.hpp"

int main() {
    orni::HttpServer app;
    app.route("/", [](orni::Request&& req, orni::Response&& res) {
            std::stringstream ss;
            ss << "You requested " << req.Queries["eggs"] << " egg!";
            res.send(ss.str());
            res.dump();
    });
}
