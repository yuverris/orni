# Orni
http server for modern C++ inspired from expressjs
# Dependencies
- [!httpparser](https://github.com/nekipelov/httpparser)

# Example


```cpp
#include "orni_http_server.hpp"

int main(){
    orni::HttpServer app;
    app.route("/", [&](orni::Request&& req, orni::Response&& res){
        res.set("Content-Type", "text/html");
        res.send("<h1>Hello from orni !</h1>");
        res.dump();
    });
    app.run(1234);
}
```
