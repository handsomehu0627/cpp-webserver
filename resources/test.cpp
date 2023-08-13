//
// Created by 胡帅帅 on 8/11/23.
//
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int main() {
//    string str1("boundary=123");
//    string str2("boundary=123\r\n");
//    string str3("\r\n");
//    const char CRLF[] = "\r\n";
//    cout << str1.substr(0, 8) << endl;
//    cout << str1.find_last_of("b") << endl;
//    cout << str2.substr(9, str2.length() - 3) << endl;
//    cout << str3.length()<< endl; // 2
//    cout << sizeof(str3)<< endl; // 3 * 8 = 24 bytes
//    cout << sizeof(CRLF)<< endl; // 3
//    string s(CRLF, CRLF+2);
//    cout << s<< endl; // 3
//    int n = str1.find_last_of("=");
//    cout << str1.substr(n+1, str1.length())<< endl;

//    const char buf[] = {"abc"};
//    string buf2("content");
//
//    int fd = open("index.txt", O_RDWR | O_CREAT | O_TRUNC, 0664);
//    string strFd = to_string(fd);
//    cout << strFd << endl;
//    flock(fd, LOCK_EX);
//    write(fd, &buf2, buf2.length());
//    flock(fd, LOCK_UN);
//    close(fd);

    char * srcDir = getcwd(nullptr, 256);
    strncat(srcDir, "/resources/files", 16);
    cout << srcDir << endl;










    return 0;
}
