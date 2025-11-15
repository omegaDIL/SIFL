#include <SFML/Graphics.hpp> 
#include <string>
#include <optional>
#include "GUI.hpp"


int main()
{
	sf::Vector2u windowSize{ 1000, 1000 };
	sf::ContextSettings settings{};
	settings.antiAliasingLevel = 64; 
	sf::RenderWindow window{ sf::VideoMode{ windowSize }, "Template sfml 3", sf::State{}, settings };
	
	// Creates interfaces.
	IGUI mainInterface{ &window, 1080 };
	IGUI otherInterface{ &window, 1080 };
	MGUI overlayInterface{ &window, 1080 }; // An overlay interface that will always be drawn. Not mean to be the main one

	// This is used to get information about the currently hovered item.
	GUIPtr curGUI{};
	curGUI = &mainInterface; // Start with the main interface. The operator= is overriden to allow any interface type.
	std::string writingText{ "text1" }; // Identifier of which text is being written to.

	// Populates both interfaces.
	populateGUI(curGUI, writingText, &mainInterface, &otherInterface, &overlayInterface); // Adds all elements to both interfaces. see the code example. above
	mainInterface.lockInterface(); 
	otherInterface.lockInterface();
	overlayInterface.lockInterface();

	while (window.isOpen()) [[likely]]
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) [[unlikely]]
				window.close();

			else if (event->is<sf::Event::Resized>()) [[unlikely]]
				BGUI::windowResized(&window, windowSize); // Resizes the window and the interfaces.

			else if (curGUI.gInteractive != nullptr && event->is<sf::Event::MouseButtonPressed>() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				curGUI.gInteractive->eventPressed(); // Handles button pressing (only for IGUI).

			else if (curGUI.gInteractive != nullptr && event->is<sf::Event::MouseMoved>() && !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) [[likely]]
			{
				curGUI.mainItem = curGUI.gInteractive->eventUpdateHovered(window.mapPixelToCoords(event->getIf<sf::Event::MouseMoved>()->position)); // Updates the hovered item (only for IGUI). The argument is the mouse position (see SFML doc).
				overlayInterface.getDynamicSprite("overlay")->setPosition(window.mapPixelToCoords(event->getIf<sf::Event::MouseMoved>()->position)); // Moves the overlay sprite to follow the mouse.
			}
		
			else if (curGUI.gMutable != nullptr && event->is<sf::Event::TextEntered>() && writingText != "")
				if (!gui::updateWritingText(&mainInterface, writingText, event->getIf<sf::Event::TextEntered>()->unicode, gui::basicWritingFunction))
					writingText = "";
		}


		// First check if we are in the right interface, then check the identifier.

		if (curGUI.gInteractive == &otherInterface && curGUI.mainItem.identifier == "colorChanger") 
			otherInterface.getDynamicSprite("colorChanger")->rotate(sf::degrees(1));
		
		if (curGUI.gInteractive == &otherInterface && curGUI.mainItem.identifier == "slider" && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			gui::moveSlider(&otherInterface, curGUI.mainItem.identifier, window.mapPixelToCoords(sf::Mouse::getPosition(window)).y, 99); // 99 is good value for a slider: the delta interval is exactly 0.01


		window.clear(sf::Color{ 20, 20, 20 });
		curGUI->draw();
		overlayInterface.draw(); // Whatever the current interface is, its elements will always be drawn.
		window.display();
	}

	return 0;
}