/*******************************************************************
 * \file   InteractiveInterface.hpp, InteractiveInterface.cpp
 * \brief  Declare a gui in which the user can interact with, e.g. writing or pressing stuff.
 *
 * \author OmegaDIL.
 * \date   July 2025.
 *
 * \note These files depend on the SFML library.
 *********************************************************************/

#ifndef INTERACTIVEINTERFACE_HPP
#define INTERACTIVEINTERFACE_HPP

#include "MutableInterface.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <functional>
#include <cstdint>

namespace gui
{

/**
 * \brief  Manages an interface with changeable contents: texts and shapes.
 * 
 * Interactive elements are designed to respond to mouse hover events. When an element is hovered,
 * the `eventUpdateHovered` function returns a pointer to it.
 * Buttons are a special type of interactive element with an attached function. This function is
 * triggered when the button is pressed.
 * 
 * Move functions are disabled if the interface is locked.
 *
 * A code example is provided at the end of the file.
 *
 * \note This class stores UI components ; it will use a considerable amount of memory.
 * \note Each mutable elements might consume a little more memory than their fixed counterparts.
 *		 For Interactive elements, they are very memory efficient unless they have a button (see 
 *		 `std::function`)
 * \note Do not try to replicate the interactive feature: you WILL end up having very bad performances.
 *		 The functions `eventUpdateHovered` and `addInteractives` are, on the other hand, more optimized
 *		 for cache locality.
 * \warning Avoid deleting the `sf::RenderWindow` passed as an argument while this class is using it.
 * 
 * \see `MutableInterface`.
 */
class InteractiveInterface : public MutableInterface
{
public:  

	using ButtonFunction = std::function<void(InteractiveInterface*)>;

	/**
	 * \brief Represents the interactive currently hovered.
	 */
	struct Item
	{			
	public:

		std::string identifier; // The item that is hovered, either a text or a sprite.
		std::variant<std::monostate, TextWrapper*, SpriteWrapper*> ptr; // A pointer to the actual transformable.
		// Watch out if the interface is not locked, this pointer might be invalidated at any time.

		constexpr Item(std::string id, SpriteWrapper* spritePtr) noexcept
			: identifier{ id }, ptr{ spritePtr } {}

		constexpr Item(std::string id, TextWrapper* textPtr) noexcept
			: identifier{ id }, ptr{ textPtr } {}

		constexpr Item() noexcept 
			: identifier{ "" }, ptr{ std::monostate{} } {}

		constexpr Item(const Item& item) noexcept = default;
		constexpr Item(Item&& item) noexcept = default;
		constexpr Item& operator=(const Item& item) noexcept = default;
		constexpr Item& operator=(Item&& item) noexcept = default;
		constexpr ~Item() noexcept = default;
	};


	/**
	 * \brief Constructs the graphical interface.
	 * \complexity O(1)
	 *
	 * \param[in,out] window A valid pointer to the SFML window where interface elements will be rendered.
	 * \param[in] relativeScalingDefinition A scaling baseline to ensure consistent visual proportions across
	 *			  various window sizes. Scales of `sf::Transformable` elements are adjusted based on the window size
	 *			  relative to this reference value:
	 *
	 *			  - If the smallest dimension (width or height) of the window equals `relativeScalingDefinition`,
	 *				no scaling is applied (scaling factor is 1.0).
	 *			  - If the window is smaller, the scaling factor becomes less than 1.0, making elements appear smaller.
	 *			  - If the window is larger, the scaling factor becomes greater than 1.0, making elements appear larger.
	 *
	 *			  For example, if `relativeScalingDefinition` is set to 1080:
	 *			  - A window of size 1920x1080 (16/9) → factor = 1080/1080 = 1.0
	 *			  - A window of size 540x960   (9/16) → factor = 1080/540  = 0.5
	 *			  - A window of size 3840x2160 (16/9) → factor = 1080/2160 = 2.0
	 *			  - A window of size 7680x2160 (32/9) → factor = 1080/2160 = 2.0
	 *
	 *			  If set to 0, no scaling is applied regardless of the window size.
	 *
	 * \pre `window` must be a valid.
	 * \post An interface is constructed.
	 * \warning The program will assert otherwise.
	 */
	inline explicit InteractiveInterface(sf::RenderWindow* window, unsigned int relativeScalingDefinition = 1080) noexcept
		: MutableInterface{ window, relativeScalingDefinition }, m_nbOfButtonTexts{}, m_nbOfButtonSprites{}, m_allButtons{}
	{}

	InteractiveInterface() noexcept = default;
	InteractiveInterface(const InteractiveInterface&) noexcept = delete;
	InteractiveInterface(InteractiveInterface&&) noexcept = default;
	InteractiveInterface& operator=(const InteractiveInterface&) noexcept = delete;
	InteractiveInterface& operator=(InteractiveInterface&&) noexcept = default;
	virtual ~InteractiveInterface() noexcept = default;


