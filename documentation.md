---
Lua
~~~~

ModelDraw("path.gltf", "modelName", culling, transX, transY, transZ, rotW, rotX, rotY, rotZ, scaleX, scaleY, scaleZ) // draws a model (not at the moment)

SetWindow("vsync", "Value") // toggles vsync
SetWindow("Title", "Value") // Sets window title
~~~~
---
TimeUtil
~~~~
TimeUtil::updateDeltaTime // updates TimeUtil

TimeUtil::s_DeltaTime = 0.0f; // fetch global deltatime
TimeUtil::s_lastFrameTime = 0.0f; // fetch currentTime
TimeUtil::s_frameRate = 0; // fetch framerate
TimeUtil::s_frameRate1hz = 0; // fetch framerate updated at 1hz
~~~~
---
TimeAccumulator
~~~
TimeAccumulator Value;  // creates TimeAccumulator object

value.counter // fetches counter variable

value.update // updates counter
update.reset // resets counter
~~~
---
SettingsUtils
~~~
SettingsUtils::s_WindowTitle = "value" // updates window title
~~~
---
will update with more soon
~~~
???
~~~
---
