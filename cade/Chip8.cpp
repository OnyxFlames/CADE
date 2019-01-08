#include "Chip8.hpp"


static uint8_t font_set[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


void Chip8::invalidOperation() const
{
	printf("Invalid operation [IP: %d|OP: %02X]\n",
		mProgramCounter - 2,
		*(uint16_t*)(mMemory.data() + (mProgramCounter - 2)));
	std::exit(1);
}

void Chip8::resetCPU()
{
	mProgramCounter = 0x200;
	//mMemory = std::array<uint8_t, 4096>();
	mRegisters = std::array<uint8_t, 16>();
	mKeys = std::array<uint8_t, 64>();
	mIndexRegister = 0;
	mDelayTimer = 0;
	mSoundTimer = 0;
	mStack = std::array<uint16_t, 16>();
	mStackPointer = mStack.data();
	mWaitingKey = -1;
	mKeyWaiting = false;
	mGraphics = std::array<bool, 64 * 32>();
}

bool& Chip8::drawPending()
{
	return mDrawPending;
}

const bool Chip8::keyWaiting() const
{
	return mKeyWaiting;
}

uint16_t Chip8::fetch()
{
	if (mProgramCounter >= 4096)
	{
		printf("Fetch out of bounds.\n");
		std::exit(1);
	}
	else
	{
		uint16_t opcode = 0;
		opcode += (mMemory[mProgramCounter++] << 8);
		opcode += mMemory[mProgramCounter++];
		return opcode;
	}
}

void Chip8::handleEvents(const sf::Event & event)
{
}

void Chip8::execute()
{
	if (mKeyWaiting)
		return;
	uint16_t opcode = fetch();

#	define nnn	(opcode & 0x0FFF)
#	define n	(opcode & 0x000F)
#	define x	((opcode >> 8) & 0x000F)
#	define y	((opcode >> 4) & 0x000F)
#	define kk	(opcode & 0x00FF)
#if defined(DEBUG_TRACE_EXECUTION)
	printf("Instruction: %04X\n", opcode);
	printf("nnn: %3X\tx: %1X\ty: %1X\tn: %1X\tkk: %2X\n"
		, nnn
		, x
		, y
		, n
		, kk);
#endif
	switch (opcode & 0xF000)
	{
		case 0x0000:
		{
			switch (kk)
			{
				case 0xE0 /* CLS - Clear the display */:
				{
					for (uint16_t i = 0; i < (64 * 32); i++)
						mGraphics[i] = false;
					mDrawPending = true;
					break;
				}
				case 0xEE /* RET - Return from subroutine */:
				{
					mStackPointer--;
					mProgramCounter = *mStackPointer;
					break;
				}
				default: invalidOperation(); break;
			}
			break;
		}
		case 0x1000 /* JP addr */:
		{
			mProgramCounter = nnn;
			break;
		}
		case 0x2000 /* CALL addr */:
		{
			*mStackPointer = mProgramCounter;
			++mStackPointer;
			mProgramCounter = nnn;
			break;
		}
		case 0x3000 /* SE Vx, byte */:
		{
			if (mRegisters[x] == kk)
				mProgramCounter += 2;
			break;
		}
		case 0x4000 /* SNE Vx, byte */:
		{
			if (mRegisters[x] != kk)
				mProgramCounter += 2;
			break;
		}
		case 0x5000 /* SE Vx, Vy */:
		{
			if (mRegisters[x] == mRegisters[y])
				mProgramCounter += 2;
			break;
		}
		case 0x6000 /* LD Vx, byte */:
		{
			mRegisters[x] = kk;
			break;
		}
		case 0x7000 /* ADD Vx, byte */:
		{
			mRegisters[x] = mRegisters[x] + kk;
			break;
		}
		case 0x8000 /* Arithmitic */:
		{
			switch (n)
			{
				case 0x0 /* LD Vx, Vy */:
				{
					mRegisters[x] = mRegisters[y];
					break;
				}
				case 0x1 /* OR Vx, Vy */:
				{
					mRegisters[x] |= mRegisters[y];
					break;
				}
				case 0x2 /* AND Vx, Vy */:
				{
					mRegisters[x] &= mRegisters[y];
					break;
				}
				case 0x3 /* XOR Vx, Vy */:
				{
					mRegisters[x] ^= mRegisters[y];
					break;
				}
				case 0x4 /* ADD Vx, Vy */:
				{
					uint16_t res = mRegisters[x] + mRegisters[y];
					if (res > 255)
						mRegisters[0xF] = 1;
					else
						mRegisters[0xF] = 0;
					mRegisters[x] = (mRegisters[x] + mRegisters[y]);
					break;
				}
				case 0x5 /* SUB Vx, Vy */:
				{
					if (mRegisters[x] > mRegisters[y])
						mRegisters[0xF] = 1;
					else
						mRegisters[0xF] = 0;
					mRegisters[x] = mRegisters[x] - mRegisters[y];
					break;
				}
				case 0x6 /* SHR Vx */:
				{
					if (mRegisters[x] & 0x1)
						mRegisters[0xF] = 1;
					else
						mRegisters[0xF] = 0;
					mRegisters[x] = (mRegisters[x] >> 1);
					break;
				}
				case 0x7 /* SUBN Vx, Vy */:
				{
					if (mRegisters[y] > mRegisters[x])
						mRegisters[0xF] = 0;
					else
						mRegisters[0xF] = 1;
					mRegisters[x] = mRegisters[y] - mRegisters[x];
					break;
				}
				case 0x8: /* Undocumented */ break;
				case 0x9: /* Undocumented */ break;
				case 0xA: /* Undocumented */ break;
				case 0xB: /* Undocumented */ break;
				case 0xC: /* Undocumented */ break;
				case 0xD: /* Undocumented */ break;
				case 0xE /* SHL Vx */:
				{
					if (mRegisters[x] & 0x80)
						mRegisters[0xF] = 1;
					else
						mRegisters[0xF] = 0;
					mRegisters[x] = (mRegisters[x] << 1);
					break;
				}
				default: invalidOperation(); break;
			}
			break;
		}
		case 0x9000 /* SNE Vx, Vy */:
		{
			if (mRegisters[x] != mRegisters[y])
				mProgramCounter += 2;
			break;
		}
		case 0xA000 /* LD I, addr */:
		{
			mIndexRegister = nnn << 1;
			break;
		}
		case 0xB000 /* JP V0, addr */:
		{
			mProgramCounter = (nnn + mRegisters[0x0]);
			break;
		}
		case 0xC000 /* RND Vx, byte */:
		{
			mRegisters[x] = (rand() % 255) & kk;
			break;
		}
		case 0xD000 /* DRW Vx, Vy, nibble */:
		{
			mRegisters[0xF] = 0;
			uint8_t pixel = 0;
			for (uint8_t yline = 0; yline < n; yline++)
			{
				pixel = mMemory[mIndexRegister + yline];
				for (uint8_t xline = 0; xline < 8; xline++)
				{
					if ((pixel & (0x80 >> xline)) != 0)
					{
						if (mGraphics[x + xline + ((y + yline) * 64)] == true)
						{
							mRegisters[0xF] = 1;
						}
						mGraphics[x + xline + ((y + yline) * 64)] ^= true;
					}
				}
			}
			mDrawPending = true;
			break;
		}
		case 0xE000:
		{
			switch (kk)
			{
				case 0x9E /* SKP Vx */:
				{
					if (mKeys[mRegisters[x]])
						mProgramCounter += 2;
					break;
				}
				case 0xA1 /* SKNP Vx */:
				{
					if (!mKeys[mRegisters[x]])
						mProgramCounter += 2;
					break;
				}
				default: invalidOperation(); break;
			}
			break;
		}
		case 0xF000:
		{
			switch (kk)
			{
				case 0x07 /* LD Vx, DT */:
				{
					mRegisters[x] = mDelayTimer;
					break;
				}
				case 0x0A /* LD Vx, K */:
				{
					mWaitingKey = x;
					mKeyWaiting = true;

					for (uint8_t keys = 0; keys < 16; keys++)
					{
						if (mKeys[keys])
						{
							mRegisters[x] = keys;
							mKeyWaiting = false;
						}
					}
					break;
				}
				case 0x15 /* LD DT, Vx */:
				{
					mDelayTimer = mRegisters[x];
					break;
				}
				case 0x18 /* LD ST, Vx */:
				{
					mSoundTimer = mRegisters[x];
					break;
				}
				case 0x1E /* ADD I, Vx */:
				{
					mIndexRegister = mIndexRegister + mRegisters[x];
					break;
				}
				case 0x29 /* LD F, Vx */:
				{
					mIndexRegister = mRegisters[x] * 0x05;
					break;
				}
				case 0x33 /* LD B, Vx */:
				{
					mMemory[mIndexRegister] = (mRegisters[x] / 100);
					mMemory[mIndexRegister + 1] = (mRegisters[x] / 10) % 10;
					mMemory[mIndexRegister + 2] = (mRegisters[x] % 100) % 10;
					break;
				}
				case 0x55 /* LD Vx, [I] */:
				{
					for (uint16_t i = 0; i < x; ++i)
						mMemory[mIndexRegister + i] = mRegisters[i];
					mIndexRegister += x + 1;
					break;
				}
				case 0x65 /* LD Vx, [I] */:
				{
					for (uint16_t i = 0; i < x; ++i)
						mRegisters[i] = mMemory[mIndexRegister + i];
					mIndexRegister += x + 1;
					break;
				}
			default: invalidOperation(); break;
			}
			break;
		}
		default: invalidOperation(); break;
	}

#undef nnn
#undef x
#undef y
#undef kk
}

Chip8::Chip8()
{
	for (uint8_t i = 0; i < 80; i++)
		mMemory[i] = font_set[i];

	mStackPointer = mStack.data();
	mDrawPending = true;
	srand((uint32_t)time(nullptr));
}


Chip8::~Chip8()
{
}

void Chip8::loadFile(const char * filename)
{
	std::ifstream input(filename, std::ios::ate | std::ios::binary);
	if (!input.is_open())
	{
		printf("Failed to open file '%s'\n", filename);
		std::exit(1);
	}
	size_t size = (size_t)input.tellg();
	input.seekg(std::ios::beg, 0);
	if (size < (4096 - 0x200))
	{
		input.read((char*)&mMemory[0x200], size);
	}
	else
	{
		printf("File too large!\n");
		std::exit(1);
	}
	mProgramSize = (uint16_t)size;
}

const float Chip8::getTimerRate() const
{
	return TIMER_UPDATE_RATE;
}

const float Chip8::getCPURate() const
{
	return CPU_CLOCK_RATE;
}

uint8_t& Chip8::delayTimer()
{
	return mDelayTimer;
}

uint8_t& Chip8::soundTimer()
{
	return mSoundTimer;
}

uint16_t& Chip8::indexRegister()
{
	return mIndexRegister;
}

uint16_t& Chip8::programCounter()
{
	return mProgramCounter;
}

const uint16_t & Chip8::programSize() const
{
	return mProgramSize;
}

std::array<bool, 64 * 32>& Chip8::graphics()
{
	return mGraphics;
}

std::array<uint16_t, 16>& Chip8::stack()
{
	return mStack;
}

uint16_t & Chip8::stackTop()
{
	return *mStackPointer;
}

uint32_t Chip8::stackSize()
{
	return (uint32_t)(mStackPointer - mStack.data());
}

void Chip8::pressKey(int8_t key)
{
	if (key == -1)
		return;

	mKeys[key] = true;

	if (mWaitingKey)
	{
		mRegisters[mWaitingKey] = key;
		mKeyWaiting = false;
	}
}

void Chip8::releaseKey(int8_t key)
{
	if (key == -1)
		return;
	mKeys[key] = false;
}
