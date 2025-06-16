gamut: set of colors that a device can display
OpenGL supports RGBA and sRGB
intensities of each color is quantised to a discrete set of values - called the bit depth

- lower left pixel in an OpenGL window is pixel (0, 0). (x, y) fills the region bound by x on the left, x + 1 on th
right, y to the bottom and y + 1 to the top.
- color, depth and stencil buffers form the frame buffer.
- default frame buffer is associated with the main window

- there can be many color buffers, but the main color buffer of the default frame buffer is what is displayed.
- the pixels in a color buffer may store a single color per pixel or may logically divide the pixel into subpixels
for "multisampling"
- double buffering: ONLY the main color buffer has this feature. Basically has a front and back buffer that is
swapped every frame.

- Depth buffer: stores a depth value for each pixel. used for determining visibility
- Stencil buffer: used to restrict drawing to certain portions of the screen.


### Clearing Buffers:

