/***************************************************************************************
 * Project: Worker AI Agent
 * File:    Source.cpp
 * Author:  PersTheory
 * Email:   officer@perstheory.com
 * Date:    January 1, 2025
 * Version: 1.0
 ****************************************************************************************/

#define NOMINMAX

#include <string>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include "utils.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <codecvt>
#include <locale>
#include <filesystem>
#include <sstream>
#include <string>
#include <map>
#include <regex>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <locale>
#include <codecvt>
#include <iostream>
#include <fstream>
#include <deque>
#include <string>
#include <inja/inja.hpp>
#include <zlib.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <unordered_set>
#include <curl/curl.h>
#include "Json.h"

class Request {
public:
    Request(const std::string& url, const std::string& method, const std::string& post_data)
        : url(url), method(method), post_data(post_data), curl(curl_easy_init()) {
        if (!curl) {
            throw std::runtime_error("Failed to initialize curl");
        }
    }

    ~Request() {
        if (curl) {
            curl_easy_cleanup(curl);
        }
    }

    std::string execute() {
        if (!curl) {
            throw std::runtime_error("Curl not initialized");
        }

        buffer.clear();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        struct curl_slist* chunk = NULL;
        for (const auto& header : headers) {
            chunk = curl_slist_append(chunk, header.c_str());
            if (!chunk) {
                throw std::runtime_error("Failed to append header");
            }
        }

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(chunk);  // free the list again

        if (res != CURLE_OK) {
            throw std::runtime_error("Failed to load data from url: " + url);
        }

        return buffer;
    }

    void addHeader(const std::string& header) {
        headers.push_back(header);
    }
    std::string url;
    std::string method;
    std::string post_data;
    std::vector<std::string> headers;
    std::string buffer;
    CURL* curl;
private:


    static size_t write_data(void* contents, size_t size, size_t nmemb, void* userp)
    {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
};


namespace fs = std::filesystem;
const fs::path baseDirectory = fs::current_path() / "ProjectDirectory";

// Convert UTF-8 string to wide string
std::wstring utf8_to_wstring(const std::string& utf8_str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(utf8_str);
}

// Converts wstring to string using UTF-8 encoding
std::string wstringToString(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string strTo(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], sizeNeeded, nullptr, nullptr);
    return strTo;
}

// Converts string to wstring using UTF-8 encoding
std::wstring stringToWstring(const std::string& str) {
    if (str.empty()) return {};
    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
    std::wstring wstrTo(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], sizeNeeded);
    return wstrTo;
}

std::wstring readFileContent(const std::wstring& filePath)
{
    try {

        std::wifstream wif(filePath);

        if (!wif.is_open()) {
            return L"File Can Not Be Opened";
        }

        wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));

        std::wstringstream wss;
        wss << wif.rdbuf();
        wif.close();

        return wss.str();
    }
    catch (const std::exception& e) {
        return L"Error: " + std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(e.what());
    }
}

bool writeFileContent(const std::wstring& filePath, const std::wstring& content)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string narrowPath = converter.to_bytes(filePath);

    std::wofstream wof(narrowPath, std::ios::trunc);
    if (!wof.is_open()) {
        return false;
    }
    wof << content;
    wof.close();
    return true;
}

std::wstring convertToFitInJSON(const std::wstring& input) {
    std::wstring result;
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == L'\n') {
            result += L"\\n";
        }
        else if (input[i] == L'\t') {
            result += L"\\t";
        }
        else if (input[i] == L'\"') {
            result += L"\\\"";
        }
        else if (input[i] == L'\\') {
            result += L"\\\\";
        }
        else {
            result += input[i];
        }
    }
    return result;
}

std::wstring systemInstructions() {
    return readFileContent(L"instructions\\instructions.txt");
}

std::wstring systemInstructions2() {
    return readFileContent(L"instructions\\make-changes-instruction.txt");
}

std::wstring replaceSubStringOnce(std::wstring& str, const std::wstring& from, const std::wstring& to) {
    std::wstring result = str;
    size_t start_pos = 0;
    while ((start_pos = result.find(from, start_pos)) != std::wstring::npos) {
        result.replace(start_pos, from.length(), to);
    }
    return result;
}

std::wstring memoryFile=L"ProjectDirectory\\memory.txt";
std::wstring finalContentFile = L"ProjectDirectory\\final-content.txt";
std::wstring changesFile = L"ProjectDirectory\\target.txt";
std::wstring modifyInstructionFile = L"ProjectDirectory\\modify-instructions.txt";
std::wstring promptHistoryFile = L"ProjectDirectory\\prompts-history.txt";

