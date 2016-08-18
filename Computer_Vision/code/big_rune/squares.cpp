// The "Square Detector" program.
// It loads several images sequentially and tries to find squares in
// each image

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */


#include <iostream>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>     //for using the function sleep
using namespace cv;
using namespace std;

static void help()
{
    cout <<
    "\nA program using pyramid scaling, Canny, contours, contour simpification and\n"
    "memory storage to find squares in a list of images\n"
    "Returns sequence of squares detected on the image.\n"
    "the sequence is stored in the specified memory storage\n"
    "Call:\n"
    "./squares\n"
    "Using OpenCV version %s\n" << CV_VERSION << "\n" << endl;
}


int thresh = 50, N = 5;
double squareThresh = 0.2;
int overlapThresh = 50;
int sqaureSizeThresh = 10000;
int greyThresh = 140;
const char* wndname = "Square Detection Demo";

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
static double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
static void findSquares( const Mat& image, vector<vector<Point> >& squares )
{
    squares.clear();

//s    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

    // down-scale and upscale the image to filter out the noise
    //pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    //pyrUp(pyr, timg, image.size());


    // blur will enhance edge detection
    Mat timg(image);
    medianBlur(image, timg, 9);
    Mat gray0(timg.size(), CV_8U), gray;

    vector<vector<Point> > contours;
    vector<vector<Point> > tempSquares;
    // find squares in every color plane of the image
    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
        mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for( int l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny(gray0, gray, 5, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, Mat(), Point(-1,-1));
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/N;
            }

            // find contours and store them all as a list
            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

            vector<Point> approx;
            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( approx.size() == 4 &&
                    fabs(contourArea(Mat(approx))) > 1000 && fabs(contourArea(Mat(approx))) < sqaureSizeThresh && 
                    isContourConvex(Mat(approx)) )
                {
                    double maxCosine = 0;
		    // check length of edges of square are close
		    double edge = sqrt(pow(approx[1].x - approx[0].x, 2) + pow(approx[1].y - approx[0].y, 2));
                    for( int j = 2; j < 5; j++ )
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
			double thisEdge = sqrt(pow(approx[j%4].x - approx[j-1].x, 2) + pow(approx[j%4].y - approx[j-1].y, 2));
			if (abs((edge-thisEdge)/edge)> squareThresh){
				edge = -1;				
			}
			
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( maxCosine < 0.3 && edge >= 0)
                        tempSquares.push_back(approx);
			//squares.push_back(approx);
                }
            }

        }
    }
    if (tempSquares.size() > 0){
    cout<<tempSquares.size()<<"!!!"<<endl;
    std::vector<vector<Point> >::iterator it1 = tempSquares.begin();
    while(it1 != tempSquares.end()){
	vector<Point> sq1 = (*it1);
        std::vector<vector<Point> >::iterator it2 = (it1+1);
	while(it2 != tempSquares.end()){
		vector<Point> sq2 = (*it2);
                double center_X1 = (sq1[0].x + sq1[1].x + sq1[2].x + sq1[3].x)/4.0;
		double center_Y1 = (sq1[0].y + sq1[1].y + sq1[2].y + sq1[3].y)/4.0;
		double center_X2 = (sq2[0].x + sq2[1].x + sq2[2].x + sq2[3].x)/4.0;
		double center_Y2 = (sq2[0].y + sq2[1].y + sq2[2].y + sq2[3].y)/4.0;
		double L2Distance = sqrt(pow(center_X1 - center_X2, 2) + pow(center_Y1 - center_Y2, 2));
		if(L2Distance < overlapThresh){//abs(center_X1 - center_X2) < overlapThresh && abs(center_Y1 - center_Y2) < overlapThresh){
			break;
		}
		it2++;
	} 
	if(it2 == tempSquares.end()){
		squares.push_back(sq1);		
	}
	it1++;
    }
    }
}


