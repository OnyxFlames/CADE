#pragma once
#include <SFML/Graphics.hpp>

#include "Common.hpp"

#include "Chip8.hpp"

/*
	Emulator class
		Includes rendering and input handling via SFML
*/

class Application
{
private:
	sf::Font debug_font;
	sf::Text debug_text, debug_keys_active, debug_instruction;
	// Pixel data
	sf::RectangleShape pixel;
	const sf::Color pixel_color = sf::Color(255, 255, 255, 255);
	// ...
	sf::Sprite buffer_sprite;
	sf::RenderTexture render_buffer;
	sf::RenderWindow window;
	void process_events();
	void update(sf::Time dt);
	void render();

	// update stuff
	sf::Time
		elapsed_timer = sf::Time::Zero,
		update_timer = sf::Time::Zero,
		frametime = sf::Time::Zero;

private /* Data */:
	Chip8 chip8;
public:
	enum RenderingMode : uint8_t
	{
		Classic,
		HighRes,
		ColourClassic,
		ColourHighRes,
		ModeCount,
	} current_mode = HighRes;
	void setRenderingMode(RenderingMode mode);
	Application();
	~Application();
	void loadFile(char* filename);
	void run();
};

