//
// Author: Tassilo Tanneberger on 1/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#include "./recorder/record_server.hpp"
#include "./signal_handler.hpp"
#include "./network/udp_socket.hpp"

#include <thread>
#include <filesystem>

auto random_data(std::size_t size) -> std::string {
    std::string str(" ", size);

    for (auto i = 0u; i < size; i++) {
        str[i] = 65 + (rand() % 24);
    }
    return str;
};

int main(){
    using namespace std::chrono_literals;
    namespace fs = std::filesystem;
    spdlog::set_level(spdlog::level::debug);

    signal(SIGINT, signalHandlerSigTerm);
    srand((unsigned int)time(nullptr));

    std::string this_host = "127.0.0.1";
    std::string robot_host = "192.168.60.3";
    unsigned short port_client = 9988;
    unsigned short port_robot = 9009;
    std::string file = "./file_record.rd";
    Connection client_a{ this_host, port_client };
    Connection robot_addr{ this_host, port_robot };

    Recorder rec;
    SocketUdp sock{ robot_addr };
    //sock.setTarget(robot_addr);

    std::chrono::microseconds time = 10s;
    rec.receive(sock, file, time);

    //generate_mock_data();
    //std::this_thread::sleep_for(15s);
    //receive_mocked_data();
    //std::this_thread::sleep_for(15s);
}

