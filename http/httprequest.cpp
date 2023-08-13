/*
 * @Author       : mark
 * @Date         : 2020-06-26
 * @copyleft Apache 2.0
 */
#include "httprequest.h"

#include <algorithm>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <assert.h>
#include<stdio.h>
#include<vector>
#include <dirent.h>
#include <iostream>

using namespace std;

const unordered_set<string> HttpRequest::DEFAULT_HTML{
            "/index", "/register", "/login",
             "/welcome", "/video", "/picture", };

const unordered_set<string> HttpRequest::AVAILABLE_FILES{
        "/index.html", };

const unordered_map<string, int> HttpRequest::DEFAULT_HTML_TAG {
            {"/register.html", 0}, {"/login.html", 1},  };

void HttpRequest::Init() {
    srcDir = getcwd(nullptr, 256); // get current working directory
    strncat(srcDir, "/resources/files/", 16);
    method_ = path_ = version_ = body_ = "";
    state_ = REQUEST_LINE;
    header_.clear();
    post_.clear();
}

bool HttpRequest::IsKeepAlive() const {
    if(header_.count("Connection") == 1) {
        return header_.find("Connection")->second == "keep-alive" && version_ == "1.1";
    }
    return false;
}

bool HttpRequest::parse(Buffer& buff) { // readBuff
    const char CRLF[] = "\r\n";
    if(buff.ReadableBytes() <= 0) {
        return false;
    }

    // handle one line for each single loop
    while(buff.ReadableBytes() && state_ != FINISH) {
        // sizeof("\r\n") = 24
        // from the perspective of string length, "\r\n".length() = 2
        const char* lineEnd = search(buff.Peek(), buff.BeginWriteConst(), CRLF, CRLF + 2);
        std::string line(buff.Peek(), lineEnd); // does not contain /r/n
        switch(state_)
        {
        case REQUEST_LINE:
            if(!ParseRequestLine_(line)) {
                return false;
            }
            ParsePath_();
            break;    
        case HEADERS: // loop read multiple lines
            ParseHeader_(line);
            if(buff.ReadableBytes() <= 2) {
                state_ = FINISH;
            }
            break;
        case BODY:
            ParseBody_(line);
            break;
        case FILE:
            ParseBody_(line);
            break;
        default:
            break;
        }
        if(lineEnd == buff.BeginWrite()) { break; }
        buff.RetrieveUntil(lineEnd + 2); // line + /r/n
    }
    LOG_DEBUG("[%s], [%s], [%s]", method_.c_str(), path_.c_str(), version_.c_str());
    return true;
}

void HttpRequest::ParsePath_() {
    if(path_ == "/") {
        path_ = "/index.html"; 
    }
    else {
        for(auto &item: AVAILABLE_FILES) {
            if(item == path_) {
                path_ = item; // define the final file to send
                break;
            }
        }
    }
}

bool HttpRequest::ParseRequestLine_(const string& line) {
    regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    smatch subMatch;
    if(regex_match(line, subMatch, patten)) {   
        method_ = subMatch[1];
        path_ = subMatch[2];
        version_ = subMatch[3];
        state_ = HEADERS;
        return true;
    }
    LOG_ERROR("RequestLine Error");
    return false;
}

void HttpRequest::ParseHeader_(const string& line) {
    regex patten("^([^:]*): ?(.*)$");
    smatch subMatch;
    if(regex_match(line, subMatch, patten)) {
        header_[subMatch[1]] = subMatch[2];
        int n = line.find("boundary");
        if (n != string::npos) {
            int m;
            if ( m = line.find_last_of("=") != string::npos) {
                header_["boundary"] = line.substr(m + 1, line.length());
            }
        }
    }
    else {
        state_ = BODY;
    }
}

void HttpRequest::ParseBody_(const string& line) {
    body_ = line;

    if (state_ == FILE) {
        if (fileParseState != DONE) {
            ParsePost_();
            return;
        }
        state_ = FINISH;
        return;
    }

    ParsePost_();

    LOG_DEBUG("Body:%s, len:%d", line.c_str(), line.size());
}

int HttpRequest::ConverHex(char ch) {
    if(ch >= 'A' && ch <= 'F') return ch -'A' + 10;
    if(ch >= 'a' && ch <= 'f') return ch -'a' + 10;
    return ch;
}

