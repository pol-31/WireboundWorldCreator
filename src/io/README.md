from [GLFW documentation](https://www.glfw.org/docs/3.3/input_guide.html):
GLFW provides many kinds of input. While some can only be polled, like time,
or only received via callbacks, like scrolling, many provide both callbacks
and polling. Callbacks are more work to use than polling but is less CPU
intensive and guarantees that you do not miss state changes.
---
we tend to use callbacks - although it is less explicit, we don't need to
pool requests IsPressed() each frame and therefore pollute our rendering loop;
anyway it still guarantees we won't miss state changes