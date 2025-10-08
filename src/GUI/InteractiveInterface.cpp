#include "InteractiveInterface.hpp"
#include <cstdint>
#include <iostream>

namespace gui
{

InteractiveInterface::~InteractiveInterface() noexcept
{
	m_allButtons.clear();

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
	if (s_hoveredItem.igui == igui && !std::holds_alternative<std::monostate>(s_hoveredItem.ptr))
	{
		bool holdText{ std::holds_alternative<TextWrapper*>(s_hoveredItem.ptr) }; // If false, it holds a SpriteWrapper*
		if ((holdText &&  igui->m_lockState && std::get<TextWrapper*>(s_hoveredItem.ptr)->getText().getGlobalBounds().contains(cursorPos)) // Almost guaranteed to not have cache misses
		|| (!holdText &&  igui->m_lockState && std::get<SpriteWrapper*>(s_hoveredItem.ptr)->getSprite().getGlobalBounds().contains(cursorPos)) // Almost guaranteed to not have cache misses
		||  (holdText && !igui->m_lockState && igui->getDynamicText(s_hoveredItem.identifier)->getText().getGlobalBounds().contains(cursorPos))
		|| (!holdText && !igui->m_lockState && igui->getDynamicSprite(s_hoveredItem.identifier)->getSprite().getGlobalBounds().contains(cursorPos)))
		return s_hoveredItem;
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

} // gui namespace