	/**
	 * \brief Removes a text from the GUI. No effet if not there.
	 * \complexity O(1).
	 *
	 * \param[in] name: The identifier of the text.
	 * 
	 * \pre The interface must not be locked.
	 * \post The text is removed from the interface.
	 * \warning The program will assert otherwise.
	 *
	 * \see `removeDynamicSprite`.
	 */
	virtual void removeDynamicText(std::string_view identifier) noexcept override;
	
	/**
	 * \brief Removes a sprite from the GUI. No effet if not there.
	 * \complexity O(1).
	 *
	 * \param[in] name: The identifier of the sprite.
	 *
	 * \pre The interface must not be locked.
	 * \post The sprite is removed from the interface.
	 * \warning The program will assert otherwise.
	 * 
	 * \see `removeDynamicText`.
	 */
	virtual void removeDynamicSprite(std::string_view identifier) noexcept override;

	/**
	 * \brief Turns an existing transformable into an interactive element.
	 * \complexity O(1)
	 *
	 * If both a sprite and a text share the same identifier, the interactive status (and button
	 * behavior) is applied to both. But, they act as two independent buttons: deleting one does
	 * not affect the other.
	 *
	 * If both transformables are already interactive, nothing happens. However, if
	 * only one is interactive (e.g., because the other was added later), the status
	 * is still applied to the non-interactive one.
	 * 
	 * If neither a sprite or a text has the corresponding identifier, the function does nothing for
	 * fallback behavior.
	 *
	 * \param[in] identifier The ID of the text to turn into a button.
	 * \param[in] function   The function executed when the button is pressed.
	 *
	 * \note Creating a button is not recommended for performance-critical code or for complex functions
	 *		 requiring many arguments. Check for the return value of `eventUpdateHovered` instead.
	 * \note May invalidate any pointers of any TransformableWrapper in this gui.
	 * \note Interactives are drawn below non-interactive elements if they overlap.
	 * 
	 * \pre The interface must not be locked.
	 * \post Elements will be turned into interactive ones.
	 * \warning The program will assert otherwise.
	 */
	void addInteractive(std::string identifier, ButtonFunction function = nullptr) noexcept;

	/**
	 * \brief Prevents any addition of new elements to the interface.
	 * \complexity O(1) if shrinkToFit is false
	 * \complexity O(N + M) otherwise. N is the number of texts and M the number of sprites.
	 *
	 * Once you have added all your elements, you can lock the interface to avoid futur modifications.
	 * Contrary to MutableInterface and similar to BasicInterface, memory can only be reduced if
	 * `shrinkToFit` is true. But be aware that `shrinkToFit` can be time consuming if you have a lot
	 * of elements. In debug mode, a crash will be triggered if a modification is attempted after locking.
	 * 
	 * IT APPLIES TO addInteractive() AS WELL.
	 * 
	 * In addition, the stability of pointers is guaranteed in all locked mutable interfaces. Hence, it
	 * does not affect editing elements that are already added, or straightly using getter functions
	 * after locking.
	 * 
	 * It helps with eventUpdateHovered() optimizations as well, as it can directly use the pointer of
	 * the previously hovered element to check if the mouse is still over it, avoiding a full recheck
	 * 
	 * \param[in] shrinkToFit If true, the function will call `shrink_to_fit` on both the texts and
	 *						  sprites.
	 */
	virtual void lockInterface(bool shrinkToFit = true) noexcept override;


	/**
	 * \brief Updates the hovered element when the mouse mouve, if the gui is interactive.	 
	 * \complexity O(1), if the interactive element stills contain the mouse the same as previously.
	 * \complexity O(N), otherwise; where N is the number of interactive elements in your active interface.
	 * 
	 * You may choose not to update the hovered element on every mouse move—for example,
	 * only when the mouse button is not pressed as well.
	 * 
	 * Does not check elements that are hidden.
	 * 
	 * Texts are prioritized over sprites if they overlap. If two texts/sprites overlap, one will be
	 * chosen (from the user perspective) in an undefined manner (but deterministic, therefore it will
	 * always be the same one if the situation is the same).
	 *
	 * This function is cache-optimized.
	 * 
	 * \param[out] activeGUI: The current GUI. No effect if not interactive
	 * \param[in]  cursorPos: The position of the cursor/touch event WITHIN the window's view.
	 * \param[in]  forceRecheck: If true, forces a full recheck of all interactive elements. By default
	 *							 in a locked interface, the last element is rechecked the next call. If true, It would
	 *							 skip that recheck and directly do a full recheck. In an unlocked interface, it would
	 *							 do nothing. Change it to true if you don't have a lot of interactive elements and want
	 *							 to skip that recheck (which may be more costly than a full recheck in that case).
	 *                            
	 * \return The item that is currently hovered.
	 * 
	 * \pre activeGUI must not be nullptr
	 * \post The hovered element will be updated according to the gui
	 * \warning Asserts if activeGUI is nullptr.
	 */
	static Item eventUpdateHovered(InteractiveInterface* activeGUI, sf::Vector2f cursorPos, bool forceRecheck = false) noexcept;

