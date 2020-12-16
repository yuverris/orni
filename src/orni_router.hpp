#ifndef ORNI_ROUTER_HPP
#define ORNI_ROUTER_HPP
#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <sstream>
#include "httpparser/urlparser.h"
#include "orni_wrapper.hpp"


namespace orni {
    struct Request {
        std::map<std::string, std::string> Headers;
        std::map<std::string, std::string> Queries;
        std::map<std::string, std::string> Params;
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
            m_Headers[ke] = val;
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
        void setStatus(int s) { m_Status = s; }
        void send(const std::string_view& cn) { m_Body << cn; }
        void Redirect(const std::string& neUrl) {
            setStatus(301);
            set("Location", neUrl);
        }
    };
    namespace router {
        typedef std::map<std::string, std::string> query_t;  // queries such as "?id=5&name=joe"
        typedef std::map<std::string, std::string> param_t;  // params such as "/path/:id/:name"
        typedef std::function<void(orni::Request, orni::Response)> route_callback;
        typedef std::map<std::string, route_callback> route_t;

        void split(std::string const &str, const char delim,
                std::vector<std::string> &out)
        {
            std::stringstream ss(str);

            std::string s;
            while (std::getline(ss, s, delim)) {
                out.push_back(s);
            }
        }
        //  function for parsing paths such "/path/:id/?name=joe"
        //  exurl is the template url the match with ex: "/path/:id/"
        //  url is the requested url check if it matches with the templated url ex: "/path/5/" 
        //  should out put: params["id"] -> 5
        param_t getParamsFromUrl(const std::string& exurl, const std::string& url) {
            param_t params;
            std::vector<std::string> splited_exurl;
            std::vector<std::string> splited_url;
            split(exurl, '/', splited_exurl);
            split(url, '/', splited_url);
            size_t index = 0;
            std::string path;
            for (auto& p: splited_exurl) {
                size_t pos = p.find(":");
                ++index;
                if (pos != std::string::npos) {
                    std::string newStr = p.substr(pos, p.size());  // get the string between the ':' to the end of the path
                    std::string param = newStr.substr(1, p.size()); // extract the parameter wuthout the ':'
                    params[param] = splited_url[index-1];
                }
            }
            return params;
        }
        //  check if the given url matches the given templated url
        bool isTemplatedUrl(const std::string& exurl, const std::string& url) {
            size_t expos = url.find(":");
            return exurl.substr(0, expos) == url.substr(0, expos);
        }

        //  find a registered route that matches a certain template then get it's callback
        std::tuple<route_callback, std::string> getTemplateCallback(const std::string& templateurl, 
                const route_t& cbs) {
            size_t pos = templateurl.find(':');
            //  assert(pos != std::string::npos);
            for(auto& [route, cb]: cbs) {
                if (isTemplatedUrl(templateurl, route)) {
                    return { cb, route };
                }
            }
            return { nullptr, std::string{}};
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
                res.set("content-type", "text/html");
                res.send("<h1>404<h1>\nNot Found");
                res.dump();
            };
            route_callback ServerErrorPage = [&](Request&& req, Response&& res) {
                res.setStatus(500);
                res.set("content-type", "text/html");
                res.send("<h1>500<h1>\nServer Error");
                res.dump();
            };
            ;
            public:
            route_t getRoutes() {
                return m_Routes;
            }
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
            void setNotFoundPage(const route_callback& cb) {
                NotFoundPage = cb;
            }
            void setServerErrorPage(const route_callback& cb) {
                ServerErrorPage = cb;
            }
            //  get a valid route from m_Routes that matches the requested route 
            //  either a templated Url or a basic url
            RouteObject getValidRoute(const std::string& req_url) {
                auto [rcb, templ] = getTemplateCallback(req_url, m_Routes);
                if (rcb != nullptr && !templ.empty()) {
                    param_t params = getParamsFromUrl(templ, req_url);
                    query_t queries;
                    return RouteObject{ .Params = params, .Queries = queries, .rcb = rcb };
                } else {
                    auto rcF = m_Routes.find(req_url);
                    if(rcF->second) return RouteObject{ .rcb = rcF->second };
                    return RouteObject { .rcb = NotFoundPage };
                }
            }
            void RenderNotFoundPage(orni::Request&& req, orni::Response&& res) {
                NotFoundPage(std::move(req), std::move(res));
            }
            void RenderServerErrorPage(orni::Request&& req, orni::Response&& res) {
                ServerErrorPage(std::move(req), std::move(res));
            }
        };
    }  // namespace router
}  // namespace orni
#endif  // ORNI_ROUTER_HPP
