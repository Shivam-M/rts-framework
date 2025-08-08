### Using the level designer

1. Run `level_designer.py` - an example level is provided to start with
2. `File -> Open` to select an existing JSON level file (ideally but not limited to UI/Panel files)
3. The preview pane will show a rough version of how it should appear in-game
4. The first item in the object panel must be the (only) panel object
5. New objects can be added from the top left menu bar `Add Object`
6. Child objects:
    - must have the `parent` property set to the panel name - a variable can be used to make this easier e.g. `&name`
    - should have their names prefixed with the parent name - these are used for data mappings
    - are automatically offset and scaled based on the parent panel location and size
    - can be copied/pasted/deleted with ctrl+c/ctrl+v/del
7. The raw JSON can be viewed by going to `Edit -> Raw JSON`
8. Level files that begin with a number are automatically loaded in order from `0.json` onwards (supports padding)
9. Default behaviour is to show these UI panels - they can be hidden with `UIManager::Hide("[PANEL_NAME]");`
    #### Current Limitations and Unsupported Features
    - Left/Centre/Right alignment for `Text` objects
    - Fonts and size scaling for `Text` objects
    - Variable concatenation
    - List item constraints for properties

### Mapping UI elements with game data
1. In `ui_manager.h` declare a new static method 
    ```cpp
    static void MapExample(Panel* panel, void* moveable);
    ```
    - **Note:** UI Panel mappings are not exclusive to moveables, they can be mapped to any object/data type
2. In `ui_manager.cpp` add a new entry to `UIManager::method_mappings_` with the panel name and newly declared method
    ```cpp
    map<string, MappingFunction> UIManager::method_mappings_ = {
         {"ui_example", &UIManager::MapExample}
    };
    ```
3. Define the method, cast to the expected data type, and set text object properties directly using `Text* text_object = panel->getTextByName(const string& name)` 
    ```cpp
    void UIManager::MapExample(Panel* panel, void* moveable) {
        Moveable* example = static_cast<Moveable*>(moveable);
        
        panel->getTextByName("ui_example_title")->setContent(example->getName());
        panel->getTextByName("ui_example_title")->setColour(example->getColour());
        panel->getTextByName("ui_example_subtitle")->setContent("ID: " + to_string(example->getID()));
    }
    ```
4. Map an object to the UI panel from anywhere in the game
    ```cpp
    UIManager::AssignValues("ui_example", example);
    ```
5. Show/Hide/Toggle the panel visibility
    ```cpp
    UIManager::Show("ui_example");
    UIManager::Hide("ui_example");
    UIManager::Toggle("ui_example");
    ```