#ifndef SSLINESEGMENT_H
#define SSLINESEGMENT_H

#include "SSPathOperator.h"

class SSLineSegment: public SSPathOperator
{
public:
    SSLineSegment();

    Point p;
};

#endif // SSLINESEGMENT_H
