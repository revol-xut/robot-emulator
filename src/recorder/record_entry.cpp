//
// Author: Tassilo Tanneberger on 1/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#include "record_entry.hpp"

RecordEntry::RecordEntry() noexcept = default;

RecordEntry::RecordEntry(const std::string & data, unsigned int relativ_time_stamp) noexcept : m_timestamp(relativ_time_stamp), m_data(data) {};

RecordEntry::~RecordEntry() noexcept = default;

auto RecordEntry::format() const -> std::string {

	uint32_t size = static_cast<uint32_t>(m_data.size());

	std::string output;
	output.reserve(size + sizeof(size) + sizeof(m_timestamp));  // Size and Timestamp
	output.resize(size + sizeof(size) + sizeof(m_timestamp));

	std::memcpy((char*)output.data(), &size, sizeof(size)); //NOLINT Copying size
	std::memcpy((char*)output.data() + sizeof(size), &m_timestamp, sizeof(m_timestamp)); //NOLINT Copying relativ timestamp
	std::copy(std::begin(m_data), std::end(m_data), std::begin(output) + sizeof(size) + sizeof(m_timestamp)); 

	return output;
}

auto RecordEntry::getData() const noexcept -> std::string {
	return m_data;
}

auto RecordEntry::getTimeStamp() const noexcept -> uint32_t {
	return m_timestamp;
}