# SFML INTERFACE FRAMEWORK LAYER
Thanks for checking out SILF 1.0.2!<br>

---
## Summary
SIFL stands for SFML Interface Framework Layer and is a SFML-based library designed to easily create performant yet visually-rich graphical user interfaces.
Most functions run in (amortized) O(1) time and are safe to call repeatedly, to support fallback behavior mechanisms.
Therefore, it is very suitable for 2D simulations, games and other apps that heavily depends on visuals but need a user-friendly API<br>

---
## Author & License
This library was created by Lesaffre Ismael, under the pseudonym "[omegaDIL](https://github.com/omegaDIL)".<br>
email: Contact me at <lesaffreismael@proton.me><br>
repo: Find SIFL on GitHub [here](https://github.com/omegaDIL/SIFL).

This project is licensed under the Boost software license, version 1.0. See the LICENSE.txt file for details.<br>
Simply put, you can use, reproduce, display, distribute, execute, and transmit the software, and prepare derivative works of it, for commercial or non-commercial purposes, free of charge, as long as you keep the copyright notice and license text when distributing the source code.

---
## Compatibility
Fully compatible with desktop OS (windows, macOS, linux)<br>
While the library has not been tested on mobile or touch-enabled devices (Android/iOS), there is no inherent reason it would be incompatible. For touch input, simply call updateHovered and pressed when a touch began or moved, and unpressed when the touch ends.<br>
You may need to change the CMakeLists.txt to properly link SFML on your platform.<br>

---
## Key features
- Fine-grained control over memory management for sf\::Texture, enabling sprite animation, resolution-specific textures, and more.
- Handle of window resizing.
- Pre-made features such as writing over sf\::text, progress bar and more
- Hover detection for any sf::Transformable object
- Simple menu switching system.
- Button creation with custom function bindings for specific events.
- Different interfaces available for different menu types

---
### Set up
First of all, this library requires x64 SFML and C++20 or higher installed on your system.

You have then two options to install the library:
1. Copy and paste the content of the src/SIFL folder in your own project.
2. Use fetch content with CMake (recommended).

If you choose the first option, you will have to set up everything yourself, including sfml. You will also need to adapt your include directives to point to your own local directory where you have installed SIFL. 

If you choose the second option (recommended), you will have to edit your CMakeLists a little bit. Below is how you do it.
```cmake
# First, C++20 or higher must be set.
set(CMAKE_CXX_STANDARD 20) 

# ...

include(FetchContent) # Not needed if you already do it.

# The actual library
FetchContent_Declare(
    SIFL
    GIT_REPOSITORY https://github.com/omegaDIL/SIFL.git
    GIT_TAG        v1.0.2
)
FetchContent_MakeAvailable(SIFL)

# ...

# add_executable(YourProject main.cpp) # This is your executable/library. 
target_link_libraries(YourProject PRIVATE SIFL::SIFL) # After "add_executable/library". "YourProject" should be replaced by the actual name of your project.

# ...
```
However, you may encounter this error (usually on Linux). This happens because SFML tries to find its dependencies for its network components.
```
CMake Error at /usr/share/cmake-4.4/Modules/FindPackageHandleStandardArgs.cmake:XXX (message):
  Could NOT find MbedTLS (missing: MBEDTLS_INCLUDE_DIR MBEDTLS_LIBRARY
  MBEDX509_LIBRARY MBEDCRYPTO_LIBRARY)
```
You have two possibilities: either install MbedTLS if you need the network capability of SFML, or add the following line in the |`CMakeLists.txt` file

```
FetchContent_Declare(
        SIFL
		# blabla
) # The declare instruction is here
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE) # This must be placed "FetchContent_MakeAvailable(SIFL)"
FetchContent_MakeAvailable(SIFL)
```

If you chose that option with FetchContent, your `#include` directives should contain the name of the folder `SIFL\` followed by the specific file such as `#include <SIFL/GUI.hpp>` or `#include <SIFL\BasicInterface.hpp>`. Note that the GUI.hpp file contains includes the whole library.

Functions ```loadTextureFromFile``` and ```loadFontFromFile``` in file GraphicalResources.hpp have a default value corresponding to the path to go to load resources. It is the folder assets of the SIFL library by default, which has a preinstalled font. You may change the folder yourself if you want another place to load your textures from.<br>
However, when using texts, the library will try to load `defaultFont.ttf` by default. Be sure to have a font named like that if you decided to change the folder.

The library is implemented in the namespace `gui`.

The library uses `using` directives to shorten the name of the gui classes (BGUI, MGUI, IGUI). It is not used anywhere in the library, so you can rename them if you encounter naming conflict. They are not defined inb the `gui` namespace.

The function `populateGUI()` (defined in `GUI.cpp`) is where you could add all your interface elements. You may modify it to fit your project needs, or remove it to do that somewhere else. You decide.

You SHOULD NOT modify anything else unless you are 100% sure<br>

You can use the file example.cpp as a starting point for your project ("tests/executable/example.cpp").<br>

---
## Overview - How to learn
Before continuing reading, make sure you are familiar with SFML basics, especially sf\::RenderWindow, sf\::Sprite, sf\::Text, sf\::Texture and sf\::Font.<br>

From now on, you should focus on understanding the components within the list below. It is an exhaustive list, sorted by type, of what you need to know (other components exist, but are used internally). They are the core building blocks to use it properly.<br>
Once you see how they work, you should go reading the code example of, in order, BasicInterface, MutableInterface, InteractiveInterface and the GUI.hpp/GUIPtr class example. Then, you can read the code example at the following section of this readme to see how to use everything together.<br>
There is also some code example in GraphicalResources and CompoundElements that may be useful depending on your use case.<br>

<u>Exceptions:</u><br>
- loadingGraphicalResourceFailure (file: GraphicalResources)

<u>Classes:</u><br>
- **TextWrapper** (file: GraphicalResources)
- **SpriteWrapper** (file: GraphicalResources)
- **BasicInterface** (file: BasicInterface)
- MutableInterface (file: MutableInterface)
- **InteractiveInterface** (file: InteractiveInterface)
- struct InteractiveInterface::Item (file: InteractiveInterface)
- **currentGUI** (file: GUI)

<u>Functions:</u><br>
- **populateGUI** (file: GUI)
- showErrorUsingWindow (file: GUI)
- createTextureFromDrawables (file: GraphicalResources)
- loadFontFromFile (file: GraphicalResources)
- loadTextureFromFile (file: GraphicalResources)

<u>Files:</u><br>
- CompoundElements (divided in 4 sections: progress bar, mqb, slider and writing; each may be useful depending on your use case)

---
## Code example
This is the same example as in GUI.hpp. The current implementation of example.cpp/populateGUI are another example of how to use the library.<br>
Change them in your project as you see fit.<br>
```
int main()
{
	sf::ContextSettings settings{};
	settings.antiAliasingLevel = 16; 
	sf::RenderWindow window{ sf::VideoMode{ { 1000, 1000 } }, "Template sfml 3", sf::State{}, settings };
	sf::View currentView{ window.getView() };

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
				BGUI::windowResized(&window, currentView); // Resizes the window and the interfaces.

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

---
## Concerns
- All features of sf\::sprite and sf\::text are still available
- Does maintain good cache locality with both the hover detection and the drawing function.
- All asserts are only active in debug mode to avoid performance loss in release mode. You can technically bypass all of them in release mode, but obviously that may lead to undefined behavior like crashes.

---
## Limitations & common mistakes
- It does not use modules to support C++20 compilers that did not implement them yet.
- Interfaces don't support sf::Shader. (But works with SpriteWrapper)
- The order of drawing is fixed and can't be modified nor fully controlled easily.
- The first time you create a text using any gui type, a default font is loaded. Do it yourself if you want to load it before.

---
## Advanced features
There are a few features that can seem more complex than the rest. While most of them can be "harder" to understand, they are still easy to use. Ranked from hardest to easiest:<br>
- Order of drawing
- Reserved textures
- Switching between interfaces
- Interface locking

### Order of drawing:
The drawing order determines which interface elements appear on top when they overlap. In SFML, elements drawn later have higher visual priority. Interfaces store their elements in a vector for cache locality, but because removals must remain O(1), this vector is frequently rearranged. As a result, the exact internal order can change at runtime. <br>
However, two rules are always guaranteed:
- All texts are drawn **after** all sprites.
- Interactive sprites are **drawn** before non-interactive ones (and the same applies to texts).

Beyond that, the order inside each category depends on the interface type and on the operations you perform.<br>

<u>**BasicInterface**</u>:<br>
The simplest behavior: elements are drawn in the order they were added. Adding something later means it will be drawn later.<br>

<u>**MutableInterface**</u>:<br>
Same as BasicInterface, except for removals. When you remove an element, it is swapped with the last one to allow O(1) deletion. This means the former last element (the most recently added) suddenly takes the position of the removed one. Tracking the exact order becomes harder, but still manageable.<br>

<u>**InteractiveInterface**</u>:<br>
The most complex case. Each vector (sprites and texts) is split into two continuous segments:
- All interactive elements (first part).
- All non-interactive elements (second part).

Example: if there are 5 elements with three interactives, the interactive ones occupy indices 0–2, and the non-interactive ones occupy indices 3–4.

Initially, the order behaves like a MutableInterface. But things change when you toggle interactivity: when an element becomes interactive, it is swapped with the first non-interactive element. This grows the interactive segment and shrinks the non-interactive segment.<br>
Interactive elements are therefore drawn in the order they **become** interactive, not in the order they were added.

Because of this swap, the non-interactive ordering also shifts: the non-interactive element that was at the start of the second segment moves to the old position of the newly-interactive element.

Removals complicate things further:
- Removing a non-interactive element behaves like MutableInterface.
- Removing an interactive element is different: it is swapped with the last element of the vector—which is normally non-interactive. This breaks the continuity of the interactive segment, creating a “hole”.

To fix this hole, the last interactive element is swapped into it, while the swapped-out non-interactive moves to the start of the non-interactive segment. In short: When an interactive element is removed, the last non-interactive becomes the first non-interactive, and the last interactive moves into the removed element’s position.<br>

For a complete understanding, refer to the explanation file.<br>
Locking an interface (see below) can help you avoid unexpected swaps. Use a different interface type if you need stricter ordering guarantees.

### Reserved textures:
Textures within SpriteWrapper are categorized as either<br>
- Reserved
- Non reserved (or put simply 'shared')

The differences lie in the resource lifetime and the number of sprites that use them.

Reserved are textures that can be used by only one instance. They can't be removed using the function removeTexture, but they are removed when the sprite's destructor is called. Shared textures on the other hand can be applied to any amount of sprites. They are removed by you (call the removeTexture function) and not by the destructor of the instances (even if all of them were to be deleted). One sprite can have as many textures as you want, and you can stack reserved textures with shared ones. When a reserved texture is created with createTexture, the first instance to use it becomes its owner. If you try to set a claimed reserved texture to an instance it will either crash (debug mode) or do nothing (release mode). You can technically bypass all verifications in release mode without any ub, except if you delete the owning instance. In that case, the texture will be deleted, possibly crashing your program if other instances still used it. Those other instances would have acted like the texture was shared.

Loading, unloading and accessing are made by static functions even for reserved textures.

### Main interface switching:
In a software, you usually have distinct menus that you can switch to. Here, each menu can be represented by an interface, and in that context, you need to be able to switch to another interface.

To do so, you should use the structure GUIPtr. This structure consists mainly of 3 public pointers: gBasic, gMutable and gInteractive. GUIPtr has an overloaded operator= with all three interface pointers. When you set a GUIPtr, it sets its pointers to the targeted instance, except the pointers that point to a derived type. For instance, if you set an instance to a mutable interface, the pointers gBasic and gMutable are valid and point to the same instance, but gInteractive is set to nullptr.<br>

### Interface locking:
Locking an interface is done by a simple call to .lockInterface(). It prevents any removal, addition for a specific interface in order to ensure that its pointers remain stable.

You can still modify any dynamic element because it does not move around pointers. However, you can't make an existing element interactive as it may require to swap elements under the hood, thus failing to ensure pointer stability. That stability can improve memory usage (see doc for each interface type) and performance by not calling getDynamics, which have pointer redirections (see std\::unordered_map\::find()).

Locking is recommended. Keep in mind that since locking prevents additions and removals, it may not be suitable for all use cases. You can still split the interface, locking the static one and leaving the dynamic ones unlocked.<br>

--- 
## Maintenance policy
The library is maintained by Lesaffre Ismael.

This project will be maintained as long as it is useful to the author. While it is realistically impossible to say that this library will be maintained forever, SIFL is guaranteed to be maintained at least until September 2029 for now. It will likely extend beyond this point.<br>
If the project stop being maintained, it will be clearly stated in the README.md file of the latest release.

This project will have updates other than bug fixes to add new features or improve existing ones, at least until September 2029. After that, it will depend on the author's availability and interest. The README.md file will be adjusted accordingly to reflect whether new features will be added to the project.<br>
Until September 2029, the goal is to have at least one important update per year.

### Here are potential future updates
- explaining LLM usage during development
- using modules instead of headers
- adding proper testing and benchmarking
- simplifying some parts of the library like the files GraphicalResources.
- adding a feature to group elements to offer similar performance than interactive in regard to cache locality
- fixing all warnings with clang-tidy
- allow the user to change the default font name and path more easily
- allow the user to change the assets folder more easily.
- adding the boost license at the top of every files
- removing populateGUI, and more generally rework on the GUI.?pp file.
- using [this wyhash](https://github.com/tommyettinger/waterhash/tree/master) instead to eliminate the hash that was generated (derived from the actual wyhash) by an llm
