#ifndef LOGS_HEADER
#define LOGS_HEADER

#include <stdio.h>

const char DEFAULT_LOG_NAME[] = "Logs.html";

class Logs
{
    private:

        FILE* file_;

    public:

        explicit Logs ();
        explicit Logs (const char* name);

        ~Logs ();
        
        
        void error  (const char* frmt, ...);
        void warn   (const char* frmt, ...);
        void debug  (const char* frmt, ...);
        void trace  (const char* frmt, ...);

        void print  (const char* frmt, ...);

    private:

        char* getTimeString_ (char* buf);
};

extern Logs Logs;

#endif
