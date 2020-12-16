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
#include "orni_router.hpp"

namespace orni {
    enum class HttpMethod {
        GET,
        PUT,
        DELETE,
        PATCH,
        POST,
    };

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
                .Queries = StQueries,
                .ContentType = StHeaders["Content-Type"],
                .Body = _body,
                .Method = req.method,
                .Url = req.uri,
        };
        return retReq;
    }

    class HttpServer : public orni::router::Router {
        orni::Logger m_Logger;
        public:
        void run(int x = 5000) {
            //  rewrite this block just a complete mess of code
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
                    auto [params, queries, cb]  = getValidRoute(preq.uri);
                    Request req = ParserToRequest(preq);
                    req.Params = params;
                    cb(req, Response(GetConn()));
                    CloseConn();
                } catch(...) {
                    RenderServerErrorPage(ParserToRequest(preq), Response(GetConn()));
                    CloseConn();
                }
            }
            CloseSocket();
        }
    };
}  // namespace orni

#endif  // ORNI_HTTP_SERVER_HPP
