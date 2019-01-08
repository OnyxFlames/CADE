#include "Application.hpp"

#include "Debug.hpp"

#include <sstream>

void Application::setRenderingMode(RenderingMode mode)
{
	switch (mode)
	{
	case RenderingMode::Classic:
		pixel.setSize({ 16.f, 16.f });
		pixel.setScale(1.0f, 1.0f);
		pixel.setFillColor(pixel_color);
		break;
	case RenderingMode::HighRes:
		pixel.setSize({ 16.f, 16.f });
		pixel.setScale(0.935f, 0.935f);
		pixel.setFillColor(pixel_color);
		break;
	case RenderingMode::ColourClassic:
		pixel.setSize({ 16.f, 16.f });
		pixel.setScale(1.0f, 1.0f);
		break;
	case RenderingMode::ColourHighRes:
		pixel.setSize({ 16.f, 16.f });
		pixel.setScale(0.935f, 0.935f);
		break;
	}
	chip8.drawPending() = true;
}

Application::Application()
	:	window(sf::VideoMode(64 * 16, 32 * 16), "CADE - Chip-8 Emulator")
{
	debug_font.loadFromFile("../fonts/consola.ttf");
	debug_text.setFont(debug_font);
	debug_text.setFillColor(sf::Color::Green);
	debug_text.setOutlineColor(sf::Color::Black);
	debug_text.setOutlineThickness(2.f);
	debug_text.setScale({ .50f, .50f });

	debug_keys_active = debug_text;
	debug_keys_active.setPosition(0, 20.f);
	debug_keys_active.setString("Implement me!\n");

	debug_instruction = debug_text;

	printf("F1 to restart\nShift + Escape to exit.\n");

	setRenderingMode(current_mode);

	render_buffer.create(64 * 16, 32 * 16);
	buffer_sprite.setTexture(render_buffer.getTexture());
}


Application::~Application()
{

}

void Application::loadFile(char * filename)
{
	chip8.loadFile(filename);
}


static int8_t sfml_to_chip8(sf::Keyboard::Key key)
{
	switch (key)
	{
	case sf::Keyboard::Num1: return 0x1;
	case sf::Keyboard::Num2: return 0x2;
	case sf::Keyboard::Num3: return 0x3;
	case sf::Keyboard::Num4: return 0xC;
	case sf::Keyboard::Q: return 0x4;
	case sf::Keyboard::W: return 0x5;
	case sf::Keyboard::E: return 0x6;
	case sf::Keyboard::R: return 0xD;
	case sf::Keyboard::A: return 0x7;
	case sf::Keyboard::S: return 0x8;
	case sf::Keyboard::D: return 0x9;
	case sf::Keyboard::F: return 0xE;
	case sf::Keyboard::Z: return 0xA;
	case sf::Keyboard::X: return 0x0;
	case sf::Keyboard::C: return 0xB;
	case sf::Keyboard::V: return 0xF;
	default: return -1;
	}
}


void Application::process_events()
{
	sf::Event e;
	while (window.pollEvent(e))
	{
		//chip8.handleEvents(e);
		switch (e.type)
		{
		case sf::Event::Closed:
			window.close();
			break;
		case sf::Event::KeyPressed:
			switch (e.key.code)
			{
			case sf::Keyboard::Escape:
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
					window.close();
				break;
			case sf::Keyboard::F1:
				chip8.resetCPU();
				break;
			case sf::Keyboard::BackSpace:
				if (current_mode == Classic)
					current_mode = (RenderingMode)(ModeCount - 1);
				else
					current_mode = (RenderingMode)(current_mode - 1);
				setRenderingMode(current_mode);
				break;
			case sf::Keyboard::Enter:
				if (current_mode == (RenderingMode)(ModeCount - 1))
					current_mode = Classic;
				else
					current_mode = (RenderingMode)(current_mode + 1);
				setRenderingMode(current_mode);
				break;
				default:
				{
					if (sfml_to_chip8(e.key.code) != -1 && sfml_to_chip8(e.key.code) <= 0x0F)
						chip8.pressKey(sfml_to_chip8(e.key.code));
				}
			}
		case sf::Event::KeyReleased:
			//if (sfml_to_chip8(e.key.code) != -1 && sfml_to_chip8(e.key.code) <= 0x0F)
				//chip8.pressKey(sfml_to_chip8(e.key.code));
			break;
		}
	}
}

void Application::update(sf::Time dt)
{
	static uint64_t frame_rate = 0;
	elapsed_timer += dt;
	update_timer += dt;
	frametime += dt;
	// timer tick rate
	if (elapsed_timer >= sf::seconds(chip8.getTimerRate()))
	{
		if (chip8.delayTimer() > 0)
			chip8.delayTimer()--;
		if (chip8.soundTimer() > 0)
			chip8.soundTimer()--;
		elapsed_timer = sf::Time::Zero;
	}
	// execution rate
	if (update_timer >= sf::seconds(chip8.getCPURate()))
	{
		chip8.execute();
		update_timer = sf::Time::Zero;
	}
	++frame_rate;
	if (frametime.asSeconds() >= 1.f)
	{
		debug_text.setString(std::to_string(frame_rate));
		frame_rate = 0;
		frametime = sf::Time::Zero;
	}

	std::stringstream ss;

	ss << "I: " << chip8.indexRegister() << "\n";
	ss << "PC: " << chip8.programCounter() << " - " << std::hex << std::setw(4) << std::setfill('0');
	chip8.programCounter() -= 2;
	ss << chip8.fetch() << std::dec << "\n";
	ss << "DT: " << (uint16_t)chip8.delayTimer() << "\n";
	ss << "ST: " << (uint16_t)chip8.soundTimer() << "\n";
	ss << "SP: " << chip8.stackSize() << "\n";
	ss << "Program Size: " << chip8.programSize() << "\n";
	ss << "Awaiting key: " << (chip8.keyWaiting() ? "true" : "false") << "\n";
	ss << "Draw Queue: " << (chip8.drawPending() ? "true" : "false") << "\n";

	debug_keys_active.setString(ss.str());

}
static sf::Color randomColor()
{
	sf::Color options[] = 
	{
		sf::Color::Red - sf::Color(100, 0, 0, 0),
		sf::Color::Green - sf::Color(0, 100, 0, 0),
		sf::Color::Blue - sf::Color(0, 0, 100, 0),
	};
	return options[rand() % 3];
}

void Application::render()
{
	if (chip8.drawPending())
	{
		render_buffer.clear();
		for (uint16_t y_line = 0; y_line < 32; y_line++)
		{
			for (uint16_t x_line = 0; x_line < 64; x_line++)
			{
				auto gfx = chip8.graphics();
				if (gfx[(x_line + (y_line * 64))] == 1)
				{
					pixel.setPosition(x_line * 16.f, y_line * 16.f);
					if (current_mode == RenderingMode::ColourClassic || current_mode == RenderingMode::ColourHighRes)
						pixel.setFillColor(randomColor());
					render_buffer.draw(pixel);
				}
			}
		}
		render_buffer.display();
		chip8.drawPending() = false;
	}
	window.clear();
	window.draw(buffer_sprite);
	window.draw(debug_text);
	window.draw(debug_keys_active);
	window.display();
}

void Application::run()
{
	sf::Clock runtime_clock;
	while (window.isOpen())
	{
		process_events();
		update(runtime_clock.restart());
		render();
	}
}
