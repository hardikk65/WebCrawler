#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <gumbo.h>
#include <queue>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

/* Fixed Global Variables */
int urls_crawled = 0;
const int MAX_CRAWL = 100;
std::queue<std::string> response_queue;
std::unordered_set<std::string> visited_urls;
std::unordered_set<std::string> crawled_urls;
std::mutex m;
std::condition_variable cv;
bool done = false; 






/* Function declarations*/
size_t write_callback(char* data, size_t size, size_t nmemb, void* userdata);
static void search_for_links(GumboNode* node, std::vector<std::string>* urls);
CURLcode get_response(CURL* curl, std::string& url, std::string& response);
void crawl_worker();





int main() {
    std::string url;
    std::cout << "Enter URL to crawl: ";
    std::cin >> url;

    {
        std::lock_guard<std::mutex> lock(m);
        response_queue.push(url);
        visited_urls.insert(url);
    }

    const int NUM_THREADS = 8;
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(crawl_worker);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "\n\nCrawled " << urls_crawled << " URLs\n";
    std::cout << "Discovered " << visited_urls.size() << " unique URLs\n";
    return 0;
}





/* Function definitions*/

size_t write_callback(char* data, size_t size, size_t nmemb, void* userdata) {
    std::string* response = static_cast<std::string*>(userdata);
    size_t total_size = size * nmemb;
    response->append(data, total_size);
    return total_size;
}

static void search_for_links(GumboNode* node, std::vector<std::string>* urls) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }
    
    GumboAttribute* href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
        if (href->value) {
            urls->push_back(href->value);
        }
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        search_for_links(static_cast<GumboNode*>(children->data[i]), urls);
    }
}

CURLcode get_response(CURL* curl, std::string& url, std::string& response) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);  // 10-second timeout
    
    return curl_easy_perform(curl);
}

void crawl_worker() {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "CURL initialization failed in thread " << std::this_thread::get_id() << std::endl;
        return;
    }

    while (true) {
        std::string parent_url;
        
        {
            std::unique_lock<std::mutex> lock(m);
            
            cv.wait(lock, [] { 
                return !response_queue.empty() || done; 
            });
            

            if (done && response_queue.empty()) {
                break;
            }
            
            if (urls_crawled >= MAX_CRAWL) {
                done = true;
                cv.notify_all();
                break;
            }
            
            parent_url = response_queue.front();
            response_queue.pop();
            crawled_urls.insert(parent_url);
            urls_crawled++;
        }

        std::cout << "[Thread " << std::this_thread::get_id() 
                  << "] Crawling: " << parent_url 
                  << " (" << urls_crawled << "/" << MAX_CRAWL << ")\n";

        std::string response;
        CURLcode result = get_response(curl, parent_url, response);
        
        if (result != CURLE_OK) {
            std::cerr << "CURL error (" << result << "): " 
                      << curl_easy_strerror(result) << " - " << parent_url << std::endl;
            continue;
        }

        GumboOutput* output = gumbo_parse(response.c_str());
        std::vector<std::string> child_urls;
        search_for_links(output->root, &child_urls);
        gumbo_destroy_output(&kGumboDefaultOptions, output);

        {
            std::lock_guard<std::mutex> lock(m);
            for (const auto& link : child_urls) {
                // Skip if we've reached max crawl
                if (urls_crawled >= MAX_CRAWL) {
                    done = true;
                    break;
                }
                
                // Filter valid URLs
                if ((link.find("http://") == 0 || link.find("https://") == 0) &&
                    visited_urls.find(link) == visited_urls.end()) {
                    
                    response_queue.push(link);
                    visited_urls.insert(link);
                }
            }
        }
        
        cv.notify_all();
    }
    
    curl_easy_cleanup(curl);
}