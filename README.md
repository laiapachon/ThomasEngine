# ThomasEngine

DESCRIPTION:

This engine has been created for a subject called Motors 3D. The intention of this engine is to be similar as Unity, 
in which you can add FBX to the scene with its textures if they are in the Assets folder. This FBX are able to move 
and rotate. In the menu the user can see if there is any error, all the measures of the FBXs and also change them.
On the configuration menu, the user can see the FPS level, enable and disable different options, also change the 
measures of the window, see all inputs and all the informations of the Engine. 

TEAM MEMBERS:

Enric Morales: (https://github.com/enricmc19)
Laia Pachón: (https://github.com/laiapachon)

LINK TO THE REPOSITORY ON GITHUB:

(https://github.com/laiapachon/ThomasEngine)

We are a team of CITM University, in Terrassa, who are creating an engine for videogames.

Week 1 and 2:
- We had added ImGui, MathGeoLib and Glew.

Weel 3:
- Enable and Disable buttons
- Color material implementation
- Texture enable and disable button implementation
- Light enable and disable button implementation
- Start searching about Wireframe mode


Week 4: 
- Primitives
- Starting the implementation of FBX

Week 5:
- Implementing FBX by Buffer
- Primitives by direct mode
- File system implementation

Week 6:
- Devil implementation
- FBX
- Texture
- Game Object
- Inspector
- Hierarchy

New Features:

- New Camera controls and frustrum culling
- Scene and Game tabs work as they should on editor
- Functional Mouse Picking with visible RayCast
- Game Object duplication
- New Scene Loaded by default
- Mesh active state
- Scene serialization
- Bounding Boxes can be toggled on inspector
- Guizmo for Game Object translation
- Parent,child and empty Game Object creation
- Camera component with its own configuration on inspector
- Hierarchy able to parent, reparent, and unparent
- Working Drag & Drop from Assets Folder directory
- Several Memory leaks solved


CONTROLS:

- Camera controls:
	- LSHIFT: Use controls faster
	- F: Front view
	- E: Moving camera Up
	- Q: Moving camera Down
	- W: Moving camera to Front
	- S: Moving camera Back
	- A: Moving camera Left
	- D: Moving camera Right
	- MOUSE WHEEL: Zoom In and Out
	- Alt+Left click: orbit around the camera position
	
	
- Short Cuts:
- Numbers from 1-9 open and close the tabs
- Ctrl+N: New Scene
- Ctrl+S: Save Scene
- Ctrl+D: Duplicate Object
- Ctrl+Shift+N: New GameObject Empty
- Ctrl+Shift+G: New GameObject Parent
- Alt+Shift+N: New GameObject Child
- Ctrl+Shift+F: Align with view
- Ctrl+Alt+W: Use TranslateSnap
- Ctrl+Alt+E: Use RotateSnap
- Ctrl+Alt+R: Use ScaleSnap
- Alt+Shift+F: Align with selected
- Alt+Shift+A: Toggle active state
- Alt+F: ResetViewRotation


Important Issues found:
- Deleting duplicated game objects may cause a crash
- Deleting game objects in general, can cause a random crash
- Resource manager does not create a custom file format
- Scene Serialization Load does not render any Game Object, but "works"
- Unable to make a functional Play/Pause/Stop System
- Guizmo only works for translation of Game Objects purpose, Rotation and Scale only work using the inspector
- Asset import can fail

