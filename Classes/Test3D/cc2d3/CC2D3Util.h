#ifndef __CC2D3UTILH__
#define __CC2D3UTILH__

#include <iostream>
#include <ctime>

#include "cocos2d.h"
using namespace cocos2d;

#include "ui/CocosGUI.h"
using namespace cocos2d::ui;

//#define LOGUTIL(LEVEL_INFO, tagtag::TAG(),...) CCLOG(__VA_ARGS__)
//#define LOGUTIL(LEVEL_INFO, tagtag::TAG(),...)

#include "LogUtil.h"
//namespace tagtag{
//    inline const char *TAG(){return "";}
//};

typedef long long long64;

#ifndef MAXFLOAT
const float MAXFLOAT = 999999.99999f;
#endif

#define SAFE_REMOVE_NULL(node) if(node){node->removeFromParentAndCleanup(true);node->release();node=nullptr;}
template <typename T>
inline T *SAFE_RETAIN(T* node) {
    if (node)
        node->retain();
    return node;
}


//=== === === === === === Single Shortcut === === === === === ===
inline Director* DirectorInstance()        {    return Director::getInstance(); }
inline Scheduler* SchedulerInstance()    {    return DirectorInstance()->getScheduler(); }
inline TextureCache* sharedTextureCache() { return DirectorInstance()->getTextureCache(); }
inline UserDefault* sharedUserDefault() { return UserDefault::getInstance(); }

inline cocos2d::Size ScreenSize()    {     return DirectorInstance()->getVisibleSize(); }

inline void cacheTextureAsync(const std::string &tname, const std::function<void(Texture2D*)>& callback) {
    return sharedTextureCache()->addImageAsync(tname, callback);
}

void TextureCacheClear();
Texture2D *cacheTexture(const std::string &tname);

inline void SpriteFrameLoad(const char *plistname) {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistname);
}

inline bool nodeReallyVisible(Node *n) {
    while (nullptr != n) {
        if (!n->isVisible())
            return false;
        n = n->getParent();
    }
    return true;
}

template <typename T>
inline T* AnyCreateFunc(T* created) {
    if (created && created->init()) {
        created->autorelease();
    } else {
        delete created;
        created = nullptr;
    }
    return created;
}

//=== === === === === === === === === === === ===
//Coco do Lazy Draw, need a HARD KICK it to flush buffered GL Call...
inline void renderNow() {
    CCLOG("renderNow()");
    Director::getInstance()->getRenderer()->render();
}

//=== === === === === === Time And Clock Util === === === === === ===
long64 SystemCurrentTimeMillis_fake();
long64 SystemCurrentTimeMillis();
std::string SimpleDateFormatNow(const std::string &format);


// Simple Math Utils ===== =====
template <typename T>
inline T minvalue(const T &a, const T &b) {
    return a > b ? b : a;
}

template <typename T>
inline T maxvalue(const T &a, const T &b) {
    return a < b ? b : a;
}

template <typename T>
inline T clampvalue(const T &val, const T &min, const T &max) {
    if (min > val)
        return min;
    if (max < val)
        return max;
    return val;
}

//=== === === === === === Texture & Sprite Util === === === === === ===
inline Vec2 scaleFitSizeOutput(const cocos2d::Size &inputSize, const cocos2d::Size &targetSize) {
    if (inputSize.width == 0 || inputSize.height == 0) {
        return Vec2(1, 1);
    }
    return Vec2(targetSize.width / inputSize.width, targetSize.height / inputSize.height);
}
inline float scaleFitSizeInside(const cocos2d::Size &inputSize, const cocos2d::Size &targetSize) {
    const Vec2 &scale = scaleFitSizeOutput(inputSize, targetSize);
    return minvalue(scale.x, scale.y);
}
inline float scaleFitSizeOutside(const cocos2d::Size &inputSize, const cocos2d::Size &targetSize) {
    const Vec2 &scale = scaleFitSizeOutput(inputSize, targetSize);
    return maxvalue(scale.x, scale.y);
}
inline void spriteSetTexture(Sprite *s, Texture2D *t) {
    s->setTexture(t);
    if (t) {
        cocos2d::Rect rect = cocos2d::Rect::ZERO;
        rect.size = t->getContentSizeInPixels();
        s->setTextureRect(rect);
    }
}

