/*
 * Copyright ©  2020-2021 Dammi 
 * License: AGPLv3
 *
 */
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
  explicit Exception(const std::string& str) : m_Error(str) {}
  virtual const std::string_view what() const throw() { return m_Error; }
  virtual ~Exception() = default;
};
class SocketPP {
  int m_Sockfd;  // socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in m_SockAddr;
  sa_family_t m_Family = AF_INET;
  int m_Port = 3100;
  int m_Type = SOCK_STREAM;
  int m_maxConn = 20;
  int m_Connection;


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
      throw Exception(ss.str());                                                            
    }
  }
  void Send(const std::string& str) {
    write(m_Connection, str.c_str(), str.size());
  }
  template<size_t N>
  void Recv(char (&buffer)[N]) {
    read(m_Connection, buffer, N);
  }
  void setPort(int nPort) { m_Port = nPort; }                                           
  void CloseConn() { close(m_Connection); }
  void CloseSocket() { close(m_Sockfd); }
  int getPort() { return m_Port; }
  int GetConn() { return m_Connection; }
};
}  // namespace orni
#endif  // ORNI_WRAPPER_HPP
