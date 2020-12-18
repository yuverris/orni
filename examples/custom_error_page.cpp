#include <orni/orni_http_server.hpp>

int main() {
    orni::HttpServer app;
    app.setNotFoundPage([](orni::Request&& req, orni::Response&& res) {
            res.send("my custom 404 page");
            res.dump();
    });  //  for handling 404 errors
    app.setServerErrorPage([](orni::Request&& req, orni::Response&& res) {
            res.send("my custom 500 page");
            res.dump();
    });  //  for handling 500 server errors
    app.run(1234); //  or any port default to 5000
}
