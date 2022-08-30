/* ============================================================================
   
   Controle de Relés por WiFi com ESP32

   Vídeo Aula de Engenharia Eletrônica, a Clássica de Sexta!
   Aula número 369

   Compilador: Arduino IDE 1.8.4
   Autor: Eng. Wagner Rambo
   Março de 2021

============================================================================ */


// ============================================================================
// --- Bibliotecas Auxiliares ---
#include <Arduino.h>                      //inclui biblioteca Arduino
#include <WiFi.h>                         //inclui biblioteca WiFi


// ============================================================================
// --- Mapeamento de Hardware ---
#define   relay1   21                    //pino para controle do relé 1
#define   relay2   22                    //pino para controle do relé 2
#define   led13    13                    //pino para controle do LED 1
#define   ledESP32 2                     //pino para controle do LED 2 - led integrado na placa ESP32 (azul)
// ============================================================================
// --- Constantes Auxialires ---
const char *ssid     = "REDE";  //atribuir nome da rede WiFi
const char *password = "SENHA"; //atribuir senha da rede


// ============================================================================
// --- Objetos ---
WiFiServer server(80);                    //define a porta que o servidor irá utilizar
                                          //(normalmente utiliza-se a porta padrão 80)

// ============================================================================
// --- Protótipo das Funções ---
int Led13ONN_OFF();
                                

// ============================================================================
// --- Configurações Iniciais ---
void setup(){
  Serial.begin(9600);                    //inicializa Serial em 115200 baud rate
  pinMode(relay1, OUTPUT);               //configura saída para relé 1
  pinMode(relay2, OUTPUT);               //configura saída para relé 2
  pinMode(led13, OUTPUT);                //configura saída para LED 13
  pinMode(ledESP32, OUTPUT);             //configura saída para LED integrado na placa ESP32 (azul)
  digitalWrite(led13, LOW);              //desliga LED 13
  digitalWrite(ledESP32, HIGH);          //liga LED integrado na placa ESP32 (azul)
  Serial.println();                      //
  Serial.print("Conectando-se a ");      //
  Serial.println(ssid);                  //
  WiFi.begin(ssid, password);            //inicializa WiFi, passando o nome da rede e a senha

  while(WiFi.status() != WL_CONNECTED){  //aguarda conexão (WL_CONNECTED é uma constante que indica sucesso na conexão)
    delay(1000);                          //
    Serial.print(".");                   //vai imprimindo pontos até realizar a conexão...
  }

  Serial.println("");                    //mostra WiFi conectada
  Serial.println("WiFi conectada");      //
  Serial.println("Endereço de IP: ");    //
  Serial.println(WiFi.localIP());        //mostra o endereço IP
  digitalWrite(ledESP32, LOW);           //desliga LED integrado na placa ESP32 (azul)
  server.begin();                        //inicializa o servidor web

}


// ============================================================================
// --- Loop Infinito ---
void loop(){
  //verifica se existe um cliente conectado com dados a serem transmitidos
  WiFiClient client = server.available();       
  
  if(client){                                   //existe um cliente?
    digitalWrite(ledESP32, HIGH);               //liga LED integrado na placa ESP32 (azul)
    Serial.println("Novo Cliente Definido");    //informa por serial
    String currentLine = "";                    //string para armazenar a mensagem do cliente
    while(client.connected()){                  //repete enquanto o cliente estiver conectado
      if(client.available()){                   //existem dados a serem lidos?
        char c = client.read();                 //salva em c
        Serial.write(c);                        //mostra na Serial
        
        if(c == '\n'){                            //é um caractere de nova linha?
          if(currentLine.length()==0){          //a mensagem terminou?
            int aux = digitalRead(led13);
            //gera a página HTML
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            
            // configuração de estilo do site
            client.print("<style type=\"text/css\"> a{margin: 50px 50px; background: #0000ff; color: #ffffff; text-decoration: none; padding: 1% 20% 1%; border-radius: 10px; font-size: 8.0em;}</style>");
            // o conteúdo do cabeçalho HTTP
            client.print("<a> LED 13 </a>  </br></br></br>");
            if(aux == HIGH) client.print("<a href=\"/T2\"> HIGH </a> </br></br></br>");
            else client.print("<a href=\"/T2\"> LOW </a> </br></br></br>");
            // A resposta HTTP termina com outra linha em branco:
            client.println();
            /*
            client.print("<h2>Clique <a href=\"/H1\">AQUI</a> para ligar o rele 1.</h2><br>");
            client.print("<h2>Clique <a href=\"/L1\">AQUI</a> para desligar o rele 1.</h2><br>");
            client.print("<h2>Clique <a href=\"/H2\">AQUI</a> para ligar o rele 2.</h2><br>");
            client.print("<h2>Clique <a href=\"/L2\">AQUI</a> para desligar o rele 2.</h2><br>");
            client.println();*/
            break;                                //encerra o laço
            
          } //end if currentLine.length
          
          else currentLine = "";                 //senão, impede string de ficar com espaços em branco
        
        } //end if c
        //adicionamos o caractere como parte da mensagem, se diferene de retorno/nova linha
        else if(c != '\r') currentLine += c;    

        //verifica para ligar ou desligar o led 13
        if(currentLine.endsWith("GET /T2")) {
          Led13ONN_OFF();
        }
          
      } //end if client.available()
      
    } //end if while client.connected

    client.stop();                           //finaliza conexão
    Serial.println("Client Disconnected.");
    digitalWrite(ledESP32, LOW);                //desliga o LED 13
  }   //end if client 
}     //end loop

// Função para ligar o LED 13
int Led13ONN_OFF(){
  if(digitalRead(led13)) digitalWrite(led13, LOW);
  else digitalWrite(led13, HIGH);
  return 0;
}

// ======================================================== //
                // --- Final do Programa --- //
