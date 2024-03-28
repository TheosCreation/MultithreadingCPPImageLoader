#pragma once

#define CURL_STATICLIB //removing this will give link warnings
#include <curl/curl.h>
#include <string.h>
#include <iostream>

//this class will download fromm urls to a specified string.
class CDownloader
{
public:
	CDownloader();
	~CDownloader();
	void Init();

	bool Download(const char * _url, std::string& _outputStr);

	bool DownloadToFile(const char* _url, const char* _outputFile);

private:
	bool m_globalInit;
	static size_t WriteData(void* _buffer, size_t _size, size_t _nmemb, void* _param);

};