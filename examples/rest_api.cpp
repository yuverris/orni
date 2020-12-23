#include <orni/orni.hpp>  // nlohmann/json is already installed

int main() {
    orni::HttpServer app;
    nlohmann::json jsonRes = {{"posts", nlohmann::json::array()}};
    app.Get("/posts", [&](orni::Request& req, orni::Response& res) {
        res.SendJson(jsonRes);
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
        res.SendJson(example);
    });
    app.run(1234);
}
