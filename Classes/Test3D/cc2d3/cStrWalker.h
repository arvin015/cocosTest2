#ifndef CSTRWALKERH
#define CSTRWALKERH

class cStrWalker{
    const char *head, *tail;
public:
    const char *curtoken, *nexttoken;
    int fulllen, curlen;
public:
    cStrWalker(const char *_head, int _len){
        head = _head;
        tail = _head + _len;
        curtoken = _head;
        nexttoken = _head;
        fulllen = _len;
    }
    
	//delimit : list of delimit char
	//return -1: ended;
	//return 0 ~ n: length of current token;
	//return curtoken: start of this token (invalid when return -1);
	int next(const char delimit);
    int nextnonempty(const char delimit);
    int next(const char *delimit);
    int nextnonempty(const char *delimit);
    int cpcurtoken(char *outbuf, int limit);
	
	float nextfloat(const char delimit, float missing);
	float nextnonemptyfloat(const char *delimit, float missing);
};

#endif