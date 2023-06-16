void servo() {

  keyPosition = keypad_object.getKey();
  while(passwordCount == 3){ 
     server.handleClient();                       // Generate a response to the web client
     delay(2);                                    //allow the cpu to switch to other tasks

     attachInterrupt(SENSOR, Interrupt, HIGH);    // Interrupt to detect a specific change in the PIR sensor pin
     servoMotor.attach(12);
     servoMotor.write(98);                        // Turn counter-clockwise at slow speed
     keyPosition = keypad_object.getKey();
     Serial.println(keyPosition);
     character = keyPosition;
      
     // Deactivate the system
     if(character == 'A'){
       Serial.println("System Deactivated");
       passwordCount = 0;
       servoMotor.detach();
       detachInterrupt(SENSOR);                   // Disable the interrupt function
       digitalWrite(LED, LOW);                   
       digitalWrite(BUZZER, LOW);
     }
     server.handleClient();
     delay(2);
     delay(3000);
     servoMotor.write(95);                        // Stop the servo for 2 seconds
     delay(2000);
     server.handleClient();
     delay(2);
     servoMotor.write(94);                        // Turn clockwise at slow speed
     keyPosition = keypad_object.getKey();
     Serial.println(keyPosition);
     character = keyPosition;
        
     if(character == 'A'){
       Serial.println("System Deactivated");
       passwordCount = 0;
       servoMotor.detach();
       detachInterrupt(SENSOR);
       digitalWrite(LED, LOW);                   
       digitalWrite(BUZZER, LOW);
     }
     server.handleClient();
     delay(2);
     delay(2100);
     servoMotor.write(95);
     delay(2000);
     server.handleClient();
     delay(2);
  }    
}
