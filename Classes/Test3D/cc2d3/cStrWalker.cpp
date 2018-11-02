#include "cStrWalker.h"

#include "CC2DMath.h"

// === === === Walker === === ===
int indexOf(const char c, const char *str){
    int i = 0;
    char c2 = str[0];
    while(0 != c2){
        if(c == c2)
            return i;
        i++;
        c2 = str[i];
    }
    return -1;
}

int cStrWalker::next(const char delimit){
    if(nexttoken > tail){
        curlen = -1;
        return -1;
    }
    curtoken = nexttoken;
    while(nexttoken <= tail){
        bool brk = (delimit == *nexttoken);
        nexttoken++;
        if(brk)
            break;
    }
    curlen = (int)((nexttoken - curtoken) - 1);
    return curlen;
}

int cStrWalker::nextnonempty(const char delimit){
    while(0 == next(delimit));
    return curlen;
}

int cStrWalker::next(const char *delimit){
    if(nexttoken > tail){
        curlen = -1;
        return -1;
    }
    curtoken = nexttoken;
    while(nexttoken <= tail){
        bool brk = (0 <= indexOf(*nexttoken, delimit));
        nexttoken++;
        if(brk)
            break;
    }
    curlen = (int)((nexttoken - curtoken) - 1);
    return curlen;
}
int cStrWalker::nextnonempty(const char *delimit){
    while(0 == next(delimit));
    return curlen;
}

int cStrWalker::cpcurtoken(char *outbuf, int limit){
    int len = std::max(0, std::min(limit - 1, curlen));
    for(int i = 0; i < len; i++){
        outbuf[i] = curtoken[i];
    }
    outbuf[len] = 0;
    return len;
}

float cStrWalker::nextfloat(const char delimit, float missing) {
	int hasnext = next(delimit);
	if (0 == hasnext)
		return 0.0f;
	if (0 > hasnext)
		return missing;
	return strtod(curtoken, NULL);;
}

float cStrWalker::nextnonemptyfloat(const char *delimit, float missing) {
	int hasnext = nextnonempty(delimit);
	if (0 == hasnext)
		return 0.0f;
	if (0 > hasnext)
		return missing;
	return atof(curtoken);
}

