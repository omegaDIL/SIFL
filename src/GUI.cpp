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

	item = gui::InteractiveInterface::resetHovered();

	return *this;
}

constexpr GUIPtr& GUIPtr::operator=(gui::BasicInterface* ptr) noexcept
{
	gBasic = ptr;
	gMutable = nullptr;
	gInteractive = nullptr;

	item = gui::InteractiveInterface::resetHovered();

	return *this;
}

constexpr GUIPtr& GUIPtr::operator=(gui::MutableInterface* ptr) noexcept
{
	gBasic = ptr;
	gMutable = ptr;
	gInteractive = nullptr;

	item = gui::InteractiveInterface::resetHovered();

	return *this;
}

constexpr GUIPtr& GUIPtr::operator=(gui::InteractiveInterface* ptr) noexcept
{
	gBasic = ptr;
	gMutable = ptr;
	gInteractive = ptr;

	item = gui::InteractiveInterface::resetHovered();

	return *this;
}

void populateGUI(GUIPtr& cur, std::string& writing, IGUI* main, IGUI* other) noexcept
{
	ENSURE_VALID_PTR(main, "main was nullptr when populateGUI was called");

	main->addDynamicText("text1", "entry", { 500, 400 });
	main->addInteractive("text1", [&writing](IGUI*) mutable { writing = "text1"; });
	main->addDynamicText("text2", "entry", { 500, 500 });
	main->addInteractive("text2", [&writing](IGUI* igui) mutable { writing = "text2"; });
	main->addDynamicText("other", "switch", { 500, 800 });
	main->addInteractive("other", [other, &cur](IGUI*) mutable { cur = other; });
	main->addText("Hi!!\nWelcome to my GUI", sf::Vector2f{ 200, 150 }, 48, sf::Color{ 255, 255, 255 }, "__default", gui::Alignment::Left);
	gui::addMQB(main, "mqb", { 50, 50 }, { 0, 50 }, 10, true, true, 1);

	sf::RectangleShape rect{ { 50, 50 } };
	other->addDynamicSprite("colorChanger", gui::createTextureFromDrawables(rect), sf::Vector2f{ 500, 850 });
	other->addInteractive("colorChanger");
	other->addDynamicText("main", "switch", { 500, 500 });
	other->addInteractive("main", [main, &cur](IGUI*) mutable { cur = main; });
	gui::addSlider(other, "slider", { 300, 500 });
}