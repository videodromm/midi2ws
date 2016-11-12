#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxLibwebsockets.h"
#include "ofxXmlSettings.h"

#define NUM_MESSAGES 30 // how many past messages we want to keep


class ofApp : public ofBaseApp, public ofxMidiListener
{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void exit();

    ofxLibwebsockets::Client client;

    // queue of rec'd messages
    vector<string> messages;

    // string to send to clients
    string toSend;

    // websocket methods
    void onConnect( ofxLibwebsockets::Event& args );
    void onOpen( ofxLibwebsockets::Event& args );
    void onClose( ofxLibwebsockets::Event& args );
    void onIdle( ofxLibwebsockets::Event& args );
    void onMessage( ofxLibwebsockets::Event& args );
    void onBroadcast( ofxLibwebsockets::Event& args );
    // midi
    void newMidiMessage(ofxMidiMessage& eventArgs);
    ofxMidiIn midiIn;
    ofxMidiIn midiInLaunchpad;
    ofxMidiIn midiInNanoKontrol;
    ofxMidiMessage midiMessage;
    float normalizedValue;
	unsigned int name;
	float value;
	unsigned int previousName;
	float previousValue;

    stringstream text;

    // xml settings
    ofxXmlSettings settings;
    string settingsFile;
    string host;
    unsigned int port;
};
