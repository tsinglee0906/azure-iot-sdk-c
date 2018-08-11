// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <string.h>
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/constbuffer.h"
#include "azure_c_shared_utility/platform.h"

#include "iothub_client_core_ll.h"
#include "internal/iothub_client_authorization.h"
#include "iothub_device_client_ll.h"
#include "iothub_transport_ll.h"
#include "internal/iothub_client_private.h"
#include "iothub_client_options.h"
#include "iothub_client_version.h"
#include "internal/iothub_client_diagnostic.h"
#include <stdint.h>

#ifndef DONT_USE_UPLOADTOBLOB
#include "internal/iothub_client_ll_uploadtoblob.h"
#endif

typedef struct IOTHUB_DEVICE_CLIENT_LL_HANDLE_DATA_TAG
{
    IOTHUB_CLIENT_CORE_LL_HANDLE coreHandle;
} IOTHUB_DEVICE_CLIENT_LL_HANDLE_DATA;

IOTHUB_DEVICE_CLIENT_LL_HANDLE IoTHubDeviceClient_LL_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    IOTHUB_DEVICE_CLIENT_LL_HANDLE_DATA* result;
    if ((result = malloc(sizeof(IOTHUB_DEVICE_CLIENT_LL_HANDLE_DATA))) == NULL)
    {
        LogError("Failed to allocate device client ll handle");
    }
    else
    {
        if ((result->coreHandle = IoTHubClientCore_LL_CreateFromConnectionString(connectionString, protocol)) == NULL)
        {
            LogError("Failed to create core handle");
            IoTHubDeviceClient_LL_Destroy(result);
            result = NULL;
        }
    }
    return result;
}

IOTHUB_DEVICE_CLIENT_LL_HANDLE IoTHubDeviceClient_LL_Create(const IOTHUB_CLIENT_CONFIG* config)
{
    IOTHUB_DEVICE_CLIENT_LL_HANDLE_DATA* result;
    if ((result = malloc(sizeof(IOTHUB_DEVICE_CLIENT_LL_HANDLE_DATA))) == NULL)
    {
        LogError("Failed to allocate device client ll handle");
    }
    else
    {
        if ((result->coreHandle = IoTHubClientCore_LL_Create(config)) == NULL)
        {
            LogError("Failed to create core handle");
            IoTHubDeviceClient_LL_Destroy(result);
            result = NULL;
        }
    }
    return result;
}

IOTHUB_DEVICE_CLIENT_LL_HANDLE IoTHubDeviceClient_LL_CreateWithTransport(const IOTHUB_CLIENT_DEVICE_CONFIG * config)
{
    IOTHUB_DEVICE_CLIENT_LL_HANDLE_DATA* result;
    if ((result = malloc(sizeof(IOTHUB_DEVICE_CLIENT_LL_HANDLE_DATA))) == NULL)
    {
        LogError("Failed to allocate device client ll handle");
    }
    else
    {
        if ((result->coreHandle = IoTHubClientCore_LL_CreateWithTransport(config)) == NULL)
        {
            LogError("Failed to create core handle");
            IoTHubDeviceClient_LL_Destroy(result);
            result = NULL;
        }
    }
    return result;
}

IOTHUB_DEVICE_CLIENT_LL_HANDLE IoTHubDeviceClient_LL_CreateFromDeviceAuth(const char* iothub_uri, const char* device_id, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    IOTHUB_DEVICE_CLIENT_LL_HANDLE_DATA* result;
    if ((result = malloc(sizeof(IOTHUB_DEVICE_CLIENT_LL_HANDLE_DATA))) == NULL)
    {
        LogError("Failed to allocate device client ll handle");
    }
    else
    {
        if ((result->coreHandle = IoTHubClientCore_LL_CreateFromDeviceAuth(iothub_uri, device_id, protocol)) == NULL)
        {
            LogError("Failed to create core handle");
            IoTHubDeviceClient_LL_Destroy(result);
            result = NULL;
        }
    }
    return result;
}

void IoTHubDeviceClient_LL_Destroy(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle)
{
    if (iotHubClientHandle != NULL)
    {
        IoTHubClientCore_LL_Destroy(iotHubClientHandle->coreHandle);
        free(iotHubClientHandle);
    }
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_LL_SendEventAsync(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_LL_SendEventAsync(iotHubClientHandle->coreHandle, eventMessageHandle, eventConfirmationCallback, userContextCallback);
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_LL_GetSendStatus(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_LL_GetSendStatus(iotHubClientHandle->coreHandle, iotHubClientStatus);
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_LL_SetMessageCallback(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_LL_SetMessageCallback(iotHubClientHandle->coreHandle, messageCallback, userContextCallback);
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_LL_SetConnectionStatusCallback(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK connectionStatusCallback, void * userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_LL_SetConnectionStatusCallback(iotHubClientHandle->coreHandle, connectionStatusCallback, userContextCallback);
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_LL_SetRetryPolicy(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitInSeconds)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_LL_SetRetryPolicy(iotHubClientHandle->coreHandle, retryPolicy, retryTimeoutLimitInSeconds);
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_LL_GetRetryPolicy(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY* retryPolicy, size_t* retryTimeoutLimitInSeconds)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_LL_GetRetryPolicy(iotHubClientHandle->coreHandle, retryPolicy, retryTimeoutLimitInSeconds);
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_LL_GetLastMessageReceiveTime(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle, time_t* lastMessageReceiveTime)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_LL_GetLastMessageReceiveTime(iotHubClientHandle->coreHandle, lastMessageReceiveTime);
    }
    return result;
}

void IoTHubDeviceClient_LL_DoWork(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle)
{
    if (iotHubClientHandle != NULL)
    {
        IoTHubClientCore_LL_DoWork(iotHubClientHandle->coreHandle);
    }
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_LL_SetOption(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle, const char* optionName, const void* value)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_LL_SetOption(iotHubClientHandle->coreHandle, optionName, value);
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_LL_SetDeviceTwinCallback(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK deviceTwinCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_LL_SetDeviceTwinCallback(iotHubClientHandle->coreHandle, deviceTwinCallback, userContextCallback);
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_LL_SendReportedState(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle, const unsigned char* reportedState, size_t size, IOTHUB_CLIENT_REPORTED_STATE_CALLBACK reportedStateCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_LL_SendReportedState(iotHubClientHandle->coreHandle, reportedState, size, reportedStateCallback, userContextCallback);
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_LL_SetDeviceMethodCallback(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC deviceMethodCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_LL_SetDeviceMethodCallback(iotHubClientHandle->coreHandle, deviceMethodCallback, userContextCallback);
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_LL_DeviceMethodResponse(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle, METHOD_HANDLE methodId, const unsigned char* response, size_t response_size, int status_response)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_LL_DeviceMethodResponse(iotHubClientHandle->coreHandle, methodId, response, response_size, status_response);
    }
    return result;
}

#ifndef DONT_USE_UPLOADTOBLOB
IOTHUB_CLIENT_RESULT IoTHubDeviceClient_LL_UploadToBlob(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle, const char* destinationFileName, const unsigned char* source, size_t size)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_LL_UploadToBlob(iotHubClientHandle->coreHandle, destinationFileName, source, size);
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_LL_UploadMultipleBlocksToBlob(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle, const char* destinationFileName, IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK_EX getDataCallbackEx, void* context)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        result = IoTHubClientCore_LL_UploadMultipleBlocksToBlobEx(iotHubClientHandle->coreHandle, destinationFileName, getDataCallbackEx, context);
    }
    return result;
}
#endif
