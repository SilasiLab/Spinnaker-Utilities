/*
  This program allows users to display single channel frames from cameras using the Spinnaker SDK
  in an OpenCV window.

  If you need to modify this program to handle frames with a different number of channels, it will
  be useful to understand:

  1. How pixels/images are represented/stored in memory
  2. How OpenCV Mat objects are structured
  3. The fact that an array decays to a pointer to the first element of that array
  4. Pointer arithmetic basics


  Author: Julian Pitney
  Contact: JulianPitney@gmail.com
  Last Modification: 2018|07|15 @ 3:57AM
*/


#include<iostream>
#include<opencv2/highgui.hpp>
#include<opencv2/core.hpp>
#include<stdlib.h>
#include<chrono>

using namespace std;
using namespace cv;


// Number of nanoseconds in a second (Used for FPS calculation)
const int ns_per_second = 1000000000;
// Dimmensions of image being received
const int cols = 1000;
const int rows = 400;
// Statically allocated array for holding single channel image
uint8_t img_raw[cols*rows];
// OpenCV Mat where frame will be placed
Mat img(rows, cols, CV_8UC1);
// Toggle this to turn stream on and off
bool streaming = true;




int main(int argc, char* argv[])
{

  // Create display window for live camera feed
  namedWindow("PtGrey Live Feed", WINDOW_AUTOSIZE);
  // Initialize time and frame count vars for periodic FPS calculation
  auto time_of_last_fps_calculation = chrono::steady_clock::now();
  int frames_since_last_fps_calculation = 0;
  // Seed random number generator with current time for additional randomness
  srand(time(NULL));


  while(streaming)
  {


    /*
      This block generates monochrome frames from random noise. This is meant
      to simulate the output of Spinnaker's Image::GetData() function.
      The aforementioned function returns a pointer to an array of
      unsigned 8 bit integers (monochrome pixels). Instead of a multidimensional
      array representing an n by m matrix, Spinnaker stores all the frame's pixels
      contiguously in a single array. This allows the optimization described
      in the next comment.

      NOTE:  1. Replace <img_raw> with Spinnaker Image::GetData() output for live streaming camera.
             2. rand() is a very expensive function and when we replace it with the real data from Spinnaker
                our FPS will ~tripple.
    */
    for(uint i = 0; i < cols*rows; i++)
    {
      img_raw[i] = rand() % 255;
    }


    /*
      Since our frame's pixels are stored contiguously in memory we can simply
      wrap our frame pointer in an OpenCV Mat. We provide a pointer
      to our first pixel, the pointer step size, and the dimensions
      of our matrix. We're essentially passing our frame by reference
      and supplying the relevant frame header information for the Mat
      to understand it's format. This is quite efficient.
    */
    Mat img(rows, cols, CV_8UC1, img_raw);


    // Display the frame
    imshow("PtGrey Live Feed", img);
    waitKey(1);

    // Get the current time and calculate how long it's been since
    // the last FPS calculation
    auto current_time = chrono::steady_clock::now();
    auto diff = current_time - time_of_last_fps_calculation;
    auto diff_ns = chrono::duration_cast<chrono::nanoseconds>(diff);


    // If one second has passed since the last FPS calculation, recalculate FPS.
    if(diff_ns.count() > ns_per_second)
    {
      time_of_last_fps_calculation = chrono::steady_clock::now();
      cout << "FPS: " << frames_since_last_fps_calculation << endl;
      frames_since_last_fps_calculation = 0;
    }
    else
    {
      frames_since_last_fps_calculation++;
    }

  }


  return 0;
}
