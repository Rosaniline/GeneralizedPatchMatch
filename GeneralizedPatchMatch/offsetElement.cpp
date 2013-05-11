//
//  offset.cpp
//  GeneralizedPatchMatch
//
//  Created by Rosani Lin on 13/5/11.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#include "offsetElement.h"



offsetElement::offsetElement () {
    
    offset = Point(0);
    error = INFINITY;
}


offsetElement::~offsetElement () {
    
    offset.~Point();
}











p_offset::p_offset () {

    // Number of Nearest Neighbors, deafault to be 5
    NEAREST_K = 5;
    initialize ();
    
}

p_offset::p_offset (int k) {
    
    NEAREST_K = k;
    initialize ();

}

p_offset::p_offset (const p_offset &an_offset) {
    
//    cout<<"p_offset copied.\n";
    
    NEAREST_K = an_offset.size();
    
    offset = new offsetElement[NEAREST_K];
    
    for (int i = 0; i < NEAREST_K; i ++) {
        
        offset[i] = an_offset.get(i);
    }
    
}


p_offset::~p_offset() {


}


void p_offset::initialize () {
    
    offset = new offsetElement[NEAREST_K];
    
    for (int i = 0; i < NEAREST_K; i ++) {
        
        offset[i].offset = Point(0);
        offset[i].error = INFINITY;
    }
    
}



void p_offset::push (offsetElement element) {
    
    for (int i = 0; i < NEAREST_K; i ++) {
        
        if ( offset[i].error > element.error ) {
            
            for (int j = NEAREST_K - 1; j > i; j --) {
                
                offset[j] = offset[j - 1];
            }
            
            offset[i] = element;
            
            break;
        }
        
    }

}


void p_offset::push(const Point &candi_offset, double candi_error) {
    
    for (int i = 0; i < NEAREST_K; i ++) {
        
        if ( offset[i].error > candi_error ) {
            
            for (int j = NEAREST_K - 1; j > i; j --) {
                
                offset[j] = offset[j - 1];
            }
            
            offset[i].offset = candi_offset;
            offset[i].error = candi_error;
            
            break;
        }
        
    }
    
}


offsetElement p_offset::get (int i) const {
    
    return offset[i];
}

offsetElement& p_offset::at(int i) {
    
    return offset[i];
}


int p_offset::size() const {
    
    return NEAREST_K;
}






