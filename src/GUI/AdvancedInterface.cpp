#include "AdvancedInterface.hpp"
#include <algorithm>
#include <cmath>
#ifndef NDEBUG
#include <cassert>
#endif //NDEBUG

namespace gui
{

inline const static std::string sliderIdPrefixe{ "_sc_" };
inline const static std::string mqbIdPrefixe{ "_mqb_" };

sf::Texture loadSolidRectange(sf::Vector2f scale, float outlineThickness) noexcept
{
	static constexpr sf::Color fillColor{ 20, 20, 20 };
	static constexpr sf::Color outlineColor{ 80, 80, 80 };

	sf::RectangleShape shape{ scale };
	shape.setFillColor(fillColor);
	shape.setOutlineColor(outlineColor);
	shape.setOutlineThickness(-std::abs(outlineThickness));

	return createTextureFromDrawables(shape);
}

sf::Texture loadCheckBoxTexture(sf::Vector2f scale, float outlineThickness) noexcept
{
	static constexpr sf::Color outlineColor{ 80, 80, 80 };
	
	auto texture{ loadSolidRectange(scale, outlineThickness) };
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

void addSlider(InteractiveInterface* gui, std::string identifier, sf::Vector2f pos, unsigned int length) noexcept
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function addSlider was called");

	static constexpr std::string_view sliderBackgroundTextureName{ "__sb" };
	static constexpr std::string_view sliderCursorTextureName{ "__sc" };
	constexpr unsigned int size{ 20 };
	constexpr unsigned int thicknessFactor{ 20 };
	constexpr float goldenRatioFactor{ 1.618f };
	constexpr float outlineThickness{ size / 10.f };

	if (SpriteWrapper::getTexture(sliderBackgroundTextureName) == nullptr)
	{
		SpriteWrapper::createTexture(std::string{ sliderBackgroundTextureName }, loadSolidRectange(sf::Vector2f{ size, size * thicknessFactor }, outlineThickness), SpriteWrapper::Reserved::No);
		SpriteWrapper::createTexture(std::string{ sliderCursorTextureName }, loadSolidRectange(sf::Vector2f{ size * goldenRatioFactor, size }, outlineThickness), SpriteWrapper::Reserved::No);
	}

	gui->addDynamicSprite(identifier, sliderBackgroundTextureName, pos, sf::Vector2f{ 1.f, length / 300.f });
	gui->addDynamicSprite(sliderIdPrefixe + identifier, sliderCursorTextureName, pos);
	gui->addInteractive(identifier);

	sf::Vector2f posText{ gui->getDynamicSprite(sliderIdPrefixe + identifier)->getSprite().getGlobalBounds().position };
	posText.x -= outlineThickness;
	gui->addDynamicText(std::move(identifier), "", posText, 20, sf::Color::White, "__default", Alignment::Right);
}

void hideSlider(InteractiveInterface* gui, const std::string& identifier, bool hide)
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function hideSlider was called");

	auto* const backPtr  { gui->getDynamicSprite(identifier) };
	auto* const sliderPtr{ gui->getDynamicSprite(sliderIdPrefixe + identifier) };
	auto* const textPtr  { gui->getDynamicText(identifier) };

	if (backPtr == nullptr || sliderPtr == nullptr || textPtr == nullptr)
		throw std::invalid_argument{ "The slider with the identifier " + identifier + " does not exist." };

	backPtr->hide   = hide;
	sliderPtr->hide = hide;
	textPtr->hide   = hide;
}

