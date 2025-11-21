#include "GUI.hpp"

void showErrorsUsingWindow(const std::string& errorTitle, const std::ostringstream& errorMessage, unsigned int characterSize) noexcept
{
	sf::RenderWindow window{ sf::VideoMode{ { 720, 720 } }, errorTitle };
	sf::View view{ window.getView() };
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
				gui::BasicInterface::windowResized(&window, view);
		}

		window.clear();
		gui.draw();
		window.display();
	}
}

void populateGUI(GUIPtr& cur, IGUI* main, IGUI* settings, MGUI* overlay, sf::RenderWindow* window, sf::ContextSettings& context, sf::View& currentView, sf::State& currentState) noexcept
{
	ENSURE_VALID_PTR(main, "main was nullptr when populateGUI was called");

	sf::Vector2f windowSize{ static_cast<sf::Vector2u>(window->getSize()) };

	main->addDynamicText("play", "play", sf::Vector2f{ windowSize.x / 2.f, windowSize.y / 2.5f });
	main->addInteractive("play");
	main->addDynamicText("settings", "settings", { 500, 600 });
	main->addInteractive("settings", [&cur, settings](IGUI*) mutable {cur = settings; });
	main->addDynamicText("close", "close", { 500, 800 });
	main->addInteractive("close", [window](IGUI*) { window->close(); });
	main->addText("My Awesome Game\n(GUI example)", sf::Vector2f{ 500, 150 }, 48);

	settings->addDynamicText("back", "back", sf::Vector2f{ 500, 200 });
	settings->addInteractive("back", [&cur, main](IGUI*) mutable { cur = main; });
	settings->addDynamicText("fs", "fullscreen", { 700, 400 });
	settings->addInteractive("fs", [&currentView, &currentState, context, window](IGUI*) mutable 
	{
		currentState = (currentState == sf::State::Fullscreen) ? sf::State::Windowed : sf::State::Fullscreen;
		window->create(sf::VideoMode::getDesktopMode(), "Template sfml 3", currentState, context); // Recreate the window with new settings (fullscreen).
		BGUI::windowResized(window, currentView); // create would not trigger a resize event (even if we still resize).	
	});
	gui::addSlider(settings, "aliasing", { 300, 500 });

	sf::CircleShape overlayTex{ 20, 120 };
	overlayTex.setFillColor(sf::Color{ 255, 255, 255, 80 });
	overlay->addDynamicSprite("overlay", gui::createTextureFromDrawables(overlayTex), sf::Vector2f{ 0, 0 });
}