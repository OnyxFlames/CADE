#include "Application.hpp"


void Application::setRenderingMode(RenderMode mode)
{
	mRenderingMode = mode;
	switch (mRenderingMode)
	{
	case RenderMode::Classic:
		mPixel.setSize({ 16.f, 16.f });
		mPixel.setFillColor(sf::Color::White);
		mRenderSprite.setColor(sf::Color::White);
		mPixel.setScale({ 1.f, 1.f });
		break;
	case RenderMode::ClassicRed:
		mPixel.setSize({ 16.f, 16.f });
		mPixel.setFillColor(sf::Color::White);
		mRenderSprite.setColor(sf::Color::Red);
		mPixel.setScale({ 1.f, 1.f });
		break;
	case RenderMode::ClassicGreen:
		mPixel.setSize({ 16.f, 16.f });
		mPixel.setFillColor(sf::Color::White);
		mRenderSprite.setColor(sf::Color::Green);
		mPixel.setScale({ 1.f, 1.f });
		break;
	case RenderMode::ClassicBlue:
		mPixel.setSize({ 16.f, 16.f });
		mPixel.setFillColor(sf::Color::White);
		mRenderSprite.setColor(sf::Color::Cyan);
		mPixel.setScale({ 1.f, 1.f });
		break;
	case RenderMode::HighRes:
		mPixel.setSize({ 16.f, 16.f });
		mPixel.setFillColor(sf::Color::White);
		mRenderSprite.setColor(sf::Color::White);
		mPixel.setScale({0.9f, 0.9f});
		break;
	case RenderMode::HighResRed:
		mPixel.setSize({ 16.f, 16.f });
		mPixel.setFillColor(sf::Color::White);
		mRenderSprite.setColor(sf::Color::Red);
		mPixel.setScale({ 0.9f, 0.9f });
		break;
	case RenderMode::HighResGreen:
		mPixel.setSize({ 16.f, 16.f });
		mPixel.setFillColor(sf::Color::White);
		mRenderSprite.setColor(sf::Color::Green);
		mPixel.setScale({ 0.9f, 0.9f });
		break;
	case RenderMode::HighResBlue:
		mPixel.setSize({ 16.f, 16.f });
		mPixel.setFillColor(sf::Color::White);
		mRenderSprite.setColor(sf::Color::Cyan);
		mPixel.setScale({ 0.9f, 0.9f });
		break;
	}
	chip.drawPending() = true;
	mPixel.setPosition(-16.f, -16.f);
}

Application::Application()
	: mWindow(sf::VideoMode(64 * 16, 32 * 16), "CADE - Chip-8 Emulator")
	, mRenderingMode(Classic)
//	, mv(chip.getMemory())
{
	mRenderBuffer.create(64 * 16, 32 * 16);
	mRenderSprite.setTexture(mRenderBuffer.getTexture());
	mRenderBuffer.setSmooth(true);

	setRenderingMode(mRenderingMode);
#if defined(DEBUG_HUD)
	mFont.loadFromFile("../resources/fonts/consola.ttf");
	mTexts.setFont(mFont);
	mTexts.setScale(0.55f, 0.55f);
	mTexts.setFillColor(sf::Color::Green);
	mTexts.setOutlineColor(sf::Color::Black);
	mTexts.setOutlineThickness(2.0f);
#endif
	printf("Escape to pause.\nShift + Escape to exit.\nF1 to reset.\nF2 to load ROM.\nPage Up to swap render modes\n");
}


Application::~Application()
{
}

Chip8 & Application::getCPU()
{
	return chip;
}


void Application::process_events()
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
			case sf::Keyboard::Escape:
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
					mWindow.close();
				else
					mPaused = !mPaused;
				break;
			case sf::Keyboard::F1:
				chip.resetCPU();
				break;
			case sf::Keyboard::F2:
#if defined(PLATFORM_WINDOWS)
			{
				chip.resetCPU();
				TCHAR fileBuff[260] = { 0 };
				OPENFILENAMEA ofn;
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = NULL;
				ofn.lpstrFile = fileBuff;
				ofn.nMaxFile = sizeof(fileBuff);
				if (GetOpenFileNameA(&ofn) == 0)
					printf("Failed to load ROM '%s'\n", ofn.lpstrFile);
				else
				{
					// Just the rom name
					printf("Loading ROM '%s'\n", ofn.lpstrFile);
					// Need the full path
					chip.loadROM(ofn.lpstrFile);
				}
			}
