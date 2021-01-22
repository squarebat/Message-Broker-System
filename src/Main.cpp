#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "SystemContext.h"

using namespace std;
namespace po = boost::program_options;

int main(int argc, char** argv) {
    SystemContext &systemContext = SystemContext::GenerateContext(argc, argv);
    systemContext.StartAPI();
    return 0;
}