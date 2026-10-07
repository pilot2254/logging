module logging

pub enum Severity {
	info
	success
	debug
	warning
	error
	fatal
}

pub fn (s Severity) str() string {
	return match s {
		.info { 'INFO' }
		.success { 'SUCCESS' }
		.debug { 'DEBUG' }
		.warning { 'WARNING' }
		.error { 'ERROR' }
		.fatal { 'FATAL' }
	}
}