/**
 * for POST request
 */
void HttpRequest::ParsePost_() {

    // no matter the upload is success or not, always return the index.html
    if (method_ == "POST" && header_["Content-Type"] == "multipart/form-data") {
        // body_ == line
        state_ = FILE;
        if (fileParseState == PENDING) {
            fileParseState = HEAD;
            LOG_INFO("start processing file content");
        }

        if (fileParseState == HEAD) {
            if (body_.find("--" + header_["boundary"]) != string::npos) return;
            int n = body_.find("filename");
            if ( n != string::npos) {
                uploadFileName = body_.substr(n+10, body_.length() - 1);
                fileParseState = CONTENT;
            }
        }

        // process content
        if (fileParseState == CONTENT) {
            if (body_.find("--" + header_["boundary"] + "--") != string::npos) {
                fileParseState = DONE;
                SaveFile();
                return;
            }
            fileContentBuffer.append(body_);
        }
    }
}

/**
 * create local file from fileBuffer
 * with flock
 */
void HttpRequest::SaveFile() {

    int fd = open((srcDir + uploadFileName).c_str(), O_RDWR | O_CREAT | O_TRUNC, 0664);
    write(fd, &fileContentBuffer, fileContentBuffer.length());
    close(fd);

    UpdateFileIndex();
}

void HttpRequest::UpdateFileIndex() {

    vector<string >files;
    string str1(srcDir);
    str1.append("index.html");

    int fd = open(str1.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0664);
    flock(fd, LOCK_EX); // file lock

    GetDirFiles(srcDir, files);
    string indexHtml = CreateIndexHtml(files);

    flock(fd, LOCK_UN);
    close(fd);

}

string HttpRequest::CreateIndexHtml(std::vector<std::string> &files) {
    string common = GetIndexHtmlCommonPart();

}

std::string HttpRequest::GetIndexHtmlCommonPart() {

    return
}

void HttpRequest::GetDirFiles(char *dir, std::vector<std::string> &files) {
    struct dirent *ptr;
    DIR *dir1;
    dir1=opendir(dir);
    cout << "文件列表: "<< endl;
    while((ptr=readdir(dir1))!=NULL)
    {

        // skip '.' and '..' directories
        if(ptr->d_name[0] == '.') continue;
        // skip those non-user files
        if (ptr->d_name == "index.html" || ptr->d_name == "error.html") continue;
        files.push_back(ptr->d_name);
    }

    for (int i = 0; i < files.size(); ++i)
    {
        cout << files[i] << endl;
    }

    closedir(dir1);
}

/**
 * parse query params
 */
void HttpRequest::ParseFromUrlencoded_() {
    if(body_.size() == 0) { return; }

    string key, value;
    int num = 0;
    int n = body_.size();
    int i = 0, j = 0;

    for(; i < n; i++) {
        char ch = body_[i];
        switch (ch) {
        case '=':
            key = body_.substr(j, i - j);
            j = i + 1;
            break;
        case '+':
            body_[i] = ' ';
            break;
        case '%':
            num = ConverHex(body_[i + 1]) * 16 + ConverHex(body_[i + 2]);
            body_[i + 2] = num % 10 + '0';
            body_[i + 1] = num / 10 + '0';
            i += 2;
            break;
        case '&':
            value = body_.substr(j, i - j);
            j = i + 1;
            post_[key] = value;
            LOG_DEBUG("%s = %s", key.c_str(), value.c_str());
            break;
        default:
            break;
        }
    }
    assert(j <= i);
    if(post_.count(key) == 0 && j < i) {
        value = body_.substr(j, i - j);
        post_[key] = value;
    }
}

std::string HttpRequest::path() const{
    return path_;
}

std::string& HttpRequest::path(){
    return path_;
}
std::string HttpRequest::method() const {
    return method_;
}

std::string HttpRequest::version() const {
    return version_;
}

std::string HttpRequest::GetPost(const std::string& key) const {
    assert(key != "");
    if(post_.count(key) == 1) {
        return post_.find(key)->second;
    }
    return "";
}

std::string HttpRequest::GetPost(const char* key) const {
    assert(key != nullptr);
    if(post_.count(key) == 1) {
        return post_.find(key)->second;
    }
    return "";
}