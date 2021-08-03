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
    
    signal(SIGINT, signalHandlerSigTerm);

    using namespace std::chrono_literals;
    namespace fs = std::filesystem;
    spdlog::set_level(spdlog::level::debug);

    std::string host = "127.0.0.1";
    unsigned short port_client = 8899;
    unsigned short port_server = 9988;
   // std::string path = "./records/";
    Connection client_a{ host, port_client };
    Connection server_a{ host, port_server };
    SocketUdp server{ client_a, server_a };

    server.bindSocket();
    std::this_thread::sleep_for(5s);
    std::cout << "tansmitting" << std::endl;
    for (auto i = 0u; i < 30; i++) {
        server.write(random_data(50));

        std::this_thread::sleep_for(200ms);
    }

    // creates directory if it doesn't exists
    /*if (!fs::exists(path)) {
        fs::create_directories(path);
    }

    // creates record server
    {
        RecordServer record_server{ ip, port_rec };
        std::cout << "start listening" << std::endl;
        record_server.listenAndAccept("./records/example_", 15);
        std::cout << "Ending Receiving Process" << std::endl;
        record_server.close();
    }

    
    auto file = *fs::directory_iterator(path);

    std::cout << "using record file: " << file.path() << std::endl;

    Recorder::convertToHumanReadible(file.path().string(), std::string("debug_file.hr"));

    // playes first record file it found
    {
        PlaybackServer play_backserver{ ip, port_play };
        std::cout << "accepting connections" << std::endl;
        play_backserver.listenAndAccept(file.path().string(), 20);
        play_backserver.close();
    }

    std::cout << "Done transmitting !" << std::endl; */
}

