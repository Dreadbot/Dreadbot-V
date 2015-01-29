# vision2015
Tracks the L's on the yellow totes

Returns X rotation and the ratio between the areas of the 
left and right L's, which can be used to line up with the
tote. 

To build on ubuntu:

1) Install dependencies
sudo apt-get install freenect build-essential git libopencv-dev opencv-data

2) Navigate to directory where you want to place the software
cd ~/Documents

3) clone the repository
git clone https://github.com/faust1706/Vision-IR-2015.git
cd Vision-IR-2015/build-vision2015-Desktop-Debug

4) build software
make

5) Run it
./vision2015

