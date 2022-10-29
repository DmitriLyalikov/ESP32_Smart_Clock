#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/**
 * @brief Unit of transfer between queues
 * 
 * xTimeStamp: Timestamp 
 * ulValue: uint16_t payload data
 */
typedef struct xPayload_t
{
    TickType_t xTimeStamp;
    uint16_t ulValue;
}xPayload_t;

/**
 * @brief Initialize Mailbox Queue
 * @returns QueueHandle_T: Handle to initialize queue  
 */
QueueHandle_t vQueueInit(void);

/**
 * @brief Write a value to queue
 * 
 * @param Queue : queue handle of type QueueHandle_t
 * @param ulNewValue : uin16_t value to write
 */
void vUpdateQueue(QueueHandle_t Queue, uint16_t ulNewValue);

/**
 * @brief Read queue and pop value read, 
 * 
 * @param pxData : Pointer to struct of type xPayload_t to read into
 * @param Queue  : Queue handle of type QueueHandle_t to read from
 */
void vReadQueue(xPayload_t *pxData, QueueHandle_t Queue);

