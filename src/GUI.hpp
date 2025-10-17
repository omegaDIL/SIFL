/*******************************************************************
 * \file   GUI.hpp
 * \brief  Group all entities for creating and managing graphical user interfaces.
 *
 * \author OmegaDIL.
 * \date   July 2024.
 *
 * \note This file depends on the SFML library.
 * \note A code example is provided at the end.
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

using BGUI = gui::BasicInterface;
using MGUI = gui::MutableInterface;
using IGUI = gui::InteractiveInterface;

/**
 * @brief Creates a new instance of a window to display an error message.
 * @complexity Depends on the user who stops the window.
 *
 * @param[in] errorTitle: The title of the window.
 * @param[in] errorMessage: The message to be displayed.
 *
 * @note This function is blocking and will terminate once the user closes the new window.
 * @note Don't forget to put the character \n to avoid the text to not be seen entirely when you
 *       have a long line.
 *
 * @see `sf::RenderWindow`, `IGUI`.
 */
void showErrorsUsingWindow(const std::string& errorTitle, const std::ostringstream& errorMessage) noexcept;

struct currentGUI
{
	// "Default" constructors
	inline currentGUI() noexcept : gInteractive{ nullptr }, gMutable{ nullptr }, gBasic{ nullptr }, item{}
	{ IGUI::resetHovered(); }
	inline currentGUI(std::nullptr_t) noexcept : currentGUI{} {}

	// Rule of five
	currentGUI(const currentGUI&) noexcept = default;
	currentGUI(currentGUI&&) noexcept = delete;
	currentGUI& operator=(const currentGUI&) noexcept = default;
	currentGUI& operator=(currentGUI&&) noexcept = delete;
	~currentGUI() noexcept = default; 

	// Assignment operator -> sets to nullptr derived classes
	currentGUI& operator=(std::nullptr_t) noexcept;
	currentGUI& operator=(BGUI* ptr) noexcept;
	currentGUI& operator=(MGUI* ptr) noexcept;
	currentGUI& operator=(IGUI* ptr) noexcept;

	// Conversion functions -> returns nullptr if the conversion is not possible
	operator BGUI*() noexcept { return gBasic;	     }
	operator MGUI*() noexcept { return gMutable;	 }
	operator IGUI*() noexcept { return gInteractive; }

	// Can be used like an interface ptr
	BGUI* operator->() noexcept { return gBasic; }


	BGUI* gBasic;
	MGUI* gMutable;
	IGUI* gInteractive;
	IGUI::Item item;
};

//TODO: Complete you own populateGUI function
// The current impl is an example (see \code)
void populateGUI(currentGUI& cur, std::string& writing, IGUI* main, IGUI* other) noexcept;

// TODO: module C++20
// TODO: changer hash
// TODO: swap not working -> may invalidate other functions
// TODO: add all examples
// TODO: review mqb
#endif // GUI_HPP