#pragma once

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>

class Json:public rapidjson::Document
{
public:
	Json(const char* str) {
		
		if (Parse(str).HasParseError()) {
			//fprintf(stderr, "\nError(offset %u): %s\n",(unsigned)d.GetErrorOffset(),GetParseError_En(d.GetParseError()));
			std::cout << "Json::Json Parse Error." << std::endl;
		}
		
	}

	Json() {
	}

	void parse(const char* str) {

		if (Parse(str).HasParseError()) {
			//fprintf(stderr, "\nError(offset %u): %s\n",(unsigned)d.GetErrorOffset(),GetParseError_En(d.GetParseError()));
			std::cout<<"Json::Json Parse Error."<<std::endl;
		}

	}

	~Json() {
		
	}
	
	std::string toString() {
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		Accept(writer);
		return buffer.GetString();
	}
};