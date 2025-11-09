#include "CompoundElements.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>
#ifndef NDEBUG
#include <cassert>
#endif //NDEBUG

#include <iostream>

namespace gui
{

inline const static std::string progressBarIdPrefix{ "_pb_" };
inline const static std::string sliderIdPrefix{ "_sc_" };
inline const static std::string mqbIdPrefix{ "_mqb_" };

/**
 * \brief Creates a texture from a solid rectangle shape.
 * \complexity O(1).
 * 
 * \param[in] scale The size of the rectangle.
 * \param[in] outlineThickness The thickness of the outline. Can be negative -> see SFML doc.
 * \param[in] fillColor The fill color.
 * \param[in] outlineColor The outline color.
 * 
 * \return The texture.
 */
static sf::Texture loadSolidRectangle(sf::Vector2f scale, float outlineThickness, sf::Color fillColor = sf::Color{ 20, 20, 20 }, sf::Color outlineColor = sf::Color{ 80, 80, 80 }) noexcept
{
	sf::RectangleShape shape{ scale };
	shape.setFillColor(fillColor);
	shape.setOutlineColor(outlineColor);
	shape.setOutlineThickness(outlineThickness);

	return createTextureFromDrawables(shape);
}


void addProgressBar(MutableInterface* gui, std::string identifier, sf::Vector2f pos, unsigned int length) noexcept
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function addProgessBar was called");

	static constexpr std::string_view progressBarBackgroundTextureName{ "__pb" };
	static constexpr std::string_view progressBarFillTextureName{ "__pf" };

	constexpr unsigned int size{ 20 };
	constexpr sf::Vector2f size2f{ size * size , size };

	if (SpriteWrapper::getTexture(progressBarBackgroundTextureName) == nullptr)
	{
		constexpr float outlineThickness{ size / 10.f };
		SpriteWrapper::createTexture(std::string{ progressBarBackgroundTextureName }, loadSolidRectangle(size2f, outlineThickness, sf::Color::Transparent, sf::Color{ 7, 135, 7 }), SpriteWrapper::Reserved::No);
		SpriteWrapper::createTexture(std::string{ progressBarFillTextureName }, loadSolidRectangle(size2f, 0, sf::Color{ 3, 60, 3 }), SpriteWrapper::Reserved::No);
	}

	const float lengthFactor{ length / 150.f };
	// sf::Vector2f{ 0.f, 1.f } because we assume it is empty at the beginning.
	// Therefore, the size can be 0. moveProgressBar will set the correct size later.
	gui->addDynamicSprite(identifier, progressBarFillTextureName, sf::Vector2f{ 0, 0 }, sf::Vector2f{ 0.f, 1.f }, sf::IntRect{}, sf::degrees(0), gui::Alignment::Left | gui::Alignment::Top);
	gui->addDynamicSprite(progressBarIdPrefix + identifier, progressBarBackgroundTextureName, pos, sf::Vector2f{ lengthFactor, 1.f });
	gui->addDynamicText(std::move(identifier), "0%", pos, size);
}

void moveProgressBar(MutableInterface* gui, const std::string& identifier, float progress)
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function updateProgressBar was called");
	assert(((progress >= 0.f) && (progress <= 1.f)) && ("The progress value was not between 0 and 1 when the function updateProgressBar was called"));

	auto* const backPtr{ gui->getDynamicSprite(progressBarIdPrefix + identifier) };
	auto* const fillPtr{ gui->getDynamicSprite(identifier) };
	auto* const textPtr{ gui->getDynamicText(identifier) };

	if (backPtr == nullptr || fillPtr == nullptr || textPtr == nullptr) [[unlikely]]
		throw std::invalid_argument{ "The progress bar with the identifier " + identifier + " does not exist." };

	progress = round(progress * 100) / 100; // Rounding to the nearest integer percentage.

	const sf::Sprite& backSprite{ backPtr->getSprite() };
	const sf::Sprite& fillSprite{ fillPtr->getSprite() };

	// 2 is the thickness of the outline, so 4 for both sides.
	// 4 * scale because the outline scales with the sprite.
	const float outlineThickness{ 4 * backSprite.getScale().x };
	// getGlobalBounds because we want the size after scaling (the visual size).
	const float maxLength{ backSprite.getGlobalBounds().size.x - outlineThickness };
	// getLocalBounds because we want the original size to get the scales.
	const float curLength{ fillSprite.getLocalBounds().size.x }; 
	const float newLength{ maxLength * progress / curLength }; 
	// Accounts for the window resizing since it had a 0 scale (see proportionKeeper() doc).

	fillPtr->setScale(sf::Vector2f{ newLength, fillSprite.getScale().y });
	// outlineThickness / 2.f because we deal with one border, not two (in x).
	// The outline is fixed since its scale is always 1 (in y).
	fillPtr->setPosition(backSprite.getGlobalBounds().position + sf::Vector2f{ outlineThickness / 2.f, 2.f });

	std::ostringstream content{};
	content << progress * 100 << '%';
	textPtr->setContent(content);
}

