#include <Arduino.h>

#include<FreeRTOSConfig.h>
//telemetry-> staeliite sending the data, command-> ground system sending instructions to the staellite

// Task Handles 
TaskHandle_t commandTaskHandle = NULL;
TaskHandle_t telemetryTaskHandle = NULL;
TaskHandle_t downlinkTaskHandle = NULL;
TaskHandle_t monitorTaskHandle = NULL;
//queue handles
QueueHandle_t telemetryQueue;
//Event group handle
EventGroupHandle_t systemEventGroup;
// Semaphore 
SemaphoreHandle_t serialMutex;


#define DOWNLINK_DELAY    1500
#define MONITOR_DELAY     2000
#define TelemetryQueueSize 10
#define BIT_NORMAL_MODE (1<<0)  //00000001
#define BIT_SAFE_MODE (1<<1) //00000010
#define BIT_TELEMETRY_OK (1<<2)
#define BIT_DOWNLINK_OK (1<<3)
#define BIT_FAULT_STATE (1<<4)

struct telemetryPacket{
uint16_t apid; // appliaction id, the packet is cmg from where
uint16_t sequence;// for ordering the packets, incremented with each new packet
uint32_t timestamp;// when was the data recorded
uint16_t sensorData; // data from the sensor
uint16_t checksum; //for error checking
};

void CommandTask(void *pvParameters) {
    
    while (1) {
        if(xEventGroupGetBits(systemEventGroup)&BIT_FAULT_STATE){

            xSemaphoreTake(serialMutex,portMAX_DELAY);
            Serial.printf("[COMMAND] System in FAULT STATE, attempting recovery...\n");
            xSemaphoreGive(serialMutex);

            vTaskDelay(pdMS_TO_TICKS(5000)); // giving time for recovery
             continue; // skip the rest of the loop and check the state again
        }

        EventBits_t currentBits = xEventGroupGetBits(systemEventGroup);
        // toggling between normal and safe mode for simulating commands from ground station
        if(currentBits & BIT_NORMAL_MODE){
            xSemaphoreTake(serialMutex,portMAX_DELAY);
            Serial.printf("[COMMAND] Switching to SAFE MODE\n");
            xSemaphoreGive(serialMutex);

            xEventGroupClearBits(systemEventGroup, BIT_NORMAL_MODE);
            xEventGroupSetBits(systemEventGroup, BIT_SAFE_MODE);
        }
        else if (currentBits & BIT_SAFE_MODE){
            xSemaphoreTake(serialMutex,portMAX_DELAY);
            Serial.printf("[COMMAND] Switching to NORMAL MODE\n");
            xSemaphoreGive(serialMutex);
            xEventGroupClearBits(systemEventGroup, BIT_SAFE_MODE);
            xEventGroupSetBits(systemEventGroup, BIT_NORMAL_MODE);
        }
        else {
        xSemaphoreTake(serialMutex,portMAX_DELAY);
        Serial.printf("[COMMAND] Invalid state, resetting to NORMAL\n");
        xSemaphoreGive(serialMutex);
        xEventGroupSetBits(systemEventGroup, BIT_NORMAL_MODE);
        }

        vTaskDelay(pdMS_TO_TICKS(5000));  

    }
}

void TelemetryTask(void *pvParameters) {    
    struct telemetryPacket telPacket;
    telPacket.apid=1; //source identifier, which subsystem generated the data
    telPacket.sequence=0; //incremented with each new packet, for ordering and tracking
    while (1) {
        if(xEventGroupGetBits(systemEventGroup)&BIT_FAULT_STATE){
            xSemaphoreTake(serialMutex,portMAX_DELAY);
            Serial.printf("[TELEMETRY]  FAULT MODE\n");
            xSemaphoreGive(serialMutex);
            vTaskDelay(pdMS_TO_TICKS(5000));// giving time for recovery
            continue;
        }

        EventBits_t currentBits=xEventGroupGetBits(systemEventGroup);
       if(currentBits & BIT_SAFE_MODE){
        xSemaphoreTake(serialMutex,portMAX_DELAY);
        Serial.printf("[TELEMETRY] In SAFE MODE\n");   
        xSemaphoreGive(serialMutex);    
       }
       else{
        xSemaphoreTake(serialMutex,portMAX_DELAY);
        Serial.printf("[TELEMETRY] In NORMAL MODE\n");         
        xSemaphoreGive(serialMutex);
       }
       
        telPacket.timestamp=millis();
        telPacket.sensorData=analogRead(A0);    
        telPacket.sequence++; //increment sequence number for each new packet
        telPacket.checksum=(telPacket.apid ^ telPacket.sensorData ^ (telPacket.timestamp&0xFFFF)); 
        
        if(xQueueSend(telemetryQueue,&telPacket,0)==pdPASS){ // it is better to drop old data ratehr than blocking the task
            xSemaphoreTake(serialMutex,portMAX_DELAY);
            Serial.printf("[TELEMETRY] Generated packet SEQ:%u\n", telPacket.sequence);
            xSemaphoreGive(serialMutex);
            xEventGroupSetBits(systemEventGroup, BIT_TELEMETRY_OK); //indicating telemetry data is ready to be sent
            } 
        if(currentBits & BIT_SAFE_MODE){ vTaskDelay(pdMS_TO_TICKS(3000));} 
        else vTaskDelay(pdMS_TO_TICKS(1000)); 
       
    }
}

