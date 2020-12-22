#include <orni/orni.hpp>

int main() {
    orni::HttpServer app;
    app.Get("/", [](orni::Request& req, orni::Response& res) {
        res.Send("Hello !");
    });
    app.setNotFoundPage([](orni::Request& req, orni::Response& res) {
        res.SetStatus(404);
        res.Send("my custom 404 page");
    });  //  for handling 404 errors
    app.setServerErrorPage([](orni::Request& req, orni::Response& res) {
        res.SetStatus(500);
        res.Send("my custom 500 page");
    });             //  for handling 500 server errors
    app.run(1234);  //  or any port default to 5000
}
