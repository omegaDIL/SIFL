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
 * Here curGUI is an instance of currentGUI.
 */
struct currentGUI
{
	// "Default" constructors
	constexpr inline currentGUI() noexcept : gInteractive{ nullptr }, gMutable{ nullptr }, gBasic{ nullptr }, item{}
	{ gui::InteractiveInterface::resetHovered(); }
	constexpr inline currentGUI(std::nullptr_t) noexcept : currentGUI{} {}

	// Rule of five
	constexpr currentGUI(const currentGUI&) noexcept = default;
	constexpr currentGUI(currentGUI&&) noexcept = delete;
	constexpr currentGUI& operator=(const currentGUI&) noexcept = default;
	constexpr currentGUI& operator=(currentGUI&&) noexcept = delete;
	constexpr ~currentGUI() noexcept = default; 

	// Assignment operator -> sets to nullptr derived pointers
	constexpr currentGUI& operator=(std::nullptr_t) noexcept;
	constexpr currentGUI& operator=(gui::BasicInterface* ptr) noexcept;
	constexpr currentGUI& operator=(gui::MutableInterface* ptr) noexcept;
	constexpr currentGUI& operator=(gui::InteractiveInterface* ptr) noexcept;

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

//TODO: Complete you own populateGUI function
// The current impl is an example (see \code)
void populateGUI(currentGUI& cur, std::string& writing, IGUI* main, IGUI* other) noexcept;

// TODO: add all examples
// TODO: rework locking comment
#endif // GUI_HPP