void hideProgressBar(MutableInterface* gui, const std::string& identifier, bool hide)
{
	assert(gui != nullptr && "The gui was nullptr when the function hideProgressBar was called");

	auto* const backPtr{ gui->getDynamicSprite(progressBarIdPrefix + identifier) };
	auto* const fillPtr{ gui->getDynamicSprite(identifier) };
	auto* const textPtr{ gui->getDynamicText(identifier) };

	if (backPtr == nullptr || fillPtr == nullptr || textPtr == nullptr) [[unlikely]]
		throw std::invalid_argument{ "The progress bar with the identifier " + identifier + " does not exist." };

	backPtr->hide = hide;
	fillPtr->hide = hide;
	textPtr->hide = hide;
}

void removeProgressBar(MutableInterface* gui, const std::string& identifier) noexcept
{
	assert(gui != nullptr && "The gui was nullptr when the function removeProgressBar was called");

	gui->removeDynamicSprite(progressBarIdPrefix + identifier);
	gui->removeDynamicSprite(identifier);
	gui->removeDynamicText(identifier);
}


void addSlider(InteractiveInterface* gui, std::string identifier, sf::Vector2f pos, unsigned int length) noexcept
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function addSlider was called");

	static constexpr std::string_view sliderBackgroundTextureName{ "__sb" };
	static constexpr std::string_view sliderCursorTextureName{ "__sc" };

	constexpr unsigned int size{ 20 };
	constexpr float outlineThickness{ size / 10.f };

	if (SpriteWrapper::getTexture(sliderBackgroundTextureName) == nullptr)
	{
		SpriteWrapper::createTexture(std::string{ sliderBackgroundTextureName }, loadSolidRectangle(sf::Vector2f{ size, size * size }, -outlineThickness), SpriteWrapper::Reserved::No);
		SpriteWrapper::createTexture(std::string{ sliderCursorTextureName },     loadSolidRectangle(sf::Vector2f{ size * std::numbers::phi, size }, -outlineThickness), SpriteWrapper::Reserved::No);
	}

	const float lengthFactor{ length / 400.f };
	gui->addDynamicSprite(identifier, sliderBackgroundTextureName, pos, sf::Vector2f{ 1.f, lengthFactor });
	gui->addDynamicSprite(sliderIdPrefix + identifier, sliderCursorTextureName, pos);
	gui->addInteractive(identifier);

	sf::Vector2f posText{ gui->getDynamicSprite(sliderIdPrefix + identifier)->getSprite().getGlobalBounds().position };
	posText.x -= outlineThickness;
	gui->addDynamicText(std::move(identifier), "", posText, size, sf::Color::White, "__default", Alignment::Right);
}

