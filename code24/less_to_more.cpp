// test frame 83 84


#include <stdio.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <map>
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"

using namespace std;
using namespace cv;




void drawMatch(Mat& img1, Mat& img2, Mat& img_matches, double ratio = 0.8, bool extended = false){
   
    int minHessian=400;
    SurfFeatureDetector  detector(minHessian, 4, 3, false);
    vector<KeyPoint>key_points_img1, key_points_img2;
    detector.detect(img1, key_points_img1);
    detector.detect(img2, key_points_img2);
 
    
    if(key_points_img1.size() < 4 || key_points_img2.size() < 4){
        return ;
    }

    Mat descriptor_img1, descriptor_img2;
    detector.compute(img1, key_points_img1, descriptor_img1);
    detector.compute(img2, key_points_img2, descriptor_img2);
    
    
    BFMatcher matcher;
    vector<vector<DMatch>>matches;
    matcher.knnMatch(descriptor_img1, descriptor_img2, matches, 2);
    
    vector<DMatch> good_matches;

    if(descriptor_img1.empty() || descriptor_img2.empty()){
        return ;
    }
    if (matches.size() >30){
        for(int i = 0; i < matches.size(); i++)
        {
            
            if (matches[i][0].distance < ratio * matches[i][1].distance)
            {
                good_matches.push_back(matches[i][0]);
            }
        }

    }
    else
    {
        for(int i = 0; i < matches.size(); i++)
        {
            good_matches.push_back(matches[i][0]);
        }
    }
    
    if (good_matches.size() < 4){
        return ;
    }

    
    sort(good_matches.begin(), good_matches.end());


    
    vector<Point2f> img1_points;
    vector<Point2f> img2_points;
    
    int num_good = min((int)good_matches.size(), 30);
    vector<DMatch> first_matches;
     
    for(unsigned int i = 0; i < num_good; ++i)
    {
        img1_points.push_back(key_points_img1[good_matches[i].queryIdx].pt);
        img2_points.push_back(key_points_img2[good_matches[i].trainIdx].pt);
        first_matches.push_back(good_matches[i]);
    }

    

   

    drawMatches(img1, key_points_img1, img2, key_points_img2, first_matches, img_matches);
    
    putText(img_matches, to_string(key_points_img1.size()) + "   " + to_string(key_points_img2.size()), Point(0, 50), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0) );
    Mat H = findHomography(img1_points, img2_points, RANSAC, 8);
    
    vector<Point2f> img1_cross(5);
    Point2f middle = Point2f(img1.cols/2, img1.rows/2);
    img1_cross[0]=middle+Point2f(0,-50);
    img1_cross[1]=middle+Point2f(0, 50);
    img1_cross[2]=middle+Point2f(-50, 0);
    img1_cross[3]=middle+Point2f(50, 0);
    img1_cross[4]=middle;
    vector<Point2f> img2_cross(5);
   
    if (!H.empty()){
        
        perspectiveTransform(img1_cross, img2_cross, H);

        line(img_matches, img1_cross[0], img1_cross[1], Scalar(0, 255, 0), 2);
        line(img_matches, img1_cross[2], img1_cross[3], Scalar(0, 255, 0), 2);
        
        line(img_matches, img2_cross[0]+Point2f(img1.cols, 0), img2_cross[1]+Point2f(img1.cols, 0), Scalar(0, 255, 0), 2);
        line(img_matches, img2_cross[2]+Point2f(img1.cols, 0), img2_cross[3]+Point2f(img1.cols, 0), Scalar(0, 255, 0), 2);
        circle(img_matches, img2_cross[4]+Point2f(img1.cols, 0), 5, Scalar(0, 0, 255), 2);
    }
       
    return ;
  
}

int main(int argc, char const *argv[])
{
    string comp_folder = "/home/jiaming/opencv24/code24/out_frames/";
    string folder = "/home/jiaming/opencv24/code24/img";
    string img1_name = "1/";
    string img2_name = "2/";
    string extention = ".png";
    int i=0;
    Mat img_match;
    Mat reverse;
    while (i<115)
    {

    Mat img1 = imread(folder + img1_name + to_string(i) + extention, IMREAD_COLOR);
    Mat img2 = imread(folder + img2_name + to_string(i) + extention, IMREAD_COLOR);
    Mat org = imread(comp_folder + to_string(i) + extention, IMREAD_COLOR);
    drawMatch(img2, img1, reverse);
    drawMatch(img1, img2, img_match);
    imshow("Matches", img_match);
    imshow("Original", org);
    imshow("Reverse", reverse);
    waitKey(0);
    i++;
    }
    
   
    return 0;
}

