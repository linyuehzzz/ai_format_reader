#include "SSAIReader.h"

SSAIReader::SSAIReader(const char *readFilePath)
{
    this->readFilePath = readFilePath;
}

void SSAIReader::ReadAIFile()
{
    FILE *fp = fopen(this->readFilePath, "r");
    if(fp == 0x00)
    {
        fputs("Can't open the file!", stderr);
        return ;
    }

    ReadXRef(fp);
}

void SSAIReader::ReadXRef(FILE *fp)
{
    /* 从最后一行读取xref的偏移地址（相对于文件开始） */
    char ch; //用来保存读取的字符
    int xref = 0; //Cross-reference Table的偏移字节数
    int k = -10; //%%EOF占10个字节
    fseek(fp,k,SEEK_END);//移动指针离文件结尾 1 字节处
    ch = fgetc(fp);
    while (ch != '\n')
    {
        xref += (ch - '0') * pow(10,-k - 10);
        //文件内部指针从文件结尾向前移动
        k--;
        fseek(fp, k, SEEK_END);
        ch = fgetc(fp);
    }

    /* 读取xref的起始object number和数量信息 */
    /******* xref
     *       begin count *******/
    int begin = 0, count = 0;
    fseek(fp, xref + sizeof(char) * 6, SEEK_SET);//跳过交叉表头xref
    ch = fgetc(fp);
    while (ch != ' ')
    {
        begin *= 10;
        begin += (ch - '0');
        ch = fgetc(fp);
    }

    ch = fgetc(fp);
    while (ch != '\n')
    {
        count *= 10;
        count += (ch - '0');
        ch = fgetc(fp);
    }

    /* 分配内存,存储xref中的对象索引信息表（xref第三行到最后一行） */
    /******* nnnnnnnnnn(byte address) ggggg(generation number) N(status) eol *******/
    int len = count * 20;//每行20字节，包括换行符
    char* buf = new char[len];
    fread(buf, 1, len, fp);

    /* 从buf中读取PrivateData的Object Number */
    int m = -1;
    int on, ref, gn;
    bool status = false;
    //* Get Page *//
    int pID = 0, kidID = 0, iID = 0, priID = 0, dID = 0;
    vector<int> dIDs(0);
    for(int i = begin;i < count; i++)
    {
        on = 0;
        ref = 0;
        gn = 0;
        //get object number
        on = i;
        //get byte address
        m++;
        ch = *(buf + m);
        while (ch != ' ')
        {
            ref *= 10;
            ref += (ch - '0');
            m++;
            ch = *(buf + m);
        }
        //get generation number
        m++;
        ch = *(buf + m);
        while (ch != ' ')
        {
            gn *= 10;
            gn += (ch - '0');
            m++;
            ch = *(buf + m);
        }
        //get status
        m++;
        ch = *(buf + m);
        if(ch == 'f') //free entry
            status = false;
        else if(ch == 'n' && pID == 0 && kidID == 0 && iID == 0 && priID == 0 && dID == 0)
        {
            //GetPageRef
            status = true;
            pID = GetPage(fp,on,ref,gn);
            if(pID != 0)
            {
                m = m + 19 * (pID - i - 1);
                i = pID - 1;
            }
        }
        else if(ch == 'n' && pID != 0 && kidID == 0 && iID == 0 && priID == 0 && dID == 0)
        {
            //GetKidRef
            status = true;
            kidID = GetKid(fp,on,ref,gn);
            if(kidID != 0)
            {
                m = m + 19 * (kidID - i - 1);
                i = kidID - 1;
            }
        }
        else if(ch == 'n' && pID != 0 && kidID != 0 && iID == 0 && priID == 0 && dID == 0)
        {
            //GetIllustratorRef
            status = true;
            iID = GetIllustrator(fp,on,ref,gn);
            if(iID != 0)
            {
                m = m + 19 * (iID - i - 1);
                i = iID - 1;
            }
        }
        else if(ch == 'n' && pID != 0 && kidID != 0 && iID != 0 && priID == 0 && dID == 0)
        {
            //GetPrivateRef
            status = true;
            priID = GetPrivate(fp,on,ref,gn);
            if(priID != 0)
            {
                m = m + 19 * (priID - i - 1);
                i = priID - 1;
            }
        }
        else if(ch == 'n' && pID != 0 && kidID != 0 && iID != 0 && priID != 0 && dID == 0)
        {
            //GetPrivateDataRef
            status = true;
            dIDs = GetDataRef(fp,on,ref,gn);
//            dID = dIDs.at(i);
//            if(dID != 0)
//            {
//                m = m + 19 * (dID - i - 1);
//                i = dID - 1;
//            }
        }
        else
            break;
        m++;
        ch = *(buf + m);
    }

    /* 从读取并存储PrivateData */
    SSObjectInfo* pd;
    int numBlock = dIDs.size();
    for(int i = 0; i < numBlock; i++)
    {

    }


    delete[] buf;
}

