/**
 * Generic class that helps load libman
 *
 * @author The Northern Bites - Bowdoin College, Brunswick, ME
 *
**/

#pragma once

#include <dlfcn.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>

#include "ManPreloaderInterface.h"
#include "NullStream.h"

#define DEBUG_PRELOADER
#ifdef DEBUG_PRELOADER
#define debug_preloader_out std::cout
#else
#define debug_preloader_out (*NullStream::NullInstance())
#endif

template <typename man_load_method, typename man_unload_method>
class ManPreloader: public ManPreloaderInterface {

public:
    ManPreloader(std::string libman_name = "/home/nao/naoqi/lib/libman.so",
            std::string loadman_method_name = "loadMan",
            std::string unloadman_method_name = "unloadMan")
        :      libman_name(libman_name),
               loadman_method_name(loadman_method_name),
               unloadman_method_name(unloadman_method_name){
    }

    virtual ~ManPreloader() {}

    virtual void reloadMan() {}
    virtual void createMan() {}
    virtual void destroyMan() {}

protected:

    //call the loadMan function from the man library
    virtual void launchMan() {}

    //imports the man library
    void importLibMan() {
        debug_preloader_out << "Importing " + libman_name + " ... ";
        libman_handle = dlopen(libman_name.c_str(),
                RTLD_LAZY);
        if (!libman_handle)
        {
            std::cout << dlerror() << std::endl;
            std::exit(1);
        }
        debug_preloader_out << "done"<<std::endl;
    }
    //closes the library
    void closeLibMan() {
        debug_preloader_out << "Closing " + libman_name + " ... ";
        if (dlclose(libman_handle) != 0) {
             std::cout << dlerror() << std::endl;
             std::exit(1);
        }
        debug_preloader_out << "done"<<std::endl;
    }

    //manually links to a method exported in man
    template <class T> T linkToManMethod(std::string name) {
        debug_preloader_out << "Manually linking to " + name + " ... ";
        T method = reinterpret_cast<T>(
                dlsym(libman_handle, name.c_str()));
        if (method == NULL)
        {
            std::cout << dlerror() << std::endl;
            std::exit(1);
        }
        return method;
    }
    //links the functions that are used to create man
    void linkManLoaderMethods() {
        loadMan = linkToManMethod<man_load_method>(loadman_method_name);
        unloadMan = linkToManMethod<man_unload_method>(unloadman_method_name);
    }

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
