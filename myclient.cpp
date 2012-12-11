
#include <iostream>
#include "Socket.hpp"

using namespace std;

int main(void)
{
    try
    {
        Socket::UDP sock;
        
        sock.bind(3000);
        
        sock.send("127.0.0.1", 2000, "request");
        
        Socket::Datagram received = sock.receive();
        
        cout << received.data << endl;
        
        sock.close();
    }
    catch (Socket::Exception &e)
    {
        cout << e.what() << endl;
    }
    
    return 0;
}

