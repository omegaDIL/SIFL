 /*******************************************************************
 * \file   AdvancedInterface.hpp, AdvancedInterface.cpp
 * \brief  Declare a gui with a slider, and multiple question boxes.
 *
 * \author OmegaDIL.
 * \date   July 2025.
 *
 * \note These files depend on the SFML library.
 *********************************************************************/

#ifndef ADVANCEDINTERFACE_HPP
#define ADVANCEDINTERFACE_HPP

#include "Interactiveinterface.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <vector>
#include <optional>

namespace gui
{

using UserFunction = std::function<void(double)>;
using GrowthSliderFunction = std::function<double(double)>;

/**
 * \brief Adds a slider to the interactive gui. The user can move the slider up and down to change its value.
 * \complexity O(1).
 * 
 * The slider consists of a background and cursor rectangles + a text displaying the current value.
 * The interactive element is the background rectangle (that the user can click and drag the cursor on).
 * 
 * \param[out] gui The interactive gui to which the slider will be added.
 * \param[in]  identifier The unique identifier for the slider.
 * \param[in]  pos The position of the slider.
 * \param[in]  length The length of the slider. Default is 300.
 * 
 * \note All elements are dynamic elements, so they need an identifier. The background rectangle And
 *		 the text have the identifier you gave as argument, while the cursor rectangle has the identifier
 *		 "_sb_" + identifier.
 * \note Textures are added to SpriteWrapper with names "__sb" and "__sc".
 * \note After creating the slider, you can use the function `moveSlider` to change its default value.
 * 
 * \pre The gui must be a valid ptr.
 * \warning The program will assert otherwise.
 * 
 * \see moveSlider, hideSlider.
 */
void addSlider(InteractiveInterface* gui, std::string identifier, sf::Vector2f pos, unsigned int length = 300) noexcept;

/**
 * \brief Hides or shows the slider and its elements.
 * \complexity O(1).
 * 
 * \param[out] gui The interactive gui containing the slider.
 * \param[in]  identifier The unique identifier for the slider.
 * \param[in]  hide If true, the slider is hidden. If false, it is shown. Default is true.
 * 
 * \pre The slider must exist in the gui.
 * \post It will be hidden or shown.
 * \throw std::invalid_argument Strong exception guarantee: nothing happens.
 * 
 * \pre The gui must be a valid ptr.
 * \warning The program will assert otherwise.
 */
void hideSlider(InteractiveInterface* gui, const std::string& identifier, bool hide = true);

/**
 * \brief Allows the user to move the slider up and down, changing its value.
 * \complexity O(1).
 * 
 * \param[out] gui The interactive gui containing the slider.
 * \param[in]  identifier The unique identifier for the slider.
 * \param[in]  posY The new position of the cursor on the slider (the y coordinate).
 * \param[in]  intervals The number of intervals to divide the slider into. Default is -1 (no intervals).
 *						 The number of intervals is the number of steps between the min and max values.
 *						 That means if you set it to 0, the slider will only have two values: min and max.
 *						 Any negative value means no intervals.
 * \param[in]  growth A function that takes a float between 0 and 1 (the relative position of the
 *					  cursor on the slider) and returns a float (the value of the slider). Default
 *					  is the mathematical function f(x) = x (linear growth).
 * \param[in]  user A function that will be executed when the slider value is changed. It takes the
 *					current value.
 * 
 * \note For user-friendliness, it is recommended to call this function as long as the mouse is pressed.
 * 
 * \pre The slider must exist in the gui.
 * \post It will be changed.
 * \throw std::invalid_argument Strong exception guarantee: nothing happens.	
 *
 * \pre The gui must be a valid ptr.
 * \warning The program will assert otherwise.
 */
double moveSlider(InteractiveInterface* gui, const std::string& identifier, double yPos, int intervals = -1, const GrowthSliderFunction& growth = [](float x) {return x; }, const UserFunction& user = nullptr);


/**
 * \brief Adds a multiple question box to the interactive gui. The user can check or uncheck boxes.
 * \complexity O(N), where N is the number of boxes.
 * 
 * \param[out] gui The interactive gui to which the multiple question box will be added.
 * \param[in]  identifier The unique identifier for the multiple question box.
 * \param[in]  posInit The position of the first box.
 * \param[in]  posDelta The delta position between two boxes.
 * \param[in]  numberOfBoxes The number of boxes in the multiple question box.
 * \param[in]  defaultCheckedBox The index of the box that will be checked by default. Negative values
 *								 means no box is checked by default. Default is -1.
 * 
 * \note Boxes are 0-indexed.
 * \note All elements are dynamic elements, so they need an identifier. Each box has the identifier
 *		"_mqb_identifier_i" where i is the index of the box (0 to numberOfBoxes - 1).
 * \note Textures are added to SpriteWrapper with names "__ub" and "__cb".
 * 
 * \pre The gui must be a valid ptr.
 * \pre numberOfBoxes must be greater than 0.
 * \pre defaultCheckedBox must not be equal to or greater than numberOfBoxes.
 * \warning The program will assert otherwise.
 * 
 * \see hideMQB, isMQB, checkBox.
 */
void addMQB(InteractiveInterface* gui, std::string identifier, sf::Vector2f posInit, sf::Vector2f posDelta, short numberOfBoxes, short defaultCheckedBox = -1) noexcept;

/**
 * \brief Hides or shows the multiple question box and its elements.
 * \complexity O(N), where N is the number of boxes.
 * 
 * It hides all elements that have an identifier starting with "_mqb_identifier_i" until i grows
 * larger than the number of boxes.	If one element were to be missing, it would stop there.
 * 
 * \param[out] gui The interactive gui containing the multiple question box.
 * \param[in]  identifier The unique identifier for the multiple question box.
 * \param[in]  hide If true, the slider is hidden. If false, it is shown. Default is true.
 * 
 * \pre The mqb must exist in the gui.
 * \post It will be hidden or shown.
 * \throw std::invalid_argument Strong exception guarantee: nothing happens.
 * 
 * \pre The gui must be a valid ptr.
 * \warning The program will assert otherwise.
 */
void hideMQB(InteractiveInterface* gui, const std::string& identifier, bool hide = true);

using mqbInfo = std::pair<std::string, short>;
/**
 * \brief Returns the box identifier and the number of the mqb if identifier belongs to a mqb
 * \complexity O(1) when this function returns std::nullopt.
 * \complexity O(N) where N is the size of the identifier you gave.
 * 
 * \param[in] gui The interactive gui containing the multiple question box.
 * \param[in] identifier The identifier of the sprite element you want to check.
 * 
 * \return The identifier and the box number, or nullopt if the element doesn't exist within the 
 *		   gui/isn't a box
 * 
 * \pre The gui must be a valid ptr.
 * \warning The program will assert otherwise.
 */
std::optional<mqbInfo> isMqb(InteractiveInterface* gui, std::string identifier) noexcept;

/**
 * \brief Change the state of the box at index 'check' in the multiple question box.
 * \complexity O(N), where N is the number of boxes.
 * 
 * All boxes are retrieved through their identifiers: the same technique as in hideMQB.
 * 
 * This function is not particularly efficient, but it is not meant to be called multiple times
 * per frame or even per second.
 * 
 * \param[out] gui The interactive gui containing the multiple question box.
 * \param[in]  identifier The unique identifier for the multiple question box.
 * \param[in]  check The index of the box to check or uncheck.
 * \param[in]  multipleChoices If true, the user can check or uncheck multiple boxes. Default is true.
 * \param[in]  atLeastOne If true, at least one box must always be checked. Default is false.
 *
 * \returns A vector containing the indexes of the currently checked boxes.
 * 
 * \note Boxes are 0-indexed.
 *
 * \pre The mqb must exist in the gui.
 * \post It will be hidden or shown.
 * \throw std::invalid_argument Strong exception guarantee: nothing happens.
 * 
 * \pre The gui must be a valid ptr.
 * \pre The new check box index must be within range.
 * \warning The program will assert otherwise.
 * 
 * \see isMqb
 */
std::vector<short> checkBox(InteractiveInterface* gui, mqbInfo mqb, bool multipleChoices = false, bool atLeastOne = true);

} // gui namespace

#endif //ADVANCEDINTERFACE_HPP