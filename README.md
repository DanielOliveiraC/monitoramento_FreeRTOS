# Projeto: Controle de LED e Botão com FreeRTOS no Raspberry Pi Pico W <img src="https://github.com/user-attachments/assets/c027dd6c-cd57-463e-88f1-4452e381bce7" width="40"></img>

## Descrição <img src="https://github.com/user-attachments/assets/1c825e92-2f29-4899-93bf-88f1165aee8f" width="30"></img>
Este projeto é desenvolvido em C para o Raspberry Pi Pico W e utiliza o sistema operacional em tempo real FreeRTOS. Ele consiste em três tasks (tarefas) principais:

1. Task de Monitoramento do Botão: Detecta o estado do botão conectado ao pino 5.
2. Task de Processamento do Estado do Botão: Processa o estado do botão e envia notificações para a task que controla o LED.
3. Task de Controle do LED: Aciona ou desliga o LED conectado ao pino 12, de acordo com as notificações recebidas da task de processamento.

## Como Funciona <img src="https://github.com/user-attachments/assets/a9b23893-9302-45de-9995-5cb7a7b832d5" width="30"></img>
- Quando o botão é pressionado, a task de monitoramento atualiza a variável button_state.
- A task de processamento verifica periodicamente o estado do botão e notifica a task do LED se o botão estiver pressionado.
- A task do LED acende o LED por um curto intervalo de tempo ao receber a notificação.

## Estrutura do Código <img src="https://github.com/user-attachments/assets/124b6345-6e3a-4966-8ba8-db6c3feb9ba8" width="30"></img>
### Importações e Pinagem <img src="https://github.com/user-attachments/assets/76f0624e-6cfa-4d70-8661-9caa7ed44352" width="30"></img>
```
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

const uint led_pin = 12;
#define button_pin 5
int button_state = 0;
```

### Tasks <img src="https://github.com/user-attachments/assets/3bfeb98b-607c-4d42-958e-3e7f5a03b855" width="30"></img>
- Task para Monitorar o Botão:
```
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
```
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
```
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
```
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

## Importante: Configuração da variável de Ambiente do FreeRTOS <img src="https://github.com/user-attachments/assets/6e4e6bd3-4c88-4639-b608-8d6e933c0976" width="30"></img>
Para que o projeto funcione corretamente, é necessário configurar a variável de ambiente do FreeRTOS no seu sistema. Neste projeto, o FreeRTOS não está incluído diretamente. Portanto, se o projeto for baixado e rodado em outro computador, haverá uma incompatibilidade devido ao caminho da variável de ambiente. Certifique-se de:

1. Baixar o FreeRTOS
2. Configurar a variável de ambiente `FREERTOS_PATH` para apontar para o diretório do FreeRTOS

### Linux <img src="https://github.com/user-attachments/assets/1ea71d74-4d29-4217-85ed-f6864acf2470" width="30"></img>
```
export FREERTOS_PATH=/caminho/para/FreeRTOS
```

### Windows (PowerShell) <img src="https://github.com/user-attachments/assets/1f75261c-4d7b-4874-8051-2d0162d6fe3b" width="30"></img>
```
$env:FREERTOS_PATH = "C:\caminho\para\FreeRTOS"
```
Após o download do FreeRTOS e devida configuração da variável de ambiente `FREERTOS_PATH`, o código está pronto para testes! 