float moveSlider(InteractiveInterface* gui, const std::string& identifier, float yPos, int intervals, const GrowthSliderFunction& growth, const UserFunction& user)
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function moveSlider was called");

	auto* const backgroundSlider{ gui->getDynamicSprite(identifier) };
	auto* const cursorSlider{ gui->getDynamicSprite(sliderIdPrefixe + identifier) };
	auto* const textSlider{ gui->getDynamicText(identifier) };

	if (backgroundSlider == nullptr || cursorSlider == nullptr)
		throw std::invalid_argument{ "The slider with the identifier " + identifier + " does not exist." };

	const float bias{ backgroundSlider->getSprite().getGlobalBounds().position.y };
	const float length{ backgroundSlider->getSprite().getGlobalBounds().size.y };
	yPos = std::clamp(yPos, bias, bias + length) - bias;

	if (intervals >= 0)
	{
		++intervals; // The intervals exclude the min and max positions, so we add 1 to the number of intervals.
		yPos = (length * round((yPos * intervals) / length)) / intervals; // Rounding to the nearest interval position.
	}

	const float value{ growth(1 - ((yPos) / length)) }; // Get the value of the slider.
	
	yPos += bias;
	cursorSlider->setPosition(sf::Vector2f{ cursorSlider->getSprite().getPosition().x, yPos });
	textSlider->setContent(value);
	textSlider->setPosition(sf::Vector2f{ textSlider->getText().getPosition().x, yPos }); // Aligning the text with the cursor.

	if (user != nullptr)
		user(value); // Call the function associated with the slider.

	return value;
}


void addMQB(InteractiveInterface* gui, std::string identifier, sf::Vector2f posInit, sf::Vector2f posDelta, short numberOfBoxes, short defaultCheckedBox) noexcept
{
	assert(numberOfBoxes > 0 && "The number of boxes must be greater than 0 in the function addMQB");
	assert(defaultCheckedBox <= numberOfBoxes && "The default checked box must be within range in the function addMQB");
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function addMQB was called");

	static constexpr std::string_view uncheckedMqbTextureName{ "__ub" };
	static constexpr std::string_view checkedMqbTextureName{ "__cb" };
	static constexpr sf::Vector2f boxSize{ 20, 20 };
	static constexpr float outlineThickness{ 2.f };

	if (SpriteWrapper::getTexture(uncheckedMqbTextureName) == nullptr) 
	{	// For the first time only, create the textures.
		SpriteWrapper::createTexture(std::string{ uncheckedMqbTextureName }, loadSolidRectange(boxSize,   outlineThickness), SpriteWrapper::Reserved::No);
		SpriteWrapper::createTexture(std::string{ checkedMqbTextureName },   loadCheckBoxTexture(boxSize, outlineThickness), SpriteWrapper::Reserved::No);
	}

	sf::Vector2f curPos{ posInit.x - (boxSize.x / 2.f), posInit.y - (boxSize.y / 2.f) }; // The "boxSize / 2" counteracts the origin not being at the center of the sprite.
	const std::string identifierBox{ mqbIdPrefixe + std::move(identifier) + '_' };
	for (unsigned int i{ 0 }; i < numberOfBoxes; ++i)
	{
		const std::string identifierBoxTemp{ identifierBox + std::to_string(i) };
		gui->addDynamicSprite(identifierBoxTemp, uncheckedMqbTextureName, curPos, {1.f, 1.f}, sf::IntRect{}, sf::degrees(0), gui::Alignment::Top | gui::Alignment::Left);
		gui->getDynamicSprite(identifierBoxTemp)->addTexture(checkedMqbTextureName);
		gui->addInteractive(identifierBoxTemp);

		if (i == defaultCheckedBox) 
			gui->getDynamicSprite(identifierBoxTemp)->switchToNextTexture();

		curPos += posDelta;
	}
}

void hideMQB(InteractiveInterface* gui, const std::string& identifier, bool hide)
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function hideMQB was called");

	const std::string identifierBox{ mqbIdPrefixe + identifier + "_" };

	auto* mqb{ gui->getDynamicSprite(identifierBox + '0') };
	if (mqb == nullptr)
		throw std::invalid_argument{ "The mqb with the identifier " + identifier + " does not exist." };

	unsigned int index{ 0 };

	do
	{
		mqb->hide = hide;
		mqb = gui->getDynamicSprite(identifierBox + std::to_string(++index));
	} while (mqb != nullptr);
}

