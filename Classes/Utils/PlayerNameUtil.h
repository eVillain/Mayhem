#ifndef PlayerNameUtil_h
#define PlayerNameUtil_h

#if defined(_WIN32)
#include <windows.h>
#include <Lmcons.h>
#elif defined(__APPLE__)
#include <cstdlib>
#endif

class PlayerNameUtil
{
public:

    static const std::string getLoginUserName() {
#if defined(_WIN32)
        TCHAR username[UNLEN + 1];
        DWORD size = UNLEN + 1;
        GetUserName((TCHAR*)username, &size);
        std::wstring ws(username);
        return std::string(ws.begin(), ws.end());
#elif defined(__APPLE__)
        char* username = getenv("USER");
        if (username != NULL)
        {
            return std::string(username);
        }
#else
        char* username = getenv("USERNAME");
        if (username != NULL)
        {
            return std::string(username);
        }
#endif
        return "";
    }

};

#endif /* PlayerNameUtil_h */
