unit module Logging::Time;

sub get-time(--> Str) is export {
    my $now = DateTime.now;
    sprintf "%04d-%02d-%02d %02d:%02d:%02d",
        $now.year, $now.month, $now.day,
        $now.hour, $now.minute, $now.second.Int;
}
