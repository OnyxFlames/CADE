#pragma once

#include <SFML/Graphics.hpp>

#include "Common.hpp"

class MemoryViewer
{
private:
	sf::Font mFont;
	sf::Text mTexts;
	bool mPaused = false;
	sf::RenderWindow mWindow;

	std::array<byte_t, 4096>* mMemory;
public:
	MemoryViewer(std::array<byte_t, 4096>& memory);
	~MemoryViewer();
	void run();
	void process_events();
	void update(sf::Time dt);
	void render();
};

