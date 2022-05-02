# Host decoding

The bounding boxes generation task is let to the computer, the camera only returns the frames and the vectors separately.

## C++

All the code is inside the [main.cpp file](./main.cpp).

This code is used for the benchmark of all the .blob files inside the specified directory (change it inside the source code). Each .blob runs for a given time before the next one is loaded (also specified in the code).

The [main.cpp](main.cpp) is compiled into DepthAI_BLOB_benchmark.exe with the [CMakeLists.txt](../CMakeLists.txt) at the root of this repository.

The required libraries are DepthAI:Core, DepthAI:OpenCV and OpenCV (make sure the paths to the DLLs are known by the system! )

## Python

The requirements are listed in the [requirements.txt file](./requirements.txt).

The [main.py file](./main.py) displays a window with the camera view and the bounding boxes around the objects detected by the neural network.

> **⚠ Important:** \
This code is too slow to accurately measure the FPS of the neural network. \
Prefer the C++ code to bnechmark the computing power of the camera but the bounding boxes will not be visible!