#include "erl/UserInterface.h"
#include "erl/System.h"
#include <iostream>
namespace erl { void runInteractive(ReliefHub& hub){std::cout<<"ERL-HUB\n1. Load demonstration scenario\n2. Print audit report\n0. Exit\nChoice: ";int c=0;while(std::cin>>c&&c!=0){if(c==1){hub.loadDemoScenario();std::cout<<"Demo loaded.\n";}else if(c==2)std::cout<<hub.auditReport();std::cout<<"Choice: ";}} }

