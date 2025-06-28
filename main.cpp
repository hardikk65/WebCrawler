#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>




// Stores chunk in a string
size_t write_callback(char* data, size_t size, size_t nmemb, void* userdata){

	std::string* response = static_cast<std::string*>(userdata);
	response->append(data,nmemb);
	return nmemb;
	
}

CURL* init_easy(){
	return curl_easy_init();
}


// Parsing links
static void search_for_links(GumboNode* node) {

	if (node->type != GUMBO_NODE_ELEMENT) {
		return;
	}
	GumboAttribute* href;
	if (node->v.element.tag == GUMBO_TAG_A &&
		(href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
		std::cout << href->value << std::endl;
	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i) {
		search_for_links(static_cast<GumboNode*>(children->data[i]));
	}
}



int main(){


	CURL *curl;
	CURLcode result;

	std::string url;
	std::string response;


	std::cout<<"Enter URL to crawl: ";
	std::cin>>url;


	// Initialising curl_easy
	curl = init_easy();

	if(curl == NULL){
		std::cerr<<"Curl Request failed\n";
		return -1;
	}

	

	/* Specify the URL */
	curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
	/*Add data to this callback */
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_callback);
	/* Writes data in response*/
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response);
	
	/* Add user agen*/
	curl_easy_setopt(curl,CURLOPT_USERAGENT,"libcurl-agent/1.0");

	result = curl_easy_perform(curl);

	if(result != CURLE_OK){
		std::cerr<<"Some Error occured\n";
		return -1;
	}

	GumboOutput* output = gumbo_parse(response.c_str());
	search_for_links(output->root);

	gumbo_destroy_output(&kGumboDefaultOptions,output);
	curl_easy_cleanup(curl);
}

