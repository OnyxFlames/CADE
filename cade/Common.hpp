#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

typedef uint8_t byte_t;

//#define DEBUG_TRACE_EXECUTION
//#define DEBUG_TRACE_STATE
//#define DEBUG_HUD

#ifdef _WIN32
#include <Windows.h>
#define PLATFORM_WINDOWS
#define PLATFORM_NAME "Windows"
#else
#define PLATFORM_UNKNOWN
#define PLATFORM_NAME "Unknown"
#endif