/**
 * @file test_sys_resource.c
 * @author Dmitri Lyalikov (Dlyalikov01@manhattan.edu)
 * @brief Unit Tests for System Resources
 * Functions to test:
 *    vQueueInit()
 *    vUpdateQueue()
 *    VReadQueue()
 * 
 * @version 0.1
 * @date 2022-11-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include "unity.h"
#include "sys_resource.h"

/**
 * @brief Test Initialize Queue of size xPayload_t 
 *     Verify the queue we intialize is of length xPayload_t and width of one unit
 * 
 */
TEST_CASE("Queue_Init", "[args]")
{
    QueueHandle_t xTimeSyncQueue;
    xTimeSyncQueue = vQueueInit();

    TEST_ASSERT_EQUAL(sizeof(xTimeSyncQueue()), sizeof(xPayload_t));
}

/**
 * @brief Test Queue Update utility 
 * 
 */
TEST_CASE("Queue_Update_Ok", "[args]")
{

    TEST_ASSERT_EQUAL(0, 0);
}

TEST_CASE("Queue_Read_Ok", "[args]")
{
    
    TEST_ASSERT_EQUAL(0, 0);
}

/**
 * @brief Read Empty Queue
 *  My understanding is that xQueuePeek() will block until the queue is full,
 */
TEST_CASE("Queue_Read_Empty", "[args]")
{
    
}
