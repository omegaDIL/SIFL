**Summary**<br>
This project is a SFML-based library designed to easily create performant yet visually-rich graphical user interfaces.
Most functions run in (amortized) O(1) time and are safe to call repeatedly, to support fallback behavoir mechanisms.
Therefore, it is very suitable for 2D simulations, games and other apps that heavily depends on visuals but need a user-friendly API<br>
-------------------‐-------------------------------------------------<br>
**Compatibility**<br>
Fully compatible with desktop OS (windows, macOS, linux)<br>
While the library has not been tested on mobile or touch-enabled devices (Android/iOS), there is no inherent reason it would be incompatible. For touch input, simply call updateHovered and pressed when a touch began or moved, and unpressed when the touch ends.<br>
You may need to change the CMakeLists.txt to properly link SFML on your platform.<br>
-------------------‐-------------------------------------------------<br>
**Key Features**<br>
- Fine-grained control over memory management for sf\::Texture, enabling sprite animation, resolution-specific textures, and more.
- Handle of window resizing.
- Pre-made features such as writing over sf\::text, progress bar and more
- Hover detection for any sf::Transformable object
- Simple menu switching system.
- Button creation with custom function bindings for specific events.
- Different interfaces available for different menu types

-------------------‐-------------------------------------------------<br>
**Overview - what you should get familiar with up**<br>
<u>Exceptions:</u><br>
- loadingGraphicalResourceFailure (file: GraphicalResources)<br>

<u>Functions:</u><br>
- **populateGUI** (file: GUI)
- showErrorUsingWindow (file: GUI)
- createTextureFromDrawables (file: GraphicalResources)
- loadFontFromFile (file: GraphicalResources)
- loadTextureFromFile (file: GraphicalResources)

<u>Classes:</u><br>
- **TextWrapper** (file: GraphicalResources)
- **SpriteWrapper** (file: GraphicalResources)
- **BasicInterface** (file: BasicInterface)
- MutableInterface (file: MutableInterface)
- **InteractiveInterface** (file: InteractiveInterface)
- struct InteractiveInterface::Item (file: InteractiveInterface)
- **currentGUI** (file: GUI)

<u>Files:</u><br>
- CompoundElements (divided in 4 sections: progress bar, mqb, slider and writing; each may be useful depending on your use case)

-------------------‐-------------------------------------------------<br>
**Code example**<br>
(see GUI.hpp for full example with populateGUI)<br>
The current implementation of main.cpp/populateGUI are another example of how to use the library.<br>
Change them in your project as you see fit.<br>
```
int main()
{
	sf::Vector2u windowSize{ 1000, 1000 };
	sf::ContextSettings settings{};
	settings.antiAliasingLevel = 64; 
	sf::RenderWindow window{ sf::VideoMode{ windowSize }, "Template sfml 3", sf::State{}, settings };
	
	// Creates interfaces.
	IGUI mainInterface{ &window, 1080 };
	IGUI otherInterface{ &window, 1080 };
	MGUI overlayInterface{ &window, 1080 }; // An overlay interface that will always be drawn. Not mean to be the main one

	// This is used to get information about the currently hovered item.
	GUIPtr curGUI{};
	curGUI = &mainInterface; // Start with the main interface. The operator= is overriden to allow any interface type.
	std::string writingText{ "text1" };  // Identifier of which text is being written to.

	// Populates both interfaces.
	populateGUI(curGUI, writingText, &mainInterface, &otherInterface, &overlayInterface); // Adds all elements to both interfaces. see the code example. above
	mainInterface.lockInterface(); 
	otherInterface.lockInterface();
	overlayInterface.lockInterface();

	while (window.isOpen()) [[likely]]
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) [[unlikely]]
				window.close();

			else if (event->is<sf::Event::Resized>()) [[unlikely]]
				BGUI::windowResized(&window, windowSize); // Resizes the window and the interfaces.

			else if (curGUI.gInteractive != nullptr && event->is<sf::Event::MouseButtonPressed>() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				curGUI.gInteractive->eventPressed(); // Handles button pressing (only for IGUI).

			else if (curGUI.gInteractive != nullptr && event->is<sf::Event::MouseMoved>() && !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) [[likely]]
			{
				curGUI.mainItem = curGUI.gInteractive->eventUpdateHovered(window.mapPixelToCoords(event->getIf<sf::Event::MouseMoved>()->position)); // Updates the hovered item (only for IGUI). The argument is the mouse position (see SFML doc).
				overlayInterface.getDynamicSprite("overlay")->setPosition(window.mapPixelToCoords(event->getIf<sf::Event::MouseMoved>()->position)); // Moves the overlay sprite to follow the mouse.
			}
		
			else if (curGUI.gMutable != nullptr && event->is<sf::Event::TextEntered>() && writingText != "") // See compoundElements.hpp for more info about text writing.
				if (!gui::updateWritingText(&mainInterface, writingText, event->getIf<sf::Event::TextEntered>()->unicode, gui::basicWritingFunction)) 
					writingText = "";
		}


		// First check if we are in the right interface, then check the identifier.

		if (curGUI.gInteractive == &otherInterface && curGUI.mainItem.identifier == "colorChanger") 
			otherInterface.getDynamicSprite("colorChanger")->rotate(sf::degrees(1));
		
		if (curGUI.gInteractive == &otherInterface && curGUI.mainItem.identifier == "slider" && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			gui::moveSlider(&otherInterface, curGUI.mainItem.identifier, window.mapPixelToCoords(sf::Mouse::getPosition(window)).y, 99); // 99 is good value for a slider: the delta interval is exactly 0.01


		window.clear(sf::Color{ 20, 20, 20 });
		curGUI->draw();
		overlayInterface.draw(); // Whatever the current interface is, its elements will always be drawn.
		window.display();
	}

	return 0;
}
```
-------------------‐-------------------------------------------------<br>
**Concerns**<br>
- All features of sf\::sprite and sf\::text are still available
- Does maintain good cache locality with both the hover detection and the drawing function.

