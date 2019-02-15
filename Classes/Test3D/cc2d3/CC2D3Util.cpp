#include "CC2D3Util.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void TextureCacheClear() {
    //LOGUTIL(LEVEL_INFO, tagtag::TAG(),"TextureCacheClear()");
    //sharedTextureCache()->removeUnusedTextures();
    //LOGUTIL(LEVEL_INFO, tagtag::TAG(),"%s", sharedTextureCache()->getCachedTextureInfo().c_str());
    sharedTextureCache()->removeAllTextures();
}
Texture2D *cacheTexture(const std::string &tname) {
	if (tname.empty())
		return nullptr;

	Texture2D *t = sharedTextureCache()->addImage(tname);
	if (!t) {
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cacheTexture() failed '%s'", tname.c_str());
		return t;
	}
	//if(t->getReferenceCount() == 1)
	//	LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cacheTexture() added %s", tname.c_str());
	return t;
}


void runOnUiThreadBlocking(const std::function<void()> &function) {
	if (std::this_thread::get_id() == DirectorInstance()->getCocos2dThreadId()){
		function();
		return;
	}
    long64 rid = SystemCurrentTimeMillis_fake();
	std::condition_variable cv;
	std::mutex cv_m;
	bool done = false;
	runOnUiThread([&function, &done, &cv_m, &cv, rid]() {
		{
            //LOGUTIL(LEVEL_INFO, tagtag::TAG(),"runOnUiThreadBlocking() runonuithread %lld", rid);
			std::unique_lock<std::mutex> lk2(cv_m);
			function();
			done = true;
            //LOGUTIL(LEVEL_INFO, tagtag::TAG(),"runOnUiThreadBlocking() notitfy %lld", rid);
            cv.notify_one();
		}
	});
	{
		std::unique_lock<std::mutex> lk(cv_m);
        //LOGUTIL(LEVEL_INFO, tagtag::TAG(),"runOnUiThreadBlocking() %lld done=%c", rid, done?'y':'n');
		while (!done) {
            //LOGUTIL(LEVEL_INFO, tagtag::TAG(),"runOnUiThreadBlocking() !done block wait %lld", rid);
			cv.wait(lk);
		}
        //LOGUTIL(LEVEL_INFO, tagtag::TAG(),"runOnUiThreadBlocking() done unblock %lld", rid);
	}
}

bool is_file(const std::string &fullpath) {
	if (fullpath.empty())
		return false;
	auto fi = FileUtils::getInstance();
	bool p = fi->isPopupNotify();
	fi->setPopupNotify(false);
	bool ret = fi->isFileExist(fullpath);
	fi->setPopupNotify(p);
	return ret;
}

bool is_dir(const std::string &fullpath) {
	if (fullpath.empty())
		return false;
	auto fi = FileUtils::getInstance();
	bool p = fi->isPopupNotify();
	fi->setPopupNotify(false);
	bool ret = fi->isDirectoryExist(fullpath);
	fi->setPopupNotify(p);
	return ret;
}

bool file_exists(const std::string &fullpath) {
	if (fullpath.empty())
		return false;
	auto fi = FileUtils::getInstance();
	bool p = fi->isPopupNotify();
	fi->setPopupNotify(false);
	bool ret = fi->isFileExist(fullpath) || fi->isDirectoryExist(fullpath);
	fi->setPopupNotify(p);
	return ret;
}

