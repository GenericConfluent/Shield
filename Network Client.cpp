#include <iostream>
#include <SFML/Network.hpp>

const unsigned short PORT = 3499;
sf::IpAddress sender = "192.168.1.70";//change to suit your needs
sf::UdpSocket socket;


int main(int argc, char* argv[])
{
    socket.bind(PORT);
    sf::Packet packet;
    sf::IpAddress remoteAddress;
    unsigned short remotePort;
    socket.receive(packet, remoteAddress, remotePort);

    std::cout << "Recieved IP: " << remoteAddress.toString() << "; Recieved Port: " << remotePort <<'\n';

	return 0;
}
