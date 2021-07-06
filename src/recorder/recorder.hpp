//
// Author: Tassilo Tanneberger on 3/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#ifndef ROBO_SIMULATOR_RECORDER_HPP
#define ROBO_SIMULATOR_RECORDER_HPP

#include "record_entry.hpp"

// Network headers for receiving and transmitting saved data
#include "../network/public_server.hpp"
#include "../network/client.hpp"

#include <chrono>
#include <string>
#include <list>

using Duration = std::chrono::duration<double, std::chrono::microseconds>;

class Recorder{
private:
    double m_start_recording = 0; /*! timestamp when the recording was started */
    std::list<RecordEntry> m_record_entries = {}; 
    /*! 
     * read(...) will put read record entries into this list
     * transmittObj(...) will transmitt this list of record entries
     * receive(...) will use m_record_entries as a queue in a producer consumer style pattern
     */


    bool m_finished_recording = false; /*! will terminate the write thread */
    std::mutex m_shared_mutex; /*! locks m_record_entries when receive adds a new element or writeThread takes one out*/

    /*!
    * @brief Thread for receiving packages and than it will be pushed into m_record_entries
    * @param data_source readable datasource
    * @param receive_time how long the socket will receive packages
    */
    void receiveThread(DataSource& data_source, Duration receive_time);

    /*!
    * @brief Writtes every entry in m_record_entries into the specified file and will
    * wait until receiveThread is terminated.
    * @param path filepath
    */
    void writeThread(const std::string& path);

public:
    static bool m_forcefull_termination; /*! Used by the Signal handler to terminate all processes */

    Recorder() noexcept;
    ~Recorder() noexcept;

    /*!
    * @brief automatically creates client socket and starts recording
    * @param host ipv4 address of data source
    * @param port 
    * @param duration how long data will be received
    */
    static void performRecording(const std::string& host, unsigned short port, Duration duration);

    /*!
    * @brief takes an record save file and converts it into human readable format 
    * @param input_file properly formatted save file
    * @param output_file file in humanreadable formatt
    */
    static void convertToHumanReadible(const std::string& input_file, const std::string& output_file);

    /*!
    * @brief Writes the content of m_record_entries into the specified file 
    * @param path
    */
    void write(const std::string& path) const;
    /*!
    * @brief Reads the given file and creates new record entries from it and appends them
    * to the m_record_entries list
    * @param path
    */
    void read(const std::string& path);

    /*!
    * @brief transmitts every record entry found in the given file
    * @param socket writeable socket over which the read data will be send
    * @param file properly formattet record file
    */
    void transmittFile(DataSink& socket, const std::string& file) const;

    /*!
    * @brief transmitts every record entry that is saved in the object it self
    * @param socket writeable socket over which the read data will be send
    */
    void transmittObj(DataSink& socket) const;

    /*!
    * @brief receives packages for some given amount of time and creates are record file outof it
    * @param socket readable socket over which the data will be read.
    * @param file the resulting file that contains the record
    * @param receive_time How long the socket will receive packages
    */
    void receive(DataSource& socket, const std::string& file, Duration receive_time);
};

#endif //ROBO_SIMULATOR_RECORDER_HPP
