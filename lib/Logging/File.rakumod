unit class Logging::File;

has IO::Handle $!stream;

method open(Str $path --> Bool) {
    $!stream.close if $!stream;
    try {
        $!stream = open $path, :a;
        CATCH { default { return False } }
    }
    return True;
}

method is-open(--> Bool) {
    return $!stream.defined;
}

method write(Str $line) {
    if $!stream {
        $!stream.say($line);
        $!stream.flush;
    }
}
