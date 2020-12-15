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

#include "httpparser/httprequestparser.h"
#include "httpparser/request.h"
#include "orni_wrapper.hpp"

namespace orni {
enum class HttpMethod {
  GET,
  PUT,
  DELETE,
  PATCH,
  POST,
};
HttpMethod str_to_method(const std::string& str) {
  if (str == "GET") {
    return HttpMethod::GET;
  }
  if (str == "PUT") {
    return HttpMethod::PUT;
  }
  if (str == "DELETE") {
    return HttpMethod::DELETE;
  }
  if (str == "POST") {
    return HttpMethod::POST;
  }
  if (str == "PATCH") {
    return HttpMethod::PATCH;
  }
  throw orni::Exception("unkown http method");
}
std::string_view method_to_str(const HttpMethod& meth) {
  switch (meth) {
    case HttpMethod::GET:
      return "GET";
      break;
    case HttpMethod::PUT:
      return "PUT";
      break;
    case HttpMethod::PATCH:
      return "PATCH";
      break;
    case HttpMethod::DELETE:
      return "DELETE";
      break;
    case HttpMethod::POST:
      return "POST";
      break;
  }
}
struct Request {
  std::map<std::string, std::string> Headers;
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
    for (auto& [key, value] : m_Headers) {
      ss << key << ":"
         << " " << value << "\r\n";
    }
    ss << "\n" << m_Body.str();
    auto str = ss.str();
    write(m_Conn, str.c_str(), str.size());
  }
  void renderTemplate(){};
  void setStatus(int s) { m_Status = s; }
  void send(const std::string_view& cn) { m_Body << cn; }
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
  std::string _body(req.content.begin(), req.content.end());
  ;
  std::map<std::string, std::string> StHeaders;
  for (auto& i : req.headers) {
    StHeaders[i.name] = i.value;
  }
  Request retReq{
      .Headers = StHeaders,
      .ContentType = StHeaders["Content-Type"],
      .Body = _body,
      .Method = req.method,
      .Url = req.uri,
  };
  return retReq;
}

class HttpServer : public SocketPP {
  std::map<std::string, route_callback> m_Routes;
  std::map<std::string, std::string> m_RouteAliases;

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
  void alias(const std::string& new_route, const std::string& path) {
      auto alias_find = m_RouteAliases.find(new_route);
      if (alias_find != m_RouteAliases.end()) {
          std::stringstream ss;
          ss << new_route << " alias already exists";
          throw orni::Exception(ss.str());
      }
      auto route_find = m_Routes.find(path);
      if (route_find != m_Routes.end()) {
          m_RouteAliases[new_route] = path;
      } else {
          std::stringstream ss;
          ss << path << " route doesn't exist";
          throw orni::Exception(ss.str());
      }
  }
  void run(int x) {
    setPort(x);
    init_socket();
    Bind();
    Listen();
    while (1) {
      Accept();
      char rawHttpReq[4096];
      Recv(rawHttpReq);
      httpparser::HttpRequestParser parser;
      httpparser::Request preq;
      parser.parse(preq, rawHttpReq, rawHttpReq + 4096);
      auto getRoute = m_Routes.find(preq.uri);
      auto getAlias = m_RouteAliases.find(preq.uri);
      try {
      if (getRoute != m_Routes.end()) {
        getRoute->second(ParserToRequest(preq), Response(GetConn()));
        CloseConn();
      } else if (getAlias != m_RouteAliases.end()) {
        m_Routes[getAlias->second]
            (ParserToRequest(preq), Response(GetConn()));
        CloseConn();
      } else {
         NotFoundErr(ParserToRequest(preq), Response(GetConn()));
        CloseConn();
      }
      } catch(...) {
          ServerErr(ParserToRequest(preq), Response(GetConn()));
          CloseConn();
      }
    }
    CloseSocket();
  }
  void run() {
    init_socket();
    Bind();
    Listen();
    while (1) {
      Accept();
      char rawHttpReq[4096];
      Recv(rawHttpReq);
      httpparser::HttpRequestParser parser;
      httpparser::Request preq;
      parser.parse(preq, rawHttpReq, rawHttpReq + 4096);
      auto getRoute = m_Routes.find(preq.uri);
      auto getAlias = m_RouteAliases.find(preq.uri);
      try {
      if (getRoute != m_Routes.end()) {
        getRoute->second(ParserToRequest(preq), Response(GetConn()));
        CloseConn();
      } else if (getAlias != m_RouteAliases.end()) {
        m_Routes[getAlias->second]
            (ParserToRequest(preq), Response(GetConn()));
        CloseConn();
      } else {
         NotFoundErr(ParserToRequest(preq), Response(GetConn()));
        CloseConn();
      }
      } catch(...) {
          ServerErr(ParserToRequest(preq), Response(GetConn()));
          CloseConn();
      }
    }
    CloseSocket();
  }
};
}  // namespace orni

#endif  // ORNI_HTTP_SERVER_HPP
