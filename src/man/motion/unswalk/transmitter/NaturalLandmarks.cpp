#include "NaturalLandmarks.hpp"
#include "blackboard/Blackboard.hpp"

using namespace boost::asio;
using namespace std;

NaturalLandmarksTransmitter::NaturalLandmarksTransmitter(Blackboard *bb) :
   Adapter(bb),
   NaoTransmitter((bb->config)["transmitter.nlport"].as<int>(), (bb->config)["transmitter.address"].as<string>()) {}

void NaturalLandmarksTransmitter::tick() {}
