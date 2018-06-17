#ifndef SSPATH_H
#define SSPATH_H

#include "../SSObject.h"
#include "SSPathOperator.h"
#include <vector>

class SSPath: public SSObject
{
public:
    SSPath();

    vector<SSPathOperator* > path;
};

#endif // SSPATH_H
