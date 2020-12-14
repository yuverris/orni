# Orni
http &amp; websocket server for modern C++
# Dependencies
- boost http_parser

# Example


```cpp
#include "orni_http_server.hpp"

int main(){
    orni::HttpServer app;
    app.get("/", [&](const orni::Request& req, const orni::Response& res){
        res.set("Content-Type", "text/html");
        res.send("<h1>Hello from orni !</h1>");
    });
    app.run(1234);
}
```
