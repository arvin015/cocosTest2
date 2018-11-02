#ifndef _LOGUTIL_H_
#define _LOGUTIL_H_

typedef enum
{
    LEVEL_NONE = 0, /* no log output */
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_WARNING,
    LEVEL_ERROR
} LEVEL_TYPE;

const char LOG_TYPE[5][10] = {
    "",
    "Debug",
    "Info",
    "Warning",
    "Error"
};


/* 配置文件中的配置项 */
const char LOG_CONFIG_SECTION[] = "LOG";
const char LOG_CONFIG_LEVELTYPE[] = "LEVELTYPE"; // 打印日志的等级
const char LOG_CONFIG_FILENAME[20] = "log.txt"; // 默认日志文件名
const char LOG_DIRNAME[32] = "log"; //日志文件夹

#define _LOGUTIL(_level ,_method , _format, ...) 

#define LOGUTIL(_level ,_method , _format, ...) \
LogUtil::getInstance()->writeLog(_level ,_method , _format, ##__VA_ARGS__)
#define LOGUTILf(_format, ...) \
LogUtil::getInstance()->writeLog(_format, ##__VA_ARGS__)
#define LOGFILE(_fileName) \
LogUtil::getInstance()->setFileName(_fileName)
#define LOGENABLE(_enable) \
LogUtil::getInstance()->setEnable(_enable)
#define LOGLEVEL(_level) \
LogUtil::getInstance()->setLevelCtrl(_level)
#define LOGREMOVE \
LogUtil::getInstance()->remove()
#define MAXLENGTH 2048

#include "cocos2d.h"

namespace tagtag{
    inline const char *TAG(){return "";}
};

class LogUtil : public cocos2d::Ref{
    public:
    
    virtual ~LogUtil();
    static LogUtil* getInstance();
    static LogUtil* pLog;
    
    void writeLog(LEVEL_TYPE level , const char* method , const char* format , ...);
    void writeLog(const char* format , ...);
    std::string getDirPath();
    std::string getFilePath();
    void getAllFile(const char *dir, int depth);
    bool remove();
    std::string getCurrentDay();
    std::string getCurrentTime();
    std::string combine(LEVEL_TYPE level, const char *method , const char* log);
    
    CC_SYNTHESIZE(bool, enable, Enable);
    CC_SYNTHESIZE(std::string, fileName, FileName);
    CC_SYNTHESIZE(LEVEL_TYPE, levelCtrl, LevelCtrl);
    
    private:
    LogUtil();
    std::vector<std::string> files;
};

#endif
