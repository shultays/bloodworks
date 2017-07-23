
#pragma warning(disable: 4996)

#include "DirentHelper.h"
#include "cPackHelper.h"
#include <iostream>

#define CURL_STATICLIB 
#include <curl/curl.h>


bool isTypeCorrect(const DirentHelper::File& f)
{
	return f.isTypeOf("png") || f.isTypeOf("json") || f.isTypeOf("ogg") || f.isTypeOf("lua");
}

void sendFile(const std::string& file)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL *curl = curl_easy_init();
	if (curl) {

		struct curl_httppost *formpost = NULL;
		struct curl_httppost *lastptr = NULL;
		struct curl_slist *headerlist = NULL;

		/* Fill in the filename field */
		curl_formadd(&formpost,
			&lastptr,
			CURLFORM_COPYNAME, "userfile",
			CURLFORM_FILE, file.c_str(),
			CURLFORM_END);

		/* Fill in the submit field too, even if this is rarely needed */
		curl_formadd(&formpost,
			&lastptr,
			CURLFORM_COPYNAME, "upload",
			CURLFORM_COPYCONTENTS, "upload",
			CURLFORM_END);


		curl_easy_setopt(curl, CURLOPT_URL, "http://bloodworks.enginmercan.com/upload.php");

		//curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

		/* Perform the request, res will get the return code */
		CURLcode res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);

		/* then cleanup the formpost chain */
		curl_formfree(formpost);
		/* free slist */
		curl_slist_free_all(headerlist);
	}
	curl_global_cleanup();


}

int main(int argn, const char* argv[])
{
	std::string path;
	
	if (argn > 1)
	{
		path = argv[1];
	}
	if (path.length() == 0)
	{
		std::cin >> path;
	}

	DirentHelper::Folder folder("guns/rifle");
	std::vector<DirentHelper::File> files = folder.getAllFiles(true);
	bool allValid = true;
	for (auto& f : files)
	{
		if (isTypeCorrect(f) == false)
		{
			allValid = false;
			std::cout << "Invalid type : " << f.folder + f.file << std::endl;
		}
		else
		{
			std::cout << "valid : " << f.folder + f.file << std::endl;
		}
	}
	if (allValid)
	{
		std::string tempName = "temp.bld";
		cPackHelper::packFolder(folder.name, tempName);
		sendFile(tempName);
		cPackHelper::deleteFile(tempName);
	}

	std::cin >> path;
    return 0;
}

