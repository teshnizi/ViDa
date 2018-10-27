#include <iostream>
#include <curl/curl.h>
#include <string.h>
#include <sstream>


using namespace std;

CURL* curl;

string WolframAppID = "H4K447-WA9Y2J6P79";

#define codepow "%5E"
#define codemult "%2A"
#define codeadd "%2B"
#define codesub "-"
#define codediv "%2F"
#define codespace "%20"


struct buffer {
    char *ptr;
    size_t len;
};

void init_buffer(struct buffer *s) {
    s->len = 0;
    s->ptr = (char*)malloc(s->len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct buffer *s)
{
    size_t new_len = s->len + size*nmemb;
    s->ptr = (char*)realloc(s->ptr, new_len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size*nmemb;
}

string cast_to_integral_URL(string expression){
    string ret = "http://api.wolframalpha.com/v2/query?appid=" + WolframAppID + "&input=integrate%20";
    for (int i = 0; i < expression.length(); i++){
        switch (expression[i]){
            case ' ':
                ret += codespace;
                break;
            case '^':
                ret += codepow;
                break;
            case '*':
                ret += codemult;
                break;
            case '-':
                ret += codesub;
                break;
            case '+':
                ret += codeadd;
                break;
            case '/':
                ret += codediv;
                break;
            default:
                ret += expression[i];
        }
    }
    return ret+"&includepodid=IndefiniteIntegral";
}

void prepare_curl(){
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
}

void cleanup(){
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

string integrate(string message){

    CURL *curl;
    CURLcode res;
    string ans;

    curl = curl_easy_init();
    if(curl) {
        string s = cast_to_integral_URL(message);
        cout << s << endl;
        buffer b;

        curl_easy_setopt(curl, CURLOPT_URL, cast_to_integral_URL(message).c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &b);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

//        printf("%s", b.ptr);
        stringstream ss(b.ptr);
        string line;
        while (getline(ss, line)){
            if (line.length() < 25)continue;
            if (line[14] == 'i' &&
                        line[15] == 'n' &&
                        line[16] == 't' &&
                        line[17] == 'e' &&
                        line[18] == 'g'){
                int it = 14;
                while (line[it++] != '=');
                it++;
                int it2 = it;
                cout << line << endl;
                while (!(line[it2++] == 'c' && line[it2+1] != 'o'));
                ans = line.substr(it, it2 - it - 4);
                break;
            }
        }
        curl_easy_cleanup(curl);
    }
    return ans;
}