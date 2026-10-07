use lib 'lib';
use Logging;

sub MAIN() {
    info("this is info");
    success("this is success");
    debug("this is a debug message");
    warning("this is a warning");
    error("this is an error");
    fatal("this is fatal");

    say "\n\n";

    say "also comes with args support: ";
    info("my name is %s and im %d years old", "mike", 17);

    say "\n\n";
    say "you can also hide locations";
    Logging::Logger.get().set-show-location(False);
    info("location hidden");
}
