#include "../include/logging.hpp"

int main()
{
	logging::debug("this is a debug message");
	logging::info("this is info");
	logging::success("this is a success");
	logging::warning("this is a warning");
	logging::error("this is an error");
	logging::fatal("this is fatal");



	std::cout << "\n\n";



	auto& log = logging::logger::get();

	log.set_show_location(false);
	logging::info("location hidden");

	log.set_show_pid(true);
	logging::info("pid shown");

	log.set_use_color(false);
	logging::info("colors off");

	log.set_use_color(true);
	log.set_show_pid(false);

	log.set_show_time(false);
	logging::info("time hidden");

	log.set_show_severity(false);
	logging::info("time and severity hidden");

	log.set_show_time(true);
	log.set_show_severity(true);

	log.set_log_to_file(false);
	logging::info("this one isnt written to the file");
	log.set_log_to_file(true);

	log.set_min_severity(logging::severity::warning);
	logging::info("you wont see this");
	logging::warning("but you will see this");

	log.set_min_severity(logging::severity::debug);

	log.set_auto_flush(true);				//flush every line instead of just warning and above
	logging::info("flushed right away");
	log.set_auto_flush(false);
	log.flush();							//or flush by hand whenever you want



	std::cout << "\n\n";



	std::cout << "also comes with args support: " << '\n';
	logging::info("my name is {} and im {} years old", "mike", 17);

	//sinks get every line too, here we just count the errors
	int errors = 0;
	log.add_sink([&](logging::severity sev, const std::string& line)
	{
		if (sev >= logging::severity::error) errors++;
	});
	logging::error("this goes to the console, the file and the sink");
	logging::info("errors seen by the sink: {}", errors);
	log.clear_sinks();

	//log.set_abort_on_fatal(true); //would kill the program right after a fatal message

	std::cin.get();

	return 0;
}