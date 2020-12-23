#ifndef ORNI_ROUTER_HPP
#define ORNI_ROUTER_HPP
#include <regex>
#include <set>

#include "httpparser/httprequestparser.h"
#include "inja.hpp"
#include "orni_parsers.hpp"
#include "orni_wrapper.hpp"
#include "util/orni_logger.hpp"

// TODO: use regex instead of spliting and striping

namespace orni {
using json = nlohmann::json;
enum class HttpMethods { Get, Post, Delete, Put, Patch, Head, Any };
struct FileObject {
    std::string filename;
    std::string content;
};
std::string httpMethodToStr(HttpMethods meth) {
    switch (meth) {
        case HttpMethods::Get:
            return "GET";
        case HttpMethods::Post:
            return "POST";
        case HttpMethods::Delete:
            return "DELETE";
        case HttpMethods::Put:
            return "PUT";
        case HttpMethods::Patch:
            return "PATCH";
        case HttpMethods::Head:
            return "HEAD";
        default:
            return {};
    }
}
HttpMethods strMethodToHttpMethod(const std::string& s) {
    if (s == "GET") {
        return HttpMethods::Get;
    } else if (s == "POST") {
        return HttpMethods::Post;
    } else if (s == "PUT") {
        return HttpMethods::Put;
    } else if (s == "DELETE") {
        return HttpMethods::Delete;
    } else if (s == "PATCH") {
        return HttpMethods::Patch;
    } else if (s == "HEAD") {
        return HttpMethods::Head;
    }
    return {};
}
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
    std::vector<std::pair<std::string, std::string>> m_Headers;
    int m_Status = 200;
    std::string m_Body;

   public:
    void Set(const std::string& ke, const std::string& val) {
        m_Headers.push_back({ke, val});
    }
    void addCookie(const std::string& name, const std::string& val) {
        std::stringstream ss;
        ss << name << '=' << val;
        m_Headers.push_back({"Set-Cookie", ss.str()});
    }
    void SetStatus(int s) { m_Status = s; }
    void Send(const std::string& text) {
        Set("Content-Type", "text/plain");
        m_Body += text;
    }
    void SendJson(const json& jsonObj) {
        Set("Content-Type", "application/json");
        m_Body += jsonObj.dump(4);
    }
    void SendHtml(const std::string& html) {
        Set("Content-Type", "text/html");
        m_Body += html;
    }
    void Redirect(const std::string& targetUrl) {
        SetStatus(301);
        Set("Location", targetUrl);
    }
    void RenderTemplate(const std::string& html, const json& jsonObj) {
        std::string out = inja::render(html, jsonObj);
        SendHtml(std::move(out));
    }
    int getStatus() const { return m_Status; }
    auto getHeaders() const { return m_Headers; }
    auto getBody() const { return m_Body; }
};

std::string parseResponse(const Response& res) {
    std::stringstream ss;
    ss << "HTTP/1.1 " << res.getStatus() << "\r\n";
    if (res.getHeaders().size() > 0) {
        for (auto& elm : res.getHeaders()) {
            ss << elm.first << ":"
               << " " << elm.second << "\r\n";
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
        orni::parsers::regularParser(req.uri.substr(
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
        retReq.Form = orni::parsers::regularParser(retReq.Body);
    }
    return retReq;
}

// function for initializing responses after callbacks like Content-Length,
// Date, etc
constexpr void afterCallbackInit(Response& res) {
    res.Set("Content-Length", std::to_string(res.getBody().size()));
}
struct RouteObject {
    const std::string _str_url;
    const route_callback rcb;
    HttpMethods allowed_method;
};
class Router : public orni::SocketPP {
    std::vector<RouteObject> m_Routes;
    route_callback NotFoundPage = [&](Request& req, Response& res) {
        res.SetStatus(404);
        res.SendHtml("<h1>404<h1>\nNot Found");
    };
    route_callback ServerErrorPage = [&](Request& req, Response& res) {
        res.SetStatus(500);
        res.SendHtml("<h1>500<h1>\nServer Error");
    };

   public:
    auto getRoutes() { return m_Routes; }
    void route(const std::string& path, const route_callback& cb,
               HttpMethods allowed_method) {
        for (auto& r_route : m_Routes) {
            if (r_route._str_url == path &&
                r_route.allowed_method == allowed_method) {
                throw orni::Exception("route already exist");
            }
        }
        m_Routes.push_back(RouteObject{
            ._str_url = path, .rcb = cb, .allowed_method = allowed_method});
    }
    void Get(const std::string& path, const route_callback& rcb) {
        route(path, rcb, HttpMethods::Get);
    }
    void Get(std::string&& path, const route_callback&& rcb) {
        route(std::move(path), std::move(rcb), HttpMethods::Get);
    }
    void Post(const std::string& path, const route_callback& rcb) {
        route(path, rcb, HttpMethods::Post);
    }
    void Post(std::string&& path, const route_callback&& rcb) {
        route(std::move(path), std::move(rcb), HttpMethods::Post);
    }
    void Put(const std::string& path, const route_callback& rcb) {
        route(path, rcb, HttpMethods::Put);
    }
    void Put(std::string&& path, const route_callback&& rcb) {
        route(std::move(path), std::move(rcb), HttpMethods::Put);
    }
    void Patch(const std::string& path, const route_callback& rcb) {
        route(path, rcb, HttpMethods::Patch);
    }
    void Patch(std::string&& path, const route_callback&& rcb) {
        route(std::move(path), std::move(rcb), HttpMethods::Patch);
    }
    void Delete(const std::string& path, const route_callback& rcb) {
        route(path, rcb, HttpMethods::Delete);
    }
    void Delete(std::string&& path, const route_callback&& rcb) {
        route(std::move(path), std::move(rcb), HttpMethods::Delete);
    }
    void Head(const std::string& path, const route_callback& rcb) {
        route(path, rcb, HttpMethods::Head);
    }
    void Head(std::string&& path, const route_callback&& rcb) {
        route(std::move(path), std::move(rcb), HttpMethods::Head);
    }
    void SetNotFoundPage(const route_callback& cb) { NotFoundPage = cb; }
    void SetServerErrorPage(const route_callback& cb) { ServerErrorPage = cb; }
    void parseRoutes(char req_url[]) {
        httpparser::HttpRequestParser parser;
        httpparser::Request preq;
        parser.parse(preq, req_url, req_url + strlen(req_url));
        Request req = ParserToRequest(preq);
        size_t Qpos = preq.uri.find('?');
        std::string purl = preq.uri.substr(0, Qpos);
        Response res;
        try {
            int r_search_index = 0;
            for (auto& route : m_Routes) {
                r_search_index++;
                std::regex exp(
                    route._str_url);  // parsing each route to a regex
                                      // expression then check if it // matches
                                      // the requested uri and execute it's
                                      // callback
                auto _matched_url = std::regex_match(purl, exp);
                HttpMethods _PathMethod = strMethodToHttpMethod(req.Method);
                logger.debug("searching for matches");
                logger.debug("url: " + purl + " route: " + route._str_url +
                             '\n');
                if (_matched_url && route.allowed_method == _PathMethod) {
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
                } else if ((!_matched_url) &&
                           r_search_index == m_Routes.size()) {
                    NotFoundPage(std::ref(req), std::ref(res));
                } else if (_matched_url && purl == route._str_url &&
                           route.allowed_method == _PathMethod) {
                    route.rcb(std::ref(req), std::ref(res));
                }
            }
        } catch (const std::exception& ex) {
            logger.debug(ex.what());
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