int SSAIReader::GetPage(FILE *fp, int on, int ref, int gn)
{
    int pID = 0;
    int on2 = 0, gn2 = 0;
    char ch; //用来保存读取的字符

    /* 检查object number和generation number与xref中信息是否匹配 */
    fseek(fp, ref, SEEK_SET);
    ch = fgetc(fp);
    while (ch != ' ')
    {
        on2 *= 10;
        on2 += (ch - '0');
        ch = fgetc(fp);
    }

    ch = fgetc(fp);
    while (ch != ' ')
    {
        gn2 *= 10;
        gn2 += (ch - '0');
        ch = fgetc(fp);
    }

    if (on != on2 || gn != gn2)
        return 0;

    /* 读取尖括号<< >>内容 */
    QString flag1, flag2;
    fseek(fp, sizeof(char) * 4, SEEK_CUR);//跳过obj eol

    ch = fgetc(fp);
    flag1.append(ch);
    ch = fgetc(fp);
    flag1.append(ch);
    if(flag1 == "<<")
    {
        ch = fgetc(fp);
        while(ch != '>')
        {
            if(ch = '/')
            {
                ch = fgetc(fp);
                while(ch != ' '&&ch != '/')
                {
                    flag2.append(ch);
                    ch = fgetc(fp);
                }

                if(flag2 == "Pages")
                {
                    QString p1,p2;
                    QChar p;
                    ch = fgetc(fp);
                    while(ch != '/'&&ch != '>')
                    {
                        p1.append(ch);
                        ch = fgetc(fp);
                    }

                    int k = 0;
                    p = p1.at(0);
                    while(p != ' ')
                    {
                        p2.append(p);
                        k++;
                        p = p1.at(k);
                    }
                    pID = p2.toInt();
                    p1.clear();
                    p2.clear();
                    break;
                }
                flag2.clear();
            }
        }
    }
    return pID;
}

int SSAIReader::GetKid(FILE *fp, int on, int ref, int gn)
{
    int kidID = 0;
    int on2 = 0, gn2 = 0;
    char ch; //用来保存读取的字符

    /* 检查object number和generation number与xref中信息是否匹配 */
    fseek(fp, ref, SEEK_SET);
    ch = fgetc(fp);
    while (ch != ' ')
    {
        on2 *= 10;
        on2 += (ch - '0');
        ch = fgetc(fp);
    }

    ch = fgetc(fp);
    while (ch != ' ')
    {
        gn2 *= 10;
        gn2 += (ch - '0');
        ch = fgetc(fp);
    }

    if (on != on2 || gn != gn2)
        return 0;

    /* 读取尖括号<< >>内容 */
    QString flag1, flag2;
    fseek(fp, sizeof(char) * 4, SEEK_CUR);//跳过obj eol

    ch = fgetc(fp);
    flag1.append(ch);
    ch = fgetc(fp);
    flag1.append(ch);
    if(flag1 == "<<")
    {
        ch = fgetc(fp);
        while(ch != '>')
        {
            if(ch = '/')
            {
                ch = fgetc(fp);
                while(ch != '['&&ch != '/'&&ch != ' ')
                {
                    flag2.append(ch);
                    ch = fgetc(fp);
                }

                if(flag2 == "Kids")
                {
                    QString p1,p2;
                    QChar p;
                    ch = fgetc(fp);
                    while(ch != '/'&&ch != '>')
                    {
                        p1.append(ch);
                        ch = fgetc(fp);
                    }

                    int k = 0;
                    p = p1.at(0);
                    while(p != ' ')
                    {
                        p2.append(p);
                        k++;
                        p = p1.at(k);
                    }
                    kidID = p2.toInt();
                    p1.clear();
                    p2.clear();
                    break;
                }
                flag2.clear();
            }
        }
    }
    return kidID;
}

