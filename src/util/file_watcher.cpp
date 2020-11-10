#include <limits.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <util/file_watcher.h>

class file_watcher_impl
{
    enum { FILE_EDIT_MASK = IN_MODIFY | IN_CLOSE_WRITE };
    enum { FILE_MOVE_MASK = IN_MOVED_TO | IN_MOVE_SELF | IN_CREATE };
    enum { FILE_MOD_MASK = FILE_EDIT_MASK | FILE_MOVE_MASK };

    void add_watcher()
    {
        fd = inotify_init1(IN_NONBLOCK);
        if (fd != -1)
        {
            wd = inotify_add_watch(fd, path, FILE_MOD_MASK);
        }
    }

    void remove_watcher()
    {
        if (wd != -1)
        {
            inotify_rm_watch(fd, wd);
            wd = -1;
        }
        if (fd != -1)
        {
            close(fd);
            fd = -1;
        }
    }

public:
    file_watcher_impl(const char* const path)
        : path(path)
        , fd(-1)
        , wd(-1)
    {
        add_watcher();
    }

    ~file_watcher_impl()
    {
        remove_watcher();
    }

    bool check_modified()
    {
        if (fd == -1 ||
            wd == -1)
        {
            return false;
        }

        enum { NUM_EVENTS = 32 };
        enum { EVENT_SIZE = sizeof(struct inotify_event) };
        enum { BUFFER_SIZE = NUM_EVENTS * (EVENT_SIZE + NAME_MAX + 1) };

        static char buffer[BUFFER_SIZE];
        ssize_t n = read(fd, buffer, BUFFER_SIZE);
        ssize_t i = 0;
        bool result = false;
        bool re_add = false;
        while (i < n)
        {
            inotify_event* event = ((struct inotify_event*)&buffer[i]);
            if (event->wd != wd)
            {
                continue;
            }

            if ((event->mask & FILE_MOD_MASK) != 0)
            {
                if ((event->mask & FILE_MOVE_MASK) != 0)
                {
                    re_add = true;
                }
                result = true;
            }
            i += EVENT_SIZE + event->len;
        }
        if (re_add)
        {
            remove_watcher();
            add_watcher();
        }
        return result;
    }

private:
    const char* const path;
    int fd;
    int wd;
};

file_watcher::file_watcher(const char* const path)
    : impl(new file_watcher_impl(path))
{
}
file_watcher::~file_watcher() { delete impl; }
bool file_watcher::check_modified() const { return impl->check_modified(); }
