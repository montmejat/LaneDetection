# LaneDetection

Basic lane finding program using OpenCV and C++. Thanks to all of the data from this repository `ndrplz/self-driving-car`! :)

## How to use (on Windows)

- Add `path_to_opencv_3.4.6\opencv\build\x64\vc15\bin` to `Path` environment variable.
- Go to `x64/Release`.
- Execute `"Lane Detection.exe" myImage.jpg` in cmd and see the result saved as `out.jpg` or `"Lane Detection.exe" myImage.jpg myImageProcessed.jpg` to specify the name of the output file. Works the same way with video files, the output is a .avi file.

## Example result

For the image located in `Lane Detection/data/test_images/solidYellowLeft.jpg` we get:

![Alt text](x64/Release/out.jpg "Example result")
