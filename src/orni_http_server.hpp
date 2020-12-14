#ifndef ORNI_HTTP_SERVER_HPP
#define ODNI_HTTP_SERVER_HPP
#include "orni_wrapper.hpp"
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/string_body.hpp>
#include <map>

namespace orni {
    //  struct for handling routes (callback, and http method) to avoid unnecessary uses of othet containers
    class Request {};
    class Response {
        int m_Status = 200;
        std::string_view m_Content_Type;
        std::string m_Headers;
    };
    typedef  std::function<void(const orni::Request&, const orni::Response&)> route_callback;
    enum class HttpMethod {
        GET,
        PUT,
        DELETE,
        PATCH,
        POSTA
    };
    struct RouteHandler {
        HttpMethod method;
        route_callback callback;
        
    };
    class HttpServer {
        orni::SocketPP HttpSocket;
        int m_Port = 4200;
        std::map<std::string, RouteHandler> m_Routes;


        template<HttpMethod m_Hm>
        void m_AddRoute(const std::string& path, const route_callback& callback) {
            auto exist = m_Routes.find(path);
            if (exist != m_Routes.end()) {
                throw orni::Exception("path already exists");
            }
            m_Routes[path] = RouteHandler{ .method = m_Hm, .callback = callback };;
        }
        public:
        void setPort(int p){
            HttpSocket.setPort(p);
        }
        int getPort(){
            return HttpSocket.getPort();
        }

    };
}  // namespace orni

#endif  // ORNI_HTTP_SERVER_HPP
