# Trabalho 3 - 2022-2

Trabalho 3 da disciplina de Fundamentos de Sistemas Embarcados (2022/2)

Para acessar o projeto, clique [aqui](https://gitlab.com/fse_fga/trabalhos-2022_2/trabalho-final-2022-2).

## Objetivo - Carrinho de controle remoto.

A ideia deste trabalho é projetar um carrinho que será controlado remotamente usando um Joystick através da plataforma Thingsboard, além disso, foram usados sensores e atuadores distribuídos baseados nos microcontroladores ESP32 interconectados via Wifi através do protocolo MQTT.


## Alunos

| Matrícula  | Aluno                              |
| ---------- | ---------------------------------- |
| 19/0111836 | Luan Vasco Cavalcante              |
| 14/0156909 | Nathalia Lorena Cardoso Dias       |
| 19/0117401 | Thalisson Alves Gonçalves de Jesus |

## Requisitos

O programa foi desenvolvido para ser utilizado em uma esp32doit-devkit-v1.

Foi desenvolvido com a extensão do Visual Studio Code Platform IO - https://platformio.org/


## Instalação

Baixe o repositório e acesse a pasta.

`git clone https://github.com/Luan-Cavalcante/Trabalho-Final-FSE`

`cd Trabalho-Final-FSE`

Para fazer a build do programa, carregar o programa na ESP32 e monitorar o programa foi utilizado como referencia o tutorial [Get Started](https://docs.platformio.org/en/latest/tutorials/espressif32/arduino_debugging_unit_testing.html).

## Sensores

 - [DHT11](https://www.filipeflop.com/produto/sensor-de-umidade-e-temperatura-dht11/)
 - [Buzzer](https://blogmasterwalkershop.com.br/arduino/como-usar-com-arduino-buzzer-5v-ativo)
 - [Joystick KY-023](https://blogmasterwalkershop.com.br/arduino/como-usar-com-arduino-modulo-joystick-ky-023)
 - [Sensor Infravermelho Reflexivo de Obstáculo KY-032](https://blogmasterwalkershop.com.br/arduino/como-usar-com-arduino-modulo-sensor-infravermelho-reflexivo-de-obstaculo-ky-032)
 - [Sensor de Luminosidade LDR](https://www.blogdarobotica.com/2020/09/29/utilizando-o-sensor-de-luminosidade-ldr-no-arduino/)

## Uso

Faça o build e em seguida o upload no dispositivo.

Ao iniciar o programa, navegue pelo dashboard para visualizar e controlar as funcionalidades implementadas.


## Vídeo

[Apresentação](https://youtu.be/xF1mK7a11ss)

## Imagens

O Dashboard ficou com as funções listadas na imagem abaixo : 

![image](https://user-images.githubusercontent.com/67024690/218892946-2e5a37ab-f92a-41cd-95de-c3e69a0a9d66.png)

Onde, os widgets de direção (seta pra cima,baixo, esquerda e direita) e o botão da buzina foram criados pelos alunos.

O Carrinho pronto está na imagem abaixo : 

![image](https://user-images.githubusercontent.com/67024690/218897079-6de74383-a7f7-45f4-91c3-9c64762dbba9.png)

## Problemas

 - O Carrinho ficou muito pesado.
 - Os motores estavam sem redução.

## Melhorias

 - Deixar o carrinho mais leve.
 - Usar motores com redução.

## Agradecimentos 

Agradecemos ao Professor Renato Sampaio pela dedicação durante o semestre e pelas ótimas aulas ministradas para a turma de Fundamentos de Sistemas Embarcados.