	/**
	 * \brief Activate the button if it is pressed
	 * \complexity O(1).
	 *
	 * \param[out] activeGUI: The current GUI.
	 *
	 * \note If there are no buttons, you don't have to call this function.
	 * 
	 * \pre activeGUI must not be nullptr
	 * \post The hovered button will be executed (or nothing happens)
	 * \warning Asserts if activeGUI is nullptr.
	 */
	static void eventPressed(InteractiveInterface* activeGUI) noexcept;

	/**
	 * \brief Resets the hovered item. Useful if the current displayed interface changes
	 * \complexity O(1).
	 * 
	 * \return An empty item.
	 */
	constexpr inline static Item resetHovered() noexcept{ return (s_hoveredItem = Item{}); }
	
protected:
	 
	inline static Item s_hoveredItem{}; // The current item that is hovered.
 
private:

	size_t m_nbOfButtonTexts; // The number of interactive texts.
	size_t m_nbOfButtonSprites; // The number of interactive sprites.

	using ButtonElement = std::pair<ButtonFunction, short>;
	std::unordered_map<std::string, ButtonElement, TransparentHash, TransparentEqual> m_allButtons; // Contains all buttons.
};


/**
 * \code
 * int main()
 * {
 *     sf::Vector2u windowSize{ 1000, 1000 };
 *     sf::RenderWindow window{ sf::VideoMode{ windowSize }, "Template sfml 3" };
 *     
 *     // Creates the two interfaces.
 *     gui::InteractiveInterface mainInterface{ &window, 1080 };
 *     gui::InteractiveInterface otherInterface{ &window, 1080 };
 *     gui::InteractiveInterface* currentInterface{ &mainInterface };
 *
 *     mainInterface.addText("Welcome to the GUI!", sf::Vector2f{ 500, 200 }, 48, sf::Color{ 255, 255, 255 }, "__default", gui::Alignment::Center, sf::Text::Bold | sf::Text::Underlined);
 *
 *     mainInterface.addDynamicText("other", "switch", { 500, 800 });
 *     mainInterface.addInteractive("other", [&otherInterface, &currentInterface](IGUI*) mutable { currentInterface = &otherInterface; gui::InteractiveInterface::resetHovered(); });
 *     // DO NOT FORGET TO RESET HOVERED ITEM WHEN SWITCHING INTERFACES.
 *
 *     otherInterface.addDynamicText("main", "switch", { 500, 500 });
 *     otherInterface.addInteractive("main", [&mainInterface, &currentInterface](IGUI*) mutable { currentInterface = &mainInterface; gui::InteractiveInterface::resetHovered(); });
 *  
 *     sf::RectangleShape rect{ { 50, 50 } };
 *     otherInterface.addDynamicSprite("colorChanger", gui::createTextureFromDrawables(rect), sf::Vector2f{ 500, 850 });
 *     otherInterface.addInteractive("colorChanger");
 *
 *     otherInterface.lockInterface();
 *     mainInterface.lockInterface();
 *
 *     gui::InteractiveInterface::Item currentItem{};
 *
 *     while (window.isOpen()) [[likely]]
 *     {
 *         while (const std::optional event = window.pollEvent())
 *         {
 *             if (event->is<sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) [[unlikely]]
 *                 window.close();
 *                 
 *             else if (event->is<sf::Event::Resized>()) [[unlikely]]
 *                 gui::BasicInterface::windowResized(&window, windowSize); // Resizes the window and the interfaces.
 *                 
 *             else if (event->is<sf::Event::MouseButtonPressed>() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
 *                 gui::InteractiveInterface::eventPressed(currentInterface); // Handles button pressing.
 *
 *             else if (event->is<sf::Event::MouseMoved>() && !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) [[likely]]
 *                 currentItem = gui::InteractiveInterface::eventUpdateHovered(currentInterface, window.mapPixelToCoords(event->getIf<sf::Event::MouseMoved>()->position)); // Updates the hovered item. The argument is the mouse position (see SFML doc).
 *          
 *             // When the interface changes, you have to reset the hovered item by calling gui::InteractiveInterface::resetHovered() like it is done at lines 20 and 24.
 *         }
 *
 *         if (currentInterface == &otherInterface && currentItem.identifier == "colorChanger")
 *             otherInterface.getDynamicSprite("colorChanger")->rotate(sf::degrees(1));
 *
 *         // The common way is to first check if the current interface is the right one, then check the identifier.
 *
 *         window.clear();
 *         currentInterface->draw();
 *         window.display();
 *     }
 *
 *     return 0;
 * }
 * \endcode
 */

} // gui namespace

#endif //INTERACTIVEINTERFACE_HPP