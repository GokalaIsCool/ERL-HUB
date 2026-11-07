#include "erl/Formatters.h"
#include <ostream>
namespace erl { std::ostream&tableRow(std::ostream&o){return o<<"| ";} std::ostream&tableSeparator(std::ostream&o){return o<<"+----------------+----------------+----------------+----------------+\n";} }

