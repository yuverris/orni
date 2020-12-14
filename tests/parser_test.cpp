#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/string_body.hpp>
#include <string>
#include <iostream>

int main(){
    std::string req = "GET /home HTTP/1.1\r\nContent-Length: 4\r\n\r\nbruh";
    boost::beast::error_code c;
    boost::beast::http::request_parser<boost::beast::http::string_body> p;
    p.put(boost::asio::buffer(req), c);
    std::cout << p.get().body() << "\n" << p.get().method_string() << p.get().target();
}
