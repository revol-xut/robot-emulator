//
// Author: Tassilo Tanneberger on 1/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#include "./network/udp_socket.hpp"
#include "./signal_handler.hpp"

#include <thread>

const std::string file = "test_record.rd";
const std::string host = "127.0.0.1";
constexpr unsigned short port = 8899;
unsigned short port_play = 9988;

auto random_data(std::size_t size) -> std::string {
    std::string str(" ", size);

    for (auto i = 0u; i < size; i++) {
        str[i] = 65 + (rand() % 24);
    }
    return str;
};

void generate_mock_data() {
    using namespace std::chrono_literals;
    Connection connection{};
    connection.host = host;
    connection.port = port;
    Client client_socket{};

    auto response = client_socket.establishConnection(connection);
    std::cout << response << std::endl;

    auto random_time = [](std::size_t max) {
        return (rand() % max);
    };

    for (auto i = 0u; i < 10; i++) {
        auto time = random_time(1000);
        std::cout << "sleeping for : " << time << "ms" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        auto random_message = random_data(random_time(100));

        std::cout << "sending: " << random_message << std::endl;
        client_socket.write(random_message);
    }

    client_socket.closeSocket();
};


void receive_mocked_data() {
    Connection connection{};
    connection.host = host;
    connection.port = port_play;
    Client client_socket{};

   
    if (client_socket.establishConnection(connection) == Response::Success) {
        std::cout << "Connected" << std::endl;
    }
    else {
        std::cerr << "Could not connect !" << std::endl;
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();
    bool first_package = false;
    while (client_socket.good()) {
        auto data = client_socket.readString();

        if (data.has_value()) {
            if (!first_package) {
                start = std::chrono::high_resolution_clock::now();
                first_package = true;
            }

            std::cout << "Received: " << data.value() << " Timestamp: "
                << (((std::chrono::high_resolution_clock::now() - start)).count() / std::pow(10, 9)) + 1.0
                << std::endl;
        }
    };
};




int main(){
    spdlog::set_level(spdlog::level::debug);
    using namespace std::chrono_literals;
    srand((unsigned int)time(nullptr));
    signal(SIGINT, signalHandlerSigTerm);
    
    std::string this_host = "127.0.0.1";
    std::string robot_host = "192.168.60.3";
    unsigned short port_client = 9988;
    unsigned short port_robot = 9008;
    // std::string path = "./records/";
    Connection client_a{ this_host, port_client };
    Connection robot_addr{ robot_host, port_robot };

    std::this_thread::sleep_for(1s);

    Client client{};
    client.establishConnection(client_a);

    while (client.good()) {
        auto response = client.readString();
        if (response.has_value()) {
            std::cout << "client received:" <<  response.value() << std::endl;
        }
        
    }



}
