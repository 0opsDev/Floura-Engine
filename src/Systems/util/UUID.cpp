#include "UUID.h"

#include <random>

static std::random_device randomDevice;
static thread_local std::mt19937_64 engine(randomDevice());
static std::uniform_int_distribution<uint64_t> uniformDistribution;


uint64_t UUID::returnHandle(){
	return uniformDistribution(engine);
}
