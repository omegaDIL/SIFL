 /*******************************************************************
 * \file   CompoundElements.hpp, CompoundElements.cpp
 * \brief  Declare a gui with a slider, and multiple question boxes.
 *
 * \author OmegaDIL.
 * \date   July 2025.
 *
 * \note These files depend on the SFML library.
 *********************************************************************/

#ifndef COMPOUNDELEMENTS_HPP
#define COMPOUNDELEMENTS_HPP

#include "Interactiveinterface.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <vector>
#include <optional>

namespace gui
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Progress bar functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Adds a progress bar of a mutable gui.
 * \complexity O(1).
 * 
 * The progress bar consist of a background rectangle, a fill rectangle and a text displaying the
 * relative progress in percentage.
 * 
 * Nothing is done if already added.
 * 
 * \param[out] gui The mutable gui to which the progress bar will be added.
 * \param[in]  identifier The unique identifier for the progress bar.
 * \param[in]  pos The position of the progress bar.
 * \param[in]  length The length of the progress bar. Default is 150.
 * 
 * \note The identifier used for the background rectangle is `_pb_ + identifier`, while the fill
 *		 rectangle's identifier is simply `identifier`. The text's identifier is also `identifier`.
 * 
 * \pre The gui must be a valid ptr.
 * \post The progress bar will be added to the gui.
 * \warning The program will assert otherwise.
 * 
 * \see moveProgressBar, hideProgressBar, removeProgressBar.
 */
void addProgressBar(MutableInterface* gui, std::string identifier, sf::Vector2f pos, unsigned int length = 150) noexcept;

/**
 * \brief Updates the progress bar to meet the given progress.
 * \complexity O(1).
 * 
 * \param[out] gui The mutable gui containing the progress bar.
 * \param[in]  identifier The unique identifier for the progress bar.
 * \param[in]  progress The new progress, between 0 and 1 (rounded to the nearest integer percentage).
 * 
 * \pre The progress bar must exist in the gui.
 * \throw std::invalid_argument Strong exception guarantee: nothing happens.
 * 
 * \pre The gui must be a valid ptr.
 * \warning The program will assert otherwise.
 * 
 * \see addProgessBar.
 */
void moveProgressBar(MutableInterface* gui, const std::string& identifier, float progress);

/**
 * \brief Hides or shows the progress bar and its elements.
 * 
 * \param[out] gui The mutable gui containing the progress bar.
 * \param[in]  identifier The unique identifier for the progress bar.
 * \param[in]  hide If true, the progress bar is hidden. If false, it is shown. Default is true.
 * 
 * \pre The progress bar must exist in the gui.
 * \throw std::invalid_argument Strong exception guarantee: nothing happens.
 * 
 * \pre The gui must be a valid ptr.
 * \warning The program will assert otherwise.
 * 
 * \see removeProgressBar, addProgessBar.
 */
void hideProgressBar(MutableInterface* gui, const std::string& identifier, bool hide = true);

/**
 * \brief Removes the progress bar and its elements from the gui.
 * 
 * Nothing happens if the identifier is not a progress bar's identifier.
 * 
 * \param[out] gui The mutable gui containing the progress bar.
 * \param[in]  identifier The unique identifier for the progress bar.
 * 
 * \pre The gui must be a valid ptr.
 * \post The progress bar will be removed from the gui.
 * \warning The program will assert otherwise.
 * 
 * \see hideProgressBar, addProgessBar.
 */
void removeProgressBar(MutableInterface* gui, const std::string& identifier) noexcept;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Progress bar functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Slider bar functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

using UserFunction = std::function<void(double)>;
using GrowthSliderFunction = std::function<double(double)>;

/**
 * \brief Adds a slider to the interactive gui. The user can move the slider up and down to change its value.
 * \complexity O(1).
 * 
 * The slider consists of a background and cursor rectangles + a text displaying the current value.
 * The interactive element is the background rectangle (that the user can click and drag the cursor on).
 * 
 * Nothing is done if already added.
 *
 * \param[out] gui The interactive gui to which the slider will be added.
 * \param[in]  identifier The unique identifier for the slider.
 * \param[in]  pos The position of the slider.
 * \param[in]  length The length of the slider. Default is 300.
 * 
 * \note All elements are dynamic elements, so they need an identifier. The background rectangle And
 *		 the text have the identifier you gave as argument, while the cursor rectangle has the identifier
 *		 `_sb_ + identifier`.
 * \note Textures are added to SpriteWrapper with names `__sb` and `__sc`.
 * \note After creating the slider, you can use the function `moveSlider` to change its default value.
 * 
 * \pre The gui must be a valid ptr.
 * \post The slider will be added to the gui.
 * \warning The program will assert otherwise.
 * 
 * \see moveSlider, hideSlider, removeSlider.
 */
