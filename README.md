# Projeto: Controle de LED e Botão com FreeRTOS no Raspberry Pi Pico W <img src="https://github.com/user-attachments/assets/af1e5dbf-42ec-4140-b3d5-53439a8c0f5d" width="40"></img>

## Descrição <img src="https://github.com/user-attachments/assets/c055db6c-9033-4a05-bc4d-dd736626a6ef" width="30"></img>
Este projeto é desenvolvido em C para o Raspberry Pi Pico W e utiliza o sistema operacional em tempo real FreeRTOS. Ele consiste em três tasks (tarefas) principais:

1. Task de Monitoramento do Botão: Detecta o estado do botão conectado ao pino 5.
2. Task de Processamento do Estado do Botão: Processa o estado do botão e envia notificações para a task que controla o LED.
3. Task de Controle do LED: Aciona ou desliga o LED conectado ao pino 12, de acordo com as notificações recebidas da task de processamento.

## Como Funciona <img src="https://github.com/user-attachments/assets/2f0144fd-d8a1-4fba-8d1a-95be451c9e22" width="30"></img>
- Quando o botão é pressionado, a task de monitoramento atualiza a variável button_state.
- A task de processamento verifica periodicamente o estado do botão e notifica a task do LED se o botão estiver pressionado.
- A task do LED acende o LED por um curto intervalo de tempo ao receber a notificação.

## Estrutura do Código <img src="https://github.com/user-attachments/assets/59ce126f-72d6-43ad-8381-2ced4f2ac2ad" width="30"></img>
### Importações e Pinagem <img src="https://github.com/user-attachments/assets/b7bb9574-715f-4e4b-8275-4dcd1b475160" width="30"></img>
```c
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

const uint led_pin = 12;
#define button_pin 5
int button_state = 0;
```

### Tasks <img src="https://github.com/user-attachments/assets/355c0819-acc0-46db-be53-b0d3a269facb" width="30"></img>
- Task para Monitorar o Botão:
```c
void vButtonTask() {
    for (;;) {
        if (!gpio_get(button_pin)) {
            button_state = 1;
        } else {
            button_state = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

- Task para Processamento do Estado Botão:
```c
void vProcessTask(void *pvParameters) {
    TaskHandle_t *vLedTask_Handle = (TaskHandle_t *)pvParameters;
    for (;;) {
        if (button_state == 1) {
            xTaskNotifyGive(*vLedTask_Handle);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

- Task para Controle do LED:
```c
void vLedTask(void *pvParameters) {
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        gpio_put(led_pin, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_put(led_pin, 0);
        vTaskDelay(100);
    }
}
```

- Função Main (*Função principal*):
```c
int main(void) {
    TaskHandle_t vLedTask_Handle = NULL;

    stdio_init_all();
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    gpio_init(button_pin);
    gpio_set_dir(button_pin, GPIO_IN);
    gpio_pull_up(button_pin);

    xTaskCreate(vButtonTask, "Button Task", 128, NULL, 2, NULL);
    xTaskCreate(vLedTask, "Led Task", 128, NULL, 1, &vLedTask_Handle);
    xTaskCreate(vProcessTask, "Process Task", 128, (void*)&vLedTask_Handle, 1, NULL);

    vTaskStartScheduler();
    return 0;
}
```

## Importante: Configuração da variável de Ambiente do FreeRTOS <img src="https://github.com/user-attachments/assets/ae072ec4-675e-440b-8ded-44cf31269c3a" width="30"></img>
Para que o projeto funcione corretamente, é necessário configurar a variável de ambiente do FreeRTOS no seu sistema. Neste projeto, o FreeRTOS não está incluído diretamente. Portanto, se o projeto for baixado e rodado em outro computador, haverá uma incompatibilidade devido ao caminho da variável de ambiente. Certifique-se de:

1. Baixar o FreeRTOS
2. Configurar a variável de ambiente `FREERTOS_PATH` para apontar para o diretório do FreeRTOS

### Linux <img src="https://github.com/user-attachments/assets/5d0af694-a4e2-405a-b0b1-1f5f25b9ebaf" width="30"></img>
```bash
export FREERTOS_PATH=/caminho/para/FreeRTOS
```

### Windows (PowerShell) <img src="https://github.com/user-attachments/assets/a91474dd-6a35-4e30-8534-a2f1aa1123b8" width="30"></img>
```bash
$env:FREERTOS_PATH = "C:\caminho\para\FreeRTOS"
```
Após o download do FreeRTOS e devida configuração da variável de ambiente `FREERTOS_PATH`, o código está pronto para testes! 

