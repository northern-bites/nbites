#include <errno.h>       // errno
#include <string.h>      // strerror(), memcpy()

#include "Common.h"

#include <sys/socket.h> // socket(), connect(), send(), recv(), setsockopt()
#include <arpa/inet.h>   // inet_aton(), htonl()
#include <netdb.h>       // gethostbyname()
#include <unistd.h>      // close(), usleep()
#include <fcntl.h>       // fcntl()
//#include <time.h>        // localtime()
//#include <sys/time.h>    // gettimeofday()
#include <sys/utsname.h> // uname()
#include <cstdio>        // printf(), fread(), popen()

#include <Python.h>
#include <structmember.h>

#include <boost/shared_ptr.hpp>

#include "commconfig.h"
#include "Comm.h"
#include "NaoPose.h"

#undef USE_GAMECONTROLLER
#define USE_GAMECONTROLLER

using namespace std;

using namespace boost;

// Static reference to Python comm module

static PyObject *comm_module = NULL;

//
// Python Comm class definitions
//

typedef struct PyComm_t {
    PyObject_HEAD
    Comm *comm;
#ifdef USE_PYTHON_GC
    PyGameController *gc;
#endif
} PyComm;

static PyObject * PyComm_new (PyTypeObject *type, PyObject *args,
							  PyObject *kwds)
{
    PyErr_SetString(PyExc_RuntimeError, "Cannot initialize a Python Comm from "
                    "Python; instances must be initialized from C++.");
    return NULL;
}

static void PyComm_dealloc (PyComm *self)
{
    self->ob_type->tp_free((PyObject *)self);
}

static PyObject * PyComm_latestComm (PyObject *self, PyObject *args)
{
    list<vector<float> >* latest;
    Py_BEGIN_ALLOW_THREADS

        latest = reinterpret_cast<PyComm*>(self)->comm->latestComm();

    Py_END_ALLOW_THREADS

        // Build tuple of tuples of joint values
        PyObject *outer = PyList_New(latest->size()), *inner, *f;
    bool success = true;
    int i, j;
    if (outer != NULL) {
        i = 0;
        while (!latest->empty()) {
            vector<float> &v = latest->front();
            inner = PyList_New(v.size());
            if (inner != NULL) {
                for (unsigned int j = 0; j < v.size(); j++) {
                    f = PyFloat_FromDouble(v[j]);
                    if (f != NULL)
                        PyList_SET_ITEM(inner, j, f);
                    else {
                        success = false;
                        break;
                    }
                }
                PyList_SET_ITEM(outer, i, inner);
                if (!success)
                    break;
                latest->pop_front();
                i++;
            }else {
                success = false;
                break;
            }
        }
    }

    // failed building tuples, recover memory
    if (!success) {
        Py_DECREF(outer);
        return NULL;
    }

    return outer;
}

