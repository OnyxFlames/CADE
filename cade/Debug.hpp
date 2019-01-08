#pragma once

#define debug_opcode(op) do { opcode_history.push_back(op); } while (false)

#include <string>

std::string opcodeToInstruction(uint16_t opcode);