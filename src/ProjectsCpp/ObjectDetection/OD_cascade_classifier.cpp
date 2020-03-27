//
// Created by 张晓民 on 2020/3/27.
//
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

using namespace std;
using namespace cv;

CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
int main(int, char**)
{
  string face_cascade_name = "Resources/haarcascade_frontalface_alt.xml";
  string eyes_cascade_name = "Resources/haarcascade_eye_tree_eyeglasses.xml"l

  return EXIT_SUCCESS;
}
