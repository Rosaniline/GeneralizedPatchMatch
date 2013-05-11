//
//  GeneralizedPatchMatch.cpp
//  GeneralizedPatchMatch
//
//  Created by Rosani Lin on 13/5/10.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#include "GeneralizedPatchMatch.h"


GeneralizedPatchMatch::GeneralizedPatchMatch (int patch_height, int patch_width, int k) {
    
    PATCHHEIGHT = patch_height, PATCHWIDTH = patch_width, NEAREST_K = k;
    
    patchTool = patchTools(PATCHHEIGHT, PATCHWIDTH);
    
    genRandom = RNG(time(NULL));
    
}

GeneralizedPatchMatch::~GeneralizedPatchMatch () {}


dynamicArray2D<p_offset> GeneralizedPatchMatch::matchImage (const cv::Mat &img_src, const cv::Mat &img_dst) {
    
    Mat src = img_src.clone(), dst = img_dst.clone();
    
    if ( src.type() != CV_64FC3 ) {
        src.convertTo(src, CV_64FC3);
        src /= 255.0;
    }
    
    if ( dst.type() != CV_64FC3 ) {
        dst.convertTo(dst, CV_64FC3);
        dst /= 255.0;
    }
    
    
    dynamicArray2D<p_offset> NearNei = dynamicArray2D<p_offset>(dst.rows, dst.cols, NEAREST_K);
    
    Mat temp = src.clone();
    
    // random initialization of Nearest Neighbor Field
    for (int i = 0; i < NearNei.rows; i ++) {
        for (int j = 0; j < NearNei.cols; j ++) {
            
            
            for (int k = 0; k < NearNei.at(i, j).size(); k ++) {
                
                Point temp_pt = Point(genRandom.uniform(0, src.cols - 1) - j, genRandom.uniform(0, src.rows - 1) - i);
                double temp_er = patchTool.SSD(src, temp_pt + Point(j, i), dst, Point(j, i));
                
                NearNei.at(i, j).push(temp_pt, temp_er);
                
            }

        }
    }
    

    
    
    r_time time;
    time.start();
    
        NearNei = match(src, dst, NearNei);
    
    time.end();
    time.printTime("sequential");
    
//    for (int k = 0; k < NEAREST_K; k ++) {
//
//        reconstDisplay(src, NearNei, k, 1);
//        reconstErrorDisplay(src, dst, NearNei, k, 1);
//        distanceDisplay(NearNei, k, 1);
//        showMat(src, "s", 0);
//        
//    }
    
    RepetendDisplay(src, NearNei);


    
    
    //    time.start();
    //
    //        dynamicArray2D<Point> mulNearNei = multilevelMatch(src, dst);
    //
    //    time.end();
    //    time.printTime("multiscale");
    //
    //    cout<<"TEST = "<<convergenceTest(NearNei, mulNearNei);
    
    src.release();
    dst.release();

    
    return NearNei;
}


dynamicArray2D<p_offset> GeneralizedPatchMatch::match (const Mat& src, const Mat& dst, dynamicArray2D<p_offset>& NearNei) {
    
    dynamicArray2D<p_offset> NearNei_pre = dynamicArray2D<p_offset>(NearNei.rows, NearNei.cols, NEAREST_K);
    
    int k = 0;
    do {
        
        k ++;
        for (int i = 0; i < NearNei_pre.rows; i ++) {
            for (int j = 0; j < NearNei_pre.cols; j ++) {
                
                NearNei_pre.at(i, j) = NearNei.get(i, j);
                
            }
        }
    

        propagation(src, dst, NearNei);
        
        
    } while (!convergenceTest(NearNei, NearNei_pre) && k < 20);

    
    
    NearNei_pre.release();
    
    return NearNei;
    
}


void GeneralizedPatchMatch::propagation(const cv::Mat &src, const cv::Mat &dst, dynamicArray2D<p_offset> &NearNei) {
    
    for (int i = 0; i < NearNei.rows; i ++) {
        for (int j = 0; j < NearNei.cols; j ++) {
            
            if ( i >= 1 ) {

                update(src, dst, NearNei.at(i, j), NearNei.at(i - 1, j), Point(j, i));
                
            }
            
            if ( j >= 1 ) {
                
                update(src, dst, NearNei.at(i, j), NearNei.at(i, j - 1), Point(j, i));
                
            }
            
            randomSearch(src, dst, NearNei.at(i, j), Point(j, i));
            
        }
    }
    
    
    for (int i = NearNei.rows - 1; i >= 0; i --) {
        for (int j = NearNei.cols - 1; j >= 0; j --) {
            
            if ( i < NearNei.rows - 1 ) {
                
                update(src, dst, NearNei.at(i, j), NearNei.at(i + 1, j), Point(j, i));
                
            }
            
            if ( j < NearNei.cols - 1 ) {
                
                update(src, dst, NearNei.at(i, j), NearNei.at(i, j + 1), Point(j, i));
                
            }
            
            randomSearch(src, dst, NearNei.at(i, j), Point(j, i));
            
        }
    }
    
    
    
}



void GeneralizedPatchMatch::randomSearch (const Mat& src, const Mat& dst, p_offset& local_off, const Point& loc) {
    
    double alpha = 1.0, w = MAX(dst.rows, src.rows);
    
    while ( alpha * w >= 1 ) {

        for (int loc_k = 0; loc_k < local_off.size(); loc_k ++) {
            
            Point random_loc = local_off.get(loc_k).offset + loc + Point(w*alpha*genRandom.uniform(-1.0, 1.0), w*alpha*genRandom.uniform(-1.0, 1.0));
            random_loc = Point((random_loc.x + src.cols)%src.cols, (random_loc.y + src.rows)%src.rows);
            
            local_off.push(random_loc - loc, patchTool.SSD(src, random_loc, dst, loc));
            
        }
        
        alpha /= 2.0;
    }

    

}




