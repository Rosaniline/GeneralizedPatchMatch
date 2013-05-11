//
//  offset.h
//  GeneralizedPatchMatch
//
//  Created by Rosani Lin on 13/5/11.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#ifndef __GeneralizedPatchMatch__offset__
#define __GeneralizedPatchMatch__offset__

#include <iostream>
#include "Rosaniline.h"

class offsetElement {
    
public:
    
    offsetElement ();
    
    offsetElement (const offsetElement& element) {
        
//        cout<<"offsetElement copied.\n";
        
        offset = element.offset;
        error = element.error;
        
    }
    
    ~offsetElement ();
    

    bool operator < (const offsetElement& rhs) const {
        
        return error < rhs.error;
    }

    bool operator == (const offsetElement& rhs) const {
        
        return ( offset == rhs.offset && error == rhs.error );
        
    }
        
    friend ostream& operator << (ostream& os, const offsetElement& element) {
        
        os << element.offset << ", " << element.error << "\n";
        return os;
    }
        
        
    Point offset;
    double error;        
    
};
        
        

class p_offset {
    
public:
    
    p_offset ();
    p_offset (int k);
    
    p_offset (const p_offset &an_offset);
    
    ~p_offset ();
    
    void initialize (); 
    
    void push (offsetElement element);
    
    void push (const Point& candi_offset, double candi_error );
    
    offsetElement get (int i) const;
    offsetElement &at(int i);
    
    int size() const;

    
    
    
    
    
    bool operator == (const p_offset& rhs) const {
        
        bool test = true;
        
        for (int i = 0; i < NEAREST_K; i ++) {
            
            test = offset[i] == rhs.get(i) ? test : false;
        }
        
        
        return test;
    }
    
    
private:
    
    int NEAREST_K;
    offsetElement *offset;
    
};




#endif /* defined(__GeneralizedPatchMatch__offset__) */
