void setup() {
	int channel = 0;

	ledcSetup(channel, 1000, 8);
	ledcAttachPin(27, channel); // pin 27 == buzzer

	ledcWriteTone(channel, 1000);
	delay(500);
	ledcWriteTone(channel, 0);
	delay(2000);
}

void loop() {

}