std::wstring getInstructions() {
    std::wstring fileContent = readFileContent(finalContentFile);
    
    return LR"()" + systemInstructions() + LR"(
)" + LR"(
)" + readFileContent(memoryFile) + LR"(
)" + LR"(
Source.txt:
)" + fileContent + LR"(
)";

}

std::wstring getIncludeInstructions() {
    std::wstring fileContent = trim(readFileContent(changesFile));
    
    return LR"()" + systemInstructions2() + LR"(
-----------------------------------
<TargetCode>
)" + fileContent + LR"(
)" + LR"(-----------------------------------
)" + readFileContent(modifyInstructionFile) + LR"(
)";
}

bool saveWStringToFile(const std::wstring& content, const std::wstring& filename, const int discardLastLines,bool clearFile=false) {
    
    std::vector<std::wstring> lines;

    if (!clearFile) {

        std::wifstream inFile(filename, std::ios::in);

        if (inFile.is_open()) {
            inFile.imbue(std::locale(inFile.getloc(), new std::codecvt_utf8<wchar_t>));
            std::wstring line;
            while (std::getline(inFile, line)) {
                lines.push_back(line);
            }
            inFile.close();
        }

        if (discardLastLines > 0) {
            if (discardLastLines >= lines.size()) {
                lines.clear();
            }
            else {
                lines.resize(lines.size() - discardLastLines);
            }
        }
    }

    std::wofstream outFile(filename, std::ios::trunc);

    if (!outFile.is_open()) {
        std::wcerr << L"Failed to open the file: " << filename.c_str() << std::endl;
        return false;
    }

    outFile.imbue(std::locale(outFile.getloc(), new std::codecvt_utf8<wchar_t>));

    for (const auto& l : lines) {
        outFile << l << L"\n";
    }

    outFile << content;

    outFile.close();

    return true;
}

std::wstring parseLine(const std::wstring& input) {
    std::wstringstream ss(input);
    std::wstring line;
    std::wstring result;

    std::wregex pattern(LR"((<Explanation-of-Modifications>|### Explanation-of-Modifications)|<Explanation-of-Code-Modifications>)");

    bool found = false;

    while (std::getline(ss, line, L'\n')) {
        
        if (found) {
            result += line + L"\n";
            continue;
        }

        std::wsmatch match;
        if (std::regex_search(line, match, pattern)) {
            found = true;
            result += line + L"\n";
        }
    }

    return result;
}

std::wstring join(const std::vector<std::wstring>& contents, const std::wstring& delimiter) {
    std::wostringstream oss;
    for (size_t i = 0; i < contents.size(); ++i) {
        oss << contents[i];
        if (i != contents.size() - 1) {
            oss << delimiter;
        }
    }
    return oss.str();
}

std::wstring removeTags(const std::wstring& source) {
    std::wstringstream sourceStream(source);
    std::wstringstream result;

    std::unordered_set<std::wstring> sourceTags;
    std::unordered_map<std::wstring, std::wstring> sourceContent;
    std::wstring line;

    std::vector<std::wstring> currentTags; //tags path
    std::vector<std::wstring> currentContents; //contents path

    std::unordered_map<int, std::wstring> sourceTagsIndex;

    int index = 0;
    std::wstring tagType = L"";
    std::wstring tag = L"";
    std::wstring outside = L"";

    while (std::getline(sourceStream, line)) {
        std::wstring trimmedLine = trim(line);
        size_t trimmedStart = trimmedLine.find(L"<");

        size_t start = line.find(L"<");
        size_t end = line.find(L">", start);
        if (trimmedStart == 0 && end != std::wstring::npos && line.find(L"#include") == std::wstring::npos && line.length() > 2) {
            continue;
        }
        else {
            result << line + L'\n';
        }
    }

    return result.str();

}