//bool file_move(const std::string &srcfile, const std::string &destfile) {
//    if (!file_exists(srcfile)) {
//        LOGUTIL(LEVEL_INFO, tagtag::TAG(),"file_move() on src missing %s > %s", srcfile.c_str(), destfile.c_str());
//        return false;
//    }
//    if (file_exists(destfile)) {
//        //LOGUTIL(LEVEL_INFO, tagtag::TAG(),"file_move() on overwrite exist %s > %s", srcfile.c_str(), destfile.c_str());
//        file_delete(srcfile);
//    }
//    int ret = rename(srcfile.c_str(), destfile.c_str());
//    if (ret != 0) {
//        //LOGUTIL(LEVEL_INFO, tagtag::TAG(),"rename failed() %s > %s", srcfile.c_str(), destfile.c_str());
//        if (!file_copy(srcfile, destfile)) {
//            LOGUTIL(LEVEL_INFO, tagtag::TAG(),"file_move() err %d, %s > %s", ret, srcfile.c_str(), destfile.c_str());
//            return false;
//        } else {
//            file_delete(srcfile);
//        }
//    }
//    return true;
//}
bool file_move(const std::string &srcfile, const std::string &destfile) {
    if (!file_exists(srcfile)) {
        LOGUTIL(LEVEL_INFO, tagtag::TAG(),"file_move() on src missing %s > %s", srcfile.c_str(), destfile.c_str());
        return false;
    }
    if (file_exists(destfile)) {
        //LOGUTIL(LEVEL_INFO, tagtag::TAG(),"file_move() on overwrite exist %s > %s", srcfile.c_str(), destfile.c_str());
        file_delete(destfile);
    }
    int ret = rename(srcfile.c_str(), destfile.c_str());
    if (ret != 0) {
        //LOGUTIL(LEVEL_INFO, tagtag::TAG(),"rename failed() %s > %s", srcfile.c_str(), destfile.c_str());
        if (!file_copy(srcfile, destfile)) {
            LOGUTIL(LEVEL_INFO, tagtag::TAG(),"file_move() err %d, %s > %s", ret, srcfile.c_str(), destfile.c_str());
            return false;
        } else {
            file_delete(srcfile);
        }
    }
    return true;
}

bool file_delete(const std::string &fullpath) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	LOGUTIL(LEVEL_INFO, tagtag::TAG(),"file_delete() '%s' hasUD=%c", fullpath.c_str(), str_contains(fullpath, "/UD/")?'y':'n');
	if(str_contains(fullpath, "/UD/"))
		return 0 == std::remove(fullpath.c_str());
	return true;
#else
	//return 0 == std::remove(fullpath.c_str());

    //Iven added
    return FileUtils::getInstance()->removeFile(fullpath);

    //return 0==unlink(fullpath.c_str());
#endif
}

bool file_copy(const std::string &old_filename, const FilePathInfo &new_filename) {
	//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"file_copy() %s < %s", new_filename.fullpath.c_str(), old_filename.c_str());

	if(!file_exists(new_filename.filedir))
		mkdir(new_filename.filedir);
	if (!is_dir(new_filename.filedir)) {
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"file_copy() dest dir err des=%s src=%s", new_filename.fullpath.c_str(), old_filename.c_str());
		return false;
	}

	FILE  *ptr_old = fopen(old_filename.c_str(), "rb");
	if (!ptr_old) {
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"file_copy() src err des=%s src=%s", new_filename.fullpath.c_str(), old_filename.c_str());
		return false;
	}
	FILE  *ptr_new = fopen(new_filename.fullpath.c_str(), "wb");
	if (!ptr_new) {
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"file_copy() des err des=%s src=%s", new_filename.fullpath.c_str(), old_filename.c_str());
		fclose(ptr_old);
		return false;
	}

	const int copyblock = 1024 * 64;
	char buffer[copyblock];
	while(1){
		int n = fread(buffer, 1, copyblock, ptr_old);
		if (n <= 0)
			break;
		fwrite(buffer, 1, n, ptr_new);
	}

	fclose(ptr_new);
	fclose(ptr_old);
	return true;
}

long64 SystemCurrentTimeMillis() {
	using namespace std;
	using namespace std::chrono;

	system_clock::time_point now = system_clock::now();

	auto duration = now.time_since_epoch();
	long64 millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"SystemCurrentTimeMillis() %lld", millis);
	return millis;
}

// e.g. "%d-%m-%Y %I:%M:%S");
std::string SimpleDateFormatNow(const std::string &format) {
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), format.c_str(), timeinfo);
	//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"SimpleDateFormatNow() %s > %s", format.c_str(), buffer);
	return std::string(buffer);
}
