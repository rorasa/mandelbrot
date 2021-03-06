#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace std::chrono;

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
    const String keys = 
        "{help h usage ? |      | print this message }"
        "{@image_size    |      | image size (must be dividable by 4}"
        "{n iteration    | 100 | Number of iteration used for convergence approximation}"
        "{RU upper_real  | 2.0  | Upper bound of real part}"
        "{RL lower_real  | -2.0 | Lower bound of real part}"
        "{IU upper_img   | 2.0  | Upper bound of imaginary part}"
        "{IL lower_img   | -2.0 | Lower bound of imaginary part}"
        "{o output       | output.png | Name of the output image}"
        "{V video        | false | Create animation video}"
        "{v view         | false | Display the output onscreen}";
    CommandLineParser parser(argc, argv, keys);
    parser.about("Mandelbrot Generator v0.2");

    if (parser.has("help")){
        parser.printMessage();
        return 0;
    }

    if (!parser.check()){
        parser.printErrors();
        return 0;
    }

    int imageSize = parser.get<int>(0);
    int approx_iteration = parser.get<int>("n");

    double lower_limit_re = parser.get<double>("lower_real");
    double upper_limit_re = parser.get<double>("upper_real");
    double lower_limit_im = parser.get<double>("lower_img");
    double upper_limit_im = parser.get<double>("upper_img");
    String outputPath = parser.get<String>("o");
    bool viewer = parser.get<bool>("v");
    bool video = parser.get<bool>("V");

    cout << "Generating Mandelbrot set with the following configuration:" << endl;

    cout << "Output path: "<< outputPath << endl;
    cout << "Image size: " << imageSize <<" x " << imageSize << endl;
    cout << "Iteration: "<< approx_iteration << endl;
    cout << "Upper limit of real part: "<< upper_limit_re << endl;
    cout << "Lower limit of real part: "<< lower_limit_re << endl;
    cout << "Upper limit of imaginary part: "<< upper_limit_im << endl;
    cout << "Lower limit of imaginary part: "<< lower_limit_im << endl;
    cout << "-----------------------------------------------------------" << endl;

    if ((imageSize % 4)!=0){
        cerr << "Error: Image size should be dividable by 4" << endl;
        return -1;
    }

    Mat mandelbrot_coordinates_re = Mat::ones(imageSize, imageSize, CV_64FC1);
    Mat mandelbrot_coordinates_im = Mat::ones(imageSize, imageSize, CV_64FC1);
    Mat mandelbrot_value_re = Mat::zeros(imageSize, imageSize, CV_64FC1);
    Mat mandelbrot_value_im = Mat::zeros(imageSize, imageSize, CV_64FC1);
    Mat mandelbrotMember;

    // Initialise target mandelbrot coordinates 
    double re_resolution = (double)imageSize/(upper_limit_re - lower_limit_re);
    double im_resolution = (double)imageSize/(upper_limit_im - lower_limit_im);

    for (double i=0.0; i<imageSize; i++){
        double re = i/re_resolution + lower_limit_re;
        double im = i/im_resolution + lower_limit_im;

        mandelbrot_coordinates_re(Range::all(),Range(i,i+1)) = mandelbrot_coordinates_re(Range::all(),Range(i,i+1)) * re;
        mandelbrot_coordinates_im(Range(imageSize-i-1,imageSize-i), Range::all()) = mandelbrot_coordinates_im(Range(imageSize-i-1,imageSize-i), Range::all()) * im;
    }

    if (viewer){
        namedWindow("Mandelbrot", WINDOW_AUTOSIZE);
    }

    VideoWriter vidOut;
    if (video){
        vidOut = VideoWriter("output.mp4",
                                     VideoWriter::fourcc('x','2','6','4'),
                                     20.0,
                                     Size(imageSize,imageSize),
                                     true);

        if (!vidOut.isOpened()){
           cerr << "Error: Failed to open output video" << endl;
           return -1;
        }
    }
    
    // Compute mandelbrot set
    high_resolution_clock::time_point start_time = high_resolution_clock::now();
    for (int i=0; i<approx_iteration; i++){
        cout << "computing iteration " << i << endl;
        computeMandelbrot(mandelbrot_value_re,
                          mandelbrot_value_im, 
                          mandelbrot_coordinates_re,
                          mandelbrot_coordinates_im);

        if (video || viewer){
            mandelbrotMember = complexAbsolute(mandelbrot_value_re,mandelbrot_value_im) <= 2.0;

            high_resolution_clock::time_point current_time = high_resolution_clock::now();
            milliseconds ms = duration_cast<milliseconds>(current_time - start_time);
            stringstream time_text;
            time_text << (double)ms.count()/1000.0 << " s";
            putText(mandelbrotMember, time_text.str(), Point(20,(int)(30*(double)imageSize/800)),
                FONT_HERSHEY_COMPLEX, (double)imageSize/1200.0, Scalar(255));

            if (video){
                Mat bgr = Mat(imageSize, imageSize, CV_8UC3);
                cvtColor(mandelbrotMember, bgr, COLOR_GRAY2BGR);
                vidOut.write(bgr);
            }

            if (viewer){
                imshow("Mandelbrot", mandelbrotMember);
                if (waitKey(30)=='q'){
                    break;
                }
            }
        }
    }

    // Final image output
    mandelbrotMember = complexAbsolute(mandelbrot_value_re,mandelbrot_value_im) <= 2.0;
    imwrite(outputPath, mandelbrotMember);

    return 0;
}