void DownLinkTask(void *pvParameters) {
    struct telemetryPacket receivedPacket;
    while (1) {
        if(xEventGroupGetBits(systemEventGroup)&BIT_FAULT_STATE){
            xSemaphoreTake(serialMutex,portMAX_DELAY);
            Serial.printf("[DOWNLINK] Alert: System in FAULT STATE..\n");
            xSemaphoreGive(serialMutex);
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }
        if(xQueueReceive(telemetryQueue,&receivedPacket,portMAX_DELAY)==pdPASS){
            xSemaphoreTake(serialMutex,portMAX_DELAY);
            Serial.printf("[DOWNLINK] Data received from queue: [APID]-%u, [Timestamp]-%lu, [SensorData]-%u, [Checksum]-%u\n", receivedPacket.apid, receivedPacket.timestamp, receivedPacket.sensorData, receivedPacket.checksum);
            xSemaphoreGive(serialMutex);
            xEventGroupSetBits(systemEventGroup,BIT_DOWNLINK_OK);// indicating downlink works  
        }   
           
        
    }
}

void HealthMonitorTask(void *pvParameters) {
    while (1) {
        EventBits_t bits= xEventGroupGetBits(systemEventGroup);
        if((bits&BIT_TELEMETRY_OK)&&(bits&BIT_DOWNLINK_OK)){
            xSemaphoreTake(serialMutex,portMAX_DELAY);
            Serial.printf("[MONITOR] Telemetry and downlink active\n");
            xSemaphoreGive(serialMutex);
            xEventGroupClearBits(systemEventGroup,BIT_FAULT_STATE); // clearing the fault bit if ok
           
        }
        else{
             if(!(bits&(BIT_TELEMETRY_OK))){
                xSemaphoreTake(serialMutex,portMAX_DELAY);
            Serial.printf("[MONITOR] ERROR: Telemetry failure\n");
            xSemaphoreGive(serialMutex);
        }
            if(!(bits&(BIT_DOWNLINK_OK))){
            xSemaphoreTake(serialMutex,portMAX_DELAY);
            Serial.printf("[MONITOR] ERROR: Downlink failure\n");
            xSemaphoreGive(serialMutex);
        }
        xEventGroupSetBits(systemEventGroup,BIT_FAULT_STATE); // indicating falt state and changing the behaviour of other tasks
        }
         xEventGroupClearBits(systemEventGroup, BIT_TELEMETRY_OK | BIT_DOWNLINK_OK); //resetting the bits for the next cycle
         
        vTaskDelay(pdMS_TO_TICKS(MONITOR_DELAY));
    }
}

void setup() {
    Serial.begin(115200);
    serialMutex=xSemaphoreCreateMutex();
    telemetryQueue=xQueueCreate(TelemetryQueueSize,sizeof(struct telemetryPacket));
    if(telemetryQueue==NULL){
        xSemaphoreTake(serialMutex,portMAX_DELAY);
        Serial.println("Failed to create telemetry queue");
        xSemaphoreGive(serialMutex);
        while(1);
    }
    xTaskCreate(CommandTask, "CommandTask", 2048, NULL, 5, &commandTaskHandle);
    xTaskCreate(TelemetryTask, "TelemetryTask", 2048, NULL, 3, &telemetryTaskHandle);
    xTaskCreate(DownLinkTask, "DownLinkTask", 2048, NULL, 2, &downlinkTaskHandle);
    xTaskCreate(HealthMonitorTask, "HealthMonitorTask", 2048, NULL, 4, &monitorTaskHandle);
    systemEventGroup=xEventGroupCreate();
    if(systemEventGroup==NULL){
         xSemaphoreTake(serialMutex,portMAX_DELAY);
        Serial.println("Failed to create event group");
        xSemaphoreGive(serialMutex);
        while(1);
    }
    xEventGroupSetBits(systemEventGroup,BIT_NORMAL_MODE);// system starts in normal mode, setting the bit to indicate that
}

void loop() {}