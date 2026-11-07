#pragma once

#include <iosfwd>

namespace erl {
class ReliefHub;
void runInteractive(ReliefHub&);
void runInteractive(ReliefHub&, std::istream&, std::ostream&);
}
