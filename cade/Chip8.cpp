#include "Chip8.hpp"

#include <vector>
#include <bitset>

unsigned char chip8_fonts[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

void Chip8::invalidOperation()
{
	mProgramCounter -= 2;
	printf("Opcode %04X is invalid. Halting execution\n", fetch());
	std::exit(1);
}

Chip8::Chip8()
{
	resetCPU();
	mDrawPending = true;
}


Chip8::~Chip8()
{
}

uint16_t Chip8::fetch()
{
	if (mProgramCounter >= 4096)
	{
		printf("Out of program bounds\n");
		return 0;
	}
	uint16_t buff = 0;
	buff += mMemory[mProgramCounter];
	buff <<= 8;
	buff += mMemory[mProgramCounter + 1];
	mProgramCounter += 2;
	return buff;
}

void Chip8::resetCPU()
{
	srand((uint32_t)time(nullptr));
	mProgramCounter = 0x200;
	mIndexRegister = 0;
	for (size_t m = 0; m < mMemory.size(); m++)
		mMemory[m] = 0;

	mGraphics = std::vector<byte_t>();
	mGraphics.resize(chip8_res_x * chip8_res_y);

	for (size_t m = 0; m < mGraphics.size(); m++)
		mGraphics[m] = false;
	mDelayTimer = 0;
	mSoundTimer = 0;
	for (size_t m = 0; m < mStack.size(); m++)
		mStack[m] = 0;
	mStackTop = mStack.data();

	for (size_t m = 0; m < mRegisters.size(); m++)
		mRegisters[m] = 0;

	for (size_t m = 0; m < mKeys.size(); m++)
		mKeys[m] = false;

	for (int i = 0; i < 80; i++)
		mMemory[i] = chip8_fonts[i];

	mRomLoaded = false;
	mDrawPending = true;
}

void Chip8::loadROM(char * ROMpath)
{
	std::ifstream input(ROMpath, std::ios::ate | std::ios::binary);
	if (!input.is_open())
	{
		printf("Failed to open file '%s'\n", ROMpath);
		return;
	}
	size_t size = (size_t)input.tellg();
	if (size >= (4096 - 0x200))
	{
		printf("ROM size too large for Chip-8.\n");
		return;
	}
	input.seekg(std::ios::beg, 0);
	input.read((char*)&mMemory[0x200], size);
	mRomLoaded = true;
}

void Chip8::execute()
{
	if (mWaitingOnKey || !mRomLoaded)
		return;
	uint16_t opcode = fetch();
#define	nnn	opcode & 0x0FFF
#define w	((opcode & 0xF000) >> 12)
#define x	((opcode & 0x0F00) >> 8)
#define y	((opcode & 0x00F0) >> 4)
#define z	(opcode & 0x000F)
#define kk	(opcode & 0x00FF)

#if defined(DEBUG_TRACE_EXECUTION)
#	define debug_instruction(str) do {	printf("[%03X] %04X: %s ",\
										mProgramCounter - 2,\
										opcode, str); } while (false)
#	define debug_arguments(str, ...) do { printf(str, __VA_ARGS__); } while (false)
#	define debug_memory_access(address, memoryBlock) do { printf("[Memory Access]: [%04d:%04d]\n", address, memoryBlock.size()); } while(false)
#	if defined(DEBUG_TRACE_STATE)
		if (mStackTop > mStack.data())
		{
			printf("Callstack: ");
			for (uint16_t* ptr = mStack.data(); ptr != mStackTop; ptr++)
				printf("[%d]", *ptr);
			printf("\n");
		}
