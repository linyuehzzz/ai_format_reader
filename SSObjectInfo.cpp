#include "SSObjectInfo.h"

SSObjectInfo::SSObjectInfo()
{
    this->on = 0;
    this->ref = 0;
    this->gn = 0;
    this->length = 0;
    this->status = false;

    this->obj = new SSObject();
}

SSObjectInfo::~SSObjectInfo()
{
    if(obj != 0x00)
    {
        delete obj;
        obj = 0x00;
    }
}
