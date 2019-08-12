#include <unistd.h>
#include <string.h>
#include <errno.h>

int main()
{
    const char * const msg = "Hello World!\n";
    const char * begin = msg;
    const char * const end = begin + strlen(msg);
   
    while (begin < end)
    {
        size_t remaining = end - begin;
        ssize_t res = write(STDOUT_FILENO, begin, remaining);
        if (res >= 0)
        {
            begin += res;
            continue; // Let's send the remaining part of this message
        }
        if (EINTR == errno)
        {
            continue; // It's just a signal, try again
        }
        return 1; // It's a real error
    }
    return 0; // OK! Let's celebrate and drink some beer!
}
