#include "../include/logging.hpp"

int main()
{
        logging::info("this is info");
        logging::debug("this is a debug message");
        logging::warning("this is a warning");
        logging::error("this is an error");
        logging::fatal("this is fatal");

        std::cout << '\n\n' << "std::cout"

        return 0;
}
