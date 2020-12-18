#ifndef ORNI_ROUTER_HPP
#define ORNI_ROUTER_HPP
#include <algorithm>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "orni_wrapper.hpp"

namespace orni {
struct Request {
    std::map<std::string, std::string> Headers;
    std::map<std::string, std::string> Queries;
    std::map<std::string, std::string> Params;
    std::map<std::string, std::string> Form;
    std::map<std::string, std::string> Cookies;
    const std::string ContentType;
    const std::string Body;
    const std::string Method;
    const std::string Url;
};

class Response {
    std::map<std::string, std::string> m_Headers;
    int m_Status = 200;
    std::stringstream m_Body;
    int m_Conn;

   public:
    explicit Response(int cn) : m_Conn(cn) {}
    void set(const std::string& ke, const std::string& val) {
        m_Headers.insert({ke, val});
    }
    void dump() const {
        std::stringstream ss;
        ss << "HTTP/1.1 " << m_Status << "\r\n";
        if (m_Headers.size() > 0) {
            for (auto& [key, value] : m_Headers) {
                ss << key << ":"
                   << " " << value << "\r\n";
            }
        }
        ss << "\n" << m_Body.str();
        auto str = ss.str();
        write(m_Conn, str.c_str(), str.size());
    }
    void addCookie(const std::string& name, const std::string& val) {
        std::stringstream ss;
        ss << name << '=' << val;
        m_Headers.insert(
            {"Set-Cookie", ss.str()});  // insert instead of [] so last cookie
                                        // we won't overwrite other cookies
    }
    void setStatus(int s) { m_Status = s; }
    void send(const std::string& cn) { m_Body << cn; }
    void Redirect(const std::string& neUrl) {
        setStatus(301);
        set("Location", neUrl);
    }
};

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
namespace router {
typedef std::map<std::string, std::string>
    query_t;  // queries such as "?id=5&name=joe"
typedef std::map<std::string, std::string>
    param_t;  // params such as "/path/:id/:name"
typedef std::function<void(orni::Request, orni::Response)> route_callback;
typedef std::map<std::string, route_callback> route_t;
Request ParserToRequest(const httpparser::Request& req) {
    httpparser::Params params;
    httpparser::parseQueryString(params, req.uri);
    std::string _body(req.content.begin(), req.content.end());
    std::map<std::string, std::string> StHeaders;
    std::map<std::string, std::string> StQueries;
    for (auto& i : req.headers) {
        StHeaders[i.name] = i.value;
    }
    for (auto& [key, val] : params) {
        StQueries[key] = val;
    }
    Request retReq{.Headers = StHeaders,
                   .Queries = StQueries,
                   .Cookies = parseCookies(StHeaders["Cookie"]),
                   .ContentType = StHeaders["Content-Type"],
                   .Body = _body,
                   .Method = req.method,
                   .Url = req.uri};
    if (retReq.ContentType == "application/x-www-form-urlencoded") {
        retReq.Form = parseForm(retReq.Body);
    }
    return retReq;
}

//  function for parsing paths such "/path/:id/?name=joe"
//  exurl is the template url the match with ex: "/path/:id/"
//  url is the requested url check if it matches with the templated url ex:
//  "/path/5/" should out put: params["id"] -> 5
param_t getParamsFromUrl(const std::string& exurl, const std::string& url) {
    param_t params;
    std::vector<std::string> splited_exurl;
    std::vector<std::string> splited_url;
    split(exurl, '/', splited_exurl);
    split(url, '/', splited_url);
    size_t index = 0;
    for (auto& p : splited_exurl) {
        size_t pos = p.find(":");
        ++index;
        if (pos != std::string::npos) {
            std::string newStr =
                p.substr(pos,
                         p.size());  // get the string between the ':' to the
                                     // end of the path
            std::string param = newStr.substr(
                1, p.size());  // extract the parameter wuthout the ':'
            params[param] = splited_url[index - 1];
        }
    }
    return params;
}
//  check if the given url matches the given templated url
bool isTemplatedUrl(const std::string& exurl, const std::string& url) {
    size_t expos = url.find(":");
    return exurl.substr(0, expos) == url.substr(0, expos);
}

//  find a registered route that matches a certain template then get it's
//  callback
std::tuple<route_callback, std::string> getTemplateCallback(
    const std::string& templateurl, const route_t& cbs) {
    for (auto& [route, cb] : cbs) {
        if (isTemplatedUrl(templateurl, route)) {
            return {cb, route};
        }
    }
    return {nullptr, std::string{}};
}

struct RouteObject {
    param_t Params;
    query_t Queries;
    route_callback rcb;
};
class Router : public orni::SocketPP {
    route_t m_Routes;
    route_callback NotFoundPage = [&](Request&& req, Response&& res) {
        res.setStatus(404);
        res.set("Content-Type", "text/html");
        res.send("<h1>404<h1>\nNot Found");
        res.dump();
    };
    route_callback ServerErrorPage = [&](Request&& req, Response&& res) {
        res.setStatus(500);
        res.set("Content-Type", "text/html");
        res.send("<h1>500<h1>\nServer Error");
        res.dump();
    };

