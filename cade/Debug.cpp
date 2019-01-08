#include "Debug.hpp"

#include <sstream>
#include <iomanip>

std::string opcodeToInstruction(uint16_t opcode)
{
	std::stringstream ss;
	switch (opcode & 0xF000)
	{
		case 0x0000:
		{
			switch (opcode & 0x00FF)
			{
			case 0xE0:
				return "CLS";
			case 0xEE:
				return "RET";
			default: 
				ss << std::hex << std::setw(4) << std::setfill('0');
				return "Invalid instruction - " + ss.str();
			}
			break;
		}
	}
}