//=== === === === === === FileUtil === === === === === ===
struct FilePathInfo {
    std::string fullpath;
    std::string filedir;
    std::string filename;
    std::string ext;
    FilePathInfo(const std::string &_fullpath) {
        fullpath = _fullpath;
        size_t sep = fullpath.find_last_of("\\/");
        if (sep == std::string::npos) {
            filedir = "";
            filename = fullpath;
        } else {
            filedir = fullpath.substr(0, sep);
            filename = fullpath.substr(sep + 1, fullpath.size() - sep);
        }
        size_t dot = filename.find_last_of(".");
        if (sep == std::string::npos) {
            ext = "";
        } else {
            ext = filename.substr(dot + 1, filename.size() - dot);
        }
    }
};
inline void mkdir(const std::string &dir){
    FileUtils::getInstance()->createDirectory(dir);
}

bool is_file(const std::string &fullpath);
bool is_dir(const std::string &fullpath);
bool file_exists(const std::string &fullpath);

bool file_copy(const std::string &old_filename, const FilePathInfo &new_filename);
inline bool file_copy(const std::string &old_filename, const std::string &new_filename) {
    return file_copy(old_filename, FilePathInfo(new_filename));
}
bool file_move(const std::string &srcfile, const std::string &destfile);
bool file_delete(const std::string &fullpath);

inline std::string CCFullPath(const std::string &path) {
    return FileUtils::getInstance()->fullPathForFilename(path);
}

inline std::string CCFile2Str(const std::string &fullPath) {
    return FileUtils::getInstance()->getStringFromFile(fullPath);
}

inline bool _Str2File(const FilePathInfo &fi, const std::string &data, bool append) {
	//if (fi.ext.compare("mp3") == 0) LOGUTIL(LEVEL_INFO, tagtag::TAG(),"Write str to mp3");
    LOGUTIL(LEVEL_INFO, tagtag::TAG(),"_Str2File() append=%c file=%s datalen=%d", append?'y':'n', fi.fullpath.c_str(), (int)data.length());
    if (!FileUtils::getInstance()->createDirectory(fi.filedir)) {
        LOGUTIL(LEVEL_INFO, tagtag::TAG(),"_Str2File() cant mkdir! append=%c file=%s", append ? 'y' : 'n', fi.fullpath.c_str());
        return false;
    }
    //if (!append && data.size() == 0) {
    //    LOGUTIL(LEVEL_INFO, tagtag::TAG(),"_Str2File() Clear File file=%s", fi.fullpath.c_str());
    //    return false;
    //}
    size_t size = data.size();
    const char *bytes = data.c_str();
    
    //FILE* f = fopen("myfile.txt", "r");
    //int result = flock(fileno(f)), LOCK_SH);
    
    const char *mode = append?"ab":"wb";
    auto fileutils = FileUtils::getInstance();
    do {
        std::string fname = fileutils->getSuitableFOpen(fi.fullpath);
        FILE *fp = fopen(fname.c_str(), mode);
        if (!fp) {
            LOGUTIL(LEVEL_INFO, tagtag::TAG(),"_Str2File() cant open file mode=%s append=%c file=%s", mode, append ? 'y' : 'n', fi.fullpath.c_str());
            break;;
        }
        fwrite(bytes, size, 1, fp);
        fclose(fp);
        return true;
    } while (0);
    return false;
}

