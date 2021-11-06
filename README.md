# Projeto 3

## Introdução

O objetivo deste trabalho é criar um sistema distribuído de automação predial utilizando um computador (PC) como sistema computacional central e microcontroladores ESP32 como dispositivos distribuídos, interconectados via Wifi através do protocolo MQTT.

A matrícula utilizada para o MQTT foi: 0461.

## Informações dos alunos

|Nome|Matrícula|
|----|---------|
|Calebe Rios de Sousa Mendes|15/0120702|
|Pedro Henrique Andrade Féo|17/0020461|

## Como rodar

- Linguagem: C
- Framework: ESP-IDF v4.4-dev-3401-gb86fe0c66c-dirty

Para configurar a rede wifi e o low power, você pode alterar tanto no arquivo `Kconfig.projbuild` ou através do `menuconfig` com o seguinte comando:

```
idf.py menuconfig
```

PS: se for abrir pelo `menuconfig`, é necessário ir em configurações do projeto.

Para buildar o projeto, é necessário utilizar o seguinte comando:

```
idf.py build
```

Após o build, é necessário o seguinte comando para dar flash no projeto e também já abrir o monitor da ESP:

```
idf.py -p /dev/ttyUSB0 flash monitor
```

Caso deseje apagar a memoria flash, é só rodar o seguinte comando:

```
idf.py erase_flash
```

## Como funciona

O botão sinalizado como EN, é utilizado pela ESP para fazer o reboot do sistema. Já o botão Boot, é utilizado para capturar o clique da entrada do dispositivo. A partir desse botão, também é possível resetar a ESP segurando por 3 segundos.

Quando a ESP não está no modo Low Power, ela necessita de um módulo DHT11 conectado a ESP para a medição da temperatura e umidade.

### Configuração do Menuconfig

Para adicionar a rede do wifi que a esp32 precisa se conectar, é necessário alterar as variáveis `ESP_WIFI_SSID` e `ESP_WIFI_PASSWORD` dentro do arquivo `Kconfig.projbuild`, que se encontra na pasta `main`.

Também dentro do arquivo `Kconfig.projbuild`, é possível alternar entre o modo low power. Ao alterar a variável `LOW_POWER` para 1, o modo de low power será ativado, no caso do 0 esse modo será desativado. PS: O modo de low power por padrão está desligado.

PS2: Caso o arquivo `sdkconfig` exista na raiz, será necessário remover ele antes do build.

## [![Video](https://user-images.githubusercontent.com/1852287/140622258-79d2607c-c58a-4316-9292-f4e396154f5d.png)](https://drive.google.com/file/d/19oRn-CUab0WhnaKUjsfuh0-YBiOEQMZ6/view?usp=sharing)
