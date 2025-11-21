#include "BasicInterface.hpp"
#include <utility>

namespace gui
{

BasicInterface::BasicInterface(sf::RenderWindow* window, unsigned int relativeScalingDefinition) noexcept
	: m_window{ window }, m_texts{}, m_sprites{}, m_relativeScalingDefinition{ relativeScalingDefinition }, m_lockState{ false }
{
	ENSURE_SFML_WINDOW_VALIDITY(m_window, "Precondition violated; the window is invalid when the constructor of BasicInterface was called");

	// Add this interface to the collection.
	auto& interfaces{ s_allInterfaces[window] };
	interfaces.push_back(this);
} 

BasicInterface::BasicInterface(BasicInterface&& other) noexcept
	: m_window{ other.m_window }, m_texts{ std::move(other.m_texts) }, m_sprites{ std::move(other.m_sprites) }, m_relativeScalingDefinition{ other.m_relativeScalingDefinition }, m_lockState{ other.m_lockState }
{
	assert((!other.m_lockState) && "Precondition violated; the moved-from interface is locked when the move constructor of BasicInterface was called");

	// Replacing the moved-from object by the new object in the collection anymore.
	auto& interfaces{ s_allInterfaces.find(other.m_window)->second };
	auto it = std::find_if(interfaces.begin(), interfaces.end(), [&other](BasicInterface* x) { return x == &other; });
	*it = this;

	// Leave the moved-from object in a valid state
	other.m_window = nullptr;
}

BasicInterface& BasicInterface::operator=(BasicInterface&& other) noexcept
{
	assert((!other.m_lockState) && "Precondition violated; the moved-from interface is locked when the move assignment operator of BasicInterface was called");
	assert((!m_lockState) && "Precondition violated; the current interface is locked when the move assignment operator of BasicInterface was called");

	// If the two interfaces are associated with different windows,
	// we need to update the global static collection s_allInterfaces accordingly.
	if (other.m_window != m_window)
	{
		// 1. Reassign the mapping of `other` in s_allInterfaces:
		//    - We're about to move `other` into `*this`, so the pointer to `other`
		//      in s_allInterfaces should now point to `this`.
		auto& interfacesOther{ s_allInterfaces.find(other.m_window)->second };
		auto itOther = std::find_if(interfacesOther.begin(), interfacesOther.end(), [&other](BasicInterface* x) { return x == &other; });
		*itOther = this;

		// As they have different windows, we do not need to worry about collision between those two loops

		// 2. Reassign the mapping of `this` in s_allInterfaces:
		//    - After the move, `other` will hold the old state of `*this`.
		//      So the entry in s_allInterfaces that previously pointed to `this`
		//      under m_window should now point to `other`.
		auto& interfacesThis{ s_allInterfaces.find(this->m_window)->second };
		auto itThis = std::find_if(interfacesThis.begin(), interfacesThis.end(), [this](BasicInterface* x) { return x == this; });
		*itThis = &other;
	}

	// Else: If both `this` and `other` share the same window, we don’t update s_allInterfaces.
	// Why?
	// - Each pointer (`this` and `other`) stays associated with the same window after the swap.
	// - We’re just exchanging internal data; the identity (address) of the interface tied to the window doesn’t change.
	// - No need to touch the mapping — it's still valid and consistent.

	// Swap the internal state between *this and other.
	// This includes all relevant members to fully transfer ownership.
	std::swap(this->m_window, other.m_window);
	std::swap(this->m_texts, other.m_texts);
	std::swap(this->m_sprites, other.m_sprites);
	std::swap(this->m_relativeScalingDefinition, other.m_relativeScalingDefinition);
	other.m_window = nullptr;
	// Both lock states are false;

	return *this;
}

BasicInterface::~BasicInterface() noexcept
{
	auto interfacesThis{ s_allInterfaces.find(this->m_window) };
	auto itThis = std::find_if(interfacesThis->second.begin(), interfacesThis->second.end(), [this](BasicInterface* x) { return x == this; });
	interfacesThis->second.erase(itThis); // Erasing this interface from the collection.

	if (interfacesThis->second.empty()) // if there are no interfaces for that window, we can erase it. 
		s_allInterfaces.erase(interfacesThis);

	m_window = nullptr;
	m_sprites.clear();
	m_texts.clear();
}

void BasicInterface::addSprite(std::string_view textureName, sf::Vector2f pos, sf::Vector2f scale, sf::IntRect rect, sf::Angle rot, Alignment alignment, sf::Color color)
{
	ENSURE_SFML_WINDOW_VALIDITY(m_window, "The window is invalid when the function addSprite of BasicInterface was called");
	assert((!m_lockState) && "Precondition violated; the interface is locked when the function addSprite of BasicInterface was called");

	float relativeScalingValue{ 1.f };
	if (m_relativeScalingDefinition != 0)
		relativeScalingValue *= static_cast<float>(std::min(m_window->getSize().x, m_window->getSize().y)) / static_cast<float>(m_relativeScalingDefinition);

	SpriteWrapper newSprite{ textureName, pos, scale * relativeScalingValue, rect, rot, alignment, color };
	m_sprites.push_back(std::move(newSprite));
}

void BasicInterface::addSprite(sf::Texture texture, sf::Vector2f pos, sf::Vector2f scale, sf::IntRect rect, sf::Angle rot, Alignment alignment, sf::Color color) noexcept
{
	// We need to craft a unique texture name to avoid collision with other existing or futur textures.
	// Since the user didn't want a specific name, he lost the capacity to retrieve the texture. 

	std::ostringstream craftUniqueName{};
	craftUniqueName << "_" << m_sprites.size();
	craftUniqueName << "_" << reinterpret_cast<size_t>(this);
	craftUniqueName << "_" << m_texts.size();

	while (SpriteWrapper::getTexture(craftUniqueName.str()) != nullptr)
		craftUniqueName << "_"; // If this exact name was given, we add an underscore until it is available.

	SpriteWrapper::createTexture(craftUniqueName.str(), std::move(texture), SpriteWrapper::Reserved::Yes);
	addSprite(craftUniqueName.str(), pos, scale, rect, rot, alignment, color); 
}

void BasicInterface::draw() const noexcept
{
	ENSURE_SFML_WINDOW_VALIDITY(m_window, "The window is invalid when the function draw of BasicInterface was called");

	for (const auto& sprite : m_sprites)
		if (!sprite.hide)
			m_window->draw(sprite.getSprite());

	for (const auto& text : m_texts)
		if (!text.hide)
			m_window->draw(text.getText());
}

void BasicInterface::lockInterface(bool shrinkToFit) noexcept
{
	m_lockState = true;

	if (shrinkToFit)
	{
		m_texts.shrink_to_fit();
		m_sprites.shrink_to_fit();
	}
}

void BasicInterface::proportionKeeper(sf::RenderWindow* resizedWindow, sf::Vector2f scaleFactor, float relativeMinAxisScale) noexcept
{	
	ENSURE_SFML_WINDOW_VALIDITY(resizedWindow, "Precondition violated; The window is invalid when the function proportionKeeper of BasicInterface was called");
	ENSURE_NOT_ZERO(relativeMinAxisScale, "Precondition violated; relativeMinAxisScale is equal to 0 when the function proportionKeeper of BasicInterface was called");
	ENSURE_NOT_ZERO(scaleFactor.x, "Precondition violated; scale factor is equal to 0 when the function proportionKeeper of BasicInterface was called");
	ENSURE_NOT_ZERO(scaleFactor.y, "Precondition violated; scale factor is equal to 0 when the function proportionKeeper of BasicInterface was called");

	const sf::Vector2f minScaling2f{ relativeMinAxisScale, relativeMinAxisScale };

	auto& interfaces{ s_allInterfaces.find(resizedWindow)->second }; // All interfaces associated with the resized window.
	for (auto it{ interfaces.begin() }; it != interfaces.end(); ++it)
	{
		auto* curInterface{ *it };

		if (curInterface->m_relativeScalingDefinition == 0)
			continue; // No scaling definition, so no need to scale.

		// Updating texts.
		sf::Vector2f pos{};
		for (auto& text : curInterface->m_texts)
		{
			text.scale(minScaling2f);

			pos = text.getText().getPosition();
			text.setPosition(sf::Vector2f{ pos.x * scaleFactor.x, pos.y * scaleFactor.y }); // Update position to match the new scale.
		}

		// Updating sprites.
		for (auto& sprite : curInterface->m_sprites)
		{
			sprite.scale(minScaling2f);

			pos = sprite.getSprite().getPosition();
			sprite.setPosition(sf::Vector2f{ pos.x * scaleFactor.x, pos.y * scaleFactor.y }); // Update position to match the new scale.
		}
	}
}

} // gui namespace