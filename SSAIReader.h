#ifndef SSAIREADER_H
#define SSAIREADER_H

#include "SSObjectInfo.h"
#include <iostream>

class SSAIReader
{
public:
    SSAIReader(const char* readFilePath);

    list<SSObjectInfo* > objs;

    void ReadAIFile();
    void ReadXRef(FILE *fp);
    int GetPage(FILE *fp, int on, int ref, int gn);
    int GetKid(FILE *fp, int on, int ref, int gn);
    int GetIllustrator(FILE *fp, int on, int ref, int gn);
    int GetPrivate(FILE *fp, int on, int ref, int gn);
    vector<int> GetDataRef(FILE *fp, int on, int ref, int gn);

    void GetObject(FILE *fp, int on, int ref, int gn);
    void GetLayer(char *stream, int length);

private:
    const char* readFilePath;
};

#endif // SSAIREADER_H
