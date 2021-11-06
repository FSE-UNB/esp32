# Projeto 3

## Introdução



## Informações dos alunos

|Nome|Matrícula|
|----|---------|
|Calebe Rios de Sousa Mendes|15/0120702|
|Pedro Henrique Andrade Féo|17/0020461|

## Como rodar

- Linguagem: C
- Framework: ESP-IDF

## Como funciona

### Configuração do Menuconfig

Para adicionar a rede do wifi que a esp32 precisa se conectar, é necessário alterar as variáveis `ESP_WIFI_SSID` e `ESP_WIFI_PASSWORD` dentro do arquivo `Kconfig.projbuild`, que se encontra na pasta `main`.

Também dentro do arquivo `Kconfig.projbuild`, é possível alternar entre o modo low power. Ao alterar a variável `LOW_POWER` para 1, o modo de low power será ativado, no caso do 0 esse modo será desativado. PS: O modo de low power por padrão está desligado.

PS2: Caso o arquivo `sdkconfig` exista na raiz, será necessário remover ele antes do build.

## Mais informações


