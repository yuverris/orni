#include <gtest/gtest.h>

#include <orni/orni.hpp>

orni::HttpServer app;

TEST(RouteTest, ParamRoute) {
    orni::router::param_t Params;
    Params = orni::router::getParamsFromUrl("/path/:id", "/path/5");
    EXPECT_EQ("5", Params["id"]);
}
TEST(ParseTest, CookieParse) {
    auto Cookies = orni::parseCookies("username=dammi; id=0");
    EXPECT_EQ("0", Cookies["id"]);
    EXPECT_EQ("dammi", Cookies["username"]);
}
TEST(ParseTest, FromParse) {
    auto Form = orni::parseForm("unit=kg&value=1");
    EXPECT_EQ("kg", Form["unit"]);
    EXPECT_EQ("1", Form["value"]);
}

TEST(ParseTest, CheckTemplatedUrl) {
    EXPECT_EQ(true, orni::router::isTemplatedUrl("/path/5", "/path/:id"));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
