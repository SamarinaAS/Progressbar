#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <thread>
#include <chrono>
 
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3000000
#define STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES         150000000

bool flag = false; 
struct myprogress {
  curl_off_t lastruntime; 
  CURL *curl;
};
 

static int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow){  
  struct myprogress *myp = (struct myprogress *)p;
  CURL *curl = myp->curl;
  curl_off_t curtime = 0;
 
  curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME_T, &curtime);
  if (flag == false){
      std::cout << "pid xferinfo: "<<std::this_thread::get_id() <<std::endl;
      std::cout<<"persent: "<<std::endl;
      flag = true;
  }

  if((curtime - myp->lastruntime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL) {
    myp->lastruntime = curtime;
//    fprintf(stderr, "TOTAL TIME: %lu.%06lu\r\n",
//            (unsigned long)(curtime / 1000000),
//            (unsigned long)(curtime % 1000000));
  }
 
  //fprintf(stderr, "UP: %lu of %lu  DOWN: %lu of %lu\r\n",
  //        (unsigned long)ulnow, (unsigned long)ultotal,
  //        (unsigned long)dlnow, (unsigned long)dltotal);
  //std::this_thread::sleep_for(std::chrono::nanoseconds(700000000));
  if ((unsigned long) dltotal!=0){
  unsigned long div = (unsigned long)((double)dlnow/dltotal*100);
  std::cout<< "\r"<<div/*<<"\n"*/<<std::flush;
  //fprintf(stderr, "\r %lu",
  //      div);
        }
  //std::cout << '\r' << dlnow/dltotal << std::flush;
 
  if(dlnow > STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES)
    return 1;
  return 0;
}

void myfunction (){
  static const char *pagefilename = "page.out";
  FILE *pagefile;
  CURL *curl;
  CURLcode res = CURLE_OK;
  struct myprogress prog;
 
  curl = curl_easy_init();
  if(curl) {
    prog.lastruntime = 0;
    prog.curl = curl;
 
    //curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/");
    curl_easy_setopt(curl, CURLOPT_URL, "https://media2.giphy.com/media/er7RmM5FjvHHajU8R2/giphy-downsized-large.gif"); 
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &prog); 
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    
    pagefile = fopen(pagefilename, "wb");
    if(pagefile) {
 

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, pagefile);
    //curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)100000);
    //std::cout<<"persent:"<<std::endl;
    res = curl_easy_perform(curl);
     std::cout<< "\n";
    if(res != CURLE_OK)
      fprintf(stderr, "%s\n", curl_easy_strerror(res));
 

    fclose(pagefile);
    }
    curl_easy_cleanup(curl);
    std::cout << "pid myfunction: "<< std::this_thread::get_id() <<std::endl;
  }
  //return (int)res;
}
 
int main(void)
{
  std::cout << "pid main: "<< std::this_thread::get_id() <<std::endl;
  std::thread tr1 (myfunction);
  tr1.join();
  return 0;
}
