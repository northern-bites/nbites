#include "NewMan.h"
#include <iostream>

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
      testDiagram()
{
    setModuleDescription("The Northern Bites' soccer player.");
    std::cout << "Man constructor." << std::endl;

    FirstModule firstMod;
    SecondModule secondMod;

    secondMod.in.wireTo(&firstMod.out);
    testDiagram.addModule(secondMod);
    testDiagram.addModule(firstMod);

    for(int i = 0; i < 3; i++)
    testDiagram.run();
}

Man::~Man()
{
    std::cout << "Man destructor." << std::endl;
}
