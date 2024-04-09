#include "Downloader.h"



CDownloader::CDownloader()
{
}

CDownloader::~CDownloader()
{
}

void CDownloader::Init()
{
	if (m_globalInit == false)
	{
		curl_global_init(CURL_GLOBAL_DEFAULT);
		m_globalInit = true;
	}
	
}

// downloads from the url and saves it in memory
bool CDownloader::Download(const char* _url, std::string& _outputStr)
{
	CURL* m_curl = curl_easy_init();
	if (m_curl)
	{
		CURLcode res;
		curl_easy_setopt(m_curl, CURLOPT_URL, _url);
		curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, CDownloader::WriteData);
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &_outputStr);
		curl_easy_setopt(m_curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		//curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L); //uncomment for debug
		res = curl_easy_perform(m_curl);
		if (res != CURLE_OK)
		{
			std::cerr << " CURL error: " << res << "\n";
		}
		curl_easy_cleanup(m_curl);
		return res == CURLE_OK;
	}
	return false;
}

// downloads from the url and saves it to a file location
bool CDownloader::DownloadToFile(const char* _url, const char* _outputFile)
{
	CURL* m_curl = curl_easy_init();
	if (m_curl)
	{
		CURLcode res;
		FILE* fp;
		fopen_s(&fp, _outputFile, "wb");
		curl_easy_setopt(m_curl, CURLOPT_URL, _url);
		curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(m_curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		res = curl_easy_perform(m_curl);
		fclose(fp);
		if (res != CURLE_OK) {
			std::cerr << "CURL error: " << res << "\n";
		}
		curl_easy_cleanup(m_curl);
		return res == CURLE_OK;
	}
	return false;
}

size_t CDownloader::WriteData(void* _buffer, size_t _size, size_t _nmemb, void* _param)
{
	std::string& text = *static_cast<std::string*>(_param);
	size_t totalsize = _size * _nmemb;
	text.append(static_cast<char*>(_buffer), totalsize);
	return totalsize;
}