std::wstring EditSource(const std::wstring& source, const std::wstring& modify) {
    std::wstringstream sourceStream(source);
    std::wstringstream modifyStream(modify);
    std::wstringstream result;

    std::unordered_set<std::wstring> modifyTags;
    std::unordered_set<std::wstring> sourceTags;
    std::unordered_map<std::wstring, std::wstring> modifyContent;
    std::unordered_map<std::wstring, std::wstring> sourceContent;
    std::wstring line;

    std::vector<std::wstring> currentTags; //tags path
    std::vector<std::wstring> currentContents; //contents path

    std::unordered_map<int, std::wstring> modifyTagsIndex;
    std::unordered_map<int, std::wstring> sourceTagsIndex;

    int index = 0;
    std::wstring tagType = L"";
    std::wstring tag = L"";
    std::wstring outside = L"";

    while (std::getline(sourceStream, line)) {
        std::wstring trimmedLine = trim(line);
        size_t trimmedStart = trimmedLine.find(L"<");

        size_t start = line.find(L"<");
        size_t end = line.find(L">", start);
        if (trimmedStart == 0 && end != std::wstring::npos && line.find(L"#include") == std::wstring::npos && line.length() > 2) {
            // Tag detected, extract its name and type
            tag = line.substr(start + 1, end - start - 1);
            if (tag.back() == L'/') {
                // Closed tag that needs to be retained

                tag.pop_back();
                tagType = L"Tag needs to be fully retained";

                if (currentTags.empty()) {
                    //outside += line + L'\n';
                    //need to write in modification text, if not will discard
                    continue;
                }
                // If there is a parent tag and it exists in the modification text, retain the line
                currentContents.back() += line + L'\n';

            }
            else if (tag.front() == L'/') {
                // Closing tag for some content
                tag = tag.substr(1);
                tagType = L"Closing tag, summarizing content into the parent tag";

                // Check if this matches the currently processed tag
                if (tag == currentTags.back()) {
                    // Confirm it matches the current tag
                    // insert this line and insert all into parent tag content
                    currentContents.back() += line + L'\n';
                    std::wstring tagFinalContent = currentContents.back();

                    // Move up one level
                    currentContents.pop_back();
                    currentTags.pop_back();

                    // Append content to the parent tag, if it exists
                    if (!currentTags.empty()) {
                        currentContents.back() += tagFinalContent;
                    }
                    else {
                        // If no parent tag exists, add to sourceContent for later use
                        //outside += tagFinalContent;//Source no have the final right to retain tag
                        sourceContent[tag] = tagFinalContent;
                        continue;
                    }

                    // Store in sourceContent
                    if (!tag.empty()) {
                        sourceContent[tag] = tagFinalContent;
                    }
                    else {
                        std::wcerr << L"Invalid key detected." << std::endl;
                    }
                }
                else {
                    // If a closing tag does not match the currently processed tag, may be not our interest
                    currentContents.back() += line + L'\n';
                }

            }
            else {
                // A newly opened tag
                tagType = L"A newly opened tag";

                // A new tag inside the currently processed tag indicates a child tag
                currentTags.push_back(tag);
                currentContents.push_back(L"");

                // Add the tag as the first line
                currentContents.back() += line + L'\n';
            }

        }
        else {
            // Content for the currently processed tag
            if (currentTags.empty()) {
                // Discard content outside any tags
                continue;
            }

            currentContents.back() += line + L"\n";
        }
    }

    index = 0;
    tag = tagType = L"";
    currentContents.clear();
    currentTags.clear();

    // Start integrating from Modify into Source
    while (std::getline(modifyStream, line)) {

        std::wstring trimmedLine = trim(line);
        size_t trimmedStart = trimmedLine.find(L"<");

        size_t start = line.find(L"<");
        size_t end = line.find(L">", start);
        if (trimmedStart == 0 && end != std::wstring::npos && line.find(L"#include") == std::wstring::npos) {
            // Tag detected, extract its name and type
            std::wstring tag = line.substr(start + 1, end - start - 1);
            if (tag.back() == L'/') {
                // Closed tag that needs to be retained
                tag.pop_back();
                tagType = L"Tag needs to be fully retained";
                // Retain the tag fully, using content from the source. Tag uniqueness must be ensured here.

                if (sourceContent.count(tag)) {
                    if (!currentTags.empty()) {
                        currentContents.back() += sourceContent[tag];
                    }
                    else {
                        outside += sourceContent[tag];
                    }
                }
                else {
                    // If the tag doesn't exist in the source, add it as a new one
                    currentContents.back() += line + L'\n';
                }

            }
            else if (tag.front() == L'/') {
                // Closing tag for some content
                tag = tag.substr(1);
                tagType = L"Closing tag, insert content into the parent tag";

                // Check if this matches the currently processed tag
                if (tag == currentTags.back()) {
                    // Confirm it matches the current tag
                    currentContents.back() += line + L'\n';
                    std::wstring tagFinalContent = currentContents.back();

                    // Move up one level
                    currentContents.pop_back();
                    currentTags.pop_back();

                    // Append content to the parent tag, if it exists
                    if (!currentContents.empty()) {
                        currentContents.back() += tagFinalContent;
                    }
                    else {
                        outside += tagFinalContent;
                    }

                }
                else {
                    // If a closing tag does not match the currently processed tag, append content
                    currentContents.back() += line + L'\n';
                }

            }
            else {
                // A newly opened tag
                tagType = L"A newly opened tag";

                // A new tag inside the currently processed tag indicates a child tag
                currentTags.push_back(tag);
                currentContents.push_back(L"");

                // Add the tag as the first line
                currentContents.back() += line + L'\n';
            }

        }
        else {
            // Content for the currently processed tag
            if (currentTags.empty()) {
                //currentTags.push_back(L"");
                //currentContents.push_back(L"");
                //Discard content outside any tags
                continue;
            }

            currentContents.back() += line + L"\n";
        }
    }
    if (!currentContents.empty()) {
        outside += join(currentContents, L"");
    }
    return outside;
}


