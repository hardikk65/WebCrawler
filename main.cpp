#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <gumbo.h>
#include <queue>
#include <unordered_set>


/* Variable Declarations */
int urls_crawled = 0;
std::queue<std::string> response_queue;
std::unordered_set<std::string> visited_urls;



/* Function declarations*/
size_t write_callback(char* data, size_t size, size_t nmemb, void* userdata);

CURL* init_easy();

static void search_for_links(GumboNode* node, std::vector<std::string>* urls);

CURLcode get_response(CURL* curl, std::string& url, std::string& response);



int main(){


	CURL* curl;
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

	response_queue.push(url);
	visited_urls.insert(url);

	/* Start BFS Traversal of URLs*/
	while(!response_queue.empty() && urls_crawled <= 100){

		std::string parent_url = response_queue.front();
		response_queue.pop();


		result = get_response(curl,parent_url,response);
		if(result != CURLE_OK)continue;
		++urls_crawled;



		std::vector<std::string> child_urls;
		GumboOutput* output = gumbo_parse(response.c_str());
		search_for_links(output->root,&child_urls);
		gumbo_destroy_output(&kGumboDefaultOptions,output);



		for(const auto &href: child_urls){
			if(href.find("http://") == 0 || href.find("https://") == 0){
				if(visited_urls.find(href) == visited_urls.end()){
					response_queue.push(href);
					visited_urls.insert(href);
				}
			}
		}


		response.clear();
	}

	std::cout << "URLs Crawled: " << urls_crawled << '\n';


	std::cout << "\n\n\n";

	for(auto it = visited_urls.begin();it != visited_urls.end();++it){
		std::cout << *it << "\n";
	}

	curl_easy_cleanup(curl);

}



/* Function definitions */

size_t write_callback(char* data, size_t size, size_t nmemb, void* userdata){

	std::string* response = static_cast<std::string*>(userdata);
	response->append(data,nmemb);
	return nmemb;
	
}

CURL* init_easy(){
	return curl_easy_init();
}

static void search_for_links(GumboNode* node,std::vector<std::string>* urls) {

	if (node->type != GUMBO_NODE_ELEMENT) {
		return;
	}
	GumboAttribute* href;
	if (node->v.element.tag == GUMBO_TAG_A &&
		(href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
		if(href && href->value && std::string(href->value)[0] != '#' && std::string(href->value)[0] != '/')urls->push_back(href->value);
	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i) {
		search_for_links(static_cast<GumboNode*>(children->data[i]),urls);
	}
}

CURLcode get_response(CURL* curl,std::string& url,std::string& response){
	
	curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_callback);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response);
	curl_easy_setopt(curl,CURLOPT_USERAGENT,"libcurl-agent/1.0");
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); 
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, ""); 

	return curl_easy_perform(curl);

}