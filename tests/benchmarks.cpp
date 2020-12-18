#include <benchmark/benchmark.h>

#include <orni/orni.hpp>

static void BM_FormParsing(benchmark::State& state) {
    for (auto _ : state) {
        orni::parseForm("username=joe&password=123&age=34");
    }
}
static void BM_CookieParsing(benchmark::State& state) {
    for (auto _ : state) {
        orni::parseCookies("username=joe; password=123; age=34");
    }
}
static void BM_PathParsing(benchmark::State& state) {
    for (auto _ : state) {
        orni::router::getParamsFromUrl("/path/:id/:path/:antoher/:path2/:name",
                                       "/path/5/home/yeet/about/joe");
    }
}

BENCHMARK(BM_FormParsing)->Range(1 << 10, 1 << 30);
BENCHMARK(BM_CookieParsing)->Range(1 << 10, 1 << 30);
BENCHMARK(BM_PathParsing)->Range(1 << 10, 1 << 30);

BENCHMARK_MAIN();
