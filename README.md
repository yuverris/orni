# Orni

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/2a7358ca0dcc4022a085bc5037cfcea7)](https://app.codacy.com/gh/Dammibruh/orni?utm_source=github.com&utm_medium=referral&utm_content=Dammibruh/orni&utm_campaign=Badge_Grade)
![version](https://img.shields.io/badge/version-v0.1--alpha-red) [![Total alerts](https://img.shields.io/lgtm/alerts/g/Dammibruh/orni.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/Dammibruh/orni/alerts/) 
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/Dammibruh/orni.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/Dammibruh/orni/context:cpp)

http server for modern C++ inspired from expressjs
# Dependencies
-  [httpparser](https://github.com/nekipelov/httpparser) (pre-installed with source code)

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

#  Docs
soon ™

#  TODO
-  [ ] params
-  [x] queries
-  [ ] built-in ORM
-  [ ] html template rendering
