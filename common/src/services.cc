#include "notshell/common/services.h"

static std::unordered_map<std::string, std::uint64_t> id_sequences{};

std::uint64_t dochkas::notshell::common::getNextId(const std::string& key) {
    return id_sequences[key]++;
}