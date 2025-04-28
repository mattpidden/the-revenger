![Banner Photo](/media/logo.png)
![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Python](https://img.shields.io/badge/python-3670A0?style=for-the-badge&logo=python&logoColor=ffdd54)
![OpenCV](https://img.shields.io/badge/opencv-%23white.svg?style=for-the-badge&logo=opencv&logoColor=white)
![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)

# The Revenger - 4x4 Rubik’s Cube Solving Robot

This project presents the development of an efficient computational solver for the 4x4 Rubik's Cube and its implementation within a robotic system that achieves a world record solving time.

The approach uses a 2D array of integer facelet identifiers, which is the most suitable data structure for the 4x4 Rubik's Cube. This data structure is used in an 8 phase solving algorithm inspired by Tsai's method, and presented as the first C++ implementation of its kind. The approach is original yet closely based on Tsai's framework, offering a unique variation that prioritises practicality and performance. The 8 phase method achieves short solutions with an average move count of 67.79 moves in near real time. Additionally, novel research was conducted into optimising a computational solver to the characteristics and constraints of a specific robotic system, an area that has not been previously explored in existing literature. Unlike existing 4x4 solvers, this project is the only one accompanied by any explanation of the underlying theory and implementation, making it the first truly transparent and practical resource for 4x4 computational solving. The robot was designed and constructed from the ground up using fully custom parts and original architecture, independent of existing designs. The integration of the computational solver and robotic system achieved a solve time of 47.93 seconds, a 39.08% improvement over the current official world record. 

These results demonstrate that the robot, named 'The Revenger', has achieved an unofficial new world record time for solving the 4x4 Rubik's Cube. In due course the evidence will be submitted to Guinness World Records for review of official recognition.

A video demonstration of the robot is available on [YouTube](https://youtu.be/tE4DD7Fw6SM).

# Project Structure
```
code/ 
  solver/
  scanner/ 
  master/
  microcontroller/ 
parts/ 
media/
```

# Instructions
To run the 8 phase solver, first download the latest binary assets from [here](https://github.com/mattpidden/the-revenger/releases) and place them in the `code/solver` directory. Then type `make` to generate the compiled solver. Run `./solver -s` to solve a cube. It expects a 96 char input of the inital cube state.

To run the master program use `python main.py` inside the `code/master` directory. 

To run the computer vision scanning use `python manual_scan.py` inside the `code/scanner` directory. It is important that you calibrate your cameras first using `python calibrate_cameras.py`.

Upload the microcontroller code to an Arduino using the offical software.
