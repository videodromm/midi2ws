#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetEscapeQuitsApp(false);
	// defaults
	host = "localhost";
	port = 8088;
	name = previousName = 0;
	value = previousValue = 0.0;

	// XML
	settingsFile = "vdsettings.xml";
	if (settings.loadFile(settingsFile)) {
		settings.pushTag("settings");
		host = settings.getValue("host", "localhost");
		port = settings.getValue("port", 8088);
		settings.popTag();
	} else {
		ofLog() << "XML error, loading default values";
		settings.pushTag("settings");
		settings.setValue("host", host);
		settings.setValue("port", "8088");
		settings.popTag();
		// save settings
		settings.saveFile(settingsFile);
	}

	// websocket client
    // 1 - get default options
    options = ofxLibwebsockets::defaultClientOptions();
    
    // 2 - set basic params
    options.host = host;
    options.port = port;
	// options.bUseSSL = false; // you'll have to manually accept this self-signed cert if 'true'!

    // advanced: set keep-alive timeouts for events like
    // loss of internet
    
    // 3 - set keep alive params
    // BIG GOTCHA: on BSD systems, e.g. Mac OS X, these time params are system-wide
    // ...so ka_time just says "check if alive when you want" instead of "check if
    // alive after X seconds"
    options.ka_time     = 1;
    options.ka_probes   = 1;
    options.ka_interval = 1;
    
    // 4 - connect
    client.connect(options);

	text.str(""); // clear
	text << (client.isConnected() ? "Client is connected" : "Client disconnected");
	cout << text.str();

	// this adds your app as a listener for the server
	client.addListener(this);

	// midi
	// print input ports to console
	midiIn.listPorts(); // via instance
	//ofxMidiIn::listPorts(); // via static as well
	if (midiIn.getNumPorts()) {
		for (int i = 0; i < midiIn.getNumPorts(); i++) {
			string pname = midiIn.getPortName(i);
			if (ofStringTimesInString(pname, "Launchpad")) {
				printf("Launchpad on %d\n", i);
				midiInLaunchpad.openPort(i);
				midiInLaunchpad.addListener(this);
			}
			if (ofStringTimesInString(pname, "nano")) {
				printf("nanoKontrol on %d\n", i);
				midiInNanoKontrol.openPort(i);
				midiInNanoKontrol.addListener(this);
			}
		}
	}
	// open port by number (you may need to change this)
	midiIn.openPort(2);
	midiIn.addListener(this);

	// don't ignore sysex, timing, & active sense messages,
	// these are ignored by default
	midiIn.ignoreTypes(false, false, false);

	// add ofApp as a listener
	midiIn.addListener(this);

	// print received messages to the console
	midiIn.setVerbose(true);
}

//--------------------------------------------------------------
void ofApp::update(){
	// midi
	switch (midiMessage.status)
	{
	case MIDI_CONTROL_CHANGE:
		name = midiMessage.control;
		value = midiMessage.value;
		break;
	case MIDI_NOTE_ON:
		name = midiMessage.pitch;
		value = midiMessage.velocity;
		break;
	case MIDI_NOTE_OFF:
		name = midiMessage.pitch;
		value = midiMessage.velocity;
		break;
	default:
		//cout << "not implemented status "  << midiMessage.status << " val "  << midiMessage.value << " ctrl "  << midiMessage.control << endl;
		break;
	}
	if (name != previousName || value != previousValue) {
		// only send if value has changed
		text.str(""); // clear
		// lmap<float>(value, 0.0, 127.0, 0.0, 1.0) (from Cinder)
		normalizedValue = value / 127;
		text << "{\"params\" :[{\"name\" : " << name << ",\"value\" : " << normalizedValue << "}]}";
		if (name != 0) client.send(text.str());
		cout << " ctrl "  << midiMessage.control << " val "  << midiMessage.value << " pitch "  << midiMessage.pitch << " velocity "  << midiMessage.velocity << endl;
		previousName = name;
		previousValue = value;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::onConnect(ofxLibwebsockets::Event& args){
	cout << "on connected" << endl;
}

//--------------------------------------------------------------
void ofApp::onOpen(ofxLibwebsockets::Event& args){
	cout << "new connection open" << args.conn.getClientIP() << ", " << args.conn.getClientName() << endl;
	messages.push_back("New connection from " + args.conn.getClientIP() + ", " + args.conn.getClientName());
}

//--------------------------------------------------------------
void ofApp::onClose(ofxLibwebsockets::Event& args){
	cout << "on close" << endl;
	messages.push_back("Connection closed");
}

//--------------------------------------------------------------
void ofApp::onIdle(ofxLibwebsockets::Event& args){
	//cout << "on idle" << endl;
}

//--------------------------------------------------------------
void ofApp::onMessage(ofxLibwebsockets::Event& args){
	cout << "got message " << args.message << endl;

	// trace out string messages or JSON messages!
	if (!args.json.isNull()){
		messages.push_back("New message: " + args.json.toStyledString() + " from " + args.conn.getClientName());
	}
	else {
		messages.push_back("New message: " + args.message + " from " + args.conn.getClientName());
	}

	// echo server = send message right back!
	//args.conn.send(args.message);
}

//--------------------------------------------------------------
void ofApp::onBroadcast(ofxLibwebsockets::Event& args){
	cout << "got broadcast " << args.message << endl;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key){
	case 'c':
		// connect ws client
		client.connect(options);
		break;
	case 'q':
		// quit
		exit();
		break;
	case 's':
		// save settings
		settings.saveFile(settingsFile);
		break;
	default:
		cout << key;
	    //client.send("Hello");
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	
}
void ofApp::newMidiMessage(ofxMidiMessage& msg) {

	// make a copy of the latest message
	midiMessage = msg;

}
void ofApp::exit() {

	// clean up
	midiIn.closePort();
	midiIn.removeListener(this);
	// save settings
	settings.saveFile(settingsFile);

}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
