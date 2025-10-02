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


	mainInterface.addDynamicSprite("azerty", "__plainGrey", { 600, 600 }, {20, 20});

	gui::addSlider(&mainInterface, "slider1", { 1460, 540 });

	gui::addMQB(&mainInterface, "mqb1", { 500, 300 }, { 50, 0 }, 8, 0);
	mainInterface.addInteractive("azerty", [](IGUI* a) {a->setWritingText("azerty"); });

	mainInterface.addDynamicText("azerty", "entry", { 500, 400 });
	mainInterface.addInteractive("azerty", [](IGUI* a) {a->setWritingText("azerty"); });
	mainInterface.lockInterface();

	mainInterface.setWritingText("azerty");

	auto test{ gui::isMqb(&mainInterface, "_mqb_mqb1_5") };

	IGUI::Item curItem{};
	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::MouseMoved>() && !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				curItem = IGUI::eventUpdateHovered(curInterface, window.mapPixelToCoords(event->getIf<sf::Event::MouseMoved>()->position));

			if (event->is<sf::Event::MouseButtonPressed>() && event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left)
			{
				IGUI::eventPressed(curInterface);

				auto mqbTest{ gui::isMqb(&mainInterface, curItem.identifier) };
				if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && mqbTest != std::nullopt)
					gui::checkBox(&mainInterface, mqbTest.value(), false, false);
			}

			if (event->is<sf::Event::TextEntered>())
				IGUI::textEntered(curInterface, event->getIf<sf::Event::TextEntered>()->unicode);

			if (event->is<sf::Event::Resized>())
				BGUI::windowResized(&window, windowSize);

			if (event->is<sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
				window.close();
		}

		if (curItem.identifier == "slider1" && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			gui::moveSlider(&mainInterface, "slider1", window.mapPixelToCoords(sf::Mouse::getPosition(window)).y);

		window.clear(sf::Color{ 20, 20, 20 });
		curInterface->draw();
		window.display();
	}

	return 0;
}