module main

import logging

fn main() {
	logging.info('this is info')
	logging.success('this is success')
	logging.debug('this is a debug message')
	logging.warning('this is a warning')
	logging.error('this is an error')
	logging.fatal('this is fatal')

	println('\n\n')

	println('also comes with args support: ')
	name := 'mike'
	age := 17
	logging.info('my name is $name and im $age years old')

	println('\n\n')
	println('you can also hide locations')
	mut log := logging.get()
	log.set_show_location(false)
	logging.info('location hidden')
}
