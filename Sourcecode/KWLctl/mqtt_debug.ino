void mqtt_debug_fan1() {
  String out = "";
  out =  "Fan1 - M: ";
  out += millis();
  out += ", Gap1: ";
  out += abs(speedSetpointFan1 - speedTachoFan1);
  out += ", tf1ts: ";
  out += tachoFan1TimeSum;
  out += ", tsf1: ";
  out += techSetpointFan1;
  out += ", ssf1: ";
  out += speedSetpointFan1;
  char _buffer[out.length()];
  out.toCharArray(_buffer,out.length()); 
  mqttClient.publish(TOPICKwlDebugstateFan1, _buffer);
}

void mqtt_debug_fan2() {
  String out = "";
  out =  "Fan2 - M: ";
  out += millis();
  out += ", Gap2: ";
  out += abs(speedSetpointFan2 - speedTachoFan2);
  out += ", tf2ts: ";
  out += tachoFan2TimeSum;
  out += ", tsf2: ";
  out += techSetpointFan2;
  out += ", ssf2: ";
  out += speedSetpointFan2;
  char _buffer[out.length()];
  out.toCharArray(_buffer,out.length()); 
  mqttClient.publish(TOPICKwlDebugstateFan2, _buffer);
}

