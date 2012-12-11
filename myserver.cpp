
#include <iostream>
#include "Socket.hpp"

using namespace std;

int main(void)
{
    try
    {
        Socket::UDP sock;
        
        sock.bind(2000);
        
        Socket::Datagram received = sock.receive();
        
        cout << received.data << endl;
        
        sock.send("127.0.0.1", 3000, "response");
        
        sock.close();
    }
    catch (Socket::Exception &e)
    {
        cout << e.what() << endl;
    }
    
    return 0;
}

