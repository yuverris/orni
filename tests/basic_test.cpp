#include <benchmark/benchmark.h>
#include "../src/orni_http_server.hpp"

static void BM_BasicHttpApp(benchmark::State& state) {
    for (auto _ : state) {
        orni::HttpServer app;
        app.route("/", [](orni::Request&& req, orni::Response&& res) {
                res.send("Hello, World!");
                res.dump();
        });
        app.run();
        app.CloseSocket();
    }
}
BENCHMARK(BM_BasicHttpApp);


BENCHMARK_MAIN();
