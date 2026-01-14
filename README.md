<p align='center'>

<img src="https://capsule-render.vercel.app/api?type=venom&color=782b90&height=200&section=header&text=lmgl&fontSize=90&fontColor=ffffff&animation=fadeIn&fontAlignY=35&desc=Modern%20OpenGL%20Engine%20Library&descAlignY=61&descAlign=50"/>


</p>

<p align='center'>

<img src="https://img.shields.io/badge/4.1-235586A4?style=for-the-badge&logo=opengl&logoColor=%235586A4&labelColor=%23ffffff&color=%235586A4&logoSize=auto"/>
<img src="https://img.shields.io/badge/17-%23044f88?style=for-the-badge&logo=cplusplus&logoColor=%23044f88&labelColor=%23ffffff&color=%23044f88"/>
<img src="https://img.shields.io/github/actions/workflow/status/lucamazzza/lmgl/.github%2Fworkflows%2Fcmake_multiplatform.yml?style=for-the-badge&logo=cmake&logoSize=auto&label=%20"/>
<img src="https://img.shields.io/github/v/release/lucamazzza/lmgl?include_prereleases&sort=date&display_name=release&style=for-the-badge&label=%20"/>

</p>

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

> [!NOTE]
> An update on Software and Hardware compatibility will be given in a further
> notice when the development of `v1.0` reaches the end.

Documentation
-------------

The code is documented using *Doxygen*, that is automatically deployed on 
[this page](https://lucamazzza.github.io/lmgl), and is constantly updated for 
every version released.

If something goes wrong...
--------------------------

Check that:
* All the needed dependencies are included as submodule
* Your compiler supports C++17

<hr>
<p align="center">
  
<img src="https://repobeats.axiom.co/api/embed/b4c43374bdf8e8698cccfe6355edcf5ca3798970.svg" alt="Repobeats analytics image"/>
  
</p>
