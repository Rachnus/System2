/**
 * -----------------------------------------------------
 * File        RequestNatives.cpp
 * Authors     David Ordnung
 * License     GPLv3
 * Web         http://dordnung.de
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2018 David Ordnung
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
 */

#include "Request.h"
#include "HTTPRequest.h"
#include "FTPRequest.h"
#include "RequestHandler.h"
#include "HTTPRequestThread.h"


cell_t NativeRequest_SetURL(IPluginContext *pContext, const cell_t *params) {
    Request *request = Request::ConvertRequest<Request>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    char *url;
    pContext->LocalToString(params[2], &url);

    request->url = url;
    return 1;
}

cell_t NativeRequest_GetURL(IPluginContext *pContext, const cell_t *params) {
    Request *request = Request::ConvertRequest<Request>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    pContext->StringToLocalUTF8(params[2], params[3], request->url.c_str(), NULL);
    return 1;
}

cell_t NativeRequest_SetPort(IPluginContext *pContext, const cell_t *params) {
    Request *request = Request::ConvertRequest<Request>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->port = params[2];
    return 1;
}

cell_t NativeRequest_GetPort(IPluginContext *pContext, const cell_t *params) {
    Request *request = Request::ConvertRequest<Request>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    return request->port;
}

cell_t NativeRequest_SetOutputFile(IPluginContext *pContext, const cell_t *params) {
    Request *request = Request::ConvertRequest<Request>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    char *outputFile;
    pContext->LocalToString(params[2], &outputFile);

    request->outputFile = outputFile;
    return 1;
}

cell_t NativeRequest_GetOutputFile(IPluginContext *pContext, const cell_t *params) {
    Request *request = Request::ConvertRequest<Request>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    pContext->StringToLocalUTF8(params[2], params[3], request->outputFile.c_str(), NULL);
    return 1;
}

cell_t NativeRequest_SetVerifySSL(IPluginContext *pContext, const cell_t *params) {
    Request *request = Request::ConvertRequest<Request>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->verifySSL = params[2];
    return 1;
}

cell_t NativeRequest_GetVerifySSL(IPluginContext *pContext, const cell_t *params) {
    Request *request = Request::ConvertRequest<Request>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    return request->verifySSL;
}

cell_t NativeRequest_GetTimeout(IPluginContext *pContext, const cell_t *params) {
    Request *request = Request::ConvertRequest<Request>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    return request->timeout;
}

cell_t NativeRequest_SetTimeout(IPluginContext *pContext, const cell_t *params) {
    Request *request = Request::ConvertRequest<Request>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->timeout = params[2];
    return 1;
}

cell_t NativeRequest_GetAnyData(IPluginContext *pContext, const cell_t *params) {
    Request *request = Request::ConvertRequest<Request>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    return request->data;
}

cell_t NativeRequest_SetAnyData(IPluginContext *pContext, const cell_t *params) {
    Request *request = Request::ConvertRequest<Request>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->data = params[2];
    return 1;
}


cell_t NativeHTTPRequest_HTTPRequest(IPluginContext *pContext, const cell_t *params) {
    char *url;
    pContext->LocalToString(params[1], &url);

    HTTPRequest *request = new HTTPRequest(url, pContext->GetFunctionById(params[2]));

    Handle_t hndl = requestHandler.CreateGlobalHandle<HTTPRequest>(request, pContext->GetIdentity());
    if (hndl == BAD_HANDLE) {
        pContext->ReportError("Couldn't create HTTPRequest handle");
    }

    return hndl;
}

cell_t NativeHTTPRequest_SetProgressCallback(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->progressCallback = pContext->GetFunctionById(params[2]);
    return 1;
}

cell_t NativeHTTPRequest_SetData(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    char *data;
    pContext->LocalToString(params[2], &data);

    request->bodyData = data;
    return 1;
}

cell_t NativeHTTPRequest_GetData(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    pContext->StringToLocalUTF8(params[2], params[3], request->bodyData.c_str(), NULL);
    return 1;
}

cell_t NativeHTTPRequest_SetHeader(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    char *header;
    char *value;
    pContext->LocalToString(params[2], &header);
    pContext->LocalToString(params[3], &value);

    request->headers[header] = value;
    return 1;
}

cell_t NativeHTTPRequest_GetHeader(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    char *header;
    pContext->LocalToString(params[2], &header);

    for (std::map<std::string, std::string>::iterator it = request->headers.begin(); it != request->headers.end(); ++it) {
        if (HTTPRequestThread::EqualsIgnoreCase(it->first, header)) {
            pContext->StringToLocalUTF8(params[3], params[4], request->headers[header].c_str(), NULL);
            return 1;
        }
    }

    return 0;
}

cell_t NativeHTTPRequest_GetHeaderName(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    if (params[2] >= static_cast<int>(request->headers.size())) {
        return 0;
    }

    // Map can't be accessed by index
    std::map<std::string, std::string>::iterator it = request->headers.begin();
    for (int i = 0; i < params[2]; i++) {
        it++;
    }

    pContext->StringToLocalUTF8(params[3], params[4], it->first.c_str(), NULL);
    return 1;
}

