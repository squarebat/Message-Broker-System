#include <SystemContext.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char** argv) {
    SystemContext& systemContext = SystemContext::GenerateContext(argc, argv);
    systemContext.StartAPI();
    return 0;
}
