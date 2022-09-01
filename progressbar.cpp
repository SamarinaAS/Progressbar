#include <curl/curl.h>
#include <stdio.h>
#include <wx/progdlg.h>
#include <wx/wx.h>
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL 3000000
#define STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES 150000000
const char* str = "https://99px.ru/sstorage/86/2017/11/image_862611170051343932802.gif";

bool stop_flag = false;
std::atomic<unsigned int> percent_glob{0};
struct myprogress {
    curl_off_t lastruntime;
    CURL *curl;
};

void countPercent(unsigned long dlnow, unsigned long dltotal) {
    if ((unsigned long)dltotal != 0) {
        percent_glob = (unsigned int)((double)dlnow / dltotal * 100);
    }
}

static int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal,
                    curl_off_t ulnow) {
    struct myprogress *myp = (struct myprogress *)p;
    CURL *curl = myp->curl;
    curl_off_t curtime = 0;

    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME_T, &curtime);

    if ((curtime - myp->lastruntime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL) {
        myp->lastruntime = curtime;
    }
    countPercent(dlnow, dltotal);
    if (dlnow > STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES||stop_flag){
        return 1;
    }
    return 0;
}

void myfunction() {
    static const char *pagefilename = "file.out";
    FILE *pagefile;
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct myprogress prog;

    curl = curl_easy_init();
    if (curl) {
        prog.lastruntime = 0;
        prog.curl = curl;

        curl_easy_setopt(curl, CURLOPT_URL, str);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &prog);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

        pagefile = fopen(pagefilename, "wb");
        if (pagefile) {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, pagefile);
            res = curl_easy_perform(curl);
            std::cout << "\n";
            if (res != CURLE_OK) fprintf(stderr, "%s\n", curl_easy_strerror(res));

            fclose(pagefile);
        }
        curl_easy_cleanup(curl);
    }
}

class myApp : public wxApp {
   public:
    bool OnInit(void);
    int OnExit(void);
    void OnQuit(wxCommandEvent & event);
    void Download(wxCommandEvent & WXUNUSED(event));
    wxPanel* panel;
    wxTextCtrl* text;
    wxFrame *frame;
    wxProgressDialog *dialog;
    enum {ID_Download=wxID_HIGHEST + 1, ID_Cancel};
};

IMPLEMENT_APP(myApp)

bool myApp::OnInit() {
    frame = new wxFrame(NULL, wxID_ANY, wxT("Loader"));
    panel=new wxPanel(frame,-1);
    this->SetTopWindow(frame);
    frame->Show(true);
    this->SetTopWindow(panel);
    panel->Show(true);
    
    text=new wxTextCtrl(panel,-1,"https://media2.giphy.com/media/er7RmM5FjvHHajU8R2/giphy-downsized-large.gif",wxPoint(20,25),wxSize(350,30));

    wxButton *download = new wxButton(panel, ID_Download, wxT("Загрузить"), wxPoint(20, 60));
    Connect(ID_Download, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(myApp::Download));
    download->SetFocus();
    return true;
}

void myApp::Download(wxCommandEvent & WXUNUSED(event)){
    wxTextCtrl *message = new wxTextCtrl (panel, wxID_ANY, "Download started", wxPoint(20, 100), wxSize(350, 30), wxTE_READONLY);
    stop_flag = false;
    std::string temp_str = std::string(text->GetValue());
    str = temp_str.c_str();
    int i, max = 100;
    dialog = new wxProgressDialog(wxT("Wait: downloading"), wxT("percent: 0"),
                                                    max, panel, wxPD_CAN_ABORT | wxPD_AUTO_HIDE);
    dialog->SetEscapeId(wxPD_CAN_ABORT);
    std::thread tr1(myfunction);
    dialog->Update(0, "0");
    percent_glob = 0;
    unsigned int temp = 0;
    do {
        if (dialog->WasCancelled()){
            stop_flag = true;
            tr1.join();
            message->ChangeValue("Download interrupted");
            delete dialog;
            return;
        }	
        temp = percent_glob;
        std::string temp_string = std::to_string(percent_glob) + " %";
        wxString temp_wxstring(temp_string);
        dialog->Update(temp, temp_wxstring);
        
    } while (temp != 100);
    dialog->Update(max);
    tr1.join();
    message->ChangeValue("Download has finished successfully");
    delete dialog;    
}

int myApp ::OnExit() { return 0; }
