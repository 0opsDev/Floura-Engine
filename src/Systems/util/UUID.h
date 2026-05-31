#ifndef UUID_H
#define UUID_H

#include <xhash>
#include <iomanip>
#include <sstream>

class UUID
{
public:
	static uint64_t returnHandle();

	static std::string UUIDToString(const uint64_t& id) {
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(16) << std::hex << id;
		return ss.str();
	}

	static uint64_t StringToUUID(const std::string& str) { return std::stoull(str, nullptr, 16); }

	static bool compare(uint64_t aID, uint64_t bID) { return aID == bID; }
};

#endif