// the function draws all the squares in the image
static void drawSquares( Mat& image, const vector<vector<Point> >& squares )
{
    for( size_t i = 0; i < squares.size(); i++ )
    {
        const Point* p = &squares[i][0];

        int n = (int)squares[i].size();
        //dont detect the border
        if (p-> x > 3 && p->y > 3)
          polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3);
    }

    imshow(wndname, image);
}


int main(int /*argc*/, char** /*argv*/)
{

    VideoCapture stream1(1);
    stream1.set(CV_CAP_PROP_FRAME_WIDTH,540);
    stream1.set(CV_CAP_PROP_FRAME_HEIGHT,360);
    clock_t t;
    if (!stream1.isOpened()) { //check if video device has been initialised
	cout << "cannot open camera";
    }
    vector<vector<Point> > squares;
    for (int frame = 0; frame <5; frame++){
    int i = 0;
    Mat cameraFrame;
    while (true) {
	if (i%1 == 0){
                t = clock();
		stream1.read(cameraFrame);
		//imshow("cam", cameraFrame);
		if(cameraFrame.empty() )
		{
		    cout << "Couldn't load "<< endl;
		    continue;
		}

		findSquares(cameraFrame, squares);
		drawSquares(cameraFrame, squares);
		cout<<squares.size()<<endl;
		for( size_t i = 0; i < squares.size(); i++ )
   		{
        		const Point* p = &squares[i][0];

        		int n = (int)squares[i].size();
        		//cout<<p->x<<"  "<<p->y<<endl;
        	}
		if (waitKey(30) >= 0){
		    break;
		}
		if(squares.size() == 9){
			break;		
		} 
	}
	i++;
    }
    int orderOfSquares[9] = {0,1,2,3,4,5,6,7,8};
    for(int j= 0; j < 8; j++){
	vector<Point> sq1 = (squares[orderOfSquares[j]]);
	double minY = (sq1[0].y + sq1[1].y + sq1[2].y + sq1[3].y)/4.0;
	int minIndex = j;
    	for(int k = j+1; k < 9; k++){
		vector<Point> sq2 = (squares[orderOfSquares[k]]);
		Y = (sq2[0].y + sq2[1].y + sq2[2].y + sq2[3].y)/4.0;
		if(Y < minY){
			minY = 	Y;
			minIndex = k;
		}
	}
	if(minIndex != j){
		int temp = orderOfsquare[j];
		orderOfsquare[j] = orderOfsquare[minIndex];
		orderOfsquare[minIndex] = temp;
	}
    }
    for(int layer = 0; layer < 3; layer++){
    	for(int j= 3*layer; j < 3*(layer+1) -1 ; j++){
	vector<Point> sq1 = (squares[orderOfSquares[j]]);
	double minX = (sq1[0].x + sq1[1].x + sq1[2].x + sq1[3].x)/4.0;
	int minIndex = j;
    	for(int k = j+1; k < 3*(layer+1); k++){
		vector<Point> sq2 = (squares[orderOfSquares[k]]);
		X = (sq2[0].x + sq2[1].x + sq2[2].x + sq2[3].x)/4.0;
		if(X < minX){
			minX = 	X;
			minIndex = k;
		}
	}
	if(minIndex != j){
		int temp = orderOfsquare[j];
		orderOfsquare[j] = orderOfsquare[minIndex];
		orderOfsquare[minIndex] = temp;
	}
    }
    }
    imwrite("image.jpg", cameraFrame); //for testing, comment out when run
    stream1.set(CV_CAP_PROP_FRAME_WIDTH,540);//for testing, comment out when run
    stream1.set(CV_CAP_PROP_FRAME_HEIGHT,360); //for testing, comment out when run
    stream1.read(cameraFrame);  //for testing, comment out when run
    imwrite("image2.jpg", cameraFrame); //for testing, comment out when run
    int count[9] = {0,0,0,0,0,0,0,0,0};
    Mat cameraFrameGrey;
    cv::cvtColor(cameraFrame, cameraFrameGrey, CV_BGR2GRAY);
    imwrite("imageG.jpg", cameraFrameGrey); //for testing, comment out when run
    Mat cameraFrameBW (cameraFrameGrey.size(), cameraFrameGrey.type());
    threshold(cameraFrameGrey, cameraFrameBW, 100, 255 ,THRESH_BINARY);
    imwrite("imageBW.jpg", cameraFrameBW); //for testing, comment out when run
    int c = 0;
    /*for(int y = 0; y < cameraFrame.rows; y++){
    	for(int x = 0; x < cameraFrame.cols; x++){
                cout<<cameraFrameBW.at<Vec3b>(y, x)<<endl;
		int grey = (int)(cameraFrameGrey.at<Vec3b>(y, x)[0]) + (int)cameraFrameGrey.at<Vec3b>(y, x)[1] + (int)cameraFrameGrey.at<Vec3b>(y, x)[2];  
		if (grey > greyThresh*3){
			for(int s = 0; s < 9; s++){
				if(pointPolygonTest(squares[s],Point2f((int)(y/3.0),(int)(x/3.0)),false) > 0){
					count[s] = count[s] + 1;					
					break;				
				}
			}
		}
	}
    }*/
    
    for(int y = 0; y < cameraFrameBW.rows; y++){
    	for(int x = 0; x < cameraFrameBW.cols; x++){  
		if ((int)cameraFrameBW.at<uchar>(y,x) == 255){
			for(int s = 0; s < 9; s++){
				if(pointPolygonTest(squares[s],Point2f((int)(x),(int)(y)),true) >= 2){
					count[s] = count[s] + 1;				
					break;				
				}
			}
		}
	}
    }
    double minRatio = 5.0;
    int minIndex = -1;
    for(int s = 0; s < 9; s++){
	double ratio = count[s]*1.0/contourArea(squares[s]);
        vector<Point> sq1 = (squares[s]);
        double center_X = (sq1[0].x + sq1[1].x + sq1[2].x + sq1[3].x)/4.0;
	double center_Y = (sq1[0].y + sq1[1].y + sq1[2].y + sq1[3].y)/4.0;
	//cout<<center_X<< " "<<center_Y<<endl;
        //cout<<contourArea(sq1)<<" "<< count[s]<< " "<<ratio<<endl;
	//cout<<"----------------------"<<endl;
	if(minIndex == -1 || minRatio > ratio){
		minRatio = ratio;
		minIndex = s;
	}	
    }
    for (int j = 0; j < 9; j++){
    	if(orderOfSquares[j] == minIndex){
		cout <<j<<endl;
		break;
	}
    }
    /*vector<Point> sq1 = (squares[minIndex]);//for testing, comment out when run
    double center_X = (sq1[0].x + sq1[1].x + sq1[2].x + sq1[3].x)/4.0; //for testing, comment out when run
    double center_Y = (sq1[0].y + sq1[1].y + sq1[2].y + sq1[3].y)/4.0; //for testing, comment out when run
    cout<<"Final square "<< center_X<< " "<<center_Y<<endl; //for testing, comment out when run
    //cout<<((float)(clock()-t))/CLOCKS_PER_SEC<<endl;*/
    usleep(500000); 
    }
    /*

    static const char* names[] = { "collage.jpg",0 };
    help();
    namedWindow( wndname, 1 );
    vector<vector<Point> > squares;

    for( int i = 0; names[i] != 0; i++ )
    {
        Mat image = imread(names[i], 1);
        if( image.empty() )
        {
            cout << "Couldn't load " << names[i] << endl;
            continue;
        }

        findSquares(image, squares);
        drawSquares(image, squares);
        //imwrite( "out", image );
        int c = waitKey();
        if( (char)c == 27 )
            break;
    }*/

    return 0;
}
