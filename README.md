LMGL
====

What is it?
-----------

LMGL is a Computer Graphics Library that provides an **Engine API**, allowing
to develop 3D graphics application without other external dependency.
The engine is developed using *GLFW*, that follows the 4.1 standard of *OpenGL*.
The library implements "modern" OpenGL, supporting *shaders*, *PBR* and all the
amenities common in modern 3D Graphics Engines.

Applications for this kind of library are broad; whenever you are in need to 
represent something complex in 3D, write 3D-based applications like games or
simulations, or even 3D editors, you can make good use of this.

You can also provide extensions for this, to support other kinds of
application in a simpler way.

The library comes also with an example application (`sandbox/`) and some
example shaders (`shaders/`).

On what does it run?
--------------------

LMGL is developed and mantained for **macOS**, supports also *Windows* (still
work in progress) and *Linux* (testing in backlog).
There are no specific hardware requirement to run (no specific GPU), just make 
sure you have the correct *drivers* installed for your GPU.

An update on Software and Hardware compatibility will be given in a further
notice when the development of `v1.0` reaches the end.

Documentation
-------------

The code is documented using *Doxygen*, that is automatically deployed on 
[this page](https://lucamazzza.github.io/lmgl), and is constantly updated for 
every version released.