static PyObject * PyComm_setData (PyObject *self, PyObject *args)
{
    PyObject *current;
    int team, player;
    std::vector<float> values;

    int size = PyTuple_Size(args);
    for (int i = 0; i < size; i++) {
        // retrive i'th object
        current = PyTuple_GET_ITEM(args, i);

        // check type
        if (!PyNumber_Check(current)) {
            PyErr_SetString(PyExc_TypeError,
                            "setData() expects all float or integer arguments");
            return NULL;
        }

        // add it to the list
        values.push_back(static_cast<float>(PyFloat_AsDouble(current)));
    }

    Py_BEGIN_ALLOW_THREADS

        // set comm data
        ((PyComm*)self)->comm->setData(values);

    Py_END_ALLOW_THREADS

        Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * PyComm_start (PyObject *self, PyObject *)
{
    int result;

    Py_BEGIN_ALLOW_THREADS

        result = ((PyComm*)self)->comm->start();

    Py_END_ALLOW_THREADS

        if (result == -1) {
            PyErr_SetString(PyExc_RuntimeError,
                            "Comm threads have already started");
            return NULL;
        }else if (result > 0) {
            PyErr_Format(PyExc_RuntimeError,
                         "pthreads return error code %i", result);
            return NULL;
        }else {
            Py_INCREF(Py_None);
            return Py_None;
        }
}

static PyObject * PyComm_stop (PyObject *self, PyObject *)
{
    Py_BEGIN_ALLOW_THREADS

        ((PyComm*)self)->comm->stop();

    Py_END_ALLOW_THREADS

        Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * PyComm_startTOOL (PyObject *self, PyObject *)
{
    int result;

    Py_BEGIN_ALLOW_THREADS

        result = ((PyComm*)self)->comm->startTOOL();

    Py_END_ALLOW_THREADS

        if (result == -1) {
            PyErr_SetString(PyExc_RuntimeError,
                            "TOOL thread has already started");
            return NULL;
        }else if (result > 0) {
            PyErr_Format(PyExc_RuntimeError,
                         "pthreads return error code %i", result);
            return NULL;
        }else {
            Py_INCREF(Py_None);
            return Py_None;
        }
}

static PyObject * PyComm_stopTOOL (PyObject *self, PyObject *)
{
    Py_BEGIN_ALLOW_THREADS

        ((PyComm*)self)->comm->stopTOOL();

    Py_END_ALLOW_THREADS

        Py_INCREF(Py_None);
    return Py_None;
}

void Comm::setLocalizationAccess(shared_ptr<LocSystem> _loc,
                                 shared_ptr<BallEKF> _ballEKF)
{
    tool.setLocalizationAccess(_loc, _ballEKF);
}

static PyObject * PyComm_getRobotName (PyObject *self, PyObject *)
{
    std::string name = ((PyComm*)self)->comm->getRobotName();
    return PyString_FromStringAndSize(name.c_str(), name.size());
}

static PyMemberDef PyComm_members[] = {
#ifdef USE_PYTHON_GC
    {"gc", T_OBJECT, offsetof(PyComm, gc), READONLY,
     "GameController object reference"},

#endif
    // Sentinel
    {NULL}
};

static PyMethodDef PyComm_methods[] = {

    {"latestComm", (PyCFunction)PyComm_latestComm, METH_NOARGS,
     "Retrieve a list of the latest received communication data"},

    {"setData", (PyCFunction)PyComm_setData, METH_VARARGS,
     "Set the current data to be sending out in the Comm UDP packets."},

    {"start", (PyCFunction)PyComm_start, METH_NOARGS,
     "Start the Comm communication threads."},

    {"stop", (PyCFunction)PyComm_stop, METH_NOARGS,
     "Stop the Comm communication threads."},

    {"startTOOL", (PyCFunction)PyComm_startTOOL, METH_NOARGS,
     "Start only the TOOLConnect thread."},

    {"stopTOOL", (PyCFunction)PyComm_stopTOOL, METH_NOARGS,
     "Stop only the TOOLConnect thread."},

    {"getRobotName", (PyCFunction)PyComm_getRobotName, METH_NOARGS,
     "Retrieve the name of this robot on the network"},

    { NULL } /* Sentinel */
};

static PyTypeObject PyCommType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "_comm.Comm",               /*tp_name*/
    sizeof(PyComm),            /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyComm_dealloc,/*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Comm object",             /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyComm_methods,            /* tp_methods */
    PyComm_members,            /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyComm_new,                /* tp_new */
};

static PyObject * PyComm_new (Comm *comm)
{
    PyComm *self;

    self = (PyComm *)PyCommType.tp_alloc(&PyCommType, 0);
    if (self != NULL) {
        self->comm = comm;
#ifdef USE_PYTHON_GC
        self->gc =
            reinterpret_cast<PyGameController*>(PyGameController_new(comm->getGC()));
        if (self->gc == NULL) {
            PyComm_dealloc(self);
            self = NULL;
        }
#endif
    }

    return (PyObject *)self;
}


//
// Python comm module definitions
//

static PyMethodDef module_methods[] = { {NULL} };

bool c_init_comm (void)
{
    if (!Py_IsInitialized())
        Py_Initialize();

    if (PyType_Ready(&PyCommType) < 0
#ifdef USE_PYTHON_GC
        || PyType_Ready(&PyGameControllerType) < 0
#endif
        ) {
        fprintf(stderr, "Error creating Comm Python class type\n");
        if (PyErr_Occurred())
            PyErr_Print();
        else
            fprintf(stderr, "But no error available!\n");
        return false;
    }

    comm_module = Py_InitModule3("_comm", module_methods,
                                 "Container module for Comm proxy class to C++");

    if (comm_module == NULL) {
        fprintf(stderr, "Error initializing Comm Python module\n");
        return false;
    }

    Py_INCREF(&PyCommType);
    PyModule_AddObject(comm_module, "Comm", (PyObject *)&PyCommType);

#ifdef USE_PYTHON_GC
    Py_INCREF(&PyGameControllerType);
    PyModule_AddObject(comm_module, "GameController",
                       (PyObject *)&PyGameControllerType);
#endif

    PyModule_AddObject(comm_module, "STATE_INITIAL",
                       PyInt_FromLong(STATE_INITIAL));
    PyModule_AddObject(comm_module, "STATE_SET",
                       PyInt_FromLong(STATE_SET));
    PyModule_AddObject(comm_module, "STATE_READY",
                       PyInt_FromLong(STATE_READY));
    PyModule_AddObject(comm_module, "STATE_PLAYING",
                       PyInt_FromLong(STATE_PLAYING));
    PyModule_AddObject(comm_module, "STATE_FINISHED",
                       PyInt_FromLong(STATE_FINISHED));
    PyModule_AddObject(comm_module, "STATE2_NORMAL",
                       PyInt_FromLong(STATE2_NORMAL));
    PyModule_AddObject(comm_module, "STATE2_PENALTYSHOOT",
                       PyInt_FromLong(STATE2_PENALTYSHOOT));

    PyModule_AddObject(comm_module, "PENALTY_NONE",
                       PyInt_FromLong(PENALTY_NONE));

    PyModule_AddObject(comm_module, "PENALTY_BALL_HOLDING",
                       PyInt_FromLong(PENALTY_SPL_BALL_HOLDING));

    PyModule_AddObject(comm_module, "PENALTY_PLAYER_PUSHING",
                       PyInt_FromLong(PENALTY_SPL_PLAYER_PUSHING));

    PyModule_AddObject(comm_module, "PENALTY_OBSTRUCTION",
                       PyInt_FromLong(PENALTY_SPL_OBSTRUCTION));

    PyModule_AddObject(comm_module, "PENALTY_INACTIVE_PLAYER",
                       PyInt_FromLong(PENALTY_SPL_INACTIVE_PLAYER));

    PyModule_AddObject(comm_module, "PENALTY_ILLEGAL_DEFENDER",
                       PyInt_FromLong(PENALTY_SPL_ILLEGAL_DEFENDER));

    PyModule_AddObject(comm_module, "PENALTY_LEAVING",
                       PyInt_FromLong(PENALTY_SPL_LEAVING_THE_FIELD));

    PyModule_AddObject(comm_module, "PENALTY_PLAYING_WITH_HANDS",
                       PyInt_FromLong(PENALTY_SPL_PLAYING_WITH_HANDS));

    PyModule_AddObject(comm_module, "PENALTY_REQUEST_FOR_PICKUP",
                       PyInt_FromLong(PENALTY_SPL_REQUEST_FOR_PICKUP));

    PyModule_AddObject(comm_module, "PENALTY_MANUAL",
                       PyInt_FromLong(PENALTY_MANUAL));


#ifdef USE_PYCOMM_FAKE_BACKEND
    shared_ptr<Synchro> synchro = shared_ptr<Synchro>(new Synchro());
    shared_ptr<Sensors> sensors = shared_ptr<Sensors>(new Sensors());

    shared_ptr<NaoPose> pose = shared_ptr<NaoPose>(new NaoPose(sensors));
    shared_ptr<Vision> vision = shared_ptr<Vision>(new Vision(pose, prof));

    PyObject *pcomm = PyComm_new(new Comm(synchro, sensors, vision));
    PyModule_AddObject(comm_module, "inst", pcomm);
#endif

    return true;
}

PyMODINIT_FUNC init_comm (void)
{
    c_init_comm();
}



//
// C++ Comm class methods
//

Comm::Comm (shared_ptr<Synchro> _synchro, shared_ptr<Sensors> s,
            shared_ptr<Vision> v)
    : Thread(_synchro, "Comm"), data(NUM_PACKET_DATA_ELEMENTS,0),
	  latest(new list<vector<float> >), sensors(s), timer(&monotonic_micro_time),
	  gc(new GameController()), tool(_synchro, s, v, gc)
{
    pthread_mutex_init(&comm_mutex,NULL);
    // initialize broadcast address structure
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(UDP_PORT);
    broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    // initialize gc broadcast address structure
    gc_broadcast_addr.sin_family = AF_INET;
    gc_broadcast_addr.sin_port = htons(GAMECONTROLLER_PORT);
    gc_broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
}

Comm::~Comm ()
{
    pthread_mutex_destroy(&comm_mutex);
}

int Comm::start ()
{
    // Run the TOOLConnect thread
    startTOOL();

    // Run the Comm (UDP+GameController) thread
    return Thread::start();
}

void Comm::run ()
{
    // Signal thread start
    running = true;
    trigger->on();

	struct timespec interval, remainder;
	interval.tv_sec = 0;
	interval.tv_nsec = SLEEP_MILLIS * 1000;

    try {
        bind();

        //discover_broadcast();

        while (running) {
            send();

            while (running && !timer.time_for_packet()) {
                PROF_ENTER(P_COMM);
                receive();
                nanosleep(&interval, &remainder);
                PROF_EXIT(P_COMM);
            }
        }
    } catch (socket_error &e) {
        fprintf(stderr, "Error occurred in Comm, thread has paused.\n");
        fprintf(stderr, "%s\n", e.what());
    }

    // Close the UDP socket
    ::close(sockn);

    // Signal thread end
    //running = false;
    //trigger->off();
}

void Comm::stop ()
{
    tool.stop();

    Thread::stop();
}

int Comm::startTOOL ()
{
    const int result = tool.start();
    if (result > 0)
        // if > 0 -> an error occurred
        // if -1 -> TOOLConnect is already started -> not an error
        fprintf(stderr, "Could not start TOOLConnect thread.\n");
    return result;
}

void Comm::stopTOOL ()
{
    tool.stop();
}


void Comm::error(socket_error err) throw()
{
    running = false;

    fprintf(stderr, "Error occurred in Comm, thread has stopped.\n");
    fprintf(stderr, "  Partially recoverable, so TOOL will continue\n");
    fprintf(stderr, "  Call comm.stop() or stopTOOL() to stop TOOL as well\n");
    fprintf(stderr, "%s\n", err.what());
}

void Comm::discover_broadcast ()
{
    // run ifconfig command to discover broadcast address
    FILE *f = popen(
        "ifconfig | grep 'Bcast' | sed -e 's/.* Bcast:\\([^ ]*\\) .*/\\1/'",
        "r");
    if (f == NULL) {
        error(SOCKET_ERROR(errno));
        return;
    }

    // read output and result (error status)
    char buf[1024];
    int len = fread(&buf[0], 1, 1024, f);
    int result = pclose(f);

    if (result == 0 && len > 0) {
        // add null character to enable processing as a normal string
        buf[len] = '\0';
        // convert address to in_addr struct
        struct in_addr addr;
        if (inet_aton(&buf[0], &addr)) {
            broadcast_addr.sin_addr = addr;
            cout << "Using broadcast address " << buf << endl;

        } else {
            error(SOCKET_ERROR(errno));
        }
    } else if (result != 0) {
        //cout<<"Failed to discover broadcast address -- command returned error";

    } else if (len <= 0) {
        //cout<<"Failed to discover broadcast address -- find broadcast returned no output";
    }

}

void Comm::bind () throw(socket_error)
{
    int one = 1;

    // create socket
    sockn = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (sockn == -1) {
        stop();
        throw SOCKET_ERROR(errno);
    }

#ifdef COMM_LISTEN
    // set bind address parameters
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(UDP_PORT);
    bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // set shared UDP socket (other processes may bind this port)
    ::setsockopt(sockn, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    // bind socket to address
    if (::bind(sockn, reinterpret_cast<const struct sockaddr*>(&bind_addr),
               sizeof(bind_addr)) == -1) {
        stop();
        throw SOCKET_ERROR(errno);
    }
#endif

    // Set broadcast enabled on the socket
    setsockopt(sockn, SOL_SOCKET, SO_BROADCAST, &one, sizeof(one));

    // Set socket to nonblocking io mode
    int flags = fcntl(sockn, F_GETFL);
    fcntl(sockn, F_SETFL, flags | O_NONBLOCK);


    // Bind GameController socket
    //
#ifdef USE_GAMECONTROLLER
    bind_gc();
#endif
}

void Comm::bind_gc () throw(socket_error)
{
    int one = 1;

    // create socket
    gc_sockn = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (gc_sockn == -1) {
        stop();
        throw SOCKET_ERROR(errno);
    }

#ifdef COMM_LISTEN
    // set bind address parameters
    struct sockaddr_in bind_addr;
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(GAMECONTROLLER_PORT);
    bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // set shared UDP socket (other processes may bind this port)
    ::setsockopt(gc_sockn, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    // bind socket to address
    if (::bind(gc_sockn, (const struct sockaddr*)&bind_addr,
               sizeof(bind_addr)) == -1) {
        stop();
        throw SOCKET_ERROR(errno);
    }
#endif

    // Set broadcast enabled on the socket
    setsockopt(gc_sockn, SOL_SOCKET, SO_BROADCAST, &one, sizeof(one));

//#ifdef COMM_LISTEN
    // Set socket to nonblocking io mode
    int flags = fcntl(gc_sockn, F_GETFL);
    fcntl(gc_sockn, F_SETFL, flags | O_NONBLOCK);
//#endif
}

void Comm::send () throw(socket_error)
{
    pthread_mutex_lock (&comm_mutex);

    // Let game controller know that we have been manually penalized
    if (gc->shouldSendManualPenalty()) {
        gc->sentManualPenalty();

        RoboCupGameControlReturnData returnPacket;

        if (gc->isManuallyPenalized()) {
            returnPacket.message = GAMECONTROLLER_RETURN_MSG_MAN_PENALISE;
        } else {
            returnPacket.message = GAMECONTROLLER_RETURN_MSG_MAN_UNPENALISE;
        }

        memcpy(returnPacket.header,
               GAMECONTROLLER_RETURN_STRUCT_HEADER,
               sizeof(returnPacket.header));

        returnPacket.version = GAMECONTROLLER_RETURN_STRUCT_VERSION;
        returnPacket.team = gc->team();
        returnPacket.player = gc->player();

        // Copy return packet data
        memcpy(&buf[0], &returnPacket, sizeof(returnPacket));

        // Unlock mutex before leaving method
        pthread_mutex_unlock (&comm_mutex);

        send(&buf[0], sizeof(returnPacket), gc_broadcast_addr);
    }
	// don't send packets if we are penalized.
	else if (!(gc->isPenalized()))
	{
        // C++ header data
        const CommPacketHeader header = {PACKET_HEADER, timer.timestamp(),
                                         gc->team(), gc->player(), gc->color()};
        memcpy(&buf[0], &header, sizeof(header));
        // variable Python extra data
        memcpy(&buf[sizeof(header)], &data[0], sizeof(float) * data.size());

        // Unlock mutex before leaving method
        pthread_mutex_unlock (&comm_mutex);

        send(&buf[0], sizeof(header) + sizeof(float) * data.size(),
             broadcast_addr);
    }
	else
	{
		pthread_mutex_unlock (&comm_mutex);
	}
}

void Comm::send (const char *msg, int len, sockaddr_in &addr) throw(socket_error)
{
#ifdef COMM_SEND
    // send the udp message
    int result = -2;

	struct timespec interval, remainder;
	interval.tv_sec = 0;
	interval.tv_nsec = 100000;

    while (result == -2) {
        result = ::sendto(sockn, msg, len, 0, (struct sockaddr*)&addr,
                          sizeof(broadcast_addr));
        // except if error is blocking error
        if (result == -1 && errno == EAGAIN) {
            result = -2;
            nanosleep(&interval, &remainder);
        }
    }

    // error
    if (result == -1) {
        if (errno == ENETUNREACH &&
            broadcast_addr.sin_addr.s_addr == htonl(INADDR_BROADCAST))
            // attempt to discover our specific broadcast address
            discover_broadcast();

        else if (errno != EAGAIN)
            error(SOCKET_ERROR(errno));
    }
#endif

    // record last time we sent a message
    timer.sent_packet();
}

void Comm::receive () throw(socket_error)
{
#ifdef COMM_LISTEN

    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(sockaddr_in);

    // receive a UDP message
    int result = ::recvfrom(sockn, &buf, UDP_BUF_SIZE, 0,
                            (struct sockaddr*)&recv_addr, &addr_len);
    while (result > 0) {
        // handle the message
        handle_comm(recv_addr, &buf[0], result);
        // check for another one
        result = ::recvfrom(sockn, &buf, UDP_BUF_SIZE, 0,
                            (struct sockaddr*)&recv_addr, &addr_len);
    }

    // if an error occured (other than nonblocking EAGAIN error)
    if (running && result == -1 && errno != EAGAIN) {
        stop();
        throw SOCKET_ERROR(errno);
    }

    // Receive GameController packets
    //
#ifdef USE_GAMECONTROLLER
    receive_gc();
#endif

#endif
}

void Comm::receive_gc () throw(socket_error)
{
#ifdef COMM_LISTEN

    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(sockaddr_in);

    // receive a UDP message
    int result = ::recvfrom(gc_sockn, &buf, UDP_BUF_SIZE, 0,
                            (struct sockaddr*)&recv_addr, &addr_len);
    while (result > 0) {
        // handle the message
        handle_gc(recv_addr, &buf[0], result);
        // check for another one
        result = ::recvfrom(gc_sockn, &buf, UDP_BUF_SIZE, 0,
                            (struct sockaddr*)&recv_addr, &addr_len);
    }

    // if an error occured (other than nonblocking EAGAIN error)
    if (running && result == -1 && errno != EAGAIN) {
        stop();
        throw SOCKET_ERROR(errno);
    }
#endif
}

void Comm::handle_comm (struct sockaddr_in &addr, const char *msg, int len)
    throw()
{
    if (len == static_cast<int>(strlen(TOOL_REQUEST_MSG)) &&
        memcmp(msg, TOOL_REQUEST_MSG, TOOL_REQUEST_LEN) == 0) {

        std::string robotName = getRobotName();
        const char *name = robotName.c_str();

        int len = TOOL_ACCEPT_LEN + 3 + strlen(name);
        char *response = (char *) malloc(len);

        memcpy(&response[0], TOOL_ACCEPT_MSG, TOOL_ACCEPT_LEN);
        response[TOOL_ACCEPT_LEN] = ':';
        response[TOOL_ACCEPT_LEN+1] = (char)strlen(name);
        response[TOOL_ACCEPT_LEN+2] = ':';
        memcpy(&response[TOOL_ACCEPT_LEN+3], name, strlen(name));

        struct sockaddr_in r_addr = addr;
        r_addr.sin_port = htons(TOOL_PORT);

        send(&response[0], len, r_addr);
        free(response);

    } else {

        // validate packet format, check packet timestamp, and parse data
        CommPacketHeader packet;
        if (validate_packet(msg, len, packet))
            parse_packet(packet, msg + sizeof(packet), len - sizeof(packet));

    }

}

void Comm::handle_gc (struct sockaddr_in &addr, const char *msg, int len) throw()
{
	gc->handle_packet(msg, len);
	if (gc->shouldResetTimer()){
		timer.reset();
	}
}

bool Comm::validate_packet (const char* msg, int len, CommPacketHeader& packet)
    throw() {
    // check packet length
	if (static_cast<unsigned int>(len) < sizeof(CommPacketHeader)){
		std::cout << "bad length" << std::endl;
		return false;
	}

    // cast packet data into CommPacketHeader struct
    packet = *reinterpret_cast<const CommPacketHeader*>(msg);

    if (memcmp(packet.header, PACKET_HEADER, sizeof(PACKET_HEADER)) != 0){
        //std::cout << "bad header" << std::endl;
        return false;
    }
    // check team number
    if (packet.team != gc->team()){
        //std::cout << "bad team number" << std::endl;
        return false;
    }

    // check player number
    if (packet.player < 0 || packet.player > NUM_PLAYERS_PER_TEAM ||
        packet.player == gc->player()){
        //std::cout << "bad player number" << std::endl;
        return false;
    }

    if (!timer.check_packet(packet)){
        //std::cout << "bad timer" << std::endl;
        return false;
    }

    // passed all checks, packet is valid
    return true;
}

void Comm::parse_packet (const CommPacketHeader &packet, const char* data, int size)
    throw()
{
    int len = size / sizeof(float);

    vector<float> v(len + 3);
    v[0] = static_cast<float>(packet.team);
    v[1] = static_cast<float>(packet.player);
    v[2] = static_cast<float>(packet.color);
    memcpy(&v[3], data, len * sizeof(float));

    if (latest->size() >= MAX_MESSAGE_MEMORY)
        latest->pop_front();
    latest->push_back(v);
}

void Comm::add_to_module ()
{
    if (comm_module == NULL) {
        if (!c_init_comm()) {
            cerr << "Comm module failed to initialize the backend" << endl;
            PyErr_Print();
        }
    }

    if (comm_module != NULL) {
        PyObject *comm = PyComm_new(this);
        PyModule_AddObject(comm_module, "inst", comm);
    }
}

list<vector<float> >* Comm::latestComm()
{
    list<vector<float> >* old = latest;
    latest = new list<vector<float> >();
    return old;
}

TeammateBallMeasurement Comm::getTeammateBallReport()
{
    // Iterate through latest, checking if anyone has a ball report
    // Choose the ball report from the robot with min uncertainty
    list<vector<float> >::iterator i;
    TeammateBallMeasurement m;
    float minUncert = 10000.0f;
    for (i = latest->begin(); i != latest->end(); ++i) {
        // Get the combined uncert x and y
        float curUncert = static_cast<float>( hypot((*i)[6],(*i)[7]) );
        // If the teammate sees the ball and its uncertainty is less than the
        // Current minimum, then we
        if ((*i)[13] > 0.0 && curUncert < minUncert) {
            minUncert = curUncert;
            m.ballX = (*i)[9];
            m.ballY = (*i)[10];
        }
    }
    return m;
}

void Comm::setData (std::vector<float> &newData)
{
    pthread_mutex_lock (&comm_mutex);

    data = newData;

    pthread_mutex_unlock (&comm_mutex);
}

std::string Comm::getRobotName ()
{
    struct utsname name_str;
    uname(&name_str);

    std::string name = name_str.nodename;

#if ROBOT(NAO_SIM)
    name.append("- ");
    name.append(robot_get_name());
#elif ROBOT(NAO)
#elif ROBOT(AIBO)
#else
#  error "Undefined robot type"
#endif

    return name;
}
