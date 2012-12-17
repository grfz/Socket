/*
 * Socket.hpp
 * This file is part of VallauriSoft
 *
 * Copyright (C) 2012 - Comina, gnuze
 *
 * VallauriSoft is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * VallauriSoft is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VallauriSoft. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <stdlib.h>

#include <arpa/inet.h>

#define MAX_BUFFER 1024

using namespace std;

namespace Socket
{
    typedef int Socket;
    typedef string Ip;
    typedef unsigned int Port;
    typedef string Data;
    
    typedef struct
    {
        Ip ip;
        Port port;
    }Address;
    
    typedef struct
    {
        Address address;
        Data data;
    }Datagram;
    
    struct sockaddr_in* to_sockaddr(Address* a)
    {   struct sockaddr_in* ret;

        ret=(struct sockaddr_in*) malloc (sizeof(struct sockaddr_in));
        ret->sin_family = AF_INET;
        inet_aton(a->ip.c_str(),&(ret->sin_addr));
        ret->sin_port=htons(a->port);
            
        return ret;
    }

    Address* from_sockaddr(struct sockaddr_in* address)
    {   Address* ret;

        ret=(Address*)malloc(sizeof(Address));
        ret->ip = inet_ntoa(address->sin_addr);
        ret->port = ntohs(address->sin_port);
        
        return ret;
    }

    class Exception
    {
    private:
        string _message;
    public:
        Exception(string error) { this->_message = error; }
        virtual const char* what() { return this->_message.c_str(); }
    };

    class UDP
    {
    private:
        
        Socket _socket_id;
        bool _binded;
        
    public:
        
        UDP(void);
        ~UDP(void);
        void close(void);
        void bind(Port port);
        void send(Ip ip, Port port, Data data);
        Datagram receive();
    };

    UDP::UDP(void)
        {
            this->_socket_id = socket(AF_INET, SOCK_DGRAM, 0);   
            if (this->_socket_id == -1) throw Exception("[Constructor] Cannot create socket");           
            this->_binded = false;
        }

    UDP::~UDP(void)
        {
        }
        
    void UDP::close(void)
        {
            shutdown(this->_socket_id, SHUT_RDWR);
        }
        
    void UDP::bind(Port port)
        {
            struct sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.s_addr=htonl(INADDR_ANY);
            address.sin_port=htons(port);
            
            if (this->_binded)
            {
                this->close();
                this->_socket_id = socket(AF_INET, SOCK_DGRAM, 0);
            }
            // ::bind() calls the function bind() from <arpa/inet.h> (outside the namespace)            
            if (::bind(this->_socket_id, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) == -1)
            {
                stringstream error;
                error << "[listen_on_port] with [port=" << port << "] Cannot bind socket";
                throw Exception(error.str());
            }
            
            this->_binded = true;
        }
        
    void UDP::send(Ip ip, Port port, Data data)
        {
            struct sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_port = htons(port);
            inet_aton(ip.c_str(), &address.sin_addr);
            
            if (sendto(this->_socket_id, (void*)data.c_str(), data.length() + 1, 0, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) == -1)
            {
                stringstream error;
                error << "[send] with [ip=" << ip << "] [port=" << port << "] [data=" << data << "] Cannot send";
                throw Exception(error.str());
            }
        }
        
    Datagram UDP::receive()
        {
            int size = sizeof(struct sockaddr_in);
            char *buffer = (char*)malloc(sizeof(char) * MAX_BUFFER);
            struct sockaddr_in address;
            Datagram ret;
            
            if (recvfrom(this->_socket_id, (void*)buffer, MAX_BUFFER, 0, (struct sockaddr*)&address, (socklen_t*)&size) == -1) throw Exception("[receive] Cannot receive");
            
            ret.data = buffer;
            ret.address.ip = inet_ntoa(address.sin_addr);
            ret.address.port = ntohs(address.sin_port);
            
            free(buffer);
            
            return ret;
        }
}

#endif   // _SOCKET_HPP_

