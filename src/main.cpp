#include <SFML/Graphics.hpp> 
#include <string>
#include <optional>
#include "GUI.hpp"


int main()
{
	sf::Vector2u windowSize{ 1000, 1000 };
	sf::RenderWindow window{ sf::VideoMode{ windowSize }, "Template sfml 3" };

	// Creates the two interfaces.
	IGUI mainInterface{ &window, 1080 };
	IGUI otherInterface{ &window, 1080 };

	// This is used to switch between interfaces.
	GUIPtr curGui{};
	// This is a flag to know which text is being written to. 
	// You can also use pointer if the interfaces are locked.
	// If there is no text to write in your project, you can remove it.
	std::string writingText{ "text1" }; 
	populateGUI(curGui, writingText, &mainInterface, &otherInterface); // Adds all elements to both interfaces. see the code example. above
	curGui = &mainInterface; // Start with the main interface. The operator= is overriden to allow any interface type.

	while (window.isOpen()) [[likely]]
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) [[unlikely]]
				window.close();
			
			else if (event->is<sf::Event::Resized>()) [[unlikely]]
				BGUI::windowResized(&window, windowSize); // Resizes the window and the interfaces.
			
			else if (event->is<sf::Event::MouseButtonPressed>() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && curGui.gInteractive != nullptr)
				IGUI::eventPressed(curGui); // Handles button pressing (only for IGUI).
						
			else if (event->is<sf::Event::MouseMoved>() && curGui.gInteractive != nullptr && !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) [[likely]]
				curGui.item = IGUI::eventUpdateHovered(curGui, window.mapPixelToCoords(event->getIf<sf::Event::MouseMoved>()->position)); // Updates the hovered item (only for IGUI). The argument is the mouse position (see SFML doc).
			
			else if (event->is<sf::Event::TextEntered>() && curGui.gMutable != nullptr && writingText != "")
				if (!gui::updateWritingText(&mainInterface, writingText, event->getIf<sf::Event::TextEntered>()->unicode, gui::basicWritingFunction))
					writingText = "";
		}


		if (curGui.gInteractive == &otherInterface && curGui.item.identifier == "colorChanger")
			otherInterface.getDynamicSprite("colorChanger")->rotate(sf::degrees(1));
		
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && curGui.gInteractive == &otherInterface && curGui.item.identifier == "slider")
			gui::moveSlider(&otherInterface, curGui.item.identifier, window.mapPixelToCoords(sf::Mouse::getPosition(window)).y, 99);

		// The "general" way is to first check if the current interface is the right one, then check the identifier.

		window.clear();
		curGui->draw();
		window.display();
	}

	return 0;
}