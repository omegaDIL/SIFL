#include "GUI.hpp"

void showErrorsUsingWindow(const std::string& errorTitle, const std::ostringstream& errorMessage, unsigned int characterSize) noexcept
{
	sf::Vector2u windowSize{ sf::Vector2u{ 720, 720 } };
	sf::RenderWindow window{ sf::VideoMode{ windowSize }, errorTitle };
	gui::BasicInterface gui{ &window }; // Create the interface to use the GUI.

	gui.addText(errorMessage.str(), sf::Vector2f{ 360, 260 }, characterSize);
	gui.addText("ok I understand - press any key", sf::Vector2f{ 360, 600 });

	while (window.isOpen()) // The function is blocking.
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>()
				|| event->is<sf::Event::KeyPressed>()
				|| event->is<sf::Event::TouchBegan>())
				window.close();

			if (event->is<sf::Event::Resized>())
				gui::BasicInterface::windowResized(&window, windowSize);
		}

		window.clear();
		gui.draw();
		window.display();
	}
}

constexpr GUIPtr& GUIPtr::operator=(std::nullptr_t) noexcept
{
	gBasic = nullptr;
	gMutable = nullptr;
	gInteractive = nullptr;

	return *this;
}

constexpr GUIPtr& GUIPtr::operator=(gui::BasicInterface* ptr) noexcept
{
	gBasic = ptr;
	gMutable = nullptr;
	gInteractive = nullptr;

	return *this;
}

constexpr GUIPtr& GUIPtr::operator=(gui::MutableInterface* ptr) noexcept
{
	gBasic = ptr;
	gMutable = ptr;
	gInteractive = nullptr;

	return *this;
}

constexpr GUIPtr& GUIPtr::operator=(gui::InteractiveInterface* ptr) noexcept
{
	gBasic = ptr;
	gMutable = ptr;
	gInteractive = ptr;

	return *this;
}

void populateGUI(GUIPtr& cur, IGUI* main, IGUI* settings, MGUI* overlay, sf::RenderWindow* window, sf::ContextSettings* context) noexcept
{
	ENSURE_VALID_PTR(main, "main was nullptr when populateGUI was called");

	main->addDynamicText("play", "play", { 500, 400 });
	main->addInteractive("play");
	main->addDynamicText("settings", "settings", { 500, 600 });
	main->addInteractive("settings", [&cur, settings](IGUI*) mutable {cur = settings; });
	main->addDynamicText("close", "close", { 500, 800 });
	main->addInteractive("close", [window](IGUI*) { window->close(); });
	main->addText("My Awesome Game\n(GUI example)", sf::Vector2f{ 500, 150 }, 48);

	settings->addDynamicText("back", "back", sf::Vector2f{ 500, 200 });
	settings->addInteractive("back", [&cur, main](IGUI*) mutable {cur = main; });
	settings->addDynamicText("fs", "fullscreen", { 700, 400 });
	settings->addInteractive("fs", [window, context](IGUI*) 
	{
		sf::Vector2u prevSize{ window->getSize() };	
		window->create(sf::VideoMode::getDesktopMode(), "Template sfml 3", sf::State::Fullscreen, *context);
		prevSize;
	});
	settings->addDynamicText("wd", "windowed", { 700, 600 });
	settings->addInteractive("wd", [window, context](IGUI*)
	{
		window->create(sf::VideoMode{ sf::Vector2u{ 1000, 1000 } }, "Template sfml 3", sf::State::Windowed, *context);
	});
	gui::addSlider(settings, "aliasing", { 300, 500 });

	sf::CircleShape overlayTex{ 20, 120 };
	overlayTex.setFillColor(sf::Color{ 255, 255, 255, 80 });
	overlay->addDynamicSprite("overlay", gui::createTextureFromDrawables(overlayTex), sf::Vector2f{ 0, 0 });
}