#include "MemoryViewer.hpp"


void MemoryViewer::process_events()
{
	sf::Event e;
	while (mWindow.pollEvent(e))
	{
		switch (e.type)
		{
		case sf::Event::Closed:
			mWindow.close();
			break;
		case sf::Event::KeyPressed:
			switch (e.key.code)
			{
			case sf::Keyboard::Up:
				mTexts.move({ 0.f, -10.f});
				break;
			case sf::Keyboard::Down:
				mTexts.move({ 0.f, 10.f});
				break;
			}
		}
	}
}

void MemoryViewer::update(sf::Time dt)
{
	std::stringstream ss;
	ss << std::hex << std::setw(3) << std::setfill('0');
	uint16_t c = 0;
	for (size_t i = 0x200; i < mMemory->size(); i += 8)
	{
		ss << i << ": " << std::setw(2) << std::setfill('0') <<
			(uint16_t)mMemory->at(i) << " " << std::setw(2) << std::setfill('0') <<
			(uint16_t)mMemory->at(i + 1) << " " << std::setw(2) << std::setfill('0') <<
			(uint16_t)mMemory->at(i + 2) << " " << std::setw(2) << std::setfill('0') <<
			(uint16_t)mMemory->at(i + 3) << " " << std::setw(2) << std::setfill('0') <<
			(uint16_t)mMemory->at(i + 4) << " " << std::setw(2) << std::setfill('0') <<
			(uint16_t)mMemory->at(i + 5) << " " << std::setw(2) << std::setfill('0') <<
			(uint16_t)mMemory->at(i + 6) << " " << std::setw(2) << std::setfill('0') <<
			(uint16_t)mMemory->at(i + 7) << " \n";
	}
	ss << std::dec;
	mTexts.setString(ss.str());
}

void MemoryViewer::render()
{
	mWindow.clear();
	mWindow.draw(mTexts);
	mWindow.display();
}

MemoryViewer::MemoryViewer(std::array<byte_t, 4096>& memory)
	: mMemory(&memory)
	, mWindow(sf::VideoMode(640, 720), "Memory")
{
	mFont.loadFromFile("../resources/fonts/consola.ttf");
	mTexts.setFont(mFont);
	mTexts.setScale(.5f, .5f);
}


MemoryViewer::~MemoryViewer()
{
}

void MemoryViewer::run()
{
}
