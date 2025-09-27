/*******************************************************************
 * \file   GUI.hpp
 * \brief  Group all entities for creating and managing graphical user interfaces.
 *
 * \author OmegaDIL.
 * \date   July 2024.
 *
 * \note This file depends on the SFML library.
 *********************************************************************/

#ifndef GUI_HPP
#define GUI_HPP

#include "GUI/GraphicalResources.hpp"
#include "GUI/BasicInterface.hpp"
#include "GUI/MutableInterface.hpp"
#include "GUI/InteractiveInterface.hpp"
#include "GUI/AdvancedInterface.hpp"

using BGUI = gui::BasicInterface;
using MGUI = gui::MutableInterface;
using IGUI = gui::InteractiveInterface; // Often enough for most apps

#include <string>
#include <sstream>

/**
 * @brief Creates a new instance of a window to display an error message.
 * @complexity constant O(1).
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

/**
 * \brief Initializes the interface
 * 
 * \param[out] gui: the graphical user inteface to initialize.
 * 
 * \warning Assert if gui is nullptr
 */
void populateGUI(IGUI* gui) noexcept; 

// TODO: complete the function `populateGUI` your own way.
// You can change the gui type, add arguments, or add more
// interfaces to populate. Feel free.

// TODO: module C++20
// TODO: changer hash
#endif // GUI_HPP