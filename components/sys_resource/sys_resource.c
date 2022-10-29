#include "sys_resource.h"

/**
 * @brief Initialize Mailbox Queue
 * @returns QueueHandle_T: Handle to initialize queue  
 */
QueueHandle_t vQueueInit(void)
{   
    return xQueueCreate(1, sizeof(xPayload_t));
}

/**
 * @brief Write a value to queue
 * 
 * @param Queue : queue handle of type QueueHandle_t
 * @param ulNewValue : uin16_t value to write
 */
void vUpdateQueue(QueueHandle_t Queue, uint16_t ulNewValue)
{
    xPayload_t xData;
    xData.ulValue = ulNewValue;
    xData.xTimeStamp = pdTICKS_TO_MS(xTaskGetTickCount());
    // xSemaphoreTake(xQueueMutex, pdMS_TO_TICKS(1000));
    xQueueOverwrite(Queue, &xData);
    // xSemaphoreGive(xQueueMutex);
}

/**
 * @brief Read queue and pop value read, 
 * 
 * @param pxData : Pointer to struct of type xPayload_t to read into
 * @param Queue  : Queue handle of type QueueHandle_t to read from
 */
void vReadQueue(xPayload_t *pxData, QueueHandle_t Queue)
{
    xQueuePeek(Queue, pxData, portMAX_DELAY);
}