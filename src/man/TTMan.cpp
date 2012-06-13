//#include "TTMan.h"
//
//using boost::shared_ptr;
//using namespace std;
//
//
//TTMan::TTMan(boost::shared_ptr<Sensors> _sensors,
//             boost::shared_ptr<Transcriber> _transcriber,
//             boost::shared_ptr<ThreadedImageTranscriber> _imageTranscriber,
//             boost::shared_ptr<ThreadedMotionEnactor> _enactor,
//             boost::shared_ptr<Lights> _lights,
//             boost::shared_ptr<Speech> _speech)
//    :Man(_sensors, _transcriber, _imageTranscriber,
//         _enactor, _lights, _speech),
//     threadedImageTranscriber(_imageTranscriber),
//     threadedEnactor(_enactor){}
//
//TTMan::~TTMan(){}
//
//
//void TTMan::startSubThreads(){
//    if(threadedEnactor->start()!=0)
//        cout << "Failed to start enactor" <<endl;
//
//    Man::startSubThreads();
//
//    // Start Image transcriber thread (it handles its own threading
//    if (threadedImageTranscriber->start() != 0)
//        cout << "Image transcriber failed to start" << endl;
//
//}
//void TTMan::stopSubThreads(){
//
//#ifdef DEBUG_MAN_THREADING
//    cout << "  TTMan stopping:" << endl;
//#endif
//
//    threadedImageTranscriber->stop();
//    threadedImageTranscriber->waitForThreadToFinish();
//
//    Man::stopSubThreads();
//
//    threadedEnactor->stop();
//    threadedEnactor->waitForThreadToFinish();
//}
//
