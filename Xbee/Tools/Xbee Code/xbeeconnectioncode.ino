

void setup( ) {
  Serial.begin( 9600 );
  Serial1.begin( 9600 );

  Serial.println( "Ready!" );
  
  delay( 500 );

  Serial1.write( "XBee 1 says it's ready!" );
  
}

void loop( ) {
  if( Serial1.available( ) ) {
    /* If data comes in from XBee, send it out to serial monitor */
    Serial.write( Serial1.read( ));
  }
  Serial1.write( "XBee 1 says it's ready!" );
  delay(500);
}