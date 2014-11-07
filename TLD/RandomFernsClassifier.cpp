//
//  RandomFernsClassifier.cpp
//  TLD
//
//  Created by 陈裕昕 on 11/4/14.
//  Copyright (c) 2014 Fudan. All rights reserved.
//

#include "RandomFernsClassifier.h"

RandomFernsClassifier::RandomFernsClassifier()
{
    
}

RandomFernsClassifier::RandomFernsClassifier(int _nFerns, int _nLeaves)
{
    nFerns = _nFerns;
    nLeaves = _nLeaves;
    
    for(int i = 0; i < nFerns; i++)
    {
        counter.push_back(tPNCounter(pow(2, nLeaves), make_pair(0, 0)));
    }
    
    for(int i = 0; i < nFerns; i++)
    {
        vector<tLeaf> leaves;
        for(int j = 0; j < nLeaves; j++)
        {
            tLeaf leaf;
            leaf.first = Point2f(float(theRNG()), float(theRNG()));
            leaf.second = Point2f(float(theRNG()), float(theRNG()));
            
            leaves.push_back(leaf);
        }
        
        ferns.push_back(leaves);
    }
}

RandomFernsClassifier::~RandomFernsClassifier()
{
    
}

void RandomFernsClassifier::update(const Mat &_img, bool c, float p)
{
    Mat img;
    GaussianBlur(_img, img, Size(3, 3), 0);
    
    //imshow("ttt", img);
    //waitKey();
    
    if(p == -1.) p = getPosteriors(img);
    
    if(c == cPos)
    {
        if(p <= pTh)
        {
            //static int count = 0;
            //cerr << ++count << endl;
            for(int iFern = 0; iFern < nFerns; iFern++)
            {
                int code = getCode(img, iFern);
                counter[iFern][code].first++;
                //cerr << iFern << " " << code << endl;
            }
        }
    }
    else
    {
        if(p >= nTh)
        {
            for(int iFern = 0; iFern < nFerns; iFern++)
            {
                int code = getCode(img, iFern);
                counter[iFern][code].second++;
                //cerr << iFern << " " << code << endl;
            }
        }
    }
}

int RandomFernsClassifier::getCode(const Mat &img, int idx)
{
    //cerr << "code at fern " << idx << ": ";
    assert(img.type() == CV_8U);
    int code = 0;
    for(int i = 0; i < nLeaves; i++)
    {
        int p1x = ferns[idx][i].first.x * img.cols;
        int p1y = ferns[idx][i].first.y * img.rows;
        int p2x = ferns[idx][i].second.x * img.cols;
        int p2y = ferns[idx][i].second.y * img.rows;
        
        // use char instead of int
        int v1 = img.at<char>(p1y, p1x);
        int v2 = img.at<char>(p2y, p2x);
        
        code = (code << 1) | (v1 < v2);
        
        //cerr << (v1 < v2);
    }
    
    //cerr << endl;
    return code;
}

float RandomFernsClassifier::getPosteriors(const Mat &img)
{
    float sumP = 0;
    for(int i = 0; i < nFerns; i++)
    {
        int code, p, n;
        code = getCode(img, i);
        p = counter[i][code].first;
        n = counter[i][code].second;
        
        if(p == 0)
            ;
            //sumP += 0.0;
        else
            sumP += (float)p / (p + n);
    }
    
    float averageP = sumP / nFerns;
    
    // debug
    //if(averageP > 0.5){
    //    cerr << averageP << endl;
    //    imshow("p", img);
    //    waitKey();
    //}
    //imshow("p", img);
    //waitKey();
    // end debug
    
    return averageP;
}

bool RandomFernsClassifier::getClass(const Mat &_img)
{
    Mat img;
    GaussianBlur(_img, img, Size(3, 3), 0);
    
    return getPosteriors(img) >= pTh;
}

void RandomFernsClassifier::train(const tTrainDataSet &trainDataSet)
{
    for(auto trainData : trainDataSet)
    {
        update(trainData.first, trainData.second);
    }
}