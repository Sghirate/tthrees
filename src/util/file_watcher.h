#pragma once

struct file_watcher
{
    file_watcher(const char* const path);
    ~file_watcher();
    bool check_modified() const;

private:
    class file_watcher_impl* impl;
};