inline bool CCStr2File(const std::string &filedir, const std::string &filename, const std::string &data) {
    LOGUTIL(LEVEL_INFO, tagtag::TAG(),"CCStr2File() dir=%s, file=%s", filedir.c_str(), filename.c_str());
    return _Str2File(FilePathInfo(filedir + "/" + filename), data, false);
    /*
     if (!FileUtils::getInstance()->createDirectory(filedir))
     return false;
     return FileUtils::getInstance()->writeStringToFile(data, filedir + "/" + filename);
     */
}

inline bool CCStr2FileAppend(const FilePathInfo &fi, const std::string &data) {
    LOGUTIL(LEVEL_INFO, tagtag::TAG(),"CCStr2FileAppend() file=%s", fi.fullpath.c_str());
    return _Str2File(fi, data, true);
    /*
     if (!FileUtils::getInstance()->createDirectory(fi.filedir))
     return false;
     size_t size = data.size();
     const char *bytes = data.c_str();
     const char *mode = "w+b";
     auto fileutils = FileUtils::getInstance();
     do {
     FILE *fp = fopen(fileutils->getSuitableFOpen(fi.fullpath).c_str(), mode);
     CC_BREAK_IF(!fp);
     fwrite(bytes, size, 1, fp);
     fclose(fp);
     return true;
     } while (0);
     return false;
     */
}
inline bool CCStr2FileAppend(const std::string &fullpath, const std::string &data) {
    return _Str2File(FilePathInfo(fullpath), data, true);
}
inline bool CCStr2File(const FilePathInfo &fi, const std::string &data) {
    return _Str2File(fi, data, false);
}
inline bool CCStr2File(const std::string &filefullpath, const std::string &data) {
    return _Str2File(FilePathInfo(filefullpath), data, false);
}
//=== === === === === === Cross Scene Scheduler and RunOnUIThread === === === === === ===
inline void runOnUiThread(const std::function<void()> &function) {
    SchedulerInstance()->performFunctionInCocosThread(function);
}
void runOnUiThreadBlocking(const std::function<void()> &function);

inline void globalunschedule(SEL_SCHEDULE selector, Ref *target) {
    SchedulerInstance()->unschedule(selector, target);
}

inline void globalscheduleRepeat(SEL_SCHEDULE selector, Ref *target, float delay, float interval) {
    globalunschedule(selector, target);
    SchedulerInstance()->schedule(selector, target, interval, kRepeatForever, delay, 0);
}

inline void globalschedule(SEL_SCHEDULE selector, Ref *target, float delay) {
    globalunschedule(selector, target);
    SchedulerInstance()->schedule(selector, target, 0, false, delay, 0);
    //delegate->runAction(DelayAct(3.0f, CallBackAct(thisclass::cb_rolldone, this)));
}

// ==== ==== ==== ==== ==== ==== String Trimming ==== ==== ==== ==== ==== ====
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>


// ==== ==== String Function ==== ====
static inline bool str2long(const std::string &str, long64 *out) {
    try {
        //*out = std::stoll(str);
        *out = atol(str.c_str());
        return true;
    } catch (std::exception const &) {}
    return false;
}
static inline long64 str2long(const std::string &str, long64 fallback=0L) {
    long64 ret;
    if (str2long(str, &ret))
        return ret;
    return fallback;
}
static inline int str2int(const std::string &str, int fallback = 0) {
    return (int)str2long(str, fallback);
}

static inline float str2float(const std::string &str, float fallback=0.0f) {
    try {
        //float f = std::stof(str);
        //float f =std::atof(str.c_str());
        float f =strtod(str.c_str(),NULL);

        return f;
    } catch (std::exception const &) {}
    return fallback;
}

inline bool str_startsWith(const std::string &src, const std::string &key) {
    const char *s = src.c_str(), *d = key.c_str();
    while (*d != 0) {
        if (*s == 0)
            return false;
        if (*d != *s)
            return false;
        d++;
        s++;
    }
    return true;
}

