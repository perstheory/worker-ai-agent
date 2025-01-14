#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include <cctype>
#include <stdexcept>
#include <iostream>

static std::string url_encode(const std::string& str) {
    std::string new_str = "";
    char c;
    int ic;
    const char* chars = str.c_str();
    char bufHex[10];
    int len = strlen(chars);

    for (int i = 0; i < len; i++) {
        c = chars[i];
        ic = c;
        
        if (c==' ') new_str += '+';
        else if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') new_str += c;
        else {
            sprintf(bufHex, "%X", c);
            if (ic < 16)
                new_str += "%0";
            else
                new_str += "%";
            new_str += bufHex;
        }
    }
    return new_str;
}

static std::string url_decode(const std::string& str) {
    std::string ret;
    size_t len = str.length();

    for (size_t i = 0; i < len; ++i) {
        if (str[i] == '%') {
            // Ensure there are at least two characters after '%'
            if (i + 2 >= len) {
                throw std::invalid_argument("Invalid URL encoding: incomplete % sequence");
            }

            // Validate hex characters
            char hex1 = str[i + 1];
            char hex2 = str[i + 2];
            if (!std::isxdigit(hex1) || !std::isxdigit(hex2)) {
                throw std::invalid_argument("Invalid URL encoding: non-hex characters in % sequence");
            }

            // Convert hex sequence to character
            int hexValue = std::stoi(str.substr(i + 1, 2), nullptr, 16);
            ret += static_cast<char>(hexValue);
            i += 2; // Skip the processed characters
        }
        else if (str[i] == '+') {
            ret += ' '; // Replace '+' with a space
        }
        else {
            ret += str[i]; // Append regular character
        }
    }

    return ret;
}


static std::wstring trim(std::wstring str)
{
    const std::wstring charsToTrim = L" \n\t\r";
    const auto strBegin = str.find_first_not_of(charsToTrim);
    if (strBegin == std::wstring::npos)
        return L"";
    const auto strEnd = str.find_last_not_of(charsToTrim);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

//trim string function
static std::string trim(std::string str)
{
	const auto strBegin = str.find_first_not_of(" ");
	if (strBegin == std::string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(" ");
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

//replace string function
static std::string replace(std::string str, std::string from, std::string to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

//replace string function
static std::string replace2(std::string str, std::string from, std::string to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos = 0; 
    }
    return str;
}

//binaryFromFile
static const char* binaryFromFile(std::string file)
{
	std::ifstream fileStream(file, std::ios::in | std::ios::binary);
	if (fileStream)
	{
		std::stringstream sstr;
		sstr << fileStream.rdbuf();
        return sstr.str().c_str();
	}
	else
	{
		return "";
	}
}

//read string from file
static std::string readStringFromFile(std::string filename) {
	
	std::ifstream file(filename);

    //check file exist
    if (file.fail()) {
        std::cout << "File not exist:"<<filename << std::endl;
    }
	
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

//write string to file
static void writeStringToFile(std::string filename, std::string content) {
	std::ofstream file(filename);
	file << content;
}

static void writeWstringToFile(std::wstring filename, std::wstring content) {
    std::wofstream file(filename);
    file << content;
}

//write binary to file
static void writeBinaryToFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "Error: Could not open file '" << filename << "' for writing" << std::endl;
        return;
    }
    file.seekp(0);
    file.write(content.c_str(), content.length());
    file.close();
}

//write string to file append
static void writeStringToFileAppend(std::string filename, std::string content) {
	std::ofstream file(filename, std::ios::app);
	file << content;
}

static std::string generateRandomFilename() {
	std::string filename = "";
	for (int i = 0; i < 10; i++) {
		filename += (char)('a' + rand() % 26);
	}
	return filename;
}

//check file exist function
static bool fileExist(const std::string& fileName) {
    std::ifstream infile(fileName);
    return infile.good();
}


static const char* get_content_type(const char* path) {

    std::string _path(path);
    size_t s = _path.find("?");

    if (s != std::string::npos) {
        _path.erase(s);
    }

    if (_path.find(".") != std::string::npos) {
        if (_path.find(".css") != std::string::npos) return "text/css";
        if (_path.find(".csv") != std::string::npos) return "text/css";
        if (_path.find(".gif") != std::string::npos) return "image/gif";
        if (_path.find(".htm") != std::string::npos) return "text/html";
        if (_path.find(".html") != std::string::npos) return "text/html";
        if (_path.find(".ico") != std::string::npos) return "image/x-icon";
        if (_path.find(".jpeg") != std::string::npos) return "image/jpeg";
        if (_path.find(".jpg") != std::string::npos) return "image/jpeg";
        if (_path.find(".js") != std::string::npos) return "text/javascript";
        if (_path.find(".json") != std::string::npos) return "application/json";
        if (_path.find(".png") != std::string::npos) return "image/png";
        if (_path.find(".pdf") != std::string::npos) return "application/pdf";
        if (_path.find(".svg") != std::string::npos) return "image/svg+xml";
        if (_path.find(".txt") != std::string::npos) return "text/plain";
        if (_path.find(".ttf") != std::string::npos) return "application/octet-stream";
        if (_path.find(".xlsx") != std::string::npos) return "application/octet-stream";
        if (_path.find(".woff") != std::string::npos) return "font/woff";
        if (_path.find(".woff2") != std::string::npos) return "font/woff2";
    }


    return "";
}

// Random string generator function.
static std::string generateRandom(int length)
{
	
    std::string alphanum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int stringLength = alphanum.length();
	
    std::string str = "";
    srand(time(NULL));
    for (int i = 0; i < length; i++) {
        str += static_cast<char>(alphanum.at(rand() % stringLength));
    }

    return str;
}