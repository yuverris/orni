#ifndef ORNI_ROUTER_HPP
#define ORNI_ROUTER_HPP
#include <regex>

#include "httpparser/httprequestparser.h"
#include "orni_parsers.hpp"
#include "orni_wrapper.hpp"
#include "util/orni_logger.hpp"

// TODO: use regex instead of spliting and striping

namespace orni {
orni::Logger logger;
struct Request {
    std::map<std::string, std::string> Headers;
    std::map<std::string, std::string> Queries;
    std::map<std::string, std::string> Form;
    std::map<std::string, std::string> Cookies;
    std::vector<std::string> Params;
    const std::string ContentType;
    const std::string Body;
    const std::string Method;
    const std::string Url;
};

class Response {
    std::map<std::string, std::string> m_Headers;
    int m_Status = 200;
    std::string m_Body;

   public:
    void set(const std::string& ke, const std::string& val) {
        m_Headers.insert({ke, val});
    }
    void addCookie(const std::string& name, const std::string& val) {
        std::stringstream ss;
        ss << name << '=' << val;
        m_Headers.insert(
            {"Set-Cookie", ss.str()});  // insert instead of [] to avoid
                                        // overwriting other cookies
    }
    void setStatus(int s) { m_Status = s; }
    void send(const std::string& cn) { m_Body += cn; }
    void Redirect(const std::string& neUrl) {
        setStatus(301);
        set("Location", neUrl);
    }
    int getStatus() const { return m_Status; }
    auto getHeaders() const { return m_Headers; }
    auto getBody() const { return m_Body; }
};

std::string parseResponse(const Response& res) {
    std::stringstream ss;
    ss << "HTTP/1.1 " << res.getStatus() << "\r\n";
    if (res.getHeaders().size() > 0) {
        for (auto& [key, value] : res.getHeaders()) {
            ss << key << ":"
               << " " << value << "\r\n";
        }
    }
    ss << "\n" << res.getBody();
    return ss.str();
}
namespace router {
typedef std::map<std::string, std::string>
    query_t;  // queries such as "?id=5&name=joe"
typedef std::map<std::string, std::string>
    param_t;  // params such as "/path/:id/:name"
typedef std::function<void(orni::Request&, orni::Response&)> route_callback;
typedef std::map<std::string, route_callback> route_t;
Request ParserToRequest(const httpparser::Request& req) {
    size_t pos = req.uri.find('?');
    std::string _body(req.content.begin(), req.content.end());
    std::map<std::string, std::string> StHeaders;
    std::map<std::string, std::string> StQueries =
        orni::parsers::parseForm(req.uri.substr(
            pos + 1,
            req.uri.size()));  // since queries are kinda same as forms style
    for (auto& i : req.headers) {
        StHeaders[i.name] = i.value;
    }
    Request retReq{.Headers = StHeaders,
                   .Queries = StQueries,
                   .Cookies = orni::parsers::parseCookies(StHeaders["Cookie"]),
                   .ContentType = StHeaders["Content-Type"],
                   .Body = _body,
                   .Method = req.method,
                   .Url = req.uri};
    if (retReq.ContentType ==
        "application/x-www-form-urlencoded") {  //  checking if a post performed
                                                //  from a form
        retReq.Form = orni::parsers::parseForm(retReq.Body);
    }
    return retReq;
}

// function for initializing responses after callbacks like Content-Length,
// Date, etc
void afterCallbackInit(Response& res) {
    res.set("Content-Length", std::to_string(res.getBody().size()));
}
struct RouteObject {
    const std::string _str_url;
    const route_callback rcb;
};
class Router : public orni::SocketPP {
    std::vector<RouteObject> m_Routes;
    route_callback NotFoundPage = [&](Request& req, Response& res) {
        res.setStatus(404);
        res.set("Content-Type", "text/html");
        res.send("<h1>404<h1>\nNot Found");
    };
    route_callback ServerErrorPage = [&](Request& req, Response& res) {
        res.setStatus(500);
        res.set("Content-Type", "text/html");
        res.send("<h1>500<h1>\nServer Error");
    };

   public:
    auto getRoutes() { return m_Routes; }
    void route(const std::string& path, const route_callback& cb) {
        bool exist;
        std::for_each(m_Routes.begin(), m_Routes.end(), [&](auto& _route_obj) {
            if (_route_obj._str_url == path) {
                exist = true;
            } else {
                exist = false;
            }
        });
        if (exist) {
            std::stringstream ss;
            ss << path << " already registered";
            throw orni::Exception(ss.str());
        } else {
            m_Routes.push_back(RouteObject{._str_url = path, .rcb = cb});
        }
    }
    void setNotFoundPage(const route_callback& cb) { NotFoundPage = cb; }
    void setServerErrorPage(const route_callback& cb) { ServerErrorPage = cb; }
    void parseRoutes(char req_url[]) {
        httpparser::HttpRequestParser parser;
        httpparser::Request preq;
        parser.parse(preq, req_url, req_url + strlen(req_url));
        Request req = ParserToRequest(preq);
        size_t Qpos = preq.uri.find('?');
        std::string purl = preq.uri.substr(0, Qpos);
        Response res;
        size_t r_search_index = 0;
        try {
            for (auto& route : m_Routes) {
                r_search_index++;
                std::regex exp(
                    route
                        ._str_url);  // parsing each route to a regex expression
                                     // and then check if it matches the
                                     // requested uri and execute it's callback
                auto _matched_url = std::regex_match(purl, exp);
                logger.debug("searching for matches");
                logger.debug("url: " + purl + " route: " + route._str_url +
                             '\n');
                if (_matched_url) {
                    logger.debug(route._str_url + " matched " + purl);
                    std::smatch matches;
                    std::regex_search(purl, matches, exp);
                    std::vector<std::string> _params;
                    for (size_t i = 0; i <= matches.size() - 1;
                         ++i) {  // yeet O(n²)
                        _params.push_back(matches[i].str());
                        logger.debug("regex iter: " + matches[i].str());
                    }
                    req.Params = _params;
                    route.rcb(std::ref(req), std::ref(res));
                    break;
                } else if (!(_matched_url) &&
                           r_search_index == m_Routes.size()) {
                    NotFoundPage(std::ref(req), std::ref(res));
                } else if (_matched_url && purl == route._str_url) {
                    route.rcb(std::ref(req), std::ref(res));
                }
            }
        } catch (...) {
            ServerErrorPage(std::ref(req), std::ref(res));
        }
        afterCallbackInit(std::ref(res));
        std::string resString = parseResponse(res);
        write(GetConn(), resString.c_str(), resString.size());
        std::stringstream log;
        log << req.Method << ' ' << purl << ' ' << res.getStatus();
        logger.info(log.str());
        logger.debug(req_url);
    }
};  // namespace router
}  // namespace router
}  // namespace orni
#endif  // ORNI_ROUTER_HPP