#endif
				break;
			case sf::Keyboard::PageUp:
				if (mRenderingMode + 1 == RenderMode::RenderModeCount)
					setRenderingMode(RenderMode::Classic);
				else
					setRenderingMode((RenderMode)(mRenderingMode + 1));
				break;
			case sf::Keyboard::Enter:
				if (mPaused)
					chip.execute();
				break;
			case sf::Keyboard::Num1:
				chip.setKey(0x1);
				break;
			case sf::Keyboard::Num2:
				chip.setKey(0x2);
				break;
			case sf::Keyboard::Num3:
				chip.setKey(0x3);
				break;
			case sf::Keyboard::Num4:
				chip.setKey(0xC);
				break;
			case sf::Keyboard::Q:
				chip.setKey(0x4);
				break;
			case sf::Keyboard::W:
				chip.setKey(0x5);
				break;
			case sf::Keyboard::E:
				chip.setKey(0x6);
				break;
			case sf::Keyboard::R:
				chip.setKey(0xD);
				break;
			case sf::Keyboard::A:
				chip.setKey(0x7);
				break;
			case sf::Keyboard::S:
				chip.setKey(0x8);
				break;
			case sf::Keyboard::D:
				chip.setKey(0x9);
				break;
			case sf::Keyboard::F:
				chip.setKey(0xE);
				break;
			case sf::Keyboard::Z:
				chip.setKey(0xA);
				break;
			case sf::Keyboard::X:
				chip.setKey(0x0);
				break;
			case sf::Keyboard::C:
				chip.setKey(0xB);
				break;
			case sf::Keyboard::V:
				chip.setKey(0xF);
				break;
			}
			break;
		case sf::Event::KeyReleased:
			switch (e.key.code)
			{
			case sf::Keyboard::Num1:
				chip.releaseKey(0x1);
				break;
			case sf::Keyboard::Num2:
				chip.releaseKey(0x2);
				break;
			case sf::Keyboard::Num3:
				chip.releaseKey(0x3);
				break;
			case sf::Keyboard::Num4:
				chip.releaseKey(0xC);
				break;
			case sf::Keyboard::Q:
				chip.releaseKey(0x4);
				break;
			case sf::Keyboard::W:
				chip.releaseKey(0x5);
				break;
			case sf::Keyboard::E:
				chip.releaseKey(0x6);
				break;
			case sf::Keyboard::R:
				chip.releaseKey(0xD);
				break;
			case sf::Keyboard::A:
				chip.releaseKey(0x7);
				break;
			case sf::Keyboard::S:
				chip.releaseKey(0x8);
				break;
			case sf::Keyboard::D:
				chip.releaseKey(0x9);
				break;
			case sf::Keyboard::F:
				chip.releaseKey(0xE);
				break;
			case sf::Keyboard::Z:
				chip.releaseKey(0xA);
				break;
			case sf::Keyboard::X:
				chip.releaseKey(0x0);
				break;
			case sf::Keyboard::C:
				chip.releaseKey(0xB);
				break;
			case sf::Keyboard::V:
				chip.releaseKey(0xF);
				break;
			}
		}
	}
	//mv.process_events();
}

void Application::update(sf::Time dt)
{
#if defined(DEBUG_HUD)
	// update debug stuff
	std::stringstream ss;

	ss << "PC: " << chip.getProgramCounter() << "\t";
	ss << "I: " << chip.getIndexRegister() << "\t";
	ss << "DT: " << (uint16_t)chip.getDelayTimer() << "\t";
	ss << "ST: " << (uint16_t)chip.getSoundTimer() << "\t";
	ss << "SP: " << (uint16_t)(chip.getStackTop() - chip.getStack().data()) << "\n";
	//ss << "Needs key: " << (chip.mWaitingOnKey ? "true" : "false") << "\n";

	for (uint8_t j = 0; j < 16; j++)
	{
		ss << "V" << std::hex << std::setw(1) << (uint16_t)j << "(" <<
			std::setw(2) << std::setfill('0') << (uint16_t)chip.getRegisters()[j] << ")\n";
	}

	for (uint8_t i = 0; i < 16; i++)
		if (chip.getKeys()[i])
			ss << "Key: " << std::hex << (uint16_t)i << std::dec << "\n";

	mTexts.setString(ss.str());
#endif

	if (mPaused)
		return;

	static sf::Time elapsed_cpu = sf::Time::Zero, elapsed_timer = sf::Time::Zero;
	elapsed_cpu += dt;
	elapsed_timer += dt;
	if (elapsed_cpu.asSeconds() > (1.f / chip.getClockSpeed()))
	{
		chip.execute();
		elapsed_cpu = sf::Time::Zero;
	}
	if (elapsed_timer.asSeconds() > (1.f / 60.f))
	{
		if (chip.getDelayTimer() > 0)
			chip.getDelayTimer()--;
		if (chip.getSoundTimer() > 0)
			chip.getSoundTimer()--;
		elapsed_timer = sf::Time::Zero;
	}
	//mv.update(dt);
}

void Application::render()
{
	if (chip.drawPending())
	{
		mRenderBuffer.clear(sf::Color(64, 64, 64));
		for (int16_t y_line = 0; y_line < 32; y_line++)
		{
			for (int16_t x_line = 0; x_line < 64; x_line++)
			{
				if (chip.getGraphics()[(x_line + (y_line * 64))] == 1)
				{
					mPixel.setPosition(x_line * 16.f, y_line * 16.f);
					mRenderBuffer.draw(mPixel);
				}
			}
		}
		mRenderBuffer.display();
		chip.drawPending() = false;
	}
	mWindow.clear();
	mWindow.draw(mRenderSprite);
	mWindow.draw(mTexts);
	if (mPaused)
	{
		sf::RectangleShape paused_shape;
		paused_shape.setSize({ (float)mWindow.getSize().x, (float)mWindow.getSize().y });
		paused_shape.setFillColor(sf::Color(0, 0,0, 192));
		mWindow.draw(paused_shape);
	}
	mWindow.display();
	//mv.render();
}

void Application::run()
{
	sf::Clock runtime_clock;
	while (mWindow.isOpen())
	{
		process_events();
		update(runtime_clock.restart());
		render();
	}
}
