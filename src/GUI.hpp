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
 *		 declares useful functions and types You can freely modify it to fit your needs.
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
 * The overloaded operator= ensures that the instance is properly set.
 * It sets derived pointers to nullptr and resets the hovered item (its own instance + the IGUI's one)
 * 
 * You can use it as if it were a basic interface pointer (see operator->), or directly access the 
 * public pointer attributes.
 * 
 * There are conversion functions to pointers for all interfaces type in order to shorten your code.
 * 
 * It is recommended for you to store the return value of `eventUpdateHovered` using this struct:
 * `curGui.item = IGUI::eventUpdateHovered(curGui, window.mapPixelToCoords(event->getIf<sf::Event::MouseMoved>()->position));`
 * Here curGUI is an instance of GUIPtr.
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
 * curGui.item = IGUI::eventUpdateHovered(curGui, window.mapPixelToCoords(event->getIf<sf::Event::MouseMoved>()->position)); // Updates the hovered item
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
	constexpr inline GUIPtr() noexcept : gInteractive{ nullptr }, gMutable{ nullptr }, gBasic{ nullptr }, item{}
	{ gui::InteractiveInterface::resetHovered(); }
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
	gui::InteractiveInterface::Item item;
};


using BGUI = gui::BasicInterface; // don't hesitate to change this alias if you encounter ambiguity issues.
using MGUI = gui::MutableInterface; // Nothing in this library use them, it is really just for you
using IGUI = gui::InteractiveInterface; // except populateGUI

//TODO: Complete you own populateGUI function. You may omit the noexcept if you want to throw exceptions.
// The current impl is an example used in the code portion below.
void populateGUI(GUIPtr& cur, std::string& writing, IGUI* main, IGUI* other) noexcept;

/**
 * Below is a code portion showing how to use the library.
 * It may be easier to understand if you check the other example files first (begin with GraphicalResources.hpp, then BasicInterface...).
 * 
 * The best way to use the library is tocopy paste this example in your own project and modify it to fit your needs.
 * You should modify: 
 * - the populate function(s)/the gui types/their number
 * - what is between the event loop and the drawing part
 * - add other event handling if needed
 * In that sense, this library is more a framework than a simple set of classes.
 * 
 * It is totally fine to have some interfaces of different types
 * 
 * \code PopulateGUI function example:
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
 * \endcode
 * 
 * \code main.cpp
 * 
 * \endcode
 * 
 * It is totally fine to have multiple interfaces displayed at the same time, but you have to manage your GUIPtr well.
 * Some may appear as an overlay and never the main one. In this case, you dont ever need to set it as the current GUI.
 * What is trickier is having multiple interfaces of type interactive and use the hover system on both.
 */

#endif // GUI_HPP