   public:
    route_t getRoutes() { return m_Routes; }
    void route(const std::string& path, const route_callback& cb) {
        auto findRoute = m_Routes.find(path);
        if (findRoute != m_Routes.end()) {
            std::stringstream ss;
            ss << path << " already registered";
            throw orni::Exception(ss.str());
        } else {
            m_Routes[path] = cb;
        }
    }
    void setNotFoundPage(const route_callback& cb) { NotFoundPage = cb; }
    void setServerErrorPage(const route_callback& cb) { ServerErrorPage = cb; }
    //  get a valid route from m_Routes that matches the requested route
    //  either a templated Url or a basic url then call it's callback
    void parseRoutes(char req_url[]) {
        httpparser::HttpRequestParser parser;
        httpparser::Request preq;
        parser.parse(preq, req_url, req_url + strlen(req_url));
        Request req = ParserToRequest(preq);
        size_t Qpos = preq.uri.find('?');
        std::string purl = preq.uri.substr(0, Qpos);
        Response res(GetConn());
        auto [rcb, templ] = getTemplateCallback(purl, m_Routes);
        //  making sure that the callback is registered & the template length
        //  matches the requested url length so we won't get blank params
        std::vector<std::string> tmpUrl, tmpTemUrl;
        split(purl, '/',
              tmpUrl);  // split the requested url to vector by '/'
        split(templ, '/', tmpTemUrl);  // split the matched template by '/'
        //  checks if the callback actually exists and
        //  compare the size of the splited url with the size
        //  of the splited tamplate then check
        //  if the reuqested route is not already registered
        //  in m_Routes ex:
        //  '
        //  consider that we have "/path/main" and "/path/:id" both
        //  are registered:
        //  req_url -> "/path/main" -> get valid route
        //  -> returns both checks [/path/:id, /path/main]
        //  in this case the "/path/main" callback will
        //  be invoked instead of "/path/:id" callback
        //  '
        //  can't think of any better way to do it
        //  (ツ)
        auto RawRoute = m_Routes.find(purl);
        if (rcb != nullptr && !templ.empty() &&
            tmpTemUrl.size() == tmpUrl.size() && RawRoute == m_Routes.end()) {
            param_t params = getParamsFromUrl(templ, purl);
            req.Params = params;
            rcb(std::move(req), std::move(res));
        } else if (RawRoute != m_Routes.end()) {
            RawRoute->second(std::move(req), std::move(res));
        } else {
            NotFoundPage(std::move(req), std::move(res));
        }
    }
};
}  // namespace router
}  // namespace orni
#endif  // ORNI_ROUTER_HPP
