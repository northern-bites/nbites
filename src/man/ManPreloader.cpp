
#include "ManPreloader.h"

#include <dlfcn.h> //dlopen, dlsym
#include <stdlib.h> //exit
#include <iostream> //cout
#include <assert.h>

using namespace std;

template<typename M1, typename M2>
ManPreloader<M1, M2>::ManPreloader(string libman_name,
        string loadman_method_name, string unloadman_method_name) :
        libman_name(libman_name),
        loadman_method_name(loadman_method_name),
        unloadman_method_name(unloadman_method_name){
}

template<typename M1, typename M2>
ManPreloader<M1, M2>::~ManPreloader() {
}

template<typename M1, typename M2>
void ManPreloader<M1, M2>::importLibMan() {
    debug_preloader_out << "Importing " + libman_name + " ... ";
    libman_handle = dlopen(libman_name.c_str(),
                           RTLD_LAZY);
    if (!libman_handle)
    {
        cout << dlerror() << endl;
        std::exit(1);
    }
    debug_preloader_out << "done"<<endl;
}

template<typename M1, typename M2>
void ManPreloader<M1, M2>::closeLibMan() {
    if (dlclose(libman_handle) != 0) {
        cout << dlerror() << endl;
        std::exit(1);
    }
}

template<typename M1, typename M2>
void ManPreloader<M1, M2>::linkManLoaderMethods() {

    loadMan = linkToManMethod<M1>(loadman_method_name);
    unloadMan = linkToManMethod<M2>(unloadman_method_name);
}

template <typename M1, typename M2> template<class T>
T ManPreloader<M1, M2>::linkToManMethod(std::string name) {
    debug_preloader_out << "Manually linking to " + name + " ... ";
    T method = reinterpret_cast<T>(
            dlsym(libman_handle, name.c_str()));
    if (method == NULL)
    {
        cout << dlerror() << endl;
        std::exit(1);
    }
    return method;
}