double moveSlider(InteractiveInterface* gui, const std::string& identifier, double yPos, int intervals, const GrowthSliderFunction& growth, const UserFunction& user)
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function moveSlider was called");
	assert(growth != nullptr && "The growth function was nullptr when the function moveSlider was called");

	auto* const backgroundSlider{ gui->getDynamicSprite(identifier) };
	auto* const cursorSlider{ gui->getDynamicSprite(sliderIdPrefix + identifier) };
	auto* const textSlider{ gui->getDynamicText(identifier) };

	if (backgroundSlider == nullptr || cursorSlider == nullptr) [[unlikely]]
		throw std::invalid_argument{ "The slider with the identifier " + identifier + " does not exist." };

	const double bias{ backgroundSlider->getSprite().getGlobalBounds().position.y };
	const double length{ backgroundSlider->getSprite().getGlobalBounds().size.y };
	yPos = std::clamp(yPos, bias, bias + length) - bias;

	if (intervals >= 0)
	{
		++intervals; // The intervals exclude the min and max positions, so we add 1 to the number of intervals.
		yPos = (length * round((yPos * intervals) / length)) / intervals; // Rounding to the nearest interval position.
	}

	const double value{ growth(1 - ((yPos) / length)) }; // Get the value of the slider.
	
	yPos += bias;
	cursorSlider->setPosition(sf::Vector2f{ cursorSlider->getSprite().getPosition().x, static_cast<float>(yPos) });
	textSlider->setContent(value);
	textSlider->setPosition(sf::Vector2f{ textSlider->getText().getPosition().x, static_cast<float>(yPos) }); // Aligning the text with the cursor.

	if (user != nullptr)
		user(value); // Call the function associated with the slider.

	return value;
}

void hideSlider(InteractiveInterface* gui, const std::string& identifier, bool hide)
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function hideSlider was called");

	auto* const backPtr{ gui->getDynamicSprite(identifier) };
	auto* const sliderPtr{ gui->getDynamicSprite(sliderIdPrefix + identifier) };
	auto* const textPtr{ gui->getDynamicText(identifier) };

	if (backPtr == nullptr || sliderPtr == nullptr || textPtr == nullptr) [[unlikely]]
		throw std::invalid_argument{ "The slider with the identifier " + identifier + " does not exist." };

	backPtr->hide = hide;
	sliderPtr->hide = hide;
	textPtr->hide = hide;
}

void removeSlider(InteractiveInterface* gui, const std::string& identifier) noexcept
{
	assert(gui != nullptr && "The gui was nullptr when the function removeSlider was called");
	
	gui->removeDynamicSprite(identifier);
	gui->removeDynamicSprite(sliderIdPrefix + identifier);
	gui->removeDynamicText(identifier);
}


/**
 * \brief Creates a texture for a checked mqb box.
 * \complexity O(1).
 *
 * \param[in] scale The size of the rectangle.
 * \param[in] outlineThickness The thickness of the outline. Can be negative -> see SFML doc.
 *
 * \return The texture.
 */
static sf::Texture loadCheckBoxTexture(sf::Vector2f scale, float outlineThickness) noexcept
{
	constexpr sf::Color fillColor{ sf::Color{ 20, 20, 20 } };
	constexpr sf::Color outlineColor{ sf::Color{ 80, 80, 80 } };

	auto texture{ loadSolidRectangle(scale, outlineThickness, fillColor, outlineColor) };
	sf::Image image{ texture.copyToImage() };

	for (unsigned int i{ 0 }; i < image.getSize().x; ++i)
	{
		for (unsigned int j{ 0 }; j < image.getSize().y; ++j)
		{
			if (std::abs(static_cast<int>(i) - static_cast<int>(j)) < std::abs(outlineThickness))
				image.setPixel(sf::Vector2u{ i, j }, outlineColor);
			else if (std::abs(static_cast<int>(image.getSize().x) - static_cast<int>(i) - static_cast<int>(j)) < std::abs(outlineThickness))
				image.setPixel(sf::Vector2u{ i, j }, outlineColor);
		}
	}

	if (!texture.loadFromImage(image))
		texture = sf::Texture{};
	return texture;
}

/**
 * \brief Checks a box
 * \complexity O(1).
 *
 * \param[in] gui The gui that contains the multiple question box.
 * \param[in] identifier The identifier under the form "_mqb_identifier_".
 * \param[in] check The 1-indexed box to check/uncheck.
 * \param[in] multipleChoices `true` if multiple boxes can be checked at once.
 * \param[in] atLeastOne `true` if at least one box must always be checked.
 * 
 * \pre The gui must be valid.
 * \pre The identifier must represent a mqb.
 * \post The box status will be updated.
 * \warning Asserts otherwise.
 */
