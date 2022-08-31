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

bool flag = false;
bool stop_flag = false;
std::atomic<unsigned int> percent_glob{0};
struct myprogress {
    curl_off_t lastruntime;
    CURL *curl;
};

void print_percent(unsigned long dlnow, unsigned long dltotal) {
    //  if (flag == false){
    //    std::cout << "pid xferinfo: "<<std::this_thread::get_id() <<std::endl;
    //    std::cout<<"persent: "<<std::endl;
    //    flag = true;
    //}
    if ((unsigned long)dltotal != 0) {
        percent_glob = (unsigned int)((double)dlnow / dltotal * 100);
        // std::cout<< "\r"<<percent_glob/*<<"\n"*/<<std::flush;
        // fprintf(stderr, "\r %lu",
        //      div);
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
        //    fprintf(stderr, "TOTAL TIME: %lu.%06lu\r\n",
        //            (unsigned long)(curtime / 1000000),
        //            (unsigned long)(curtime % 1000000));
    }

    // fprintf(stderr, "UP: %lu of %lu  DOWN: %lu of %lu\r\n",
    //        (unsigned long)ulnow, (unsigned long)ultotal,
    //        (unsigned long)dlnow, (unsigned long)dltotal);
    // std::this_thread::sleep_for(std::chrono::nanoseconds(700000000));
    //std::cout<<percent_glob<<std::endl;
    print_percent(dlnow, dltotal);
    // if ((unsigned long) dltotal!=0){
    // unsigned long div = (unsigned long)((double)dlnow/dltotal*100);
    // std::cout<< "\r"<<div/*<<"\n"*/<<std::flush;
    // fprintf(stderr, "\r %lu",
    //      div);
    //      }
    // std::cout << '\r' << dlnow/dltotal << std::flush;

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

        // curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/");
        //curl_easy_setopt(curl, CURLOPT_URL,
        //    "https://media2.giphy.com/media/er7RmM5FjvHHajU8R2/giphy-downsized-large.gif");
        curl_easy_setopt(curl, CURLOPT_URL, str);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &prog);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

        pagefile = fopen(pagefilename, "wb");
        if (pagefile) {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, pagefile);
            // curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)100000);
            // std::cout<<"persent:"<<std::endl;
            res = curl_easy_perform(curl);
            std::cout << "\n";
            if (res != CURLE_OK) fprintf(stderr, "%s\n", curl_easy_strerror(res));

            fclose(pagefile);
        }
        curl_easy_cleanup(curl);
        // std::cout << "pid myfunction: "<< std::this_thread::get_id() <<std::endl;
    }
    // return (int)res;
}

// int main(void)
//{
//  //std::cout << "pid main: "<< std::this_thread::get_id() <<std::endl;
//  std::thread tr1 (myfunction);
//  std::cout<<"percent: "<<std::endl;
// while (percent_glob !=100){
//    std::cout<< "\r"<<percent_glob/*<<"\n"*/<<std::flush;
//}
//  unsigned int temp = percent_glob;
//  do{
//    temp = percent_glob;
//    std::cout<< "\r"<<temp/*<<"\n"*/<<std::flush;
//  }while(temp !=100);
//  tr1.join();
//  return 0;
//}

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
    //wxStaticText* staticText;
};

IMPLEMENT_APP(myApp)

bool myApp::OnInit() {
    
    // std::cout << "pid main: "<< std::this_thread::get_id() <<std::endl;
    
    // std::cout<<"percent: "<<std::endl;
    // while (percent_glob !=100){
    //    std::cout<< "\r"<<percent_glob/*<<"\n"*/<<std::flush;
    //}

    //wxFrame* frame = new wxFrame(NULL, wxID_ANY, wxT("Loader"));
    frame = new wxFrame(NULL, wxID_ANY, wxT("Loader"));
    //frame=new wxFrame(0,-1,"wxTextEntryDialog",wxPoint(10,10),wxSize(320,200),wxCAPTION | wxSYSTEM_MENU);
    panel=new wxPanel(frame,-1);
    this->SetTopWindow(frame);
    frame->Show(true);
    this->SetTopWindow(panel);
    panel->Show(true);
    
    text=new wxTextCtrl(panel,-1,"https://media2.giphy.com/media/er7RmM5FjvHHajU8R2/giphy-downsized-large.gif",wxPoint(20,25),wxSize(350,20));

    
    //wxButton *download = new wxButton(frame, ID_Download, wxT("Загрузить"), wxPoint(20, 25),
    //                               wxSize(10,2), 0, wxDefaultValidator, wxT("download"));
    wxButton *download = new wxButton(panel, ID_Download, wxT("Загрузить"), wxPoint(20, 60));
    Connect(ID_Download, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(myApp::Download));
    download->SetFocus();
    //wxButton *download = new wxButton(frame, wxID_HIGHEST + 2, wxT("Загрузить"), wxPoint(120, 25),
    //                                wxSize(10,2), 0, wxDefaultValidator, wxT("download"));
                                    
    // dialog->AddMainButtonId(wxID_HIGHEST + 1);
    // for(int i = 0; i < max; i++){
    //   wxMilliSleep(500); //here are computations
    //   if(i%23) dialog->Update(i);
    //}
    

    return true;
}

void myApp::Download(wxCommandEvent & WXUNUSED(event)){
    //wxStaticText* staticText = new wxStaticText(panel, wxID_ANY, "Загрузка начата");
    //staticText->Show();
    stop_flag = false;
    std::string temp_str = std::string(text->GetValue());
    str = temp_str.c_str();
    //const char* str = std::string(text->GetValue()).c_str();
    //strcpy(str, str_const);
    int i, max = 100;
    dialog = new wxProgressDialog(wxT("Wait: downloading"), wxT("percent: 0"),
                                                    max, panel, wxPD_CAN_ABORT | wxPD_AUTO_HIDE);
    //wxButton *cancel = new wxButton(dialog, ID_Cancel, wxT("Отменить"), wxPoint(120, 25),
    //                                wxDefaultSize, 0, wxDefaultValidator, wxT("cancel"));
    //this->SetTopWindow(dialog);
   // dialog->Show(true);
    //dialog->Resume();
    dialog->SetEscapeId(wxPD_CAN_ABORT);
    //Connect(ID_Cancel, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(myApp::OnQuit));
   // cancel->SetFocus();
    std::thread tr1(myfunction);
    //dialog->ShowModal();
    //std::cout<<percent_glob<<std::endl;
    dialog->Update(0, "0");
    percent_glob = 0;
    unsigned int temp = 0;
    //temp = percent_glob;
    do {
        if (dialog->WasCancelled()){
            stop_flag = true;
            tr1.join();
            //staticText->SetLabelText("Загрузка прервана");
            delete dialog;
            return;
        }	
        temp = percent_glob;
        //std::cout<<percent_glob<<std::endl;
        std::string temp_string = std::to_string(percent_glob) + " %";
        wxString temp_wxstring(temp_string);
        // std::cout<< "\r"<<temp/*<<"\n"*/<<std::flush;
        dialog->Update(temp, temp_wxstring);
        
    } while (temp != 100);
    dialog->Update(max);
    tr1.join();
    //dialog->Resume();
    //dialog -> Destroy();
    //dialog->Resume ();
    //staticText->SetLabelText("Загрузка завершена");
    delete dialog;    
}

//void myApp::OnQuit(wxCommandEvent & WXUNUSED(event))
//{
//     dialog->Close();
//}

int myApp ::OnExit() { return 0; }