int SSAIReader::GetIllustrator(FILE *fp, int on, int ref, int gn)
{
    int iID = 0;
    int on2 = 0, gn2 = 0;
    char ch; //用来保存读取的字符

    /* 检查object number和generation number与xref中信息是否匹配 */
    fseek(fp, ref, SEEK_SET);
    ch = fgetc(fp);
    while (ch != ' ')
    {
        on2 *= 10;
        on2 += (ch - '0');
        ch = fgetc(fp);
    }

    ch = fgetc(fp);
    while (ch != ' ')
    {
        gn2 *= 10;
        gn2 += (ch - '0');
        ch = fgetc(fp);
    }

    if (on != on2 || gn != gn2)
        return 0;

    /* 读取尖括号<< >>内容 */
    QString flag1, flag2;
    fseek(fp, sizeof(char) * 4, SEEK_CUR);//跳过obj eol

    ch = fgetc(fp);
    flag1.append(ch);
    ch = fgetc(fp);
    flag1.append(ch);
    if(flag1 == "<<")
    {
        ch = fgetc(fp);
        while(ch != '>')
        {
            if(ch = '/')
            {
                ch = fgetc(fp);
                while(ch != ' '&&ch != '/')
                {
                    flag2.append(ch);
                    ch = fgetc(fp);
                }

                if(flag2 == "Illustrator")
                {
                    QString p1,p2;
                    QChar p;
                    ch = fgetc(fp);
                    while(ch != '/'&&ch != '>')
                    {
                        p1.append(ch);
                        ch = fgetc(fp);
                    }

                    int k = 0;
                    p = p1.at(0);
                    while(p != ' ')
                    {
                        p2.append(p);
                        k++;
                        p = p1.at(k);
                    }
                    iID = p2.toInt();
                    p1.clear();
                    p2.clear();
                    break;
                }
                flag2.clear();
            }
        }
    }
    return iID;
}

int SSAIReader::GetPrivate(FILE *fp, int on, int ref, int gn)
{
    int priID = 0;
    int on2 = 0, gn2 = 0;
    char ch; //用来保存读取的字符

    /* 检查object number和generation number与xref中信息是否匹配 */
    fseek(fp, ref, SEEK_SET);
    ch = fgetc(fp);
    while (ch != ' ')
    {
        on2 *= 10;
        on2 += (ch - '0');
        ch = fgetc(fp);
    }

    ch = fgetc(fp);
    while (ch != ' ')
    {
        gn2 *= 10;
        gn2 += (ch - '0');
        ch = fgetc(fp);
    }

    if (on != on2 || gn != gn2)
        return 0;

    /* 读取尖括号<< >>内容 */
    QString flag1, flag2;
    fseek(fp, sizeof(char) * 4, SEEK_CUR);//跳过obj eol

    ch = fgetc(fp);
    flag1.append(ch);
    ch = fgetc(fp);
    flag1.append(ch);
    if(flag1 == "<<")
    {
        ch = fgetc(fp);
        while(ch != '>')
        {
            if(ch = '/')
            {
                ch = fgetc(fp);
                while(ch != ' '&&ch != '/')
                {
                    flag2.append(ch);
                    ch = fgetc(fp);
                }

                if(flag2 == "Private")
                {
                    QString p1,p2;
                    QChar p;
                    ch = fgetc(fp);
                    while(ch != '/'&&ch != '>')
                    {
                        p1.append(ch);
                        ch = fgetc(fp);
                    }

                    int k = 0;
                    p = p1.at(0);
                    while(p != ' ')
                    {
                        p2.append(p);
                        k++;
                        p = p1.at(k);
                    }
                    priID = p2.toInt();
                    p1.clear();
                    p2.clear();
                    break;
                }
                flag2.clear();
            }
        }
    }
    return priID;
}

