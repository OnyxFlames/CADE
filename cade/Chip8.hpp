#pragma once

/*
	Chip8 CPU class

	TODO: Add support for SCHIP8 and Megachip8 architectures
	Maybe allow for emulating as ETI 660, or
*/

#include "Common.hpp"

#include "SFML/Graphics.hpp"

class Chip8
{
private:
	const float TIMER_UPDATE_RATE = 1.f / 60.f;
	const float CPU_CLOCK_RATE = 1.f / 1.0f;
	uint16_t mProgramSize = 0;
	std::array<uint8_t, 4096> mMemory{ 0 };
	std::array<uint8_t, 16> mRegisters{ 0 };
	std::array<uint8_t, 64> mKeys{ 0 };

	uint16_t mIndexRegister = 0;
	uint16_t mProgramCounter = 0x200;

	std::array<bool, 64 * 32> mGraphics{ false };

	uint8_t mDelayTimer = 0;
	uint8_t mSoundTimer = 0;

	std::array<uint16_t, 16> mStack{ 0 };
	uint16_t* mStackPointer{ nullptr };
	// Did the CPU recently update the graphics buffer?
	bool mDrawPending = true;
	bool mKeyWaiting = false;
	void invalidOperation() const;
public:
	void resetCPU();
	bool& drawPending();
	const bool keyWaiting() const;
	int8_t mWaitingKey = -1;
	uint16_t fetch();
	void handleEvents(const sf::Event& event);
	void execute();
	Chip8();
	~Chip8();
	void loadFile(const char* filename);
	const float getTimerRate() const;
	const float getCPURate() const;

	uint8_t& delayTimer();
	uint8_t& soundTimer();

	uint16_t& indexRegister();
	uint16_t& programCounter();
	const uint16_t& programSize() const;

	std::array<bool, 64 * 32>& graphics();
	std::array<uint16_t, 16>& stack();
	uint16_t& stackTop();
	uint32_t stackSize();

	void pressKey(int8_t key);
	void releaseKey(int8_t key);
};

