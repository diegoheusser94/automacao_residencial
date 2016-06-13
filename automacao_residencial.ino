//Programa: Automacao Residencial com Arduino
 
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

IPAddress ip(10, 1, 1, 111);
IPAddress gateway(192,168,122,1);
IPAddress subnet(255,255,255,0);
EthernetServer server(80);

int r1, r2;// Portas dos relés
boolean s1, s2; // Status dos relés


void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  r1 = 3;
  r2 = 4;
  s1 = false;
  s2 = false;
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.print("Servidor em ");
  Serial.println(Ethernet.localIP());
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);

}


void loop()
{
  
  EthernetClient client = server.available();
  if(client){
    boolean cabecalho = true;
    boolean corpo = false;
    String param = "";
    String json = "";
    char caracterAnterior = ' ';
    char c = ' ';
    while(client.connected()){
      if(client.available()){
          c = client.read();
          if(cabecalho){ // Grava o cabeçalho da requisição
            param += c;
            if(caracterAnterior == '\n' && c == '\r'){ // Verifica fim do cabeçalho
              corpo = true;
              cabecalho = false;
              Serial.println("Cabecalho:");
              Serial.print(param);
            }
          }
          if(corpo){ // Grava o conteúdo da requisição
            json += c;
            if(caracterAnterior == '}' && c == '}'){ // Verifica fim do JSON
              corpo = false;
              Serial.println("Corpo:");
              Serial.print(json);
            }
          }
          if(!cabecalho && !corpo){
            String rele = json.substring(22,23);// Posição do numero do relé
            String status = json.substring(33,37); // Posição do status do relé
            if(status == "true"){
              digitalWrite(rele.toInt(),LOW); // Liga o relé
            } else {
              digitalWrite(rele.toInt(),HIGH); // Desliga o relé
            }
            String resposta = "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\n\r";
            Serial.println("Resposta:");
            Serial.print(resposta);
            client.println(resposta);
            break;
          }
      }
      caracterAnterior = c;
    }
  }
  delay(1);
  client.stop();
}
