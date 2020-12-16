#include "../src/orni_http_server.hpp"

int main() {
    orni::HttpServer app;
    app.route("/", [](orni::Request&& req, orni::Response&& res) {
            std::stringstream ss;
            auto eggs = req.Queries["eggs"];
            if (eggs.empty()) {
                ss << "Empty order :(";
            } else if (!eggs.empty()) {
                ss << "You ordered " << eggs << " total egg!";
            }
            res.send(ss.str());
            res.dump();
   });
    app.run();
}
