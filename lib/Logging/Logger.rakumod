unit class Logging::Logger;

use Logging::File;
use Logging::Severity;
use Logging::Time;

has Logging::File $!file .= new;
has Severity $.min is rw = Info;
has Bool $.show-location is rw = True;
has Lock $!mutex .= new;

my Logging::Logger $instance;

method get() returns Logging::Logger {
    unless $instance {
        $instance = Logging::Logger.new;
        $instance.set-file('log.txt');
    }
    return $instance;
}

method set-file(Str $path --> Bool) {
    $!mutex.protect: {
        return $!file.open($path);
    }
}

method set-min-severity(Severity $s) {
    $!mutex.protect: {
        $!min = $s;
    }
}

method set-show-location(Bool $show) {
    $!mutex.protect: {
        $!show-location = $show;
    }
}

method log(Severity $s, Str $message, Str $file-name, Int $line) {
    $!mutex.protect: {
        return if $s < $!min;

        my $time-str = get-time();
        my $sev-str = severity-to-string($s);

        my $out = "[$time-str] [$sev-str]";

        if $!show-location {
            my $name = $file-name.IO.basename;
            $out ~= " [$name:$line]";
        }

        $out ~= ": $message";

        my $colored-out = do given $s {
            when Success { "\e[32m$out\e[0m" }
            when Debug   { "\e[36m$out\e[0m" }
            when Warning { "\e[33m$out\e[0m" }
            when Error   { "\e[31m$out\e[0m" }
            when Fatal   { "\e[41m\e[37m$out\e[0m" }
            default      { "\e[37m$out\e[0m" }
        }

        say $colored-out;
        $!file.write($out);
    }
}
