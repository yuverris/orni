# Orni
http &amp; websocket server for modern C++
# Dependencies
- [!httpparser](https://github.com/nekipelov/httpparser)

# Example


```cpp
#include "orni_http_server.hpp"

int main(){
    orni::HttpServer app;
    app.route("/", [&](const orni::Request& req, const orni::Response& res){
        res.set("Content-Type", "text/html");
        res.send("<h1>Hello from orni !</h1>");
        res.dump();
    });
    app.run(1234);
}
```
