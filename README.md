# MinVR-VTK
MinVR-enabled VTK programs
## Installing VTK
To compile VTK, you will first need to install CMake. An installer for CMake is
available from http://www.cmake.org/download/. If you already have CMake, you can skip this step. Once you have installed CMake, run the following command in your terminal in order to run CMake through your terminal:
```
sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install
```

Next, you'll need to download the VTK source code. You can either download a zip/tar file [here](http://www.vtk.org/download/) or download using git:
```
Prepare directory for download
 $ mkdir projects
 $ cd projects

To download the source code 
 $ git clone git://vtk.org/VTK.git
 $ cd VTK
 
To update the code (to get the newest version)
 $ git fetch origin
 $ git rebase origin/master
```
### To install on a Mac or Linux:
1. Configure VTK with CMake
```
Make a separate build directory
  $ mkdir projects/VTK-build
  $ cd projects/VTK-build
Run ccmake
  $ ccmake ../VTK
```
2. ccmake has a simple terminal based interface that allows you to customize the VTK build to your particular machine and
with the desired options. Once you run CMake using ccmake, you will be presented with a list of options that can be modified to customize the VTK build. CMake will be able to set most of these options to reasonable default values. 
    - Hit the 'c' key to configure. (Note: If you are initally seeing a blank interface, hit the 'c' key.) 
    - Keep hitting the 'c' key until the generate option is available (g key), and then hit the 'g' key. 
    
3. Once VTK has been successfully configured, the interface should quit itself. The last step is to build VTK:
```
$ cd /path/to/VTK-build
$ make
```
### To install on Windows:
idk yet

## Run Examples
#### Run the demo example:
```
Cd into project directory
  $ cd /path/to/MinVR-VTK/
Cmake and build project
  $ mkdir build
  $ cd build
  $ cmake -DVTK_DIR:PATH=/path/to/vtk_build ..
  $ make
Run project
  $ bin/demo ../data/ironProt.vtk
```
Running the other examples is a similar process, but be sure to use the correct data set (you can see which data set to use in a project's cxx file comments). 

#### Prepare to run the demo2/demo3/supernova2/supernova3 examples:
1. Because demo2 is run on an external OpenGL window, make sure that Module_vtkRenderingExternal is ON (default is OFF). You can modify vtk's configurations by going to your VTK-build directory and running "ccmake /path/to/VTK". Then, find the row with Module_vtkRenderingExternal and hit the enter key to toggle the settings. Also make sure that for the vtk_Rendering_Backend module, OpenGL2 is switched to OpenGL. When everything is set correctly, hit the 'c' key until the 'g' key appears, and then hit the 'g' key to finish configuration. 

2. If you do not have GLEW or FreeGLUT installed, install them using homebrew.

#### Run the demo2/supernova2 examples: 
1. After building demo2 or supernova2, you can run them the same way you ran demo/supernova.
 - For demo2, run:
 ```
   $ bin/demo2 ../data/ironProt.vtk
 ```
 - For supernova2, run:
 ```
   $ bin/supernova2
 ```

#### Run the demo3 example: 
1. After building demo3, run with the following command:
```
  $ bin/demo3 ../shaders/shader2.vp ../shaders/shader.fp -c ../config/desktop-freeglut.xml
```
2. If you want to run this in the yurt, use:
```
  $ bin/supernova3 -c YURT
```

#### Run the supernova3 example:
1. After building supernova3, run with the following command:
```
  $ bin/supernova3 -c ../config/desktop-freeglut.xml
```
2. If you want to run this in the yurt, use:
```
  $ bin/supernova3 -c YURT
```