inline bool str_contains(const std::string &src, const std::string &key) {
    return (src.find(key) != std::string::npos);
}
inline std::string strtolower(const std::string &s) {
	std::locale loc;
	std::string ret = "";
	for (std::string::size_type i = 0; i<s.length(); ++i)
		ret += std::tolower(s[i], loc);
	return ret;
}
inline std::string strtoupper(const std::string &s) {
	std::locale loc;
	std::string ret = "";
	for (std::string::size_type i = 0; i<s.length(); ++i)
		ret += std::toupper(s[i], loc);
	return ret;
}

template <typename T>
static inline std::vector<T> to_vector(const T &onlycell) {
    std::vector<T> ret;
    ret.push_back(onlycell);
    return ret;
}

template <typename T>
static inline std::vector<T> to_vector(const T *arr, int cnt) {
    std::vector<T> ret;
    ret.assign(arr, arr + cnt);
    return ret;
}

template <typename T>
std::vector<T> vectorUnpointer(const std::vector<T *> &psrc) {
    std::vector<T> v;
    v.reserve(psrc.size());
    for (const T* p : psrc) {
        v.push_back(*p);
    }
    return v;
}

template <typename T>
std::vector<T*> vectorPointer(std::vector<T> &psrc) {
    std::vector<T*> v;
    int cnt = psrc.size();
    v.resize(cnt);
    T *p = psrc.data();
    for (int i = 0; i < cnt; i++) {
        v[i] = &p[i];
    }
    return v;
}
template <typename T>
std::vector<const T*> vectorPointer(const std::vector<T> &psrc) {
    std::vector<const T*> v;
    int cnt = psrc.size();
    v.resize(cnt);
    const T *p = psrc.data();
    for (int i = 0; i < cnt; i++) {
        v[i] = &p[i];
    }
    return v;
}