void addSlider(InteractiveInterface* gui, std::string identifier, sf::Vector2f pos, unsigned int length = 400) noexcept;

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
 * \pre The gui must be a valid ptr. 
 * \pre growth must not be a nullptr.
 * \warning The program will assert otherwise.
 * 
 * \pre The slider must exist in the gui.
 * \throw std::invalid_argument Strong exception guarantee: nothing happens.
 * 
 * \see addSlider.
 */
double moveSlider(InteractiveInterface* gui, const std::string& identifier, double yPos, int intervals = -1, const GrowthSliderFunction& growth = [](double x) {return x; }, const UserFunction& user = nullptr);

/**
 * \brief Hides or shows the slider and its elements.
 * \complexity O(1).
 *
 * \param[out] gui The interactive gui containing the slider.
 * \param[in]  identifier The unique identifier for the slider.
 * \param[in]  hide If true, the slider is hidden. If false, it is shown. Default is true.
 *
 * \pre The slider must exist in the gui.
 * \throw std::invalid_argument Strong exception guarantee: nothing happens.
 *
 * \pre The gui must be a valid ptr.
 * \warning The program will assert otherwise.
 * 
 * \see removeSlider, addSlider.
 */
void hideSlider(InteractiveInterface* gui, const std::string& identifier, bool hide = true);

/**
 * \brief Removes the slider and its elements from the gui.
 * \complexity O(1).
 * 
 * Nothing happens if the identifier is not a slider's identifier.
 * 
 * \param[out] gui The interactive gui containing the slider.
 * \param[in]  identifier The unique identifier for the slider.
 * 
 * \pre The gui must be a valid ptr.
 * \post The slider will be removed from the gui.
 * \warning The program will assert otherwise.
 * 
 * \see hideSlider, addSlider.
 */
void removeSlider(InteractiveInterface* gui, const std::string& identifier) noexcept;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Slider bar functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// MQB functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Adds a multiple question box to the interactive gui. The user can check or uncheck boxes.
 * \complexity O(N), where N is the number of boxes.
 * 
 * The mqb consists of a series of sprites, and each represents a box. 
 * 
 * Nothing is done if already added.
 * 
 * The mqb is automatically updated via the buttons in the interactive interface. Note that when it
 * is updated, the complexity is O(N) and it is not cache efficient.
 *
 * \param[out] gui The interactive gui to which the multiple question box will be added.
 * \param[in]  identifier The unique identifier for the multiple question box.
 * \param[in]  posInit The position of the first box.
 * \param[in]  posDelta The delta position between two boxes.
 * \param[in]  numberOfBoxes The number of boxes in the multiple question box.
 * \param[in]  multipleChoices If true, the user can check or uncheck multiple boxes. Default is true.
 * \param[in]  atLeastOne If true, at least one box must always be checked. Default is false.
 * \param[in]  defaultCheckedBox The index of the box that will be checked by default. 0 
 *								 means no box is checked by default.
 * 
 * \note Boxes are 1-indexed.
 * \note All elements are dynamic elements, so they need an identifier. Each box has the identifier
 *		`_mqb_identifier_i` where i is the index of the box (1 to numberOfBoxes).
 * \note Textures are added to SpriteWrapper with names `__ub` and `__cb`.
 * 
 * \pre The gui must be a valid ptr.
 * \pre numberOfBoxes must be greater than 0.
 * \pre defaultCheckedBox must not be equal to or greater than numberOfBoxes.
 * \pre atLeastOne can't be true if no default box is provided.
 * \pre atLeastOne can't be true if only one box is added in the mqb
 * \post The multiple question box will be added to the gui.
 * \warning The program will assert otherwise.
 * 
 * \see getMQBStatus, hideMQB, removeMQB.
 */
void addMQB(InteractiveInterface* gui, const std::string& identifier, sf::Vector2f initPos, sf::Vector2f deltaPos, unsigned short numberOfBoxes, bool multipleChoices = false, bool atLeastOne = true, unsigned short defaultCheckedBox = 0) noexcept;

/**
 * \brief get the mqb status, that is to say the indexes of all boxes checked.
 * \complexity O(N), where N is the number of boxes
 * 
 * All boxes are retrieved through their identifiers by iterating until i grows larger than the number
 * of boxes.
 * 
 * This function is not cache efficient.
 * 
 * \param[out] gui The interactive gui to which the multiple question box was added.
 * \param[in]  identifier The unique identifier of the multiple question box.
 * 
 * \returns The indexes of all boxes checked
 * 
 * \note Boxes are 1-indexed.
 * 
 * \pre the gui must not be nullptr
 * \pre the identifier must represent a mqb
 * \post the status will be reported using the texture index
 * \warning asserts otherwise.
 * 
 * \see addMQB.
 */