std::wstring cleanResult(const std::wstring& input)
{
    std::wstringstream inputStream(input);
    std::wstringstream outputStream;
    std::wstring line;

    // Regex pattern to match substrings starting with ```
    std::wregex pattern(L"```");

    while (std::getline(inputStream, line)) {
        // Check if the line matches the pattern
        if (!std::regex_search(line, pattern)) {
            outputStream << line << L"\n";
        }
    }

    return outputStream.str();
}


std::string decompress_gzip(const std::string& compressed) {
    z_stream zs;
    std::memset(&zs, 0, sizeof(zs)); // Explicitly zero-initialize

    zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressed.data()));
    zs.avail_in = static_cast<uInt>(compressed.size());

    if (inflateInit2(&zs, 16 + MAX_WBITS) != Z_OK) { // 16 + MAX_WBITS to support gzip format
        throw std::runtime_error("Failed to initialize zlib for gzip decompression");
    }

    std::string decompressed;
    char buffer[8192];
    int ret;

    do {
        zs.next_out = reinterpret_cast<Bytef*>(buffer);
        zs.avail_out = sizeof(buffer);

        ret = inflate(&zs, 0);
        if (ret != Z_OK && ret != Z_STREAM_END) {
            inflateEnd(&zs);
            throw std::runtime_error("Error during gzip decompression");
        }

        decompressed.append(buffer, sizeof(buffer) - zs.avail_out);
    } while (ret != Z_STREAM_END);

    inflateEnd(&zs);
    return decompressed;
}

std::string callGPT(const std::string& api_key, const std::wstring& prompt,int type=0) {
    
    if (prompt.empty()) {
        std::cerr << "Prompt cannot be empty." << std::endl;
        return "";
    }

    // Convert std::wstring to std::string (UTF-8 encoding assumed)
    std::string utf8_prompt=wstringToString(prompt);

    if (type == 0) {
        // Construct the POST data
        std::string post_data = R"({"model":"gpt-4o","messages":[{"role":"user","content":")" +
            utf8_prompt + R"("}],"response_format":{"type": "text"},"max_tokens":4000})";

        // Initialize the request
        Request req("https://api.openai.com/v1/chat/completions", "POST", post_data.c_str());

        // Set headers
        req.headers.push_back("Content-Type: application/json");
        req.headers.push_back("Authorization: Bearer " + api_key);

        try {
            // Execute the request
            req.execute();
        }
        catch (const std::exception& e) {
            return e.what();
        }

        // Parse the response
        std::string json_str = req.buffer;
        Json doc(json_str.c_str());

        if (doc.HasMember("error")) {
            std::cerr << "API Error: " << doc["error"]["message"].GetString() << std::endl;
            return "";
        }

        std::string response_text = "";
        if (doc.HasMember("choices") && doc["choices"].IsArray() && doc["choices"].Size() > 0) {
            response_text = doc["choices"][0]["message"]["content"].GetString();
        }
        else {
            std::cerr << "Unexpected response format: " << json_str << std::endl;
            return "";
        }

        return response_text;
    }
    else if (type == 1) {
        // Construct the POST data
        std::string post_data = R"({
            "contents": [
                {
                    "parts": [
                        {
                            "text": ")" + utf8_prompt + R"("
                        }
                    ]
                }
            ]
        })";
        
        // API Endpoint
        std::string api_url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + api_key;

        // Initialize the request
        Request req(api_url, "POST", post_data.c_str());

        // Set headers
        req.headers.push_back("Content-Type: application/json");
        req.headers.push_back("Accept-Encoding: gzip");

        try {
            // Execute the request
            req.execute();
        }
        catch (const std::exception& e) {
            std::cerr << "Request failed: " << e.what() << std::endl;
            return "";
        }

        // Parse the response
        std::string response;
        try {
            response = decompress_gzip(req.buffer);
        }
        catch (const std::exception& e) {
            std::cerr << "Decompression Error: " << e.what() << std::endl;
            return "";
        }
        std::string json_str = response;
        //std::wcout << "Raw JSON: " << stringToWstring(json_str) << std::endl;

        Json doc(json_str.c_str());

        // Check for errors
        if (doc.HasMember("error")) {
            if (doc["error"].HasMember("message") && doc["error"]["message"].IsString()) {
                std::cerr << "API Error: " << doc["error"]["message"].GetString() << std::endl;
            }
            else {
                std::cerr << "API Error: Unknown error format." << std::endl;
            }
            return "";
        }

        // Extract and return response text
        if (doc.HasMember("candidates") &&
            doc["candidates"].IsArray() &&
            !doc["candidates"].Empty() &&
            doc["candidates"][0].IsObject() &&
            doc["candidates"][0].HasMember("content") &&
            doc["candidates"][0]["content"].IsObject() &&
            doc["candidates"][0]["content"].HasMember("parts") &&
            doc["candidates"][0]["content"]["parts"].IsArray() &&
            !doc["candidates"][0]["content"]["parts"].Empty() &&
            doc["candidates"][0]["content"]["parts"][0].IsObject() &&
            doc["candidates"][0]["content"]["parts"][0].HasMember("text") &&
            doc["candidates"][0]["content"]["parts"][0]["text"].IsString()) {
            return doc["candidates"][0]["content"]["parts"][0]["text"].GetString();
        }

        // Return empty if no valid text found
        std::cerr << "No valid response text found." << std::endl;
        return "";
        //return response_text;
    }
    
    return "";
}

