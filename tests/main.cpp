#include "../include/logging.hpp"

int main()
{
	logging::info("this is info");
	logging::success("this is success");
	logging::debug("this is a debug message");
	logging::warning("this is a warning");
	logging::error("this is an error");
	logging::fatal("this is fatal");

	std::cout << "\n\n";

	std::cout << "also comes with args support: " << '\n';
	logging::info("my name is {} and im {} years old", "mike", 17);

	std::cout << "\n\n";
	std::cout << "you can also hide locations\n";
	logging::logger::get().set_show_location(false);
	logging::info("location hidden");

	Sleep(INFINITE);

	return 0;
}
