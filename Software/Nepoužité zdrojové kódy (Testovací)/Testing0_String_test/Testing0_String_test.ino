// Hodne dlouhy textovy retezec o delce 169 znaku
String objekt_string = "Hodne drahou chybu udelal jeden muz z cinskeho mesta Charbin. Chtel zlikvidovat plastove prepravky, vlastni nepozornosti si vsak nevsiml, ze v jedne ma ulozene bankovky.";

void setup(){
  // Zahajeni komunikace
  Serial.begin(9600);  
}
 
void loop(){
  // Zjisteni velikosti stringu
  int velikost_stringu = objekt_string.length();

  // Vypis do konzole
  Serial.println(objekt_string);
  Serial.println("Velikost puvodniho textu: 169 znaku");
  Serial.print("Velikost objektu stringu: ");
  Serial.print(velikost_stringu);
  Serial.println(" znaku");

  // Konec programu
  while(1);  
}
