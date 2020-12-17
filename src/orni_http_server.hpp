/*
 * Copyright ©  2020-2021 Dammi 
 * License: AGPLv3
 * */
#ifndef ORNI_HTTP_SERVER_HPP
#define ORNI_HTTP_SERVER_HPP
#include <utility>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <future>
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


    class HttpServer : public orni::router::Router {
        orni::Logger m_Logger;
        void _run(std::future<int>& f) {
            int x = f.get();
            setPort(x);
            init_socket();
            Bind();
            Listen();
            std::stringstream ss;
            ss << "started server on port " << x;
            m_Logger.info(ss.str());
            while (1) {
                Accept();
                char rawHttpReq[2048];
                Recv(rawHttpReq);
                parseRoutes(rawHttpReq);
                CloseConn();
            }
            CloseSocket();
        }
        public:
        void run(int x = 5000) {
            std::promise<int> p_run;
            std::future<int> f = p_run.get_future();
            std::future<void> f_run = std::async(std::launch::async, [&](){_run(std::ref(f));});
            p_run.set_value(x);
            f_run.wait();
        }
    };
}  // namespace orni

#endif  // ORNI_HTTP_SERVER_HPP
