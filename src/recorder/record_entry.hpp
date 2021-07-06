//
// Author: Tassilo Tanneberger on 4/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#ifndef ROBO_SIMULATOR_RECORD_ENTRY_HPP
#define ROBO_SIMULATOR_RECORD_ENTRY_HPP

#include <chrono>
#include <string>
#include <mutex>

class RecordEntry {
private:
    uint32_t m_timestamp = 0;
    std::string m_data = "";
    
public:
    RecordEntry() noexcept;
    RecordEntry(const std::string& data, unsigned int relativ_time_stamp) noexcept;
    ~RecordEntry() noexcept;

    /*!
    * @brief Creates a string which will be written into the record file with all relevant information (size, timestamp, data) 
    */
    auto format() const-> std::string;

    /*!
    * @brief Returns data
    */
    auto getData() const noexcept -> std::string;

    /*
        Returns Timestamp in microseconds
    */
    auto getTimeStamp() const noexcept -> uint32_t;

};

#endif //ROBO_SIMULATOR_RECORD_ENTRY_HPP
