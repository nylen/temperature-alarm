// If serial port fails to open and this isn't here, Node 8.2.0 gives:
// "DeprecationWarning: Unhandled promise rejections are deprecated."
// Node 4.2.6 does absolutely nothing.
process.on( 'unhandledRejection', err => {
	console.error( 'unhandledRejection:', err );
	process.exit( 1 );
} );

const SerialPort = require( 'serialport' );

// Despite https://github.com/voodootikigod/node-serialport/pull/495
// introducing hupcl: false, I cannot seem to disable DTR to prevent Arduino
// resetting on connection
const port = new SerialPort( '/dev/ttyUSB0', {
	baudRate : 57600,
	// hupcl    : false,
} );

const parser = new SerialPort.parsers.Readline()
port.pipe( parser );

function requestReading() {
	port.write( 'TEMP\n' );
}

port.on( 'open', err => {
	if ( err ) throw err;

	requestReading(); // no effect: Arduino is resetting

	parser.on( 'data', line => {
		line = line.trim();
		if ( line === 'ready' ) {
			requestReading();
		} else {
			console.log( line );
			port.close( err => {
				if ( err ) throw err;
			} );
		}
	} );
} );
