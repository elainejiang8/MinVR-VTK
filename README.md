# MinVR-VTK
MinVR-enabled VTK programs
## Installing VTK
To compile VTK, you will first need to install CMake. An installer for CMake is
available from http://www.cmake.org/download/. If you already have CMake, you can skip this step. Once you have installed CMake, run the following command in your terminal so you can run CMake through your terminal:
```
sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install
```

Next, you need to download the VTK source code. You can either download a zip/tar file [here](http://www.vtk.org/download/) or download using git:
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
with the desired options. Once you run CMake using ccmake, you will be presented with a list of options that can be modified to customize the VTK build. CMake will be able to set most of these options to reasonable default values. Hit the 'c' key to configure. (Note: If you are initally seeing a blank interface, hit the 'c' key.) Keep hitting the 'c' key until the generate option is available (g key), and then hit the 'g' key. 
3. Once VTK has been successfully configured, the interface should quit itself. The last step is to build VTK:
```
$ cd /path/to/VTK-build
$ make
```
### To install on Windows:
idk yet