cell_t NativeHTTPRequest_GetHeadersCount(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    return request->headers.size();
}

cell_t NativeHTTPRequest_SetUserAgent(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    char *userAgent;
    pContext->LocalToString(params[2], &userAgent);

    request->userAgent = userAgent;
    return 1;
}

cell_t NativeHTTPRequest_SetBasicAuthentication(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    char *username;
    char *password;
    pContext->LocalToString(params[2], &username);
    pContext->LocalToString(params[3], &password);

    request->username = username;
    request->password = password;
    return 1;
}

cell_t NativeHTTPRequest_GET(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->Get();
    return 1;
}

cell_t NativeHTTPRequest_POST(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->Post();
    return 1;
}

cell_t NativeHTTPRequest_PUT(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->Put();
    return 1;
}

cell_t NativeHTTPRequest_PATCH(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->Patch();
    return 1;
}

cell_t NativeHTTPRequest_DELETE(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->Delete();
    return 1;
}

cell_t NativeHTTPRequest_HEAD(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->Head();
    return 1;
}

cell_t NativeHTTPRequest_GetFollowRedirects(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    return request->followRedirects;
}

cell_t NativeHTTPRequest_SetFollowRedirects(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->followRedirects = params[2];
    return 1;
}

cell_t NativeHTTPRequest_GetAutoReferer(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    return request->autoReferer;
}

cell_t NativeHTTPRequest_SetAutoReferer(IPluginContext *pContext, const cell_t *params) {
    HTTPRequest *request = Request::ConvertRequest<HTTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->autoReferer = params[2];
    return 1;
}


cell_t NativeFTPRequest_FTPRequest(IPluginContext *pContext, const cell_t *params) {
    char *url;
    pContext->LocalToString(params[1], &url);

    FTPRequest *request = new FTPRequest(url, pContext->GetFunctionById(params[2]));

    Handle_t hndl = requestHandler.CreateGlobalHandle<FTPRequest>(request, pContext->GetIdentity());
    if (hndl == BAD_HANDLE) {
        pContext->ReportError("Couldn't create FTPRequest handle");
    }

    return hndl;
}

cell_t NativeFTPRequest_SetProgressCallback(IPluginContext *pContext, const cell_t *params) {
    FTPRequest *request = Request::ConvertRequest<FTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->progressCallback = pContext->GetFunctionById(params[2]);
    return 1;
}

cell_t NativeFTPRequest_SetAuthentication(IPluginContext *pContext, const cell_t *params) {
    FTPRequest *request = Request::ConvertRequest<FTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    char *username;
    char *password;
    pContext->LocalToString(params[2], &username);
    pContext->LocalToString(params[3], &password);

    request->username = username;
    request->password = password;
    return 1;
}

cell_t NativeFTPRequest_SetInputFile(IPluginContext *pContext, const cell_t *params) {
    FTPRequest *request = Request::ConvertRequest<FTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    char *inputFile;
    pContext->LocalToString(params[2], &inputFile);

    request->inputFile = inputFile;
    return 1;
}

cell_t NativeFTPRequest_GetInputFile(IPluginContext *pContext, const cell_t *params) {
    FTPRequest *request = FTPRequest::ConvertRequest<FTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    pContext->StringToLocalUTF8(params[2], params[3], request->inputFile.c_str(), NULL);
    return 1;
}

cell_t NativeFTPRequest_StartRequest(IPluginContext *pContext, const cell_t *params) {
    FTPRequest *request = Request::ConvertRequest<FTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->MakeRequest();
    return 1;
}

cell_t NativeFTPRequest_GetAppendToFile(IPluginContext *pContext, const cell_t *params) {
    FTPRequest *request = Request::ConvertRequest<FTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    return request->appendToFile;
}

cell_t NativeFTPRequest_SetAppendToFile(IPluginContext *pContext, const cell_t *params) {
    FTPRequest *request = Request::ConvertRequest<FTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->appendToFile = params[2];
    return 1;
}

cell_t NativeFTPRequest_GetCreateMissingDirs(IPluginContext *pContext, const cell_t *params) {
    FTPRequest *request = Request::ConvertRequest<FTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    return request->createMissingDirs;
}

cell_t NativeFTPRequest_SetCreateMissingDirs(IPluginContext *pContext, const cell_t *params) {
    FTPRequest *request = Request::ConvertRequest<FTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->createMissingDirs = params[2];
    return 1;
}

cell_t NativeFTPRequest_GetListFilenamesOnly(IPluginContext *pContext, const cell_t *params) {
    FTPRequest *request = Request::ConvertRequest<FTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    return request->listFilenamesOnly;
}

cell_t NativeFTPRequest_SetListFilenamesOnly(IPluginContext *pContext, const cell_t *params) {
    FTPRequest *request = Request::ConvertRequest<FTPRequest>(params[1], pContext);
    if (request == NULL) {
        return 0;
    }

    request->listFilenamesOnly = params[2];
    return 1;
}