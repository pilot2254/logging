module logging

import os

struct File {
mut:
	stream os.File
	opened bool
}

fn (mut f File) open(path string) bool {
	if f.opened {
		f.stream.close()
	}
	mut stream := os.open_append(path) or { return false }
	f.stream = stream
	f.opened = true
	return true
}

fn (f &File) is_open() bool {
	return f.opened
}

fn (mut f File) write(line string) {
	if f.opened {
		f.stream.writeln(line) or { }
	}
}
