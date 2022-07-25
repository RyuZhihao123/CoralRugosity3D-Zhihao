# CoralRugosity-LIU Zhihao
A C++&amp;OpenGL software for coral rugosity computation (``HKUST VGD``). This project is developed with Qt 5.8 and OpenGL 4.0+ (can use Visual Studio 2019+ to compile this project as well).

- [Full introducion video(6min)](https://hkustconnect-my.sharepoint.com/:v:/g/personal/zliuem_connect_ust_hk/ESVDay4p9GFIv-bjDyzJM0gBut2MnGZS8UJJ52aH9RRxuw?e=Goegkb) and [Short Demo video (10sec)](https://hkustconnect-my.sharepoint.com/:v:/g/personal/zliuem_connect_ust_hk/EUOFKCo9tTZFty2rVJMw4iABEGVwWlSnkLCYim2jHT29Ig?e=NGomST)

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
 
