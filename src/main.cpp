#include <SFML/Graphics.hpp> 
#include <string>
#include <optional>
#include <iostream>
#include "GUI.hpp"

int main()
{
	sf::Vector2u windowSize{ 1000, 1000 };
	sf::RenderWindow window{ sf::VideoMode{ windowSize }, "Template sfml 3" };
	currentGUI curGui{};
	std::string writingText{ "text1" };

	IGUI mainInterface{ &window, 1080 };
	IGUI otherInterface{ &window, 1080 };
	populateGUI(curGui, writingText, &mainInterface, &otherInterface);
	curGui = &mainInterface;

	while (window.isOpen()) [[likely]]
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) [[unlikely]]
				window.close();

			else if (event->is<sf::Event::Resized>()) [[unlikely]]
				BGUI::windowResized(&window, windowSize);
			
			else if (event->is<sf::Event::MouseButtonPressed>() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && curGui.gInteractive != nullptr)
				IGUI::eventPressed(curGui);
						
			else if (event->is<sf::Event::MouseMoved>() && curGui.gInteractive != nullptr && !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) [[likely]]
				curGui.item = IGUI::eventUpdateHovered(curGui, window.mapPixelToCoords(event->getIf<sf::Event::MouseMoved>()->position));
		
			else if (event->is<sf::Event::TextEntered>() && curGui.gMutable != nullptr && writingText != "")
				if (!gui::updateWritingText(&mainInterface, writingText, event->getIf<sf::Event::TextEntered>()->unicode, gui::basicWritingFunction))
					writingText = "";
		}

		//////////////////////////////////////Actual code////////////////////////////////////////////////
		if (curGui.gInteractive == &otherInterface && curGui.item.identifier == "colorChanger")
			otherInterface.getDynamicSprite("colorChanger")->rotate(sf::degrees(1));
		
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && curGui.gInteractive == &otherInterface && curGui.item.identifier == "slider")
			gui::moveSlider(&otherInterface, curGui.item.identifier, window.mapPixelToCoords(sf::Mouse::getPosition(window)).y, 99);
		//////////////////////////////////////Actual code////////////////////////////////////////////////

		window.clear();
		curGui->draw();
		window.display();
	}

	return 0;
}