std::vector<unsigned short> getMQBStatus(InteractiveInterface* gui, const std::string& identifier) noexcept;

/**
 * \brief Hides or shows the multiple question box and its elements.
 * \complexity O(N), where N is the number of boxes.
 * 
 * All boxes are retrieved through their identifiers by iterating until i grows larger than the number
 * of boxes.
 *
 * This function is not particularly efficient, but it is not meant to be called multiple times
 * per frame or even per second.
 * 
 * Nothing happens if the identifier is not a multiple question box's identifier.
 * 
 * \param[out] gui The interactive gui containing the multiple question box.
 * \param[in]  identifier The unique identifier for the multiple question box.
 * \param[in]  hide If true, the slider is hidden. If false, it is shown. Default is true.
 * 
 * \pre The gui must be a valid ptr.
 * \post Will be hidden/shown.
 * \warning The program will assert otherwise.
 * 
 * \see removeMQB, addMQB.
 */
void hideMQB(InteractiveInterface* gui, const std::string& identifier, bool hide = true) noexcept;

/**
 * \brief Removes the multiple question box and its elements from the gui.
 * \complexity O(N), where N is the number of boxes.
 * 
 * Nothing happens if the identifier is not a multiple question box's identifier.
 * 
 * \param[out] gui The interactive gui containing the multiple question box.
 * \param[in]  identifier The unique identifier for the multiple question box.
 * 
 * \pre The gui must be a valid ptr.
 * \post The multiple question box will be removed from the gui.
 * \warning The program will assert otherwise.
 * 
 * \see hideMQB, addMQB.
 */
void removeMQB(InteractiveInterface* gui, const std::string& identifier, unsigned short numberOfBoxes) noexcept;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// MQB functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Writing text functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

using WritingFunction = std::function<bool(char32_t&, sf::String&, TextWrapper*)>;
/**
 * \brief Updates a textWrapper while the user is writing text.
 * \complexity O(1).
 * 
 * Can be called even if the interface is locked.
 * 
 * You can add an optional function that will be called before the character is added. It takes a 
 * reference to the next character, the rest of the string, and the text that is being modified. It is
 * mainly used to filter the input (e.g. to allow only numbers). To do so, you can set unicodeValue
 * to any none-printable character besides: 0x8 (backspace), 0x9 (tab) or 0xa (LF). Note 0xd (CR) is
 * automatically changed to LF before the function is called. The return value is the same as this
 * optional function (or true if no function). It is often used to signal that the writing ended (see
 * the code portion to see how). 
 * 
 * \param[out] text The textWrapper to update.
 * \param[in]  unicodeValue The unicode value of the character to add.
 * \param[in]  func An optional function to call before updating the text.
 * 
 * \return True by default, or the return value of the writingFunction if there isn't.
 *					
 * \note You should call this function only if you are sure text is a valid ptr, that is to say
 *		 when the text's interface is locked, or your code logic ensures it. Otherwise, use the
 *		 overloaded version that takes the interface and the identifier.
 * \note Exceptions depends on the writing function.
 * 
 * \pre The text must exist.
 * \warning The program will assert otherwise.
 * 
 * \code (In the event loop)
 * if (text1 != nullptr && event->is<sf::Event::TextEntered>())
 *     if (!gui::updateWritingText(text1, event->getIf<sf::Event::TextEntered>()->unicode, gui::basicWritingFunction))
 *	       text1 = nullptr; 
 * 
 * // `gui::basicWritingFunction` is the function that filer the input.
 * // When false is returned, the writing ends.
 * // You can use interactive and button elements to begin writing.
 * \endcode
 */
bool updateWritingText(TextWrapper* text, char32_t unicodeValue, const WritingFunction& func = nullptr);

/**
 * \see Similar to updateWritingText(), but works by identifiers: more secure if the interface is
 *		not locked.
 * 
 * \pre identifier must correspond to a TextWrapper in the gui.
 * \throw std::invalid_argument Strong exception guarantee: nothing happens.
 * 
 * \pre The gui must be a valid ptr.
 * \warning The program will assert otherwise.
 */
bool updateWritingText(MutableInterface* gui, std::string_view identifier, char32_t unicodeValue, const WritingFunction& func = nullptr);

/**
 * \brief This is an example of a simple WritingFunction.
 * \complexity O(1).
 * 
 * Returns true when the writing should end. Ensures the text is not left empty. Sets to italic
 * when writing and regular when it ends.
 * 
 * \param[in] c The next character.
 * \param[in,out] str The current string.
 * 
 * \return `false` if the character is any of those character: escape/CR/LF.
 *		   `true` otherwise.
 */
bool basicWritingFunction(char32_t& c, sf::String& str, TextWrapper* txt) noexcept;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Writing text functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

} // gui namespace

#endif //COMPOUNDELEMENTS_HPP