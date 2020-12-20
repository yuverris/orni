#ifndef ORNI_PARSERS_HPP
#define ORNI_PARSERS_HPP
#include <algorithm>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>
namespace orni {
namespace parsers {
void split(std::string str, const char delim, std::vector<std::string>& out) {
    std::stringstream ss(std::string{str});

    std::string s;
    while (std::getline(ss, s, delim)) {
        out.push_back(std::string{s});
    }
}
std::map<std::string, std::string> parseCookies(const std::string& str) {
    std::ostringstream oss;
    std::vector<std::string> cookies;
    std::map<std::string, std::string> out;
    std::remove_copy(str.begin(), str.end(), std::ostream_iterator<char>(oss),
                     ';');  // remove the ';' cause each cookie ends with it
    std::istringstream ss(oss.str());
    std::copy(std::istream_iterator<std::string>(ss),
              std::istream_iterator<std::string>(),
              std::back_inserter(cookies));  // split by spaces

    for (auto& cookie : cookies) {
        size_t epos = cookie.find('=');
        std::string key = cookie.substr(0, epos),
                    value = cookie.substr(
                        epos + 1,
                        cookie
                            .size());  // find '=' in the raw cookie then split
                                       // it to key, value and add it to the map
        out[key] = value;
    }
    return out;
}

//  function for parsing forms names `username=foo&password=123456`
std::map<std::string, std::string> parseForm(const std::string& str) {
    std::vector<std::string> splited;
    std::map<std::string, std::string> out;
    split(str, '&', splited);
    for (auto& form : splited) {
        size_t epos = form.find('=');
        std::string key = form.substr(0, epos),
                    value = form.substr(epos + 1, form.size());

        out[key] = value;
    }
    return out;
}
}  // namespace parsers
}  // namespace orni
#endif  // ORNI_PARSERS_HPP
