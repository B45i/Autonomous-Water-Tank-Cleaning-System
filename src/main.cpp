#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


int inletPin = D0;
int drainPin = D1;
int trigPin  = D2;
int echoPin  = D3;
int minWater = 200;
int reading;

const char *ssid     = "EvilCorp";
const char *password = "helloworld";

ESP8266WebServer server(80);


int readWaterLevel(int trigPin, int echoPin) {
	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);
	int duration = pulseIn(echoPin, HIGH);
	return  duration*0.034/2;
}

void motorControl(int motor, int value) {
	digitalWrite(motor, value);
}

void handleRoot() {
	server.send(200, "text/html",
		"<!DOCTYPE html>\n"
		"<html>\n"
		"\t<head>\n"
		"\t\t<title>Home | Autonomous Water Tank Cleaning System</title>\n"
		"\t\t<meta name=\"viewport\" content=\"initial-scale=1.0, maximum-scale=1.0, user-scalable=no\" />\n"
		"\t\t<style>\n"
		"\t\t\thtml {\n"
		"\t\t\t\tbackground: #29ce9a;\n"
		"\t\t\t\tfont-family: sans-serif;\n"
		"\t\t\t\tdisplay: table;\n"
		"\t\t\t\twidth: 100%;\n"
		"\t\t\t\theight: 100%;\n"
		"\t\t\t}\n"
		"\n"
		"\t\t\tbody {\n"
		"\t\t\t\tdisplay: table-cell;\n"
		"\t\t\t\tvertical-align: middle;\n"
		"\t\t\t\ttext-align: center;\n"
		"\t\t\t}\n"
		"\n"
		"\t\t\t.c_panel {\n"
		"\t\t\t\tdisplay: inline-block;\n"
		"\t\t\t\tpadding: 50px;\n"
		"\t\t\t\tbackground-color: #fff;\n"
		"\t\t\t\tborder-radius: 20px;\n"
		"\t\t\t\tcolor: #666;\n"
		"\t\t\t\ttext-align: center;\n"
		"\t\t\t\tmargin: 10px;\n"
		"\t\t\t}\n"
		"\n"
		"\t\t\t.c_panel_content {\n"
		"\t\t\t\ttext-align: left;\n"
		"\t\t\t\tdisplay: inline-block;\n"
		"\t\t\t}\n"
		"\n"
		"\t\t\t.c_panel_title {\n"
		"\t\t\t\tfont-size: 50px;\n"
		"\t\t\t\tfont-weight: bold;\n"
		"\t\t\t\ttext-transform: uppercase;\n"
		"\t\t\t\tpadding-bottom: 30px;\n"
		"\t\t\t}\n"
		"\n"
		"\t\t\t.switcher {\n"
		"\t\t\t\tposition: relative;\n"
		"\t\t\t\tdisplay: inline-block;\n"
		"\t\t\t\tcursor: pointer;\n"
		"\t\t\t\tpadding-left: 100px;\n"
		"\t\t\t\theight: 40px;\n"
		"\t\t\t\tline-height: 40px;\n"
		"\t\t\t\tmargin: 5px;\n"
		"\t\t\t\tfont-size: 30px;\n"
		"\t\t\t\t-webkit-user-select: none;\n"
		"\t\t\t\t\t -moz-user-select: none;\n"
		"\t\t\t\t\t\t-ms-user-select: none;\n"
		"\t\t\t\t\t\t\t\tuser-select: none;\n"
		"\t\t\t}\n"
		"\t\t\t.switcher input {\n"
		"\t\t\t\tdisplay: none;\n"
		"\t\t\t}\n"
		"\n"
		"\t\t\t.switcher_indicator::after {\n"
		"\t\t\t\tcontent: '';\n"
		"\t\t\t\tposition: absolute;\n"
		"\t\t\t\ttop: 0;\n"
		"\t\t\t\tleft: 0;\n"
		"\t\t\t\theight: 40px;\n"
		"\t\t\t\twidth: 40px;\n"
		"\t\t\t\tbackground-color: #d5d5d5;\n"
		"\t\t\t\tborder-radius: 50%;\n"
		"\t\t\t\t-webkit-transition: all .3s ease;\n"
		"\t\t\t\ttransition: all .3s ease;\n"
		"\t\t\t\t-webkit-animation-name: pulsein;\n"
		"\t\t\t\t\t\t\t\tanimation-name: pulsein;\n"
		"\t\t\t\t-webkit-animation-duration: .3s;\n"
		"\t\t\t\t\t\t\t\tanimation-duration: .3s;\n"
		"\t\t\t}\n"
		"\t\t\t.switcher_indicator::before {\n"
		"\t\t\t\tcontent: '';\n"
		"\t\t\t\tposition: absolute;\n"
		"\t\t\t\ttop: 16px;\n"
		"\t\t\t\tleft: 0;\n"
		"\t\t\t\twidth: 80px;\n"
		"\t\t\t\theight: 8px;\n"
		"\t\t\t\tbackground-color: #d5d5d5;\n"
		"\t\t\t\tborder-radius: 10px;\n"
		"\t\t\t\t-webkit-transition: all .3s ease;\n"
		"\t\t\t\ttransition: all .3s ease;\n"
		"\t\t\t}\n"
		"\t\t\tinput:checked + .switcher_indicator::after {\n"
		"\t\t\t\tbackground-color: #29ce9a;\n"
		"\t\t\t\t-webkit-transform: translateX(40px);\n"
		"\t\t\t\t\t\t\t\ttransform: translateX(40px);\n"
		"\t\t\t\t-webkit-animation-name: pulseout;\n"
		"\t\t\t\t\t\t\t\tanimation-name: pulseout;\n"
		"\t\t\t\t-webkit-animation-duration: .3s;\n"
		"\t\t\t\t\t\t\t\tanimation-duration: .3s;\n"
		"\t\t\t}\n"
		"\t\t\tinput:checked + .switcher_indicator::before {\n"
		"\t\t\t\tbackground-color: #29ce9a;\n"
		"\t\t\t}\n"
		"\t\t\tinput:disabled + .switcher_indicator::after, input:disabled + .switcher_indicator::before {\n"
		"\t\t\t\tbackground-color: #e5e5e5;\n"
		"\t\t\t}\n"
		"\n"
		"\t\t\t@-webkit-keyframes pulsein {\n"
		"\t\t\t\t0%, 100% {\n"
		"\t\t\t\t\ttop: 0px;\n"
		"\t\t\t\t\theight: 40px;\n"
		"\t\t\t\t\twidth: 40px;\n"
		"\t\t\t\t}\n"
		"\t\t\t\t50% {\n"
		"\t\t\t\t\ttop: 6px;\n"
		"\t\t\t\t\theight: 28px;\n"
		"\t\t\t\t\twidth: 52px;\n"
		"\t\t\t\t}\n"
		"\t\t\t}\n"
		"\n"
		"\t\t\t@keyframes pulsein {\n"
		"\t\t\t\t0%, 100% {\n"
		"\t\t\t\t\ttop: 0px;\n"
		"\t\t\t\t\theight: 40px;\n"
		"\t\t\t\t\twidth: 40px;\n"
		"\t\t\t\t}\n"
		"\t\t\t\t50% {\n"
		"\t\t\t\t\ttop: 6px;\n"
		"\t\t\t\t\theight: 28px;\n"
		"\t\t\t\t\twidth: 52px;\n"
		"\t\t\t\t}\n"
		"\t\t\t}\n"
		"\t\t\t@-webkit-keyframes pulseout {\n"
		"\t\t\t\t0%, 100% {\n"
		"\t\t\t\t\ttop: 0px;\n"
		"\t\t\t\t\theight: 40px;\n"
		"\t\t\t\t\twidth: 40px;\n"
		"\t\t\t\t}\n"
		"\t\t\t\t50% {\n"
		"\t\t\t\t\ttop: 6px;\n"
		"\t\t\t\t\theight: 28px;\n"
		"\t\t\t\t\twidth: 52px;\n"
		"\t\t\t\t}\n"
		"\t\t\t}\n"
		"\t\t\t@keyframes pulseout {\n"
		"\t\t\t\t0%, 100% {\n"
		"\t\t\t\t\ttop: 0px;\n"
		"\t\t\t\t\theight: 40px;\n"
		"\t\t\t\t\twidth: 40px;\n"
		"\t\t\t\t}\n"
		"\t\t\t\t50% {\n"
		"\t\t\t\t\ttop: 6px;\n"
		"\t\t\t\t\theight: 28px;\n"
		"\t\t\t\t\twidth: 52px;\n"
		"\t\t\t\t}\n"
		"\t\t\t}\n"
		"\n"
		"\t\t</style>\n"
		"\t</head>\n"
		"\t<body translate=\"no\" >\t\n"
		"\t\t<div class=\"c_panel\">\n"
		"\t\t\t<div class=\"c_panel_title\">Control panel</div>\n"
		"\t\t\t<div class=\"c_panel_content\">\n"
		"\t\t\t\t<label class=\"switcher\">\n"
		"\t\t\t\t\t<input type=\"checkbox\" name=\"inlet\" onclick=\"handleClick(this)\"/>\n"
		"\t\t\t\t\t<div class=\"switcher_indicator\"></div><span>Inlet</span>\n"
		"\t\t\t\t</label><br/><br/>\n"
		"\t\t\t\t<label class=\"switcher\">\n"
		"\t\t\t\t\t<input type=\"checkbox\" name=\"drain\" onclick=\"handleClick(this)\"/>\n"
		"\t\t\t\t\t<div class=\"switcher_indicator\"></div><span>Drain</span>\n"
		"\t\t\t\t</label><br/><br/>\n"
		"\t\t\t</div>\n"
		"\t\t\t<div class=\"c_panel_title\" id=\"water_level\">0 %</div>\n"
		"\t\t</div>\n"
		"\n"
		"\t\t<script type=\"text/javascript\">\n"
		"\t\t\tfunction updateVals() {\n"
		"\t\t\t\tlet xhttp = new XMLHttpRequest();\n"
		"\t\t\t\txhttp.onreadystatechange = function() {\n"
		"\t\t\t\t\tif (this.readyState == 4 && this.status == 200) {\n"
		"\t\t\t\t\t\tvar waterLevel = parseInt(this.responseText);\n"
		"\t\t\t\t\t\tvar waterText = document.getElementById(\"water_level\");\n"
		"\t\t\t\t\t\twaterText.innerHTML = waterLevel + \" %\";\n"
		"\t\t\t\t\t}\n"
		"\t\t\t\t}\n"
		"\t\t\t\txhttp.open(\"GET\", \"getReading\", true);\n"
		"\t\t\t\txhttp.send();\n"
		"\t\t\t}\n"
		"\n"
		"\n"
		"\t\t\tfunction handleClick(cb) {\n"
		"\t\t\t\tif(cb.checked) {\n"
		"\t\t\t\t\tthis.path = cb.name + \"On\";\n"
		"\t\t\t\t}\n"
		"\t\t\t\telse {\n"
		"\t\t\t\t\tthis.path = cb.name + \"Off\";\n"
		"\t\t\t\t}\n"
		"\t\t\t\tlet xhttp = new XMLHttpRequest();\n"
		"\t\t\t\txhttp.open(\"GET\", path, true);\n"
		"\t\t\t\txhttp.send();\n"
		"\t\t\t}\n"
		"\n"
		"\t\t\tsetInterval(updateVals, 2000);\n"
		"\t\t</script>\n"
		"\t</body>\n"
		"</html>"
	);
}

