#pragma once

#include "Common.hpp"

constexpr uint16_t chip8_res_x = 64, chip8_res_y = 32;
constexpr uint16_t schip48_res_x = 128, schip48_res_y = 64;

class Chip8
{
private:
	float mClockSpeed = 500.f;
	void invalidOperation();
	std::array<byte_t, 4096> mMemory{ 0 };
	uint16_t mIndexRegister = 0;
	uint16_t mProgramCounter = 0x200;
	bool mDrawPending = false;
	std::vector<byte_t> mGraphics;
	uint8_t mDelayTimer = 0;
	uint8_t mSoundTimer = 0;
	std::array<uint16_t, 16> mStack{ 0 };
	uint16_t* mStackTop = nullptr;
	std::array<uint8_t, 16> mRegisters{ 0 };
	std::array<bool, 16> mKeys { false };
	bool mWaitingOnKey = false;
	int8_t mKeyRegister = -1;
	bool mRomLoaded = false;
public:
	// mWaitingOnKey = false;
	Chip8();
	~Chip8();
	uint16_t fetch();
	void resetCPU();
	void loadROM(char* ROMpath);
	void execute();
	float& getClockSpeed();
	std::array<byte_t, 4096>& getMemory();
	uint16_t& getIndexRegister();
	uint16_t& getProgramCounter();
	bool& drawPending();
	std::vector<byte_t>& getGraphics();
	uint8_t& getDelayTimer();
	uint8_t& getSoundTimer();
	std::array<uint16_t, 16>& getStack();
	uint16_t*& getStackTop();
	void setKey(int8_t key);
	void releaseKey(int8_t key);
	std::array<bool, 16>& getKeys();
	std::array<uint8_t, 16>& getRegisters();
};

