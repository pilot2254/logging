unit module Logging::Severity;

enum Severity is export (
    Info    => 0,
    Success => 1,
    Debug   => 2,
    Warning => 3,
    Error   => 4,
    Fatal   => 5
);

sub severity-to-string(Severity $s --> Str) is export {
    given $s {
        when Success { 'SUCCESS' }
        when Debug   { 'DEBUG' }
        when Warning { 'WARNING' }
        when Error   { 'ERROR' }
        when Fatal   { 'FATAL' }
        default      { 'INFO' }
    }
}
