#pragma once
#include <SFML/Graphics.hpp>

#include "Chip8.hpp"

class Application
{
private:
	//MemoryViewer mv;
	sf::Font mFont;
	sf::Text mTexts;
	bool mPaused = false;
	Chip8 chip;
	sf::RenderWindow mWindow;
	sf::Sprite mRenderSprite;
	sf::RenderTexture mRenderBuffer;
	sf::RectangleShape mPixel;

	void process_events();
	void update(sf::Time dt);
	void render();
public:
	enum RenderMode
	{
		Classic,
		ClassicRed,
		ClassicGreen,
		ClassicBlue,
		HighRes,
		HighResRed,
		HighResGreen,
		HighResBlue,

		RenderModeCount,
	} mRenderingMode;
	void setRenderingMode(RenderMode mode);
	Application();
	~Application();
	Chip8& getCPU();
	void run();
};

