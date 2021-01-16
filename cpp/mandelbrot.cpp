#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<Mat> complexMultiply(Mat old_re, Mat old_im){
    vector<Mat> z;
    z.push_back(old_re.mul(old_re)-old_im.mul(old_im));
    z.push_back(2*old_re.mul(old_im));
    return z;
}

Mat complexAbsolute(Mat re, Mat im){
    Mat abs = re.mul(re) + im.mul(im);
    return abs;
}

void computeMandelbrot(Mat value_re, Mat value_im, Mat re_coordinates, Mat im_coordinates){
    vector<Mat> z = complexMultiply(value_re, value_im);
    Mat re = z[0] + re_coordinates;
    Mat im = z[1] + im_coordinates;
    re.copyTo(value_re);
    im.copyTo(value_im);
}

int main(int argc, char** argv) {
    if (argc < 2){
        cerr << "Please specify image size" << endl;
        return -1;
    }

    int imageSize = stoi(argv[1]);
    if ((imageSize % 4)!=0){
        cerr << "Image size should be dividable by 4" << endl;
        return -1;
    }

    int approx_iteration = 1000;
    if (argc>=3){
        approx_iteration = stoi(argv[2]);
    }
    cout<<"Mandelbrot value approximation iteration: "<< approx_iteration << endl;

    Mat mandelbrot_coordinates_re = Mat::ones(imageSize, imageSize, CV_64FC1);
    Mat mandelbrot_coordinates_im = Mat::ones(imageSize, imageSize, CV_64FC1);
    Mat mandelbrot_value_re = Mat::zeros(imageSize, imageSize, CV_64FC1);
    Mat mandelbrot_value_im = Mat::zeros(imageSize, imageSize, CV_64FC1);
    Mat outputImage = Mat::zeros(imageSize, imageSize, CV_8UC1);

    // Initialise target mandelbrot coordinates 
    double lower_limit_re = -2;
    double upper_limit_re = 2;
    double lower_limit_im = -2;
    double upper_limit_im = 2;

    double re_resolution = (double)imageSize/(upper_limit_re - lower_limit_re);
    double im_resolution = (double)imageSize/(upper_limit_im - lower_limit_im);

    cout<< "re_resolution: " << re_resolution << ",im_resolution: " << im_resolution << endl;
    for (double i=0.0; i<imageSize; i++){
        double re = i/re_resolution + lower_limit_re;
        double im = i/im_resolution + lower_limit_im;

        mandelbrot_coordinates_re(Range::all(),Range(i,i+1)) = mandelbrot_coordinates_re(Range::all(),Range(i,i+1)) * re;
        mandelbrot_coordinates_im(Range(imageSize-i-1,imageSize-i), Range::all()) = mandelbrot_coordinates_im(Range(imageSize-i-1,imageSize-i), Range::all()) * im;
    }

    // Compute mandelbrot set
    for (int i=0; i<approx_iteration; i++){
        cout << "computing iteration " << i << endl;
        computeMandelbrot(mandelbrot_value_re,
                          mandelbrot_value_im, 
                          mandelbrot_coordinates_re,
                          mandelbrot_coordinates_im);
    }

    // find mandelbrot set membership
    Mat mandelbrotMember = complexAbsolute(mandelbrot_value_re,mandelbrot_value_im) <= 2.0;
    
    // namedWindow("RE Coordinates", WINDOW_AUTOSIZE);
    // imshow("RE Coordinates", mandelbrot_coordinates_re);
    // namedWindow("IM Coordinates", WINDOW_AUTOSIZE);
    // imshow("IM Coordinates", mandelbrot_coordinates_im);
    namedWindow("Value RE", WINDOW_AUTOSIZE);
    namedWindow("Value IM", WINDOW_AUTOSIZE);
    double re_min, re_max, im_min, im_max;
    minMaxLoc(mandelbrot_value_re, &re_min, &re_max);
    imshow("Value RE", (mandelbrot_value_re-re_min)/(re_max-re_min));
    minMaxLoc(mandelbrot_value_im, &im_min, &im_max);
    imshow("Value IM", (mandelbrot_value_im-im_min)/(im_max-im_min));
    namedWindow("Mandelbrot", WINDOW_AUTOSIZE);
    imshow("Mandelbrot", mandelbrotMember);

    waitKey(0);

    return 0;
}