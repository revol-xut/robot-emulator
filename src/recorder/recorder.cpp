//
// Author: Tassilo Tanneberger on 1/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#include "recorder.hpp"
#include "record_entry.hpp"

#include <fstream>
#include <thread>
#include <ctime>

bool Recorder::m_forcefull_termination = false;

void Recorder::write(const std::string& path) const {
	std::ofstream file;
	file.open(path, std::ios_base::trunc);

	for (const RecordEntry& entry : m_record_entries) {
		file << entry.format();
	}

	file.close();
}

void Recorder::read(const std::string& path) {
	std::ifstream file;
	file.open(path, std::ios::binary);
	file.seekg(0);

	while (!file.eof() && file.good() && !m_forcefull_termination) {
		uint32_t size = 0;
		file.readsome((char*)&size, sizeof(size));

		uint32_t  relative_time_stamp = 0;
		file.readsome((char*)&relative_time_stamp, sizeof(relative_time_stamp));

		if (size > 0) {
			std::string data = "";
			data.reserve(size);
			file.readsome((char*)data.data(), size);

			m_record_entries.emplace_back(data, relative_time_stamp);
		} else {
			break;
		}
		
	}

	file.close();
}


void Recorder::receive(DataSource& socket, const std::string& file, Duration receive_time) {
	m_finished_recording = false;
	
	std::thread write_thread(&Recorder::writeThread, this, file);
	receiveThread(socket, receive_time);

	write_thread.join();
}

void Recorder::transmittFile(DataSink& socket, const std::string& path) const {
	std::ifstream file;
	file.open(path, std::ios::binary);
	auto start = std::chrono::system_clock::now();

	std::string data = "";
	while (!file.eof() && !m_forcefull_termination) {
		// reading size
		uint32_t size = 0;
		file.read((char*)&size, sizeof(size));

		// reading timestamp
		uint32_t relative_time_stamp = 0;
		file.read((char*)&relative_time_stamp, sizeof(relative_time_stamp));
		relative_time_stamp = relative_time_stamp / 10; //TODO: possibly a bug

		//reading data
		data.resize(size);
		file.read((char*)data.data(), size);
		

		// calculating until when to sleep
		// positive thing about this is that the time that is required for reading calculated in (or ignored)
		auto sleep_until = start + std::chrono::microseconds{ relative_time_stamp };
		std::this_thread::sleep_until(sleep_until);

		auto response = socket.write(data);

		if (response == Response::Failure) {
			break;
		}
	}

	file.close();
}

void Recorder::transmittObj(DataSink& socket) const {
	auto start = std::chrono::system_clock::now();

	for (const RecordEntry& rec_entry : m_record_entries) {

		uint32_t relative_time_stamp = rec_entry.getTimeStamp();

		auto sleep_until = start + std::chrono::microseconds{ relative_time_stamp };
		std::this_thread::sleep_until(sleep_until);
		
        if( socket.write(rec_entry.getData()) == Response::Failure){
            break;
        }
	}
}


void Recorder::receiveThread(DataSource& socket, Duration receive_time) {
	auto start = std::chrono::system_clock::now();

	// Break criteria
	// 1.) Receive_Time reached, 2.) External termination or 3.) Connection loss
	while ((std::chrono::system_clock::now() - start) < receive_time && !m_forcefull_termination && socket.good()) {
		auto data = socket.readString();
		if (data.has_value()) {
			auto timestamp = std::chrono::system_clock::now() - start;
			uint32_t formatted_timestamp = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::microseconds>(timestamp).count()) * 10;

			RecordEntry record_entry{ data.value(), formatted_timestamp };

			{
				std::lock_guard<std::mutex> guard(m_shared_mutex);
				m_record_entries.push_back(record_entry);
			}
		}
	}
	m_finished_recording = true;
}


void Recorder::writeThread(const std::string& path) {
	std::ofstream file;
	file.open(path, std::ios_base::trunc);

	while (!m_finished_recording && !m_forcefull_termination) {
		RecordEntry entry;
		if (!m_record_entries.empty()) {
			{
				std::lock_guard<std::mutex> guard(m_shared_mutex);
				entry = m_record_entries.front();
				m_record_entries.pop_front();
				// mutex is unlocked here again because it goes outof scope
			}
			
			file << entry.format();
		}
	}
	file.close();
}


void Recorder::performRecording(const std::string& host, unsigned short port, Duration duration) {
	auto time_str_gen = []() {
		time_t rawtime;
		struct tm* timeinfo;
		char buffer[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime); //NOLINT

		strftime(buffer, sizeof(buffer), "%d-%m-%Y_%H:%M:%S", timeinfo);
		return std::string(buffer);
	};

	std::string file_name = "./recording_" + time_str_gen() + ".rd";

	Client client_socket{};

	Connection connection{};
	connection.host = host;
	connection.port = port;

	if (client_socket.establishConnection(connection) == Response::Success) {
		Recorder rec;

		rec.receive(client_socket, file_name, duration);
	}
	else {
		spdlog::debug("Could not connect !");
		return;
	}
}


void Recorder::convertToHumanReadible(const std::string& input_file, const std::string& output_file) {
	std::ifstream read_file;
	read_file.open(input_file, std::ios::binary);
	std::ofstream write_file;
	write_file.open(output_file, std::ios_base::trunc);
	write_file << "Microseconds | Payload Size | Payload \n";


	std::string data = "";
	while (!read_file.eof() && !m_forcefull_termination) {
		// reading size
		uint32_t size = 0;
		read_file.read((char*)&size, sizeof(size));

		// reading timestamp
		uint32_t relative_time_stamp = 0;
		read_file.read((char*)&relative_time_stamp, sizeof(relative_time_stamp));
		relative_time_stamp = relative_time_stamp / 10;

		if (size == 0) {
			// There is the occasional zero size entry which will be removed with this
			break;
		}

		//reading data
		data.resize(size);
		read_file.read((char*)data.data(), size);

		write_file << std::to_string(relative_time_stamp) << "|" 
			<< std::to_string(size) << "|" << data << "\n";
	}
	read_file.close();
	write_file.close();
}
