# LMGL Examples

This directory contains example applications demonstrating various LMGL features.

## Building Examples

```bash
cd build
cmake ..
make
```

## Running Examples

All examples should be run from the project root directory:

```bash
cd /path/to/lmgl
./build/examples/ExampleName
```

---

## Available Examples

### 1. **Sandbox** (`main.cpp`)
**Demonstrates:** Complete 3D scene with PBR rendering

**Features:**
- PBR material rendering with IBL
- Shadow mapping (directional + point lights)
- Skybox with HDR environment
- Camera controls (WASD + mouse)
- Model loading
- Real-time FPS display

**Controls:**
- `W/A/S/D` - Move camera
- `Mouse` - Look around
- `ESC` - Exit

**Run:**
```bash
./build/examples/Sandbox
```

---

### 2. **JSON UI Demo** (`json_ui_demo.cpp`)
**Demonstrates:** JSON-based UI loading system

**Features:**
- Load complete UI from JSON file
- Button callbacks
- Toggle checkboxes
- Sliders with value display
- Font management
- Runtime element access

**JSON File:** `examples/assets/ui_demo.json`

**Run:**
```bash
./build/examples/JSONUIDemo
```

**Interact:**
- Click buttons to see console output
- Toggle the checkbox
- Drag the volume slider
- Press `ESC` to exit

---

## Quick Start: Your First LMGL Application

```cpp
#include <lmgl/lmgl.hpp>

int main() {
    using namespace lmgl;
    
    // Initialize engine
    auto& engine = Engine::get_instance();
    if (!engine.init(1280, 720, "My App")) {
        return -1;
    }
    
    // Create scene
    auto scene = std::make_shared<Scene>();
    auto camera = std::make_shared<Camera>();
    
    // Load a model
    ModelLoader loader;
    auto model = loader.load("assets/model.obj");
    if (model) {
        scene->get_root()->add_child(model);
    }
    
    // Main loop
    engine.run([&](float dt) {
        // Update
        if (Input::is_key_pressed(Key::Esc)) {
            engine.shutdown();
        }
        
        // Render
        scene->render(camera);
    });
    
    engine.free();
    return 0;
}
```

---

## Creating Your Own Example

1. Create a new `.cpp` file in this directory
2. Add it to `CMakeLists.txt`:
   ```cmake
   add_executable(MyExample my_example.cpp)
   target_link_libraries(MyExample PRIVATE lmgl)
   ```
3. Rebuild: `cmake .. && make`
4. Run: `./build/examples/MyExample`

---

## Example Features by Category

### 🎨 Rendering
- **Sandbox** - Complete PBR pipeline
- Shadow mapping
- IBL lighting
- Skybox rendering

### 🖱️ UI System
- **JSONUIDemo** - Declarative UI loading
- Canvas rendering
- UI event handling
- Font rendering

### 📦 Asset Loading
- Model loading (OBJ, FBX, GLTF)
- Texture loading
- Material creation
- HDR environment maps

### 🎮 Input
- Keyboard input
- Mouse input
- Camera controls

---

## Assets

Example assets are located in `examples/assets/`:
- `ui_demo.json` - UI definition for JSONUIDemo
- Models, textures, and other resources

---

## Need Help?

- Check the main README: `../README.md`
- View API documentation: Run `doxygen` in project root
- Look at example source code for usage patterns

---

## Tips

1. **Always run from project root** - Assets use relative paths
2. **Use the master header** - `#include <lmgl/lmgl.hpp>` includes everything
3. **Check engine initialization** - Always verify `engine.init()` returns true
4. **Use namespace aliases** - `using namespace lmgl;` for cleaner code
5. **Static Input class** - Use `Input::` instead of `engine.` for input

Happy coding! 🚀
