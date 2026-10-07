#include "../include/logging.hpp"

int main()
{
	logging::debug("this is a debug message");
	logging::info("this is info");
	logging::success("this is a success");
	logging::warning("this is a warning");
	logging::error("this is an error");
	logging::fatal("this is fatal");

	auto& log = logging::logger::get();

	log.set_show_location(false);
	logging::info("location hidden");

	log.set_show_pid(true);
	logging::info("pid shown");

	log.set_use_color(false);
	logging::info("colors off");

	log.set_use_color(true);
	log.set_show_pid(false);
	log.set_min_severity(logging::severity::warning);
	logging::info("you wont see this");
	logging::warning("but you will see this");

	log.set_min_severity(logging::severity::debug);
	std::cout << "\n\n";

	std::cout << "also comes with args support: " << '\n';
	logging::info("my name is {} and im {} years old", "mike", 17);

	std::cin.get();

	return 0;
}