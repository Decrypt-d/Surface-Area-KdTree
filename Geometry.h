#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Vec.h"

class Geometry
{
public:
    virtual bool intersect(const Vec& p, const Vec& i, Vec& np, Vec& n) const=0;
    virtual Vec samplePoint(float& pdf) const=0;
    //virtual bool boundingBoxCheck(const Vec& p, const Vec& i) =0;
};

#endif
