#include "urlparser.h"
#include <iostream>

int main() {
    httpparser::UrlParser pars;
    const char url[] = "https://example.com/lmao/bruh?id=5";
    pars.parse(url);
    std::cout << pars.path() << " " << pars.query() << '\n';
}
