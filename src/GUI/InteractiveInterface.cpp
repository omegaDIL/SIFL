#include "InteractiveInterface.hpp"
#include <cstdint>
#include <iostream>

namespace gui
{

InteractiveInterface::InteractiveInterface(sf::RenderWindow* window, unsigned int relativeScalingDefinition) noexcept
	: MutableInterface{ window, relativeScalingDefinition }, m_nbOfButtonTexts{}, m_nbOfButtonSprites{}, m_allButtons{}, m_writingTextIdentifier{ "" }, m_writingFunction{ nullptr }
{
	static constexpr std::string_view textureName{ "__plainGrey" }; 
	if (SpriteWrapper::getTexture(textureName) == nullptr) [[unlikely]]
	{
		sf::Image writingCursorImage{ sf::Vector2u{ 1u, 1u }, sf::Color{ 80, 80, 80 } };
		sf::Texture writingCursorTexture{ std::move(writingCursorImage) };
		writingCursorTexture.setRepeated(true);

		SpriteWrapper::createTexture(std::string{ textureName }, std::move(writingCursorTexture), SpriteWrapper::Reserved::No);
	}

	addDynamicSprite(std::string{ writingCursorIdentifier }, textureName, { 0, 0 }, { 5.f, 25.f }, sf::IntRect{}, sf::degrees(0), Alignment::Left);
	getDynamicSprite(writingCursorIdentifier)->hide = true;
}

InteractiveInterface::~InteractiveInterface() noexcept
{
	m_allButtons.clear(); 
	m_writingTextIdentifier.clear();
	m_writingFunction = nullptr;

	if (s_hoveredItem.igui == this)
		s_hoveredItem = Item{};
}

void InteractiveInterface::removeDynamicText(std::string_view identifier) noexcept
{
	const auto mapIterator{ m_dynamicTexts.find(identifier) };

	if (mapIterator == m_dynamicTexts.end())
		return; // No text with that identifier.

	if (s_hoveredItem.igui == this && s_hoveredItem.identifier == identifier) [[unlikely]]
		s_hoveredItem = Item{}; // Checks if the hovered text was not the removed text.

	if (m_writingTextIdentifier == identifier) [[unlikely]]
		setWritingText(""); // Checks if the writing text was not the removed text.

	const size_t index{ mapIterator->second };

	MutableInterface::removeDynamicText(identifier);

	if (index >= m_nbOfButtonTexts) // Not an interactive text.
		return;

	swapElement(index, --m_nbOfButtonTexts, m_texts, m_dynamicTexts, m_indexesForEachDynamicTexts); // Guaranteeing the contiguity of interactive texts. 

	auto buttonIterator{ m_allButtons.find(identifier) };
	--buttonIterator->second.second; // Now one less button that can use the function.
	if (buttonIterator->second.second <= 0) // If a sprite has the same identifier and is interactive, it would be equal to 1.
		m_allButtons.erase(buttonIterator);
}

void InteractiveInterface::removeDynamicSprite(std::string_view identifier) noexcept
{
	const auto mapIterator{ m_dynamicSprites.find(identifier) };

	if (mapIterator == m_dynamicSprites.end())
		return; // No sprite with that identifier.

	if (s_hoveredItem.igui == this && s_hoveredItem.identifier == identifier) [[unlikely]]
		s_hoveredItem = Item{}; // Checks if the hovered sprite was not the removed sprite.

	const size_t index{ mapIterator->second };

	MutableInterface::removeDynamicSprite(identifier);

	if (index >= m_nbOfButtonSprites) // Not an interactive sprite.
		return;

	swapElement(index, --m_nbOfButtonSprites, m_sprites, m_dynamicSprites, m_indexesForEachDynamicSprites); // Guaranteeing the contiguity of interactive sprites. 

	auto buttonIterator{ m_allButtons.find(identifier) };
	--buttonIterator->second.second; // Now one less button that can use the function.
	if (buttonIterator->second.second <= 0) // If a text has the same identifier and is interactive, it would be equal to 1.
		m_allButtons.erase(buttonIterator);
}
 
void InteractiveInterface::addInteractive(std::string identifier, ButtonFunction function) noexcept
{
	const auto textIterator	 { m_dynamicTexts.find(identifier)   };
	const auto spriteIterator{ m_dynamicSprites.find(identifier) };

	const bool doesTextExist  { textIterator   != m_dynamicTexts.end()   };
	const bool doesSpriteExist{ spriteIterator != m_dynamicSprites.end() };

	// The check "textIterator->second >= m_nb" verifies if the text is not already an interactive.
	// We guarantee that all interactive elements are at the beginning of the vector by swapping with the
	// element at index m_nbOfButtonTexts, before adding one to report the new number of interactive texts.
	if (doesTextExist && textIterator->second >= m_nbOfButtonTexts)
		swapElement(textIterator->second, m_nbOfButtonTexts++, m_texts, m_dynamicTexts, m_indexesForEachDynamicTexts); // Swapping asserts if the interface is locked

	// Same goes here
	if (doesSpriteExist && spriteIterator->second >= m_nbOfButtonSprites)
		swapElement(spriteIterator->second, m_nbOfButtonSprites++, m_sprites, m_dynamicSprites, m_indexesForEachDynamicSprites);
	
	short elemsThatUseFunction{ static_cast<short>(doesSpriteExist) + static_cast<short>(doesTextExist) };
	m_allButtons.insert_or_assign(std::move(identifier), std::make_pair(std::move(function), elemsThatUseFunction));
}

void InteractiveInterface::setWritingText(std::string_view identifier, WritableFunction function) noexcept
{
	auto* writingText{ getDynamicText(m_writingTextIdentifier) };
	if (writingText != nullptr && writingText->getText().getGlobalBounds().size.x == 0)
		writingText->setContent(emptinessWritingCharacters); // Ensure to avoid leaving the previous text empty and not clickable.
	
	SpriteWrapper* const cursor{ getDynamicSprite(writingCursorIdentifier) };

	if (identifier == "")
	{	// Disable writing
		cursor->hide = true;
		m_writingTextIdentifier = "";
		m_writingFunction = nullptr;
		return;
	}

	m_writingTextIdentifier = std::string{ identifier };
	m_writingFunction = function;

	writingText = getDynamicText(identifier);
	ENSURE_VALID_PTR(writingText, "Precondition violated; The identifier was not found in the function setWritingText in InteractiveInterface");

	const sf::FloatRect rect{ writingText->getText().getGlobalBounds() };
	float YSizeOfCursor{ cursor->getSprite().getGlobalBounds().size.y };
	cursor->scale(sf::Vector2f{ 1.f, rect.size.y / YSizeOfCursor });
	cursor->setPosition(sf::Vector2f{ rect.position.x + rect.size.x, rect.position.y + rect.size.y*0.5f});
	cursor->hide = false;
}

void InteractiveInterface::lockInterface(bool shrinkToFit) noexcept
{
	BasicInterface::lockInterface(shrinkToFit);
	// MutableInterface::lockInterface would clear 'm_indexesForEachDynamicTexts' and 
	// 'm_indexesForEachDynamicSprites', which we need for interactives.
}

InteractiveInterface::Item InteractiveInterface::eventUpdateHovered(InteractiveInterface* igui, sf::Vector2f cursorPos) noexcept
{
	ENSURE_VALID_PTR(igui, "The gui was nullptr when the function updateHovered was called in InteractiveInterface");

	// Chances are that the hovered item is the same as previously between one frame and the other.
	// Type is set to None if no item wa hovered, or if the gui was nullptr
	if (s_hoveredItem.igui == igui && !std::holds_alternative<std::monostate>(s_hoveredItem.ptr))
	{
		bool holdText{ std::holds_alternative<TextWrapper*>(s_hoveredItem.ptr) }; // If false, it holds a SpriteWrapper*
		if ((holdText &&  igui->m_lockState && std::get<TextWrapper*>(s_hoveredItem.ptr)->getText().getGlobalBounds().contains(cursorPos)) // Almost guaranteed to not have cache misses
		||  (holdText && !igui->m_lockState && igui->getDynamicText(s_hoveredItem.identifier)->getText().getGlobalBounds().contains(cursorPos))
		|| (!holdText &&  igui->m_lockState && std::get<SpriteWrapper*>(s_hoveredItem.ptr)->getSprite().getGlobalBounds().contains(cursorPos)) // Almost guaranteed to not have cache misses
		|| (!holdText && !igui->m_lockState && igui->getDynamicSprite(s_hoveredItem.identifier)->getSprite().getGlobalBounds().contains(cursorPos)))
		return s_hoveredItem; // No need to check again if the hovered item is the same.
	}

	s_hoveredItem = Item{};

	for (size_t i{ 0 }; i < igui->m_nbOfButtonTexts; ++i)
	{
		TextWrapper& text{ igui->m_texts[i] };
		if (!text.hide && text.getText().getGlobalBounds().contains(cursorPos)) [[unlikely]] // The vast majority of the time, no text is hovered.
		{
			s_hoveredItem = Item{ igui, igui->m_indexesForEachDynamicTexts.at(i)->first, &text };
			return s_hoveredItem;
		}
	}

	for (size_t i{ 0 }; i < igui->m_nbOfButtonSprites; ++i)
	{
		SpriteWrapper& sprite{ igui->m_sprites[i] };
		if (!sprite.hide && sprite.getSprite().getGlobalBounds().contains(cursorPos)) [[unlikely]] // The vast majority of the time, no sprite is hovered.
		{
			s_hoveredItem = Item{ igui, igui->m_indexesForEachDynamicSprites.at(i)->first, &sprite };
 			break;
		}
	}

	return s_hoveredItem;
}

InteractiveInterface::Item InteractiveInterface::eventUpdateHovered(BasicInterface* activeGUI, sf::Vector2f cursorPos) noexcept
{
	ENSURE_VALID_PTR(activeGUI, "The gui was nullptr when the function updateHovered was called in InteractiveInterface");

	auto* const igui{ dynamic_cast<InteractiveInterface*>(activeGUI) };
	return (s_hoveredItem = ((igui == nullptr) ? Item{} : eventUpdateHovered(igui, cursorPos)));
}

void InteractiveInterface::eventPressed(BasicInterface* activeGUI) noexcept
{
	ENSURE_VALID_PTR(activeGUI, "The gui was nullptr when the function pressed was called in InteractiveInterface");

	InteractiveInterface* const igui{ dynamic_cast<InteractiveInterface*>(activeGUI) };

	if (igui == nullptr || igui != s_hoveredItem.igui)
		return;

	auto iterator{ igui->m_allButtons.find(s_hoveredItem.identifier) };

	if (iterator == igui->m_allButtons.end())
		return;

	ButtonFunction* const buttonFunction{ &iterator->second.first };
	if (*buttonFunction != nullptr)
		(*buttonFunction)(igui);
}

void InteractiveInterface::textEntered(BasicInterface* activeGUI, char32_t character) noexcept
{
	ENSURE_VALID_PTR(activeGUI, "The gui was nullptr when the function textEntered was called in InteractiveInterface");

	InteractiveInterface* const gui{ dynamic_cast<InteractiveInterface*>(activeGUI) };

	if (gui == nullptr || gui->m_writingTextIdentifier == "")
		return;

	if (character == gui->exitWritingCharacter)
	{
		gui->setWritingText("");
		return;
	}

	auto* writingText{ gui->getDynamicText(gui->m_writingTextIdentifier) };
	ENSURE_VALID_PTR(writingText, "The identifier for the writingText was not found in the function textEntered in InteractiveInterface");
	std::string text{ writingText->getText().getString() };

	static constexpr char32_t backspaceCharacter{ 0x0008 };
	if (character == backspaceCharacter)
	{
		if(!text.empty())
			text.pop_back();
	}
	else [[likely]]
	{	
		text.push_back(character);

		if (gui->m_writingFunction != nullptr) // Call the writing function.
			gui->m_writingFunction(gui, character, text);
	}
	writingText->setContent(text);

	SpriteWrapper* const cursor{ gui->getDynamicSprite(writingCursorIdentifier) };
	sf::FloatRect rect{ writingText->getText().getGlobalBounds() };
	cursor->setPosition(sf::Vector2f{ rect.position.x + rect.size.x, rect.position.y + rect.size.y * 0.5f });
}

} // gui namespace