#	endif
#else
#define debug_instruction(str)
#define debug_arguments(arg, ...)
#define debug_memory_access(address, memoryBlock)
#endif
	switch (w)
	{
	case 0x0:
		switch (kk)
		{
		case 0xE0:
			debug_instruction("CLS");
			debug_arguments("\n");
			for (uint16_t i = 0; i < mGraphics.size(); i++)
				 mGraphics[i] = false;
			mDrawPending = true;
			break;
		case 0xEE:
			debug_instruction("RET");
			debug_arguments("\n");
			--mStackTop;
			mProgramCounter = *mStackTop;
			break;
			// SChip-8 -- Highres mode
		case 0xFF:
			mGraphics.resize(schip48_res_x * schip48_res_y);
			break;
		default:
			invalidOperation();
		}
		break;
	case 0x1:
		debug_instruction("JP");
		debug_arguments("%03Xh\n", nnn);
		mProgramCounter = nnn;
		break;
	case 0x2:
		debug_instruction("CALL");
		debug_arguments("%03Xh\n", nnn);
		if (mStackTop == mStack.data() + mStack.size())
		{
			printf("[Warning]: Callstack wraparound!\n");
			mStackTop = mStack.data();
		}
		*mStackTop = mProgramCounter;
		++mStackTop;
		mProgramCounter = nnn;
		break;
	case 0x3:
		debug_instruction("SE");
		debug_arguments("V%1X, %02Xh\n", x, kk);
		if (mRegisters[x] == kk)
			fetch(); // discard
		break;
	case 0x4:
		debug_instruction("SNE");
		debug_arguments("V%1X, %02Xh\n", x, kk);
		if (mRegisters[x] != kk)
			fetch(); // discard
		break;
	case 0x5:
		debug_instruction("SE");
		debug_arguments("V%1X, V%1X\n", x, y);
		if (mRegisters[x] == mRegisters[y])
			fetch(); // discard
		break;
	case 0x6:
		debug_instruction("LD");
		debug_arguments("V%1X, %02Xh\n", x, kk);
		mRegisters[x] = kk;
		break;
	case 0x7:
		debug_instruction("ADD");
		debug_arguments("V%1X, %02Xh\n", x, kk);
		mRegisters[x] = mRegisters[x] + kk;
		break;
	case 0x8:
		switch (z)
		{
		case 0x0:
			debug_instruction("LD");
			debug_arguments("V%1X, V%1X\n", x, y);
			mRegisters[x] = mRegisters[y];
			break;
		case 0x1:
			debug_instruction("OR");
			debug_arguments("V%1X, V%1X\n", x, y);
			mRegisters[x] = mRegisters[x] | mRegisters[y];
			break;
		case 0x2:
			debug_instruction("AND");
			debug_arguments("V%1X, V%1X\n", x, y);
			mRegisters[x] = mRegisters[x] & mRegisters[y];
			break;
		case 0x3:
			debug_instruction("XOR");
			debug_arguments("V%1X, V%1X\n", x, y);
			mRegisters[x] = mRegisters[x] ^ mRegisters[y];
			break;
		case 0x4:
			debug_instruction("ADD");
			debug_arguments("V%1X, V%1X\n", x, y);
			if (mRegisters[y] > (0xFF -  mRegisters[x]))
				mRegisters[0xF] = 1;
			else
				mRegisters[0xF] = 0;
			mRegisters[x] = (mRegisters[x] + mRegisters[y]);
			break;
		case 0x5:
			debug_instruction("SUB");
			debug_arguments("V%1X, V%1X\n", x, y);
			if (mRegisters[x] > mRegisters[y])
				mRegisters[0xF] = 1;
			else
				mRegisters[0xF] = 0;
			mRegisters[x] = (mRegisters[x] - mRegisters[y]);
			break;
		case 0x6:
			debug_instruction("SHR");
			debug_arguments("V%1X {, V%1X}\n", x, y);
			if (mRegisters[x] & 0x1)
				mRegisters[0xF] = 1;
			else
				mRegisters[0xF] = 0;
			mRegisters[x] = mRegisters[x] >> 1;
			break;
		case 0x7:
			debug_instruction("SUBN");
			debug_arguments("V%1X, V%1X\n", x, y);
			if (mRegisters[y] > mRegisters[x])
				mRegisters[0xF] = 1;
			else
				mRegisters[0xF] = 0;
			mRegisters[x] = mRegisters[y] - mRegisters[x];
			break;
		case 0xE:
			debug_instruction("SHL");
			debug_arguments("V%1X {, V%1X}\n", x, y);
			if (mRegisters[x] & 0x80)
				mRegisters[0xF] = 1;
			else
				mRegisters[0xF] = 0;
			mRegisters[x] = mRegisters[x] << 1;
			break;
		default:
			invalidOperation();
		}
		break;
	case 0x9:
		debug_instruction("SNE");
		debug_arguments("V%1X, V%1X\n", x, y);
		if (mRegisters[x] != mRegisters[y])
			fetch();
		break;
	case 0xA:
		debug_instruction("LD");
		debug_arguments("I, %3Xh\n", nnn);
		mIndexRegister = nnn;
		break;
	case 0xB:
		debug_instruction("JP");
		debug_arguments("V0, %3Xh\n", nnn);
		mProgramCounter = nnn + mRegisters[0];
		break;
	case 0xC:
		debug_instruction("RND");
		debug_arguments("V%1X, %2Xh\n", x, kk);
		mRegisters[x] = (rand() % 255) & kk;
		break;
	case 0xD:
	{
		debug_instruction("DRW");
		debug_arguments("V%1X, V%1X, %1Xh\n", x, y, z);
		const uint8_t& X = mRegisters[x], Y = mRegisters[y], HEIGHT = z;
		uint8_t p = 0;
		mRegisters[0xF] = 0;
		for (uint16_t line_y = 0; line_y < HEIGHT; line_y++)
		{
			p = mMemory[mIndexRegister + line_y];
			for (uint16_t line_x = 0; line_x < 8; line_x++)
			{	
				if ((p & (0x80 >> line_x)) != 0)
				{
#define idx ((X + line_x + ((Y + line_y) * 64)) % 2048)
					if (mGraphics[idx] == 1)
						mRegisters[0xF] = 1;
					mGraphics[idx] ^= 1;
#undef idx
				}
			
			}
		}
		mDrawPending = true;
		break;
	}
	case 0xE:
		switch (kk)
		{
		case 0x9E:
			debug_instruction("SKP");
			debug_arguments("V%1X\n", x);
			if (mKeys[mRegisters[x]])
				fetch();
			break;
		case 0xA1:
			debug_instruction("SKNP");
			debug_arguments("V%1X\n", x);
			if (!mKeys[mRegisters[x]])
				fetch();
			break;
		default:
			invalidOperation();
			break;
		}
		break;
	case 0xF:
		switch (kk)
		{
		case 0x07:
			debug_instruction("LD");
			debug_arguments("V%1X, DT\n", x);
			mRegisters[x] = mDelayTimer;
			break;
		case 0x0A:
			debug_instruction("LD");
			debug_arguments("V%1X, K\n", x);
			mWaitingOnKey = true;
			mKeyRegister = x;
			break;
		case 0x15:
			debug_instruction("LD");
			debug_arguments("DT, V%1X\n", x);
			mDelayTimer = mRegisters[x];
			break;
		case 0x18:
			debug_instruction("LD");
			debug_arguments("ST, V%1X\n", x);
			mSoundTimer = mRegisters[x];
			break;
		case 0x1E:
			debug_instruction("ADD");
			debug_arguments("I, V%1X\n", x);
			mIndexRegister = mIndexRegister + mRegisters[x];
			break;
		case 0x29:
			debug_instruction("LD");
			debug_arguments("F, V%1X\n", x);
			mIndexRegister = mRegisters[x] * 5;
			break;
		case 0x33:
			debug_instruction("LD");
			debug_arguments("B, V%1X\n", x);
			mMemory[mIndexRegister] = mRegisters[x] / 100;
			mMemory[mIndexRegister + 1] = (mRegisters[x] / 10) % 10;
			mMemory[mIndexRegister + 2] = (mRegisters[x] % 100) % 10;
			break;
		case 0x55:
			debug_instruction("LD");
			debug_arguments("[I], V%1X\n", x);
			for (uint16_t i = 0; i <= x; i++)
				mMemory[mIndexRegister + i] = mRegisters[i];
			//mIndexRegister = (mIndexRegister + x + 1) & 0x0FFF;
			break;
		case 0x65:
			debug_instruction("LD");
			debug_arguments("V%1X, [I]\n", x);
			for (uint16_t i = 0; i <= x; i++)
				mRegisters[i] = mMemory[mIndexRegister + i];

			//mIndexRegister = (mIndexRegister + x + 1) & 0x0FFF;
			break;
		default:
			invalidOperation();
		}
		break;
	}
}