std::optional<mqbInfo> isMqb(InteractiveInterface* gui, std::string identifier) noexcept
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function isMqb was called");
	
	// Minimum size is 8: _mqb_a_1
	if (gui->getDynamicSprite(identifier) == nullptr || identifier.size() < 8 || !identifier.starts_with(mqbIdPrefixe)) 
		return std::nullopt;

	std::string reverse{};
	for (size_t i{ identifier.size() - 1 }; reverse.empty() || reverse.back() != '_'; --i)
	{	// Get the number at the end of the identifier.
		reverse.push_back(identifier[i]);
		identifier.pop_back();
	}

	reverse.pop_back(); // Removes the '_'
	std::reverse(reverse.begin(), reverse.end());
	short number{ static_cast<short>(std::stoi(reverse)) };

	std::string mqbIdentifier{};
	for (size_t i{ mqbIdPrefixe.size() }; i < identifier.size(); ++i)
		mqbIdentifier.push_back(identifier[i]);

	return std::make_optional(std::make_pair(std::move(mqbIdentifier), number));
}

std::vector<short> checkBox(InteractiveInterface* gui, mqbInfo mqb, bool multipleChoices, bool atLeastOne)
{
	ENSURE_VALID_PTR(gui, "The gui was nullptr when the function checkBox was called");
	assert(!mqb.first.empty() && "The identifier of the mqb is empty in the function checkBox");

	std::string identifierBox{ mqbIdPrefixe + mqb.first + "_" };
	auto* box{ gui->getDynamicSprite(identifierBox + '0') };

	if (box == nullptr)
		throw std::invalid_argument{ "The mqb with the identifier " + mqb.first + " does not exist." };

	std::vector<SpriteWrapper*> boxes{};
	std::vector<short> checkedBoxes{};
	const short check{ mqb.second };
	unsigned int index{ 0 };

	do
	{
		boxes.push_back(box);
		if (box->getCurrentTextureIndex() == 1) // If the box is checked.
			checkedBoxes.push_back(static_cast<short>(index));
		box = gui->getDynamicSprite(identifierBox + std::to_string(++index));
	} while (box != nullptr);

	assert(check > -1 && check < boxes.size() && "The check parameter is out of range in the function checkBox ");
	assert(boxes.size() != 1 || atLeastOne != true && "The mqb is useless as it has only one box which can't be unchecked due to the variable atLeastOne being true in the function checkBox");
	assert(atLeastOne != true || !checkedBoxes.empty() && "The mqb can't be completely unchecked and yet it has no default checked box in the function checkBox");
	assert(multipleChoices != false || checkedBoxes.size() <= 1 && "The mqb can't have multiple choices and yet more than one box is checked in the function checkBox");

	if (atLeastOne == true && checkedBoxes.size() == 1 && checkedBoxes[0] == check)
		return checkedBoxes; // Nothing changes because at least one box must be checked and the box to check is already checked.

	bool wasChecked{ std::find(checkedBoxes.begin(), checkedBoxes.end(), check) != checkedBoxes.end() };

	if (multipleChoices == false && checkedBoxes.size() == 1) // Although multiple choices is false, The number of box could still be 0 (atLeastOne).
	{	// Can't have more than one box checked so we reset the mqb.
		boxes[checkedBoxes[0]]->switchToNextTexture(); // Uncheck the previously checked box.
		checkedBoxes.clear();
	}		
	
	if (multipleChoices == false && atLeastOne == false && wasChecked)
		return checkedBoxes;

	boxes[check]->switchToNextTexture(); // Check or uncheck the box at index 'check'.
	checkedBoxes.erase(std::remove(checkedBoxes.begin(), checkedBoxes.end(), check), checkedBoxes.end());

	if (!wasChecked)
		checkedBoxes.push_back(check);
	
	return checkedBoxes;
}

}