void GeneralizedPatchMatch::update (const Mat& src, const Mat& dst, p_offset& local_offs, const p_offset& nex_offs, const Point& loc) {
    
    for (int nex_k = 0; nex_k < nex_offs.size(); nex_k ++) {
        
        Point nex_pt = nex_offs.get(nex_k).offset + loc;
        
        if ( patchTool.patchWithInImg(src, nex_pt) ) {
            
            local_offs.push(nex_offs.get(nex_k).offset, patchTool.SSD(src, nex_pt, dst, loc));
            
        }
        
    }
    
    
}



bool GeneralizedPatchMatch::convergenceTest(const dynamicArray2D<p_offset> &NearNei_new, const dynamicArray2D<p_offset> &NearNei_old) {
    

//    Mat temp = Mat::zeros(NearNei_new.rows, NearNei_new.cols, CV_8UC1);
    
    bool test = true;
   
    for (int i = 0; i < NearNei_new.rows && test; i ++) {
        for (int j = 0; j < NearNei_new.cols && test; j ++) {
    
//    for (int i = 0; i < NearNei_new.rows; i ++) {
//        for (int j = 0; j < NearNei_new.cols; j ++) {
    
//            variance_count += NearNei_new.at(i, j) == NearNei_old.at(i, j) ? 0 : 1;
            for (int k = 0; k < NearNei_new.get(i, j).size(); k ++) {
                test = NearNei_new.get(i, j).get(k).error == NearNei_old.get(i, j).get(k).error ? test : false;
//                if ( !(NearNei_new.at(i, j).traverse(k).error == NearNei_old.at(i, j).traverse(k).error) ) {
//                    temp.at<uchar>(i, j) = 255;
//                }
                
            }
            
        
        }
    }
    
//    showMat(temp, "tempp", 0);
    
    return test;
}


// Display the reconstructed image according to the nearest neighbor field
void GeneralizedPatchMatch::reconstDisplay (const Mat& src, const dynamicArray2D<p_offset>& NearNei, int k, int time) {
    
    Mat mapped = Mat::zeros(NearNei.rows, NearNei.cols, CV_64FC3);
    
    for (int i = 0; i < mapped.rows; i ++) {
        for (int j = 0; j < mapped.cols; j ++) {
            
            mapped.at<Vec3d>(i, j) = src.at<Vec3d>(NearNei.get(i, j).get(k).offset + Point(j, i));
            
        }
    }
    
    showMat(mapped, "mapped", time);
    
    mapped.release();
}


void GeneralizedPatchMatch::distanceDisplay (const dynamicArray2D<p_offset>& array, int k, int time) {
    
    Mat temp = Mat::zeros(array.rows, array.cols, CV_64FC1);
    
    for (int i = 0; i < array.rows; i ++) {
        for (int j = 0; j < array.cols; j ++) {
            
            Point offset = array.get(i, j).get(k).offset;
            
            temp.at<double>(i, j) = sqrt(offset.x*offset.x + offset.y*offset.y);
            
        }
    }
    
    
    double temp_max = maxMatItem(temp);
    temp /= temp_max;
    
    showMat(temp, "distance", time);
    
    temp.release();
    
}



void GeneralizedPatchMatch::reconstErrorDisplay (const Mat& src, const Mat& dst, const dynamicArray2D<p_offset>& NearNei, int k, int time) {
    
    
    Mat temp = Mat::zeros(dst.size(), CV_64FC1), mapped = Mat::zeros(dst.size(), dst.type());
    
    
    for (int i = 0; i < mapped.rows; i ++) {
        for (int j = 0; j < mapped.cols; j ++) {
            
            mapped.at<Vec3d>(i, j) = src.at<Vec3d>(NearNei.get(i, j).get(k).offset + Point(j, i));
            
        }
    }
    
    for (int i = 0; i < dst.rows; i ++) {
        for (int j = 0; j < dst.cols; j ++) {
            
            temp.at<double>(i, j) = Vec3dDiff(dst.at<Vec3d>(i, j), mapped.at<Vec3d>(i, j));
            
        }
    }
    
//    double temp_max = maxMatItem(temp);
//    temp /= temp_max;
    
    temp = temp*3;
    
    showMat(temp, "error", time);
    
    temp.release();
}


void GeneralizedPatchMatch::RepetendDisplay(const cv::Mat src, const dynamicArray2D<p_offset> &NeaNei) {
    
    Mat temp = Mat::zeros(src.size(), CV_64FC1);
    
    for (int i = 0; i < temp.rows; i ++) {
        for (int j = 0; j < temp.cols; j ++) {
            
            for (int k = 0; k < NeaNei.get(i, j).size(); k ++) {
                
//                for (int p = -PATCHHEIGHT; p <= PATCHHEIGHT; p ++) {
//                    for (int q = -PATCHWIDTH; q <= PATCHWIDTH; q ++) {
                
                for (int n = 0; n < NeaNei.get(i, j).size(); n ++) {
                    temp.at<double>(i, j) += patchTool.SSD(src, NeaNei.get(i, j).get(k).offset + Point(j, i), src, NeaNei.get(i, j).get(n).offset + Point(j, i));
                }
                

                
//                    }
//                }
            }
            
            
        }
    }
    
    
//    double temp_max = maxMatItem(temp);
//    temp /= temp_max;
    
//    temp = Mat::ones(temp.size(), temp.type()) - temp;
    
    for (int i = 0; i < 10; i ++) {
        
        showMat(temp, "repetend", 0);
        
        temp = temp*(2);
    }
    
    
    temp.release();
    
}







