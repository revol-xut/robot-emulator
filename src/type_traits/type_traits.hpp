//
// Author: Tassilo Tanneberger on 28/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#ifndef ROBO_SIMULATOR_TYPE_TRAITS_HPP
#define ROBO_SIMULATOR_TYPE_TRAITS_HPP

#include <array>
#include <optional>

constexpr int kBufferSize = 1024;

struct RawMessage {
	std::array<char, kBufferSize> data;
	std::size_t size;
};

enum Response {
	Success,
	Failure
};


template <class T>
struct DetailedResponse {
	Response state;
	T data;
};

class Container {
public:
	virtual auto good() const -> bool = 0;
};



class DataSink : public Container {
public:
	virtual auto write(const RawMessage& message)->Response = 0;
	virtual auto write(const std::string& data)->Response = 0;
	virtual auto operator<<(const std::string& data)->DataSink& {
		write(data);
		return *this;
	}
};


class DataSource : public Container {
public:
	virtual auto readMessage()->std::optional<RawMessage> = 0;
	virtual auto readString()->std::optional<std::string> = 0;
	friend auto operator>>(DataSource& readable, std::string& data)->DataSource& {
		auto result = readable.readString();
		data = result.value_or("");
		
		return readable;
	}
};



#endif //ROBO_SIMULATOR_TYPE_TRAITS_HPP