bool quit = false;

int main()
{
    int oldMode = _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);

    std::string openai_api_key = "";
    std::string google_api_key = "";
    std::wstring prompt = L"";
    std::wstring prompt_include = L"";
    std::wstring cmd = L"";

    //chatgpt
    bool useChatGPT = true;
    std::string key = useChatGPT? openai_api_key :google_api_key;
    int modelType = useChatGPT ? 0 : 1;

    bool quit = false;
    while(true){

        if (!prompt.empty()) {
            //process prompt
            std::string result = callGPT(key, convertToFitInJSON(prompt), modelType);
            //std::wcout << L"API return:" << stringToWstring(result) << std::endl;
            std::wstring wresult = cleanResult(stringToWstring(result));
            
            //save for next step
            saveWStringToFile(L"", memoryFile, 0, true);
            saveWStringToFile(wresult, memoryFile, 0);

            //extract modification explaination part
            std::wstring toInclude = parseLine(wresult);
            saveWStringToFile(L"",modifyInstructionFile, 0, true);
            saveWStringToFile(toInclude, modifyInstructionFile,0);

            std::wcout << L"Suggest to change:" << toInclude << std::endl;
            std::wstring fileContent = trim(readFileContent(changesFile));

            prompt_include = getIncludeInstructions();
            prompt_include = replaceSubStringOnce(prompt_include,L"<Key-C++-Code>",L"<Reference-Code>");

            if (prompt_include.find(L"[Goal-Achieved]") != std::wstring::npos) {
                quit = true;
            }

            saveWStringToFile(L"----------------------\n"+prompt_include, promptHistoryFile, 0);
            std::wcout << L"Applying Changes To Code..." << std::endl;
            std::string include_result= callGPT(key, convertToFitInJSON(prompt_include), modelType);
            std::wstring winclude_result = cleanResult(stringToWstring(include_result));
            //std::wcout << L"Modification Text:" << winclude_result << std::endl;
            saveWStringToFile(L"----------------------reply:\n" + winclude_result, promptHistoryFile, 0);
            
            std::wstring final_code = EditSource(fileContent, winclude_result);
            final_code = trim(final_code);

            saveWStringToFile(L"", changesFile, 0, true);
            saveWStringToFile(final_code,changesFile,0);
            saveWStringToFile(L"", finalContentFile, 0,true);
            saveWStringToFile(removeTags(final_code),finalContentFile, 0);

            std::wcout << L"Changes Made." << std::endl;

            if (quit) {
                break;
                return 0;
            }
            
        }

        prompt = getInstructions();
        
        while (cmd.empty()) {
            
            break; //autorun
            std::wcout << prompt << std::endl;
            std::wcout << L"Enter To Continue(r to reload instruction，q to quit): " << std::endl;

            if (cmd == L"q") {
                cmd = L"";
                break;
            }
            else if (cmd == L"r") {
                prompt = getInstructions();
                cmd = L"";
            }
            else {
                break;
            }
        }

        if (!cmd.empty()) {
            prompt += LR"(
Supervisor：)" + cmd;
            cmd = L""; //reset cmd
        }
        std::wcout << L"----------------------------------------------------------" << std::endl;
        
    }
    
    _setmode(_fileno(stdout), oldMode);
    
    return 0;
}
