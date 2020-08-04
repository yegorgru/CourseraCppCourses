#pragma once

#include "database.h"
#include "date.h"
#include "condition_parser.h"
#include "node.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <memory>
#include <sstream>

std::string ParseEvent(std::istream& is);

std::ostream& operator<<(std::ostream& out, std::pair<Date, std::string> p);
