/**
 * -----------------------------------------------------
 * File        HTTPRequestThread.cpp
 * Authors     David Ordnung
 * License     GPLv3
 * Web         http://dordnung.de
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2017 David Ordnung
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

#include "HTTPRequestThread.h"
#include "HTTPResponseCallback.h"
#include "HTTPRequestMethod.h"


HTTPRequestThread::HTTPRequestThread(HTTPRequest *request, Handle_t requestHandle, IdentityToken_t *owner, HTTPRequestMethod requestMethod)
    : RequestThread(request, requestHandle, owner), request(request), requestMethod(requestMethod) {};


void HTTPRequestThread::RunThread(IThreadHandle *pHandle) {
    // Create a curl object
    CURL *curl = curl_easy_init();
    if (curl) {
        // Apply general request stuff
        this->ApplyRequest(curl);

        // Collect error information
        char errorBuffer[CURL_ERROR_SIZE + 1];
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);

        // Set write function
        DataInfo writeData = { std::string(), NULL };

        // Check if also write to an output file
        if (!this->request->outputFile.empty()) {
            // Get the full path to the file
            char filePath[PLATFORM_MAX_PATH + 1];
            smutils->BuildPath(Path_Game, filePath, sizeof(filePath), this->request->outputFile.c_str());

            // Open the file
            FILE *file = fopen(filePath, "wb");
            if (!file) {
                // Create error callback and clean up curl
                system2Extension.AppendCallback(std::make_shared<HTTPResponseCallback>("Couldn't open output file", this->request, this->requestHandle, this->owner));
                curl_easy_cleanup(curl);

                return;
            }

            writeData.file = file;
        }

        // Set the write function and data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, RequestThread::WriteData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &writeData);

        // Set the http username
        if (!this->request->username.empty()) {
            curl_easy_setopt(curl, CURLOPT_USERNAME, this->request->username.c_str());
        }

        // Set the http password
        if (!this->request->password.empty()) {
            curl_easy_setopt(curl, CURLOPT_PASSWORD, this->request->password.c_str());
        }

        // Set the follow redirect property
        if (this->request->followRedirects) {
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL);
        } else {
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);
        }

        // Set the auto referer property
        if (this->request->autoReferer) {
            curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1L);
        } else {
            curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 0L);
        }

        // Set data to send
        if (!this->request->data.empty()) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, this->request->data.c_str());
        } else {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0L);
        }

        // Set headers
        struct curl_slist *headers;
        if (!this->request->headers.empty()) {
            std::string header;
            for (std::map<std::string, std::string>::iterator it = this->request->headers.begin(); it != this->request->headers.end(); ++it) {
                header = it->first + ":" + it->second;
                headers = curl_slist_append(headers, header.c_str());
            }

            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }

        // Get response headers
        HeaderInfo headerData = { this->request, curl, std::map<std::string, std::string>() };
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HTTPRequestThread::ReadHeader);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headerData);

        // Set http method
        switch (this->requestMethod) {
            case METHOD_GET:
                curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
                break;
            case METHOD_POST:
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
                break;
            case METHOD_PUT:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                break;
            case METHOD_PATCH:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
                break;
            case METHOD_DELETE:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
                break;
            case METHOD_HEAD:
                curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
                break;
        }

        // Perform curl operation and create the callback
        std::shared_ptr<HTTPResponseCallback> callback;
        if (curl_easy_perform(curl) == CURLE_OK) {
            callback = std::make_shared<HTTPResponseCallback>(curl, writeData.content, headerData.headers, this->request, this->requestHandle, this->owner);
        } else {
            callback = std::make_shared<HTTPResponseCallback>(errorBuffer, this->request, this->requestHandle, this->owner);
        }

        // Clean up curl
        curl_easy_cleanup(curl);
        if (headers) {
            curl_slist_free_all(headers);
        }

        // Also close output file if opened
        if (writeData.file) {
            fclose(writeData.file);
        }

        // Append callback so it can be fired
        system2Extension.AppendCallback(callback);
    } else {
        system2Extension.AppendCallback(std::make_shared<HTTPResponseCallback>("Couldn't initialize CURL", this->request, this->requestHandle, this->owner));
    }
}