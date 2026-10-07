unit module Logging;

use Logging::Logger;
use Logging::Severity;

sub info(*@args) is export {
    my $frame = callframe(1);
    my $msg = @args > 1 ?? sprintf(shift(@args), |@args) !! @args[0].Str;
    Logging::Logger.get.log(Info, $msg, $frame.file, $frame.line);
}

sub success(*@args) is export {
    my $frame = callframe(1);
    my $msg = @args > 1 ?? sprintf(shift(@args), |@args) !! @args[0].Str;
    Logging::Logger.get.log(Success, $msg, $frame.file, $frame.line);
}

sub debug(*@args) is export {
    my $frame = callframe(1);
    my $msg = @args > 1 ?? sprintf(shift(@args), |@args) !! @args[0].Str;
    Logging::Logger.get.log(Debug, $msg, $frame.file, $frame.line);
}

sub warning(*@args) is export {
    my $frame = callframe(1);
    my $msg = @args > 1 ?? sprintf(shift(@args), |@args) !! @args[0].Str;
    Logging::Logger.get.log(Warning, $msg, $frame.file, $frame.line);
}

sub error(*@args) is export {
    my $frame = callframe(1);
    my $msg = @args > 1 ?? sprintf(shift(@args), |@args) !! @args[0].Str;
    Logging::Logger.get.log(Error, $msg, $frame.file, $frame.line);
}

sub fatal(*@args) is export {
    my $frame = callframe(1);
    my $msg = @args > 1 ?? sprintf(shift(@args), |@args) !! @args[0].Str;
    Logging::Logger.get.log(Fatal, $msg, $frame.file, $frame.line);
}
