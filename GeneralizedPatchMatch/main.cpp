//
//  main.cpp
//  GeneralizedPatchMatch
//
//  Created by Rosani Lin on 13/5/10.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#include <iostream>
#include "GeneralizedPatchMatch.h"


int main(int argc, const char * argv[])
{
    Mat src, dst;
    
    src = imread("/Users/xup6qup3/Dropbox/code/Dev.temp/PatchMatch/PatchMatch/sunflower1.jpg");
    dst = imread("/Users/xup6qup3/Dropbox/code/Dev.temp/PatchMatch/PatchMatch/sunflower2.jpg");
    
    resizeMat(src, 0.1);
    resizeMat(dst, 0.1);
    
    
    
    GeneralizedPatchMatch genPatchMatch = GeneralizedPatchMatch(11, 11, 10);
    
    genPatchMatch.matchImage(src, dst);
    
    return 0;
}

