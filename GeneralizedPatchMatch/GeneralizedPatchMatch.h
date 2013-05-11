//
//  GeneralizedPatchMatch.h
//  GeneralizedPatchMatch
//
//  Created by Rosani Lin on 13/5/10.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#ifndef __GeneralizedPatchMatch__GeneralizedPatchMatch__
#define __GeneralizedPatchMatch__GeneralizedPatchMatch__

#include <iostream>
#include "Rosaniline.h"
#include "offsetElement.h"


class GeneralizedPatchMatch {
    
public:
    
    GeneralizedPatchMatch (int patch_height, int patch_width, int k);
    ~GeneralizedPatchMatch ();
    
    // Run GeneralizedPatchMatch according to images
    dynamicArray2D<p_offset> matchImage (const Mat& img_src, const Mat& img_dst);
    
    
private:
    
    // Core of PatchMatch
    dynamicArray2D<p_offset> match (const Mat& src, const Mat& dst, dynamicArray2D<p_offset>& NearNei);
    
    
    // Propagation
    void propagation (const Mat& src, const Mat& dst, dynamicArray2D<p_offset>& NearNei);
    
    // Random Search
    void randomSearch (const Mat& src, const Mat& dst, p_offset& local_off, const Point& loc);
    
    
    
    // Convergence condition test
    bool convergenceTest (const dynamicArray2D<p_offset>& NearNei_new, const dynamicArray2D<p_offset>& NearNei_old);
    
    // Upsample procedure applied in multilevel mechanism
    void upsample (const Mat& src, const Mat& dst, dynamicArray2D<p_offset>& NearNei);

    
    // Update the offset with minimum error
    void update (const Mat& src, const Mat& dst, p_offset& local_offs, const p_offset& nex_offs, const Point& loc);
    
    
    
    // Display the reconstructed image according to the nearest neighbor field
    void reconstDisplay (const Mat& src, const dynamicArray2D<p_offset>& NearNei, int k, int time);
    
    // Display the locality distance of nearest neighbor
    void distanceDisplay (const dynamicArray2D<p_offset>& array, int k, int time);
    
    // Display the error distance of the reconstructed image
    void reconstErrorDisplay (const Mat& src, const Mat& dst, const dynamicArray2D<p_offset>& NearNei, int k, int time);
    
    
    void RepetendDisplay (const Mat src, const dynamicArray2D<p_offset>& NeaNei);

    
    
    patchTools patchTool;
    RNG genRandom;
    
    int PATCHHEIGHT, PATCHWIDTH, NEAREST_K;
    
};


#endif /* defined(__GeneralizedPatchMatch__GeneralizedPatchMatch__) */
