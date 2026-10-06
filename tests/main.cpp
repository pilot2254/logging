#include "../include/logging.hpp"

int main()
{
	logging::info("this is info");
	logging::debug("this is a debug message");
	logging::warning("this is a warning");
	logging::error("this is an error");
	logging::fatal("this is fatal");

	logging::logger::get().set_show_location(false);
	logging::info("location hidden");

	std::cout << "\n\n";

	std::cout << "also comes with args support: " << '\n';
	logging::info("my name is {} and im {} years old", "mike", 17);

	Sleep(INFINITE);

	return 0;
}
