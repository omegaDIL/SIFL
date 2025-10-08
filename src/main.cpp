#include <SFML/Graphics.hpp> 
#include <string>
#include <optional>
#include <iostream>
#include "GUI.hpp"

int main()
{
	sf::Vector2u windowSize{ 1920, 1080 };
	sf::RenderWindow window{ sf::VideoMode{ windowSize }, "Template sfml 3" };
	IGUI mainInterface{ &window, 1080 };
	BGUI* curInterface{ &mainInterface };

	IGUI::Item curItem{};
	gui::TextWrapper* text1{ nullptr };
	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::MouseMoved>() && !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				curItem = IGUI::eventUpdateHovered(curInterface, window.mapPixelToCoords(event->getIf<sf::Event::MouseMoved>()->position));

			if (event->is<sf::Event::MouseButtonPressed>() && event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left)
			{
				IGUI::eventPressed(curInterface);
				if (auto info = gui::isMqb(curItem.identifier))
				{
					if (info->identifier == "boxes1")
						gui::checkBox(&mainInterface, info.value(), true, false);
					else // boxes2
						gui::checkBox(&mainInterface, info.value(), false, false);
				}
			}

			if (event->is<sf::Event::Resized>())
				BGUI::windowResized(&window, windowSize);

			if (text1 != nullptr && event->is<sf::Event::TextEntered>())
				if (!gui::updateWritingText(text1, event->getIf<sf::Event::TextEntered>()->unicode, &func))
					text1 = nullptr;

			if (event->is<sf::Event::Closed>())
				window.close();
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			if (curItem.identifier == "slider1")
				gui::moveSlider(&mainInterface, "slider1", window.mapPixelToCoords(sf::Mouse::getPosition(window)).y, 99);
			else if (curItem.identifier == "slider2")
				gui::moveSlider(&mainInterface, "slider2", window.mapPixelToCoords(sf::Mouse::getPosition(window)).y, -1);
		}

		window.clear(sf::Color{ 20, 20, 20 });
		curInterface->draw();
		window.display();
	}

	return 0;
}

//TODO: replace code example of interactive
//TODO: Make functions within GUI to help user