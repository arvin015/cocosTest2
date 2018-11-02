#include "LogUtil.h"
#include <stdarg.h>
#include <sys/stat.h>

#include <stdio.h>

#if !((CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT))
#include <dirent.h>
#include <unistd.h>
#else
#include "dirent.h"
#include "unistd.h"
#endif

USING_NS_CC;
using namespace std;

LogUtil* LogUtil::pLog = NULL;

LogUtil::LogUtil()
{
    enable = true;
    fileName = LOG_CONFIG_FILENAME;
    levelCtrl = LEVEL_NONE;
    files.clear();
    this->getAllFile(this->getDirPath().c_str(), 0);
}

LogUtil::~LogUtil()
{
    if (pLog != NULL){
        delete pLog;
        pLog = NULL;
    }
}

LogUtil* LogUtil::getInstance()
{
    if (pLog == NULL ) {
        pLog = new LogUtil();
    }
    return pLog;
}

void LogUtil::writeLog(LEVEL_TYPE level, const char *method, const char *format, ...)
{
    if (!enable) {
        return;
    }
    if (NULL == format || strcmp(format, "") == 0) {
        return;
    }
    if ( level <= levelCtrl ) {
        return;
    }
    
    FILE * fp = fopen(this->getFilePath().c_str(), "at+");
    if (fp)
    {
        char szBuf[MAXLENGTH+1] = {0};
        va_list ap;
        va_start(ap, format);
        vsnprintf(szBuf, MAXLENGTH, format, ap);
        va_end(ap);
        string tmp = this->combine(level, method, szBuf);
        fwrite(tmp.c_str(), 1, tmp.size(), fp);
        fclose(fp);
        log("%s",tmp.c_str());
    }
}

void LogUtil::writeLog(const char *format, ...)
{
    FILE * fp = fopen(this->getFilePath().c_str(), "at+");
    if (fp)
    {
        char szBuf[MAXLENGTH+1] = {0};
        va_list ap;
        va_start(ap, format);
        vsnprintf(szBuf, MAXLENGTH, format, ap);
        va_end(ap);
        
        string tmp = this->combine(LEVEL_INFO, "non", szBuf);
        fwrite(tmp.c_str(), 1, tmp.size(), fp);
        fclose(fp);
        log("%s",tmp.c_str());
    }
}

string LogUtil::getDirPath()
{
    std::string writablePath =FileUtils::getInstance()->getWritablePath();
    writablePath.append(LOG_DIRNAME);
    if (access(writablePath.c_str(),0) !=0)
    {
        log("access failed! %s",writablePath.c_str());
        mkdir((writablePath.c_str()),0755);
    }
    return writablePath.append("/");
}

string LogUtil::getFilePath()
{
    //log("dir path for log is %s",this->getDirPath().c_str());
    return this->getDirPath().append(this->getCurrentDay()).append(".txt");
}

void LogUtil::getAllFile(const char *dir, int depth)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL)
    {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode))
        {
            if(strcmp(".",entry->d_name) == 0 ||
               strcmp("..",entry->d_name) == 0)
                continue;
            printf("in %*s%s/\n",depth,"",entry->d_name);
        }
        files.push_back(entry->d_name);
    }
    chdir("..");
    closedir(dp);
}

bool LogUtil::remove()
{
    if (!files.empty())
    {
        for (int i = 0 ; i < files.size() ; i++)
        {
            ::remove(files[i].c_str());
            //unlink(files[i].c_str());
            log("remove the file - %s" , files[i].c_str());
        }
        return true;
    }
    return false;
}

string LogUtil::getCurrentDay()
{
    char timeNow[20];
    time_t lt;
    struct tm * tp;
    lt = time(NULL);
    tp = localtime(&lt);
    strftime(timeNow, 20, "%Y-%m-%d", tp);
    return timeNow;
}

string LogUtil::getCurrentTime()
{
    char timeNow[20];
    time_t lt;
    struct tm * tp;
    lt = time(NULL);
    tp = localtime(&lt);
    strftime(timeNow, 20, "%Y-%m-%d %T", tp);
    return string(timeNow);
}

string LogUtil::combine(LEVEL_TYPE level, const char *method , const char* log)
{
    std::string content("[");
    content.append(this->getCurrentTime());
    content.append("]-[");
    content.append(LOG_TYPE[level]);
    content.append("]-[");
    content.append(method);
    content.append("]-[Msg:");
    content.append(log);
    content.append("]");
    content.append("\n");
    return content;
}
