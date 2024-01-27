# CoralRugosity-LIU Zhihao
This is a software designed for coral rugosity computation (``HKUST VGD Group``), which is useful for the reseach on Marine Biology or the like.

- Usage: users can interact with a 3D coral model and freely specify the region of interest. Then, the system will automatically showcase the corresponding rugosity and relavent information.

- This project is developed with ``C++ 11`` and ``OpenGL 4.0+`` (you can use ``Visual Studio 2019+`` or ``Qt5.8+`` to easily compile this project).

**Usage and introduction**, please check these two videos for its usage: [Full Introducion Video (6min)](https://hkustconnect-my.sharepoint.com/:v:/g/personal/zliuem_connect_ust_hk/ESVDay4p9GFIv-bjDyzJM0gBut2MnGZS8UJJ52aH9RRxuw?e=Goegkb) and [Short Demo Video (10sec)](https://hkustconnect-my.sharepoint.com/:v:/g/personal/zliuem_connect_ust_hk/EUOFKCo9tTZFty2rVJMw4iABEGVwWlSnkLCYim2jHT29Ig?e=NGomST)

## Update History:

**``2022/7/20``** 3nd commit: 

  - Improve algorithms and interactive control.
  - Visual charts (cross setion & height)
  - Export data.
  - UI.
  
  <div align=center><img height="300" src="https://github.com/RyuZhihao123/CoralRugosity/blob/main/figures/7_20_0.png"/></div>

**``2022/7/13`` 2nd commit**

  - Use **BVH Tree** data structure to accelerate the query algorithm.
  - Obtain the rugosity in real-time (<1s).

<div align=center><img height="300" src="https://github.com/RyuZhihao123/CoralRugosity/blob/main/figures/7_13_0.png"/></div>


**``2022/7/10`` 1st commit**

  - Interactively sketch a line on the screen to specify the desired plane to compute rugosity.
  - A PLY loader to support arbitrary mesh attributes, and binary modes.
 