template <typename T>
static inline std::string to_string(T o){
    std::ostringstream stringStream;
    stringStream << o;
    return stringStream.str();
}

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrimmed(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrimmed(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trimmed(std::string s) {
    trim(s);
    return s;
}

static inline std::string str_replace(const std::string& search, const std::string& replace, const std::string& subject) {
    std::string str = subject;
    const size_t findlen = search.length(), replen = replace.length();
    size_t pos = 0;
    while ((pos = str.find(search, pos)) != std::string::npos) {
        str.replace(pos, findlen, replace);
        pos += replen;
    }
    return str;
}

static inline std::string str_left(const std::string& instr, int len) {
    int l = (int)instr.length();
    if(l > len)
        return instr.substr(0, len);
    return instr;
}
static inline std::string str_right(const std::string& instr, int len) {
    int l = (int)instr.length();
    if (l > len)
        return instr.substr(l - len);
    return instr;
}
inline std::string str_shorten(const std::string &src, int lenlimit, int lenhead, int lentail, const std::string &glue) {
    int l = (int)src.length();
    if (l <= lenlimit)
        return src;
    return str_left(src, lenhead) + glue + str_right(src, lentail);
}

template<typename T>
static inline std::string implode(const std::string &glue, const std::vector<T> &s) {
    int cnt = (int)s.size();
    if (0 >= cnt)
        return "";
    std::string ret = to_string(s[0]);
    for (int i = 1; i < cnt; i++) {
        ret = ret + glue + to_string(s[i]);
    }
    return ret;
}

static inline std::vector<std::string> explode(const std::string &s, const std::string &delim) {
    std::vector<std::string> result;
    //std::istringstream iss(s);
    //for (std::string token; std::getline(iss, token, delim); ) {
    //    result.push_back(std::move(token));
    //}
    size_t pos1 = 0, pos;
    const size_t dlen = delim.size(), slen = s.size();
    do {
        pos = s.find(delim, pos1);
        if (pos == std::string::npos) {
            result.push_back(s.substr(pos1, slen - pos1));
            break;
        }
        result.push_back(s.substr(pos1, pos - pos1));
        pos1 = pos + dlen;
    } while (1);
    //LOGUTIL(LEVEL_INFO, tagtag::TAG(),"explode: d='%s' s='%s'", delim.c_str(), s.c_str());
    //for(const std::string &ss: result)
    //    LOGUTIL(LEVEL_INFO, tagtag::TAG(),"explode:  ss='%s'", ss.c_str());
    return result;
}

//=== === === === === === === === === === === ===
inline Vec2 GLNormalized(const Vec2 &v) {
    cocos2d::Size size = ScreenSize();
    return Vec2((2.0f * v.x / size.width), (2.0f * v.y / size.height));
}

//Color 4B 4F int convertion
inline int hexchr2int(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return 10 + c - 'a';
    if (c >= 'A' && c <= 'F')
        return 10 + c - 'A';
    return 0;
}

inline Color4B str2Color(const std::string &colorstr) {
    //input android color string: #aarrggbb output color4b
    //todo:handle hard coded color names like: red, while, black... etc
    int len = (int)colorstr.size();
    Color4B c(0, 0, 0, 255);
    if (len == 7) {
        c.r = hexchr2int(colorstr[1]) * 16 + hexchr2int(colorstr[2]);
        c.g = hexchr2int(colorstr[3]) * 16 + hexchr2int(colorstr[4]);
        c.b = hexchr2int(colorstr[5]) * 16 + hexchr2int(colorstr[6]);
    } else if (len == 9) {
        c.a = hexchr2int(colorstr[1]) * 16 + hexchr2int(colorstr[2]);
        c.r = hexchr2int(colorstr[3]) * 16 + hexchr2int(colorstr[4]);
        c.g = hexchr2int(colorstr[5]) * 16 + hexchr2int(colorstr[6]);
        c.b = hexchr2int(colorstr[7]) * 16 + hexchr2int(colorstr[8]);
    }
    return c;
}

inline float b2f(int b) {
    return (float)(b & 255) / 255.0f;
}

inline Color3B int2color3b(int i) {
    return Color3B(255 & (i >> 16), 255 & (i >> 8), 255 & (i));
}
inline int int2alphab(int i) {
    return 255 & (i >> 24);
}
inline Color4B int2color4b(int i) {
    return Color4B(255 & (i >> 16), 255 & (i >> 8), 255 & (i), 255 & (i >> 24));
}
inline Color4F int2color4f(int i, int alpha) {
    return Color4F(b2f(i >> 16), b2f(i >> 8), b2f(i), alpha / 255.0f);
}
inline Color4F int2color4f(int i) {
    return Color4F(b2f(i >> 16), b2f(i >> 8), b2f(i), b2f(i >> 24));
}

inline int color2Int(int r, int g, int b, int a) {
    return (r << 16) + (g << 8) + b + (a << 24);
}

inline int color2Int(const Color3B &c3b) {
    return (c3b.r << 16) + (c3b.g << 8) + c3b.b + (255 << 24);
}

inline int color2Int(const Color4B &c4b) {
    return (c4b.r << 16) + (c4b.g << 8) + c4b.b + (c4b.a << 24);
}

inline int color2Int(const Color4F &c4f) {
    return color2Int(Color4B(c4f));
}

// 2D Geometry Utils ===== =====
inline bool pointInRect(float x, float y, float minx, float miny, float maxx, float maxy) {
    if (x < minx)
        return false;
    if (x > maxx)
        return false;
    if (y < miny)
        return false;
    if (y > maxy)
        return false;
    return true;
}
inline bool pointInSize(const Vec2 &P, const cocos2d::Size &S) {
    //LOGUTIL(LEVEL_INFO, tagtag::TAG(),"pointInSize() p=%d,%d size=%d,%d", (int)P.x, (int)P.y, (int)S.width, (int)S.height);
    return pointInRect(P.x, P.y, 0, 0, S.width, S.height);
}

#endif