-------------------‐-------------------------------------------------<br>
**Limitations & common mistakes**<br>
- Interfaces don't support sf::Shader. (But works with SpriteWrapper)
- The order of drawing is fixed and can't be modified nor fully controlled easily.
- The first time you create a text using any gui type, a default font is loaded. Do it yourself if you want to load it before.

-------------------‐-------------------------------------------------<br>
**Advanced features**<br>
There are a few features that can seem more complex than the rest. While most of them can be "harder" to understand, they are still easy to use. Ranked from hardest to easiest:<br>
- Order of drawing
- Reserved textures
- Switching between interfaces
- Interface locking

<u>Order of drawing</u>:<br>
The order of drawing represents which elements are drawn first within an interface. In sfml, the sooner an element is drawn, the less prioritized the element is if it overlaps. The interfaces store all elements in a vector to improve cache locality, but that means that in order to maintain O(1) removal, it has to do some manipulation. Thus, the actual order can change throughout runtime. Ultimately, you can be sure of 2 things:<br>
- texts are drawn after all sprites<br>
- interactive sprites are drawn before non interactive ones ; and so do the texts.<br>

Then the actual order within them depends on the interface type and what functions you call. BasicInterface is the simplest: the later an element is added, the later it is drawn. MutableInterface is almost identical as BasicInterface except when you remove something. There, it swaps it with the last element to remove it in O(1). That means that the previous last element (which had the highest priority) has now the same position as the element that you just removed. It makes it hard to track but it is still doable. The most complex one is InteractiveInterface. Each vector (sprite + text) is splitted into 2 parts: the interactive first, then the non-interactive. Both parts are always continuous. For example, you might have a vector with 5 elements including 3 interactives. In such a case, the vector contains the 3 interactives at indexes 0,1,2 and then the non interactive at indexes 3 and 4. Therefore and at first, it respects the mutable interface order, until an element is made interactive. When you do so, it swaps that new interactive element with the first element of the non interactive part, making the interactive part (the first part) grow by one, and the none interactive part (the second part) smaller. Thus, interactive are drawn in order they are made interactive, but not in addition order (until removal). The non-interactive order is hence also modified: the element at the beginning of the second part is swapped with the previous position of that interactive element. The difficulty increases when removals are performed. Non-interactives behaves like MutableInterfaces, but interactives on the other hand are (again) more complicated. They are still swapped with the last element but it creates a "hole" in the interactive continuity. That's because the last element of the vector, that is to say the last element of the second part, is non interactive (unless there are no non interactives). It is solved by swapping the last interactive with the non interactive that makes up this hole. Ultimately, when an interactive element is removed, the last non interactive becomes the first non interactive ; and the last interactive gets the position of the previously deleted interactive. To fully understand, see the explanation file<br>
Locking an interface (below) can help you avoid dealing with unexpected swaps. Use another interface if you need <br>

<u>Reserved textures</u>:<br>
Textures within SpriteWrapper are categorized as either<br>
- Reserved
- Non reserved (or put simply 'shared')

The differences lie in the resource lifetime and the number of sprites that use them.<br>
Reserved are textures that can be used by only one instance. They can't be removed using the function removeTexture, but they are removed when the sprite's destructor is called. Shared textures on the other hand can be applied to any amount of sprites. They are removed by you (call the removeTexture function) and not by the destructor of the instances (even if all of them were to be deleted). One sprite can have as many textures as you want, and you can stack reserved textures with shared ones. When a reserved texture is created with createTexture, the first instance to use it becomes its owner. If you try to set a claimed reserved texture to an instance it will either crash (debug mode) or do nothing (release mode). You can technically bypass all verifications in release mode without any ub, except if you delete the owning instance. In that case, the texture will be deleted, possibly crashing your program if other instances still used it.<br>
Loading, unloading and accessing are made by static functions even for reserved textures.<br>

<u>Main interface switching</u>:<br>
In a software, you usually have distinct menus that you can switch to. Here, each menu can be represented by an interface, and in that context, you need to be able to switch to another interface. To do so, you should use the structure currentGUI. This structure consists mainly of 3 public pointers: gBasic, gMutable and gInteractice. currentGUI have an overloaded operator= with all three interface pointers. When you set a currentGUI, it sets its pointers to the targeted instance, except the pointers that point to a derived type. For instance, if you set currentGUI to a mutable interface, the pointers gBasic and gMutable are valid and point to the same instance, but gInteractive is set to nullptr. Note that using the operator= also clears both hovered items: its own and the one of the InteractiveInterface class. currentGUI also provides conversion functions to all interfaces types (so you can directly put a currentGUI instance as an arg). Additionally, currentGUI has an operator-> that allows you to use as if it were a pointer to a basic interface.<br>

Interface locking:<br>
Locking an interface is done by a simple call to .lockInterface(). It prevents any removal, addition for a specific interface in order to ensure that its pointers remain stable. Note that you can still modify any dynamic element because it does not move around pointers. However, you can't make an existing element interactive as it may require to swap elements under the hood, thus failing to ensure pointer stability. That stability can improve memory usage (see doc for each interface type) and performance by not calling getDynamics which have pointer redirections (see std::unordered_map::find()). Keep in mind that locking or not an interface heavily depends on your use case, so not all interfaces might benefit from it. Sometimes, it can't even be locked (but you can optimize that by using layer of interfaces). 