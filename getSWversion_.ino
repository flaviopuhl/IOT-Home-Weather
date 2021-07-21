void getSWversion(){
                                                  
  ino = (ino.substring((ino.indexOf(".")), (ino.lastIndexOf("\\")) + 1));   // Must be declared as global
  
  Serial.println(F(" "));
  Serial.println(F("################ SW Version ################\n"));
  Serial.print(F("Software Version  : ")); Serial.println(ino);
}
