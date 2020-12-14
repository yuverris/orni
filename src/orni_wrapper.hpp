/*
 *      socket wrapper from C to C++ style 😎
 * */

#ifndef ORNI_WRAPPER_HPP
#define ORNI_WRAPPER_HPP
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <sstream>

namespace orni {
class Exception {
  std::string m_Error;

 public:
  Exception(const std::string& str) : m_Error(str) {}
  virtual const std::string_view what() const throw() { return m_Error; }
  virtual ~Exception() = default;
};
class SocketPP {
  int m_Sockfd;  // socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in m_SockAddr;
  sa_family_t m_Family = AF_INET;
  int m_Connection;
  int m_Port = 3100;
  int m_Type = SOCK_STREAM;
  int m_maxConn = 20;


    public:
  void init_socket() { m_Sockfd = socket(m_Family, m_Type, 0); }
  void Bind() {
    m_SockAddr.sin_family = m_Family;
    m_SockAddr.sin_addr.s_addr = INADDR_ANY;
    m_SockAddr.sin_port = htons(m_Port);
    if (bind(m_Sockfd, (struct sockaddr*)&m_SockAddr, sizeof(sockaddr)) < 0) {
      std::stringstream err;
      err << "failed to bind socket errno " << strerror(errno);
      throw Exception(err.str());
    }
  };
  void Listen() {
    if (listen(m_Sockfd, m_maxConn) < 0) {
      std::stringstream ss;
      ss << "failed to listen errno " << strerror(errno);
      throw Exception(ss.str());
    }
  }
  void Accept() {
    socklen_t addrlen = sizeof(m_SockAddr);
    m_Connection = accept(m_Sockfd, (struct sockaddr*)&m_SockAddr, &addrlen);
    if (m_Connection < 0) {
      std::stringstream ss;
      ss << "failed to accept errno " << strerror(errno);
      throw Exception(ss.str());                                                            }
  }
  void Send(const std::string& str) {
    write(m_Connection, str.c_str(), str.size());
  }
  template<size_t N>
  void Recv(char (&buffer)[N]) {
    read(m_Connection, buffer, N);
  }
  void setPort(int nPort) { m_Port = nPort; }                                           
  int getPort() { return m_Port; }

  /*void InitServer() {
    m_init_socket();
    onServerStart();
    m_Bind();
    m_Listen();
    m_Accept();
    onConnOpen();
    char out[10240];
    while (1) {
        memset(out, 0, 10240);
        m_Listen();
        m_Accept();
        if (m_Connection > 0) {
            onConnOpen();
        }
        int res = recv(m_Connection, out, 10240, 0);
        if (res == 0) {
            onConnClose();
        }
        onMessage(std::string_view{out});
    }
  }
  void SendMessage(const std::string& msg) { m_Send(msg); }

  void SendMessage(std::string&& msg) { m_Send(std::move(msg)); }
  void CloseConn() { close(m_Connection); }
  void CloseSocket() { close(m_Sockfd); }
  void setFamily(sa_family_t fam) { m_Family = fam; }
  std::function<void()> onConnOpen;
  std::function<void(const std::string_view&)> onMessage;
  std::function<void()> onServerStart;
  std::function<void()> onConnClose;*/
};
}  // namespace orni
#endif  // ORNI_WRAPPER_HPP
