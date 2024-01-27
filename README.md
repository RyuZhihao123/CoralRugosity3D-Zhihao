# CoralRugosity-LIU Zhihao
This is a software designed for coral rugosity computation (``HKUST VGD Group``), which is useful for the fundamental reseach on Marine Biology field or the like.


- **Usage**: users can interact with 3D coral mesh models and freely specify the region of interest. Then, the system will automatically compute and showcase the corresponding rugosity and other relavent information.

- **Source Codes**: please go to this folder [[Source codes]](https://github.com/RyuZhihao123/CoralRugosity/tree/main/CoralRugosity_Zhihao).

- **Requirement**: This project is developed with ``C++ 11`` and ``OpenGL 4.0+`` (you can use ``Visual Studio 2019+`` or ``Qt5.8+`` to easily compile this project).

- **Video Demos**: [Full Introducion Video (6min)](https://drive.google.com/file/d/1VCVGPRM6rvlVhphPzhBRmh937NgIrrGS/view?usp=sharing) and [Short Demo Video (10sec)](https://drive.google.com/file/d/1rLDS7xQg8qLJw_aDXdZU3kCRQt-JaWJl/view?usp=sharing)

  <div align=center><img height="300" src="https://github.com/RyuZhihao123/CoralRugosity/blob/main/figures/7_20_0.png"/></div>

## Update History:

**``2022/7/20``** 3nd commit: 

  - Improve algorithms and interactive control.
  - Visual charts (cross setion & height)
  - Export data.
  - UI.

  <div align=center><img width="800" src="https://github.com/RyuZhihao123/CoralRugosity/blob/main/7_21_0.png"/></div>

**``2022/7/13`` 2nd commit**

  - Use **BVH Tree** data structure to accelerate the query algorithm.
  - Obtain the rugosity in real-time (<1s).

<div align=center><img height="300" src="https://github.com/RyuZhihao123/CoralRugosity/blob/main/figures/7_13_0.png"/></div>


**``2022/7/10`` 1st commit**

  - Interactively sketch a line on the screen to specify the desired plane to compute rugosity.
  - A PLY loader to support arbitrary mesh attributes, and binary modes.
 
