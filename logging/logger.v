module logging

import os
import time
import term
import sync

pub struct Logger {
mut:
	file          File
	min           Severity = .info
	show_location bool     = true
	mutex         &sync.Mutex = unsafe { nil }
}

__global (
	global_logger &Logger
)

fn init() {
	global_logger = &Logger{
		mutex: sync.new_mutex()
	}
	global_logger.file.open('log.txt')
}

pub fn get() &Logger {
	return global_logger
}

pub fn (mut l Logger) set_file(path string) bool {
	l.mutex.lock()
	defer { l.mutex.unlock() }
	return l.file.open(path)
}

pub fn (mut l Logger) set_min_severity(s Severity) {
	l.mutex.lock()
	defer { l.mutex.unlock() }
	l.min = s
}

pub fn (mut l Logger) set_show_location(show bool) {
	l.mutex.lock()
	defer { l.mutex.unlock() }
	l.show_location = show
}

pub fn (mut l Logger) log(s Severity, message string, file_name string, line string) {
	l.mutex.lock()
	defer { l.mutex.unlock() }

	if int(s) < int(l.min) {
		return
	}

	t := time.now().custom_format('YYYY-MM-DD HH:mm:ss')
	mut out := '[$t] [${s.str()}]'

	if l.show_location {
		name := os.base(file_name)
		out += ' [$name:$line]'
	}

	out += ': $message'

	colored_out := match s {
		.success { term.green(out) }
		.debug { term.cyan(out) }
		.warning { term.yellow(out) }
		.error { term.red(out) }
		.fatal { term.bg_red(term.white(out)) }
		else { out }
	}

	println(colored_out)
	l.file.write(out)
}

[inline]
pub fn info(message string) {
	mut g := global_logger
	g.log(.info, message, @FILE, @LINE)
}

[inline]
pub fn success(message string) {
	mut g := global_logger
	g.log(.success, message, @FILE, @LINE)
}

[inline]
pub fn debug(message string) {
	mut g := global_logger
	g.log(.debug, message, @FILE, @LINE)
}

[inline]
pub fn warning(message string) {
	mut g := global_logger
	g.log(.warning, message, @FILE, @LINE)
}

[inline]
pub fn error(message string) {
	mut g := global_logger
	g.log(.error, message, @FILE, @LINE)
}

[inline]
pub fn fatal(message string) {
	mut g := global_logger
	g.log(.fatal, message, @FILE, @LINE)
}