vector<int> SSAIReader::GetDataRef(FILE *fp, int on, int ref, int gn)
{
    int dID = 0;
    int on2 = 0, gn2 = 0;
    char ch; //用来保存读取的字符
    vector<int> dIDs(0);

    /* 检查object number和generation number与xref中信息是否匹配 */
    fseek(fp, ref, SEEK_SET);
    ch = fgetc(fp);
    while (ch != ' ')
    {
        on2 *= 10;
        on2 += (ch - '0');
        ch = fgetc(fp);
    }

    ch = fgetc(fp);
    while (ch != ' ')
    {
        gn2 *= 10;
        gn2 += (ch - '0');
        ch = fgetc(fp);
    }

    if (on != on2 || gn != gn2)
        return dIDs;

    /* 读取尖括号<< >>内容 */
    QString flag1, flag2, flag3;
    fseek(fp, sizeof(char) * 4, SEEK_CUR);//跳过obj eol

    ch = fgetc(fp);
    flag1.append(ch);
    ch = fgetc(fp);
    flag1.append(ch);
    if(flag1 == "<<")
    {
        ch = fgetc(fp);
        while(ch != '>')
        {
            if(ch = '/')
            {
                ch = fgetc(fp);
                while(ch != ' '&&ch != '/'&&ch != '>')
                {
                    flag2.append(ch);
                    ch = fgetc(fp);
                }

                if(flag2.length()>13)
                    flag3 = flag2.left(13);
                if(flag3 == "AIPrivateData")
                {
                    QString p1,p2;
                    QChar p;
                    ch = fgetc(fp);
                    while(ch != '/'&&ch != '>')
                    {
                        p1.append(ch);
                        ch = fgetc(fp);
                    }

                    int k = 0;
                    p = p1.at(0);
                    while(p != ' ')
                    {
                        p2.append(p);
                        k++;
                        p = p1.at(k);
                    }
                    dID = p2.toInt();
                    dIDs.push_back(dID);

                    p1.clear();
                    p2.clear();
                    flag3.clear();
                }
                flag2.clear();

            }
        }
    }
    return dIDs;
}

void SSAIReader::GetObject(FILE *fp, int on, int ref, int gn)
{
    int on2 = 0, gn2 = 0;
    char ch; //用来保存读取的字符

    /* 检查object number和generation number与xref中信息是否匹配 */
    fseek(fp, ref, SEEK_SET);
    ch = fgetc(fp);
    while (ch != ' ')
    {
        on2 *= 10;
        on2 += (ch - '0');
        ch = fgetc(fp);
    }

    ch = fgetc(fp);
    while (ch != ' ')
    {
        gn2 *= 10;
        gn2 += (ch - '0');
        ch = fgetc(fp);
    }

    if (on != on2 || gn != gn2)
        return ;

    /* 读取尖括号<< >>内容 */
    QString flag1, flag2, flag3, flag4, s;
    fseek(fp, sizeof(char) * 4, SEEK_CUR);//跳过obj eol

    ch = fgetc(fp);
    flag1.append(ch);
    ch = fgetc(fp);
    flag1.append(ch);
    QString len1;
    int len2 = 0;
    if(flag1 == "<<")
    {
        ch = fgetc(fp);
        while(ch != '>')
        {
            if(ch = '/')
            {
                ch = fgetc(fp);
                while(ch != ' '&&ch != '/')
                {
                    flag2.append(ch);
                    ch = fgetc(fp);
                }

                if(flag2 == "Length")
                {
                    len2 = 0;
                    ch = fgetc(fp);
                    while(ch != '/'&&ch != '>')
                    {
                        len1.append(ch);
                        ch = fgetc(fp);
                    }
                    len2 = len1.toInt();
                    len1.clear();
                }
                else{
                    ch = fgetc(fp);
                    while(ch != '/'&&ch != '>')
                    {
                        s.append(ch);
                        ch = fgetc(fp);
                    }
                    s.clear();
                }
                flag2.clear();
            }
        }

        if(ch == '>')
            ch = fgetc(fp);
        if(ch == '>')
        {
            ch = fgetc(fp);
            do{
                flag3.clear();
                ch = fgetc(fp);
                while(ch != '\n'&&ch != '\r')
                {
                    flag3.append(ch);
                    ch = fgetc(fp);
                }
                if(flag3 == "tream")
                {
                    char* stream = new char[len2];
                    fread(stream, 1, len2, fp);
                    GetLayer(stream, len2);

                    ch = fgetc(fp);
                    if(ch == '\n'){
                        ch = fgetc(fp);
                        while(ch != '\n'&&ch != '\r'){
                            flag4.append(ch);
                            ch = fgetc(fp);
                        }
                        if(flag4 == "endstream")
                            fputs("Finish reading stream of obj " + on, stderr);
                    }
                    delete[] stream;
                }
            }while(flag3 != "endobj");
        }
    }

}

void SSAIReader::GetLayer(char* stream, int length)
{
    char ch; //用来保存读取的字符
    int i = 0;
    ch = *(stream + i);
    while(ch != '%'&& i < length)
    {
        i++;
        ch = *(stream + i);
    }

    if(i != length)
    {

    }
}
