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
#define codeor "%7C"
#define codeopenpar "%28"
#define codeclosepar "%29"

string wolfram_compute(string message);

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

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct buffer *s) {
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

string normalize_expression(string expression){

    string ret = "";

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
            case '|':
                ret += codeor;
                break;
            case '(':
                ret += codeopenpar;
                break;
            case ')':
                ret += codeclosepar;
                break;
            default:
                ret += expression[i];
        }
    }
    return ret;
}

void replaceAll(string& str, const string& from, const string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

string integrate(string expression){
    string ret = "http://api.wolframalpha.com/v2/result?appid=" + WolframAppID + "&i=integrate%20";
    ret += normalize_expression(expression);
    return wolfram_compute(ret);
}

string convolve(string exp1, string exp2){
    string ret = "http://api.wolframalpha.com/v2/result?appid=" + WolframAppID + "&i=";
    ret += normalize_expression("convolve(" + exp1 + "," + exp2 + ")");
    return wolfram_compute(ret);
}

void prepare_curl(){
    curl_global_init(CURL_GLOBAL_ALL);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
}

void cleanup(){
    curl_global_cleanup();
}

buffer b;

string wolfram_compute(string message){

    CURLcode res;
    string ans = "";

    curl = curl_easy_init();
    if(curl) {

        init_buffer(&b);
//        cout << cast_to_integral_URL(message) << endl;
        curl_easy_setopt(curl, CURLOPT_URL, message.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &b);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

//        printf("%s", b.ptr);
        curl_easy_cleanup(curl);
        return string(b.ptr);
    }
    return ans;
}