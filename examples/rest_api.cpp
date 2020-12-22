#include <nlohmann/json.hpp>  //  or use any json library
#include <orni/orni.hpp>

int main() {
    orni::HttpServer app;
    nlohmann::json jsonRes = {{"posts", nlohmann::json::array()}};
    app.Get("/posts", [&](orni::Request& req, orni::Response& res) {
        res.Set("Content-Type", "application/json");
        res.SetStatus(200);
        res.Send(jsonRes.dump(4));
    });
    app.Post("/new", [&](orni::Request& req, orni::Response& res) {
        auto data = nlohmann::json::parse(req.Body);
        jsonRes["posts"].push_back(data);
        res.SetStatus(201);
        res.Send("created");
    });
    app.Get("/new", [&](orni::Request& req, orni::Response& res) {
        nlohmann::json example;
        example["id"] = nullptr;
        example["author"] = nullptr;
        example["title"] = nullptr;
        res.Set("Content-Type", "application/json");
        res.Send(example.dump(4));
    });
    app.run(1234);
}
