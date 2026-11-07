#pragma once

#include <string>

namespace erl {

class ReliefHub;

class BinaryPersistence {
public:
    static bool save(const ReliefHub&, const std::string& path, std::string& error);
    static bool load(ReliefHub&, const std::string& path, std::string& error);
};

} // namespace erl