static void checkBox(InteractiveInterface* gui, const std::string& identifier, unsigned short check, bool multipleChoices, bool atLeastOne) noexcept
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function checkBox was called");

	SpriteWrapper* alreadyCheckedBox{ nullptr };
	SpriteWrapper* curBoxToCheck{ gui->getDynamicSprite(identifier + std::to_string(check)) };
	bool wasAlreadyChecked{ curBoxToCheck->getCurrentTextureIndex() == 1 };

	unsigned short index{ 1 }; // 1-indexed
	auto* box{ gui->getDynamicSprite(identifier + '1') }; 
	ENSURE_VALID_PTR(box != nullptr, "The identifier did not represent a mqb when the function getMQBStatus was called");
	do
	{
		if (index != check && box->getCurrentTextureIndex() == 1) // If the box is checked.
		{
			alreadyCheckedBox = box;
			break;
		}

		box = gui->getDynamicSprite(identifier + std::to_string(++index)); // poor cache locality but not meant to be fast
	} while (box != nullptr);

	if (atLeastOne && wasAlreadyChecked && alreadyCheckedBox == nullptr)
		return; // Only one box checked which happens to be the parameter + at least one box must be checked -> nothing happens

	if (!multipleChoices && !wasAlreadyChecked && alreadyCheckedBox != nullptr) // == 1 because there could be 0 box if atLeastOne == false
		alreadyCheckedBox->switchToNextTexture(); // Not a multiple choice mqb + checking a new box -> uncheck the currently checked box to switch to the new one

	// Reverses the box status
	curBoxToCheck->switchToNextTexture();

	return;
}

void addMQB(InteractiveInterface* gui, const std::string& identifier, sf::Vector2f initPos, sf::Vector2f deltaPos, unsigned short numberOfBoxes, bool multipleChoices, bool atLeastOne, unsigned short defaultCheckedBox) noexcept
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function addMQB was called");
	assert(numberOfBoxes > 0 && "The number of boxes must be greater than 0 in the function addMQB");
	assert(defaultCheckedBox <= numberOfBoxes && "The default checked box must be within range in the function addMQB");
	assert((atLeastOne != true || defaultCheckedBox != 0) && "The mqb can't be completely unchecked and yet it has no default checked box in the function checkBox");
	assert(numberOfBoxes != 1 || atLeastOne != true && "The mqb is useless as it has only one box which can't be unchecked due to the variable atLeastOne being true in the function checkBox");

	static constexpr std::string_view uncheckedMqbTextureName{ "__ub" };
	static constexpr std::string_view checkedMqbTextureName{ "__cb" };
	static constexpr sf::Vector2f boxSize{ 20, 20 };
	static constexpr float outlineThickness{ 2.f };

	if (SpriteWrapper::getTexture(uncheckedMqbTextureName) == nullptr) 
	{	// For the first time only, create the textures.
		SpriteWrapper::createTexture(std::string{ uncheckedMqbTextureName }, loadSolidRectangle(boxSize,  outlineThickness), SpriteWrapper::Reserved::No);
		SpriteWrapper::createTexture(std::string{ checkedMqbTextureName },   loadCheckBoxTexture(boxSize, outlineThickness), SpriteWrapper::Reserved::No);
	}

	sf::Vector2f curPos{ initPos.x - (boxSize.x / 2.f), initPos.y - (boxSize.y / 2.f) }; // The "boxSize / 2" counteracts the origin not being at the center of the sprite.
	const std::string identifierBox{ mqbIdPrefix + identifier + '_' };
	for (unsigned short i{ 1 }; i <= numberOfBoxes; ++i) // 1-indexed
	{
		const std::string identifierBoxTemp{ identifierBox + std::to_string(i) };
		gui->addDynamicSprite(identifierBoxTemp, uncheckedMqbTextureName, curPos, { 1.f, 1.f }, sf::IntRect{}, sf::degrees(0), gui::Alignment::Top | gui::Alignment::Left);
		gui->getDynamicSprite(identifierBoxTemp)->addTexture(checkedMqbTextureName);
		gui->addInteractive(identifierBoxTemp, [identifierBox, i, multipleChoices, atLeastOne] (InteractiveInterface* gui) { checkBox(gui, identifierBox, i, multipleChoices, atLeastOne); });

		if (i == defaultCheckedBox) 
			gui->getDynamicSprite(identifierBoxTemp)->switchToNextTexture();

		curPos += deltaPos;
	} 
}

