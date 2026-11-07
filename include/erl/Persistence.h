#pragma once
#include <string>
namespace erl { class ReliefHub; class BinaryPersistence { public: static bool save(const ReliefHub&, const std::string&, std::string&); static bool load(ReliefHub&, const std::string&, std::string&); }; }

