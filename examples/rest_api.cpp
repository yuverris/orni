#include "../src/orni_http_server.hpp"
#include <nlohmann/json.hpp> //  or use any json library

int main(){
    orni::HttpServer app;
    nlohmann::json jsonRes = {
        {"posts", nlohmann::json::array()}
    };
    app.route("/posts", [&](orni::Request&& req, orni::Response&& res){
            res.set("Content-Type", "application/json");
            res.setStatus(200);
            res.send(jsonRes.dump(4));
            res.dump();
    });
    app.route("/new", [&](orni::Request&& req, orni::Response&& res){
            if (req.Method == "POST") {
                auto data = nlohmann::json::parse(req.Body);
                jsonRes["posts"].push_back(data);
                res.setStatus(201);
                res.send("created");
                res.dump();
            } else if (req.Method == "GET") {
                nlohmann::json example;
                example["id"] = nullptr;
                example["author"] = nullptr;
                example["title"] = nullptr;
                res.set("Content-Type", "application/json");
                res.send(example.dump(4));
                res.dump();
            }
    });
    app.run(5000);
}
