#include "InteractiveInterface.hpp"
#include <cstdint>	

namespace gui
{

InteractiveInterface::InteractiveInterface(sf::RenderWindow* window, unsigned int relativeScalingDefinition) noexcept
	: MutableInterface{ window, relativeScalingDefinition }, m_interactiveTextButtons{}, m_interactiveSpriteButtons{}, m_writingTextIdentifier{ "" }, m_writingFunction{nullptr}
{
	static constexpr std::string_view textureName{ "__plainGrey" }; 
	if (SpriteWrapper::getTexture(textureName) == nullptr) [[unlikely]]
	{
		sf::Image writingCursorImage{ sf::Vector2u{ 1u, 1u }, sf::Color{ 80, 80, 80 } };
		sf::Texture writingCursorTexture{ std::move(writingCursorImage) };
		writingCursorTexture.setRepeated(true);

		SpriteWrapper::createTexture(std::string{ textureName }, std::move(writingCursorTexture), SpriteWrapper::Reserved::No);
	}

	addDynamicSprite(std::string{ writingCursorIdentifier }, textureName, { 0, 0 }, { 5.f, 25.f }, sf::IntRect{}, sf::degrees(0), Alignment::Left | Alignment::Top);
	getDynamicSprite(writingCursorIdentifier)->hide = true;
}

InteractiveInterface::~InteractiveInterface() noexcept
{
	m_interactiveTextButtons.clear();
	m_interactiveSpriteButtons.clear();
	m_writingTextIdentifier.clear();
	m_writingFunction = nullptr;

	if (s_hoveredItem.igui == this)
		s_hoveredItem = Item{};
}

void InteractiveInterface::removeDynamicText(std::string_view identifier) noexcept
{
	const auto mapIterator{ m_dynamicTexts.find(identifier) };

	if (mapIterator == m_dynamicTexts.end())
		return; // No sprite with that identifier.

	MutableInterface::removeDynamicText(identifier);

	const size_t index{ mapIterator->second };
	const size_t m_endTextInteractives{ m_interactiveTextButtons.size() };
	if (index < m_endTextInteractives) // Only for interactive texts.
	{
		swapElement(index, m_endTextInteractives - 1, m_texts, m_dynamicTexts, m_indexesForEachDynamicTexts);

		std::swap(m_interactiveTextButtons[index], m_interactiveTextButtons[m_endTextInteractives - 1]);
		m_interactiveTextButtons.pop_back(); // Remove the last element as it is now empty.
	}

	if (m_writingTextIdentifier == identifier) [[unlikely]]
		setWritingText(""); // Checks if the writing text was not the removed text.

	if (s_hoveredItem.igui == this && s_hoveredItem.identifier == identifier) [[unlikely]]
		s_hoveredItem = Item{}; // Checks if the hovered text was not the removed text.
}

void InteractiveInterface::removeDynamicSprite(std::string_view identifier) noexcept
{
	const auto mapIterator{ m_dynamicSprites.find(identifier) };

	if (mapIterator == m_dynamicSprites.end())
		return; // No sprite with that identifier.

	MutableInterface::removeDynamicSprite(identifier);

	const size_t index{ mapIterator->second };
	const size_t m_endSpriteInteractives{ m_interactiveSpriteButtons.size() };
	if (index < m_endSpriteInteractives) // Only for interactive sprites.
	{
		swapElement(index, m_endSpriteInteractives - 1, m_sprites, m_dynamicSprites, m_indexesForEachDynamicSprites);

		std::swap(m_interactiveSpriteButtons[index], m_interactiveSpriteButtons[m_endSpriteInteractives - 1]);
		m_interactiveSpriteButtons.pop_back(); // Remove the last element as it is now empty.
	}

	if (s_hoveredItem.igui == this && s_hoveredItem.identifier == identifier) [[unlikely]]
		s_hoveredItem = Item{}; // Checks if the hovered sprite was not the removed sprite.
}
 
void InteractiveInterface::addInteractive(std::string_view identifier, ButtonFunction function) noexcept
{
	const auto textIterator	 { m_dynamicTexts.find(identifier)   };
	const auto spriteIterator{ m_dynamicSprites.find(identifier) };

	// The check "textIterator->second >= m_interactiveTextButtons.size()" verifies if the text is not already an interactive one.
	const bool newInteractiveText  { textIterator   != m_dynamicTexts.end()   && textIterator->second   >= m_interactiveTextButtons.size()	 };
	const bool newInteractiveSprite{ spriteIterator != m_dynamicSprites.end() && spriteIterator->second >= m_interactiveSpriteButtons.size() };

	if (newInteractiveText)
	{	// Swapping the element with the element next to the other interactives to maintain good cache locality
		swapElement(textIterator->second, m_interactiveTextButtons.size(), m_texts, m_dynamicTexts, m_indexesForEachDynamicTexts); // Swapping asserts if locked
		m_interactiveTextButtons.push_back((newInteractiveSprite) ? std::move(function) : function); // Do copy if we also need it for a sprite.
	}	// push_back adds one to m_interactiveTextButtons.size()

	// Same goes here
	if (newInteractiveSprite)
	{
		swapElement(spriteIterator->second, m_interactiveSpriteButtons.size(), m_sprites, m_dynamicSprites, m_indexesForEachDynamicSprites);
		m_interactiveSpriteButtons.push_back(std::move(function)); // Some compilers might trigger a false positive warning for use of a moved-from object. 
	}
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
	cursor->setPosition(sf::Vector2f{ rect.position.x + rect.size.x, rect.position.y + rect.size.y/2.f});
	cursor->hide = false;
}

InteractiveInterface::Item InteractiveInterface::eventUpdateHovered(BasicInterface* activeGUI, sf::Vector2f cursorPos) noexcept
{
	ENSURE_VALID_PTR(activeGUI, "The gui was nullptr when the function updateHovered was called in InteractiveInterface");

	InteractiveInterface* const igui{ dynamic_cast<InteractiveInterface*>(activeGUI) };

	// Chances are that the hovered item is the same as previously.
	// Type is set to None if no item wa hovered, or if the gui was nullptr
	if (s_hoveredItem.igui == igui)
	{
		if (s_hoveredItem.type == Item::Type::Text   && igui->getDynamicText(s_hoveredItem.identifier)->getText().getGlobalBounds().contains(cursorPos))
			return s_hoveredItem; // No need to check again if the hovered item is the same.
	
		if (s_hoveredItem.type == Item::Type::Sprite && igui->getDynamicSprite(s_hoveredItem.identifier)->getSprite().getGlobalBounds().contains(cursorPos))
			return s_hoveredItem; // No need to check again if the hovered item is the same.
	}

	s_hoveredItem = Item{};

	if (igui == nullptr)
		return s_hoveredItem;

	//FIX: when an interface is locked, line 165 crashed because m_indexesForEachDynamicSprites is empty (reduced memory usage).
	const size_t m_endSpriteInteractives{ igui->m_interactiveSpriteButtons.size() };
	for (size_t i{ 0 }; i < m_endSpriteInteractives; ++i)
	{
		SpriteWrapper& sprite{ igui->m_sprites[i] };
		if (!sprite.hide && sprite.getSprite().getGlobalBounds().contains(cursorPos))
		{
			s_hoveredItem = Item{ igui, igui->m_indexesForEachDynamicSprites.at(i)->first, Item::Type::Sprite, &igui->m_interactiveSpriteButtons[i] };
			break; // A text might be on top of the sprite, so we need to check it too.
		}
	}

	const size_t m_endTextInteractives{ igui->m_interactiveTextButtons.size() };
	for (size_t i{ 0 }; i < m_endTextInteractives; ++i)
	{
		TextWrapper& text{ igui->m_texts[i] };
		if (!text.hide && text.getText().getGlobalBounds().contains(cursorPos))
		{
			s_hoveredItem = Item{ igui, igui->m_indexesForEachDynamicTexts.at(i)->first, Item::Type::Text, &igui->m_interactiveTextButtons[i]};
			break;
		}
	}

	return s_hoveredItem;
}

void InteractiveInterface::eventPressed(BasicInterface* activeGUI) noexcept
{
	ENSURE_VALID_PTR(activeGUI, "The gui was nullptr when the function pressed was called in InteractiveInterface");

	InteractiveInterface* const igui{ dynamic_cast<InteractiveInterface*>(activeGUI) };

	if (igui != nullptr && igui == s_hoveredItem.igui) 
		(*s_hoveredItem.m_button)(igui);
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
	cursor->setPosition(sf::Vector2f{ rect.position.x + rect.size.x, rect.position.y});
}

} // gui namespace