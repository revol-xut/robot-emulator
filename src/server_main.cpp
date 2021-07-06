//
// Author: Tassilo Tanneberger on 1/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include "./src/recorder/record_server.hpp"
#include "./src/signal_handler.hpp"

#include <chrono>
#include <thread>
#include <experimental/filesystem>



int main(){
    signal(SIGINT, signalHandlerSigTerm);

    using namespace std::chrono_literals;
    namespace fs = std::experimental::filesystem;

    std::string ip = "127.0.0.1";
    unsigned short port_rec = 8899;
    unsigned short port_play = 9988;
    std::string path = "./records/";

    // creates directory if it doesn't exists
    if (!fs::exists(path)) {
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

    std::cout << "Done transmitting !" << std::endl;
}

