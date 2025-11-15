/*******************************************************************
 * \file   GUI.hpp
 * \brief  Group all entities for creating and managing graphical user interfaces.
 *
 * \author OmegaDIL.
 * \date   July 2024.
 *
 * \note This file depends on the SFML library.
 * \note A code example is provided at the end.
 * \note This file is here to help you. It includes all the necessary headers and
 *		 declares useful functions and types you can freely modify it to fit your needs.
 *		 If needed, don't hesitate to add a namespace around everything.
 *********************************************************************/

#ifndef GUI_HPP
#define GUI_HPP

#include "GUI/GraphicalResources.hpp"
#include "GUI/BasicInterface.hpp"
#include "GUI/MutableInterface.hpp"
#include "GUI/InteractiveInterface.hpp"
#include "GUI/CompoundElements.hpp"
#include <string>
#include <sstream>

using BGUI = gui::BasicInterface; // don't hesitate to change this alias if you encounter ambiguity
using MGUI = gui::MutableInterface; // issues. Nothing in this library use them, it is really just
using IGUI = gui::InteractiveInterface; // for you, except populateGUI which should also be modified by you.

/**
 * \brief Creates a new instance of a window to display an error message.
 * \complexity None, depends on the user who unblocks the window.
 *
 * \param[in] errorTitle: The title of the window.
 * \param[in] errorMessage: The message to be displayed.
 * \param[in] characterSize: The character size of the displayed text.
 *
 * \note This function is blocking and will terminate once the user closes the new window.
 * \note Don't forget to put the character \n to avoid the text to not be seen entirely when you
 *       have a long line.
 *
 * \see `sf::RenderWindow`, `InteractiveInterface`.
 */
void showErrorsUsingWindow(const std::string& errorTitle, const std::ostringstream& errorMessage, unsigned int characterSize = 30) noexcept;

/**
 * \brief This struct helps switching from a displayed interface to another one.
 * 
 * This structure is a "casting pointer" for the three GUI interfaces provided by the library.
 * It provides pointers to all three interface types, and an item instance. It replaces the 
 * use of a pointer to the current interface and a variable that would contain the hovered item.
 * 
 * The overloaded operator= ensures that the instance is properly set. For each type, it sets
 * its base class pointers, and the other derived class pointers to nullptr.
 * 
 * You can use it as if it were a basic interface pointer (see operator->), or directly access the 
 * public pointer attributes.
 * 
 * There are conversion functions to pointers for all interfaces type in order to shorten your code.
 * 
 * \code
 * IGUI mainInterface{ &window, 1080 };
 * IGUI otherInterface{ &window, 1080 };
 * BGUI basicInterface{ &window, 1080 };
 * //...
 * GUIPtr curGui{};
 * curGUI = &mainInterface; // Start with the main interface.
 * curGUI = &basicInterface; // Switch to a different interface type AND CALLS RESETHOVERED()
 * 
 * // when the mouse moves:
 * curGui.item = curGUI.gInteractive->eventUpdateHovered(window.mapPixelToCoords(event->getIf<sf::Event::MouseMoved>()->position)); // Updates the hovered item
 * 
 * // the drawing part
 * window.clear();
 * curGui->draw(); // Pointer is of type BasicInterface*
 * window.display();
 * \endcode
 */
struct GUIPtr
{
	// "Default" constructors
	constexpr inline GUIPtr() noexcept : gInteractive{ nullptr }, gMutable{ nullptr }, gBasic{ nullptr }, mainItem{} {}
	constexpr inline GUIPtr(std::nullptr_t) noexcept : GUIPtr{} {}

	// Rule of five
	constexpr GUIPtr(const GUIPtr&) noexcept = default;
	constexpr GUIPtr(GUIPtr&&) noexcept = delete;
	constexpr GUIPtr& operator=(const GUIPtr&) noexcept = default;
	constexpr GUIPtr& operator=(GUIPtr&&) noexcept = delete;
	constexpr ~GUIPtr() noexcept = default;

	// Assignment operator -> sets to nullptr derived pointers
	constexpr GUIPtr& operator=(std::nullptr_t) noexcept;
	constexpr GUIPtr& operator=(gui::BasicInterface* ptr) noexcept;
	constexpr GUIPtr& operator=(gui::MutableInterface* ptr) noexcept;
	constexpr GUIPtr& operator=(gui::InteractiveInterface* ptr) noexcept;

	// Conversion functions -> returns nullptr if the conversion is not possible
	constexpr operator gui::BasicInterface*() noexcept { return gBasic;	     }
	constexpr operator gui::MutableInterface*() noexcept { return gMutable;	 }
	constexpr operator gui::InteractiveInterface*() noexcept { return gInteractive; }

	// Can be used like an basic interface ptr
	constexpr gui::BasicInterface* operator->() noexcept { return gBasic; }


	gui::BasicInterface* gBasic;
	gui::MutableInterface* gMutable;
	gui::InteractiveInterface* gInteractive;
	gui::InteractiveInterface::Item mainItem;
};


//TODO: Complete you own populateGUI function. You may omit the noexcept if you want to throw exceptions.
// The current impl is an example used in the code portion below.
void populateGUI(GUIPtr& cur, std::string& writing, IGUI* main, IGUI* other, MGUI* overlay) noexcept;

/**
 * Below is a code portion showing how to use the library.
 * It may be easier to understand if you check the other example files first (begin with GraphicalResources.hpp, then BasicInterface...).
 * 
 * \code PopulateGUI function example:
 void populateGUI(GUIPtr& cur, std::string& writing, IGUI* main, IGUI* other, MGUI* overlay) noexcept
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

	sf::CircleShape overlayTex{ 20, 120 };
	overlayTex.setFillColor(sf::Color{ 255, 255, 255, 80 });
	overlay->addDynamicSprite("overlay", gui::createTextureFromDrawables(overlayTex), sf::Vector2f{ 0, 0 });
 }
 * \endcode
 * 
 * \code main.cpp
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
	std::string writingText{ "text1" };  // Identifier of which text is being written to.

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
		
			else if (curGUI.gMutable != nullptr && event->is<sf::Event::TextEntered>() && writingText != "") // See compoundElements.hpp for more info about text writing.
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
 * \endcode
 */

//TODO: review
//TODO: erase GUI folder

#endif // GUI_HPP