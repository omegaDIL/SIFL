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
 * Once you have added all your elements, you can lock the interface to avoid futur modifications.
 * Locking reduces time consumption of hover detection, and CAN reduce memory usage a little bit.
 * The locking state also ensure stability for pointers, hence you won't be able to use the move
 * functions. However, all pointers returned by getter functions will remain valid. The lock state
 * does not affect editing elements that are already added. 
 * You may not be able to lock the interface if you often add and remove elements. You can try to
 * hide them instead, but it is not always the best way. Sometimes removing elements can save memory.
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
	constexpr inline explicit InteractiveInterface(sf::RenderWindow* window, unsigned int relativeScalingDefinition = 1080) noexcept
		: MutableInterface{ window, relativeScalingDefinition }, m_nbOfButtonTexts{}, m_nbOfButtonSprites{}, m_allButtons{}
	{}

	InteractiveInterface() noexcept = delete;
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
	 * Contrary to MutableInterface::lockInterface but similar to BasicInterface::lockInterface, memory can only be saved if shrinkToFit is true.
	 * But be aware that `shrinkToFit` can be time consuming if you have a lot of elements. 
	 * However, it does speed up interaction checks (even if shrinkToFit is false).
	 * The stability of pointers is, of course, guaranteed as well.
	 *
	 * \param[in] shrinkToFit If true, the function will call `shrink_to_fit` on both the texts and
	 *						  sprites.
	 * 
	 * \note Don't forget that it also applies to addInteractive() 
	 */
	constexpr virtual void lockInterface(bool shrinkToFit = true) noexcept override;


	/**
	 * \brief Updates the hovered element when the mouse mouve, if the gui is interactive.	 
	 * \complexity O(1), if the interactive element is the same as previously.
	 * \complexity O(N), otherwise; where N is the number of interactive elements in your active interface.
	 * 
	 * You may choose not to update the hovered element on every mouse move—for example,
	 * only when the mouse button is not pressed as well.
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
	 *
	 * \return The item that is currently hovered.
	 * 
	 * \pre activeGUI must not be nullptr
	 * \post The hovered element will be updated according to the gui
	 * \warning Asserts if activeGUI is nullptr.
	 */
	static Item eventUpdateHovered(InteractiveInterface* activeGUI, sf::Vector2f cursorPos) noexcept;

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
	constexpr static void eventPressed(InteractiveInterface* activeGUI) noexcept;

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

} // gui namespace

#endif //INTERACTIVEINTERFACE_HPP