void handeleInletOn() {
	motorControl(inletPin, HIGH);
	server.send(200, "text/html", "Inlet turned on");
}

void handeleInletOff() {
	motorControl(inletPin, LOW);
	server.send(200, "text/html", "Inlet turned off");
}

void handeleDrainOn() {
	motorControl(drainPin, HIGH);
	server.send(200, "text/html", "Drain turned on");
}

void handeleDrainOff() {
	motorControl(drainPin, LOW);
	server.send(200, "text/html", "Drain turned off");
}

void handeleGetReading() {
	server.send(200, "text/html", String(reading));
}

void setupWiFi(){
	Serial.print("\nConnecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
}

void setup() {

	pinMode(echoPin, INPUT);
	pinMode(trigPin, OUTPUT);
	pinMode(inletPin, OUTPUT);
	pinMode(drainPin, OUTPUT);

	Serial.begin(115200);
	delay(1000);

	//Serial.print("Configuring access point...");
	//WiFi.softAP(ssid);
	//WiFi.softAP(ssid, password);
	setupWiFi();
	IPAddress myIP = WiFi.localIP();
	Serial.println(myIP);

	server.on("/", handleRoot);
	server.on("/inletOn", handeleInletOn);
	server.on("/inletOff", handeleInletOff);
	server.on("/drainOn", handeleDrainOn);
	server.on("/drainOff", handeleDrainOff);
	server.on("/getReading", handeleGetReading);

	server.begin();
	Serial.println("HTTP server started");

}

void loop() {
	server.handleClient();
	reading = readWaterLevel(trigPin, echoPin);
	Serial.println("Water: "+ reading);
	if (reading < minWater) {
		motorControl(inletPin, HIGH);
	}
	else {
		motorControl(inletPin, LOW);
	}
}
