/**
 * Generic class that helps load libman
 *
 * @author The Northern Bites - Bowdoin College, Brunswick, ME
 *
**/

#pragma once

#include "ManPreloaderInterface.h"
#include "NullStream.h"

#define DEBUG_PRELOADER
#ifdef DEBUG_PRELOADER
#define debug_preloader_out cout
#else
#define debug_preloader_out (*NullStream::NullInstance())
#endif

template <typename man_load_method, typename man_unload_method>
class ManPreloader: public ManPreloaderInterface {

public:
    ManPreloader(std::string libman_name = "libman.so",
            std::string loadman_method_name = "loadMan",
            std::string unloadman_method_name = "unloadMan");
    virtual ~ManPreloader();

private:
    virtual void reloadMan() {}
    virtual void createMan() {}
    virtual void destroyMan() {}

protected:

    //imports the man library
    void importLibMan();
    //closes the library
    void closeLibMan();
    //links the functions that are used to create man
    void linkManLoaderMethods();
    //call the loadMan function from the man library
    virtual void launchMan() {}
    //manually links to a method exported in man
    template <class T> T linkToManMethod(std::string name);

protected:
    man_load_method loadMan;
    man_unload_method unloadMan;

private:
    //man link variables
    void* libman_handle;
    std::string libman_name;
    std::string loadman_method_name;
    std::string unloadman_method_name;
};