float & Chip8::getClockSpeed()
{
	return mClockSpeed;
}

std::array<byte_t, 4096>& Chip8::getMemory()
{
	return mMemory;
}

uint16_t & Chip8::getIndexRegister()
{
	return mIndexRegister;
}

uint16_t & Chip8::getProgramCounter()
{
	return mProgramCounter;
}

bool & Chip8::drawPending()
{
	return mDrawPending;
}

std::vector<byte_t> & Chip8::getGraphics()
{
	return mGraphics;
}

uint8_t & Chip8::getDelayTimer()
{
	return mDelayTimer;
}

uint8_t & Chip8::getSoundTimer()
{
	return mSoundTimer;
}

std::array<uint16_t, 16>& Chip8::getStack()
{
	return mStack;
}

uint16_t *& Chip8::getStackTop()
{
	return mStackTop;
}

void Chip8::setKey(int8_t key)
{
	if (mWaitingOnKey && mKeyRegister != -1)
	{
		mRegisters[mKeyRegister] = key;
		mWaitingOnKey = false;
	}
	mKeys[key] = true;
}

void Chip8::releaseKey(int8_t key)
{
	mKeys[key] = false;
}

std::array<bool, 16>& Chip8::getKeys()
{
	return mKeys;
}

std::array<uint8_t, 16>& Chip8::getRegisters()
{
	return mRegisters;
}