std::vector<unsigned short> getMQBStatus(InteractiveInterface* gui, const std::string& identifier) noexcept
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function getMQBStatus was called");

	const std::string identifierBox{ mqbIdPrefix + identifier + '_' };

	std::vector<unsigned short> checkedBoxes{}; // The indexes of the currently checked boxes.
	unsigned short index{ 1 };
	auto* box{ gui->getDynamicSprite(identifierBox + '1') }; //1-indexed

	ENSURE_VALID_PTR(box != nullptr, "The identifier did not represent a mqb when the function getMQBStatus was called");

	do
	{
		if (box->getCurrentTextureIndex() == 1) // If the box is checked.
			checkedBoxes.push_back(index);
		box = gui->getDynamicSprite(identifierBox + std::to_string(++index)); // poor cache locality but not meant to be fast
	} while (box != nullptr);

	return checkedBoxes;
}

void hideMQB(InteractiveInterface* gui, const std::string& identifier, bool hide) noexcept
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function hideMQB was called");

	const std::string identifierBox{ mqbIdPrefix + identifier + "_" };

	unsigned short index{ 1 }; // 1-indexed
	auto* mqb{ gui->getDynamicSprite(identifierBox + '1') };
	while (mqb != nullptr)
	{
		mqb->hide = hide;
		mqb = gui->getDynamicSprite(identifierBox + std::to_string(++index)); // poor cache locality but not meant to be fast
	}
}

void removeMQB(InteractiveInterface* gui, const std::string& identifier, unsigned short numberOfBoxes) noexcept
{
	assert(gui != nullptr && "The gui was nullptr when the function removeMQB was called");

	const std::string identifierBox{ mqbIdPrefix + identifier + "_" };
	gui->removeDynamicSprite("efef"); // poor cache locality but not meant to be fast

	for (; numberOfBoxes > 0; --numberOfBoxes) // 1-indexed
		gui->removeDynamicSprite(identifierBox + std::to_string(numberOfBoxes)); // poor cache locality but not meant to be fast
}

bool updateWritingText(TextWrapper* text, char32_t unicodeValue, const WritingFunction& func)
{
	assert(text != nullptr && "The text was nullptr when the function updateWritingText was called");

	sf::String content{ text->getText().getString() };
	bool returnValue{ true };

	if (unicodeValue == 13)
		unicodeValue =  10;

	if (func != nullptr)
		returnValue = func(unicodeValue, content, text);

	if ((unicodeValue == 8) && !content.isEmpty()) // Backspace
		content.erase(content.getSize() - 1);
	else if (unicodeValue == 10 || unicodeValue == 9 || unicodeValue >= 32) // Printable characters
		content.insert(content.getSize(), unicodeValue);

	text->setContent(content);
	return returnValue;
}

bool updateWritingText(MutableInterface* gui, std::string_view identifier, char32_t unicodeValue, const WritingFunction& func)
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function updateWritingText was called");

	auto* const text{ gui->getDynamicText(identifier) };
	if (text == nullptr) [[unlikely]]
		throw std::invalid_argument{ "The text with the identifier " + std::string{ identifier } + " does not exist." };

	return updateWritingText(text, unicodeValue, func);
}

bool basicWritingFunction(char32_t& c, sf::String& str, TextWrapper* txt) noexcept
{
	if (c != 27 && c != '\n')
		return true;

	c = 0; // Effectively disabling the line break by using a none printable character.
	txt->setStyle(sf::Text::Style::Regular);

	if (str.isEmpty())
		str = "0"; // Don't leave it empty

	return false;
}

} // gui namespace