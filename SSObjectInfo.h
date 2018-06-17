#ifndef SSOBJECTINFO_H
#define SSOBJECTINFO_H

#include "Object/SSObject.h"
#include <list>
#include <vector>
#include <QString>

class SSObjectInfo
{
public:
    SSObjectInfo();
    ~SSObjectInfo();

    int on;//object number
    int ref;//字节地址
    int gn;//generation number
    bool status;
    list<vector<QString >* > dictionary;
    int length;
    SSObject* obj;
};

#endif // SSOBJECTINFO_H
