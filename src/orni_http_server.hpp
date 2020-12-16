/*
 * Copyright ©  2020-2021 Dammi 
 * License: AGPLv3
 * */
#ifndef ORNI_HTTP_SERVER_HPP
#define ORNI_HTTP_SERVER_HPP
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <cassert>
#include "httpparser/urlparser.h"
#include "httpparser/httprequestparser.h"
#include "httpparser/request.h"
#include "orni_wrapper.hpp"
#include "util/orni_logger.hpp"

namespace orni {
    //  function to match a path with a parameter from paths
    std::string getUrlParam(const std::string& path,
            const std::vector<std::string>& paths) {
        for (auto& p: paths) {
            size_t pos = p.find(":");
            if (pos != std::string::npos) {
                if (p.substr(0, pos) == path.substr(0, pos)) {
                    return p;
                }
            }
        }
    }
    void split(std::string const &str, const char delim,
            std::vector<std::string> &out)
    {
        std::stringstream ss(str);

        std::string s;
        while (std::getline(ss, s, delim)) {
            out.push_back(s);
        }
    }

    std::map<std::string, std::string> getParams(const std::string& path,
            const std::string& urlpath) {
        std::map<std::string, std::string> params;
        std::vector<std::string> out;
        std::vector<std::string> urlout;
        split(path, '/', out);
        split(urlpath, '/', urlout);
        assert(urlout.size() == out.size());
        size_t index = 0;
        for(auto& p: out) {
            index++;
            size_t cpos = p.find(":");
            if (cpos == 0) {
                std::string newStr = p.substr(cpos, p.size());
                std::string param = newStr.substr(1, p.size());
                params[param] = urlout[index-1];
            }
        }
    }
    enum class HttpMethod {
        GET,
        PUT,
        DELETE,
        PATCH,
        POST,
    };
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
        void redirect(const std::string& neUrl) {
            setStatus(301);
            set("Location", neUrl);
        }
    };
    typedef std::function<void(orni::Request, orni::Response)> route_callback;

    void ServerErr(orni::Request&& req, orni::Response&& res) {
        res.setStatus(500);
        res.set("Content-Type", "text/html");
        res.send("<h1>500 internal server error.</h1>");
        res.dump();
    }
    void NotFoundErr(orni::Request&& req, orni::Response&& res) {
        res.setStatus(404);
        res.set("Content-Type", "text/html");
        res.send("<h1>404 not found.</h1>");
        res.dump();
    }

    Request ParserToRequest(const httpparser::Request& req) {
        httpparser::Params params;
        httpparser::parseQueryString(params, req.uri);
        std::string _body(req.content.begin(), req.content.end());
        std::map<std::string, std::string> StHeaders;
        std::map<std::string, std::string> StQueries;
        for (auto& i : req.headers) {
            StHeaders[i.name] = i.value;
        }
        for (auto& [key, val]: params) {
            StQueries[key] = val;
        }
        Request retReq{
            .Headers = StHeaders,
                .ContentType = StHeaders["Content-Type"],
                .Body = _body,
                .Method = req.method,
                .Url = req.uri,
                .Queries = StQueries
        };
        return retReq;
    }

    class HttpServer : public SocketPP {
        std::map<std::string, route_callback> m_Routes;
        orni::Logger m_Logger;
        route_callback m_NotFoundPage = NotFoundErr;
        route_callback m_ServerError = ServerErr;

        void m_AddRoute(const std::string& path, const route_callback& callback) {
            auto exist = m_Routes.find(path);
            if (exist != m_Routes.end()) {
                throw orni::Exception("path already exists");
            }
            m_Routes[path] = callback;
        }

        public:
        void route(const std::string& path, const route_callback& callback) {
            m_AddRoute(path, callback);
        }
        void setNotFound(const route_callback& errRoute) {
            m_NotFoundPage = errRoute;
        }
        void setServerError(const route_callback& errRoute) {
            m_ServerError = errRoute;
        }
        void run(int x = 5000) {
            setPort(x);
            init_socket();
            Bind();
            Listen();
            std::stringstream ss;
            ss << "started server on port " << x;
            m_Logger.info(ss.str());
            while (1) {
                Accept();
                char rawHttpReq[4096];
                Recv(rawHttpReq);
                httpparser::HttpRequestParser parser;
                httpparser::Request preq;
                httpparser::UrlParser purl;
                parser.parse(preq, rawHttpReq, rawHttpReq + 4096);
                std::stringstream fulluri;
                fulluri << "http://localhost:" << x
                    << preq.uri;
                purl.parse(fulluri.str());
                try {
                    auto getRoute = m_Routes.find(purl.path());
                    if (getRoute != m_Routes.end()) {
                        getRoute->second(ParserToRequest(preq), Response(GetConn()));
                        std::stringstream ss;
                        ss << preq.method << " " << purl.path() << " " << 200;
                        m_Logger.info(ss.str());
                        CloseConn();
                    } else {
                        m_NotFoundPage(ParserToRequest(preq), Response(GetConn()));
                        std::stringstream ss;
                        ss << preq.method << " " << purl.path() << " " << 404;
                        m_Logger.warn(ss.str());
                        CloseConn();
                    }
                } catch(...) {
                    m_ServerError(ParserToRequest(preq), Response(GetConn()));
                    std::stringstream ss;
                    ss << preq.method << " " << purl.path() << " " << 500;
                    m_Logger.error(ss.str());
                    CloseConn();
                }
            }
            CloseSocket();
        }
    };
}  // namespace orni

#endif  // ORNI_HTTP_SERVER_HPP
