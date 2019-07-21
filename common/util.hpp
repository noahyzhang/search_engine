#pragma once 
#include<string>
#include<fstream>
#include<vector>
#include<boost/algorithm/string.hpp>

class FileUtil {
    public:
        static bool Read(const std::string& file_path,std::string* content)
        {
            std::ifstream file(file_path.c_str());
            if(!file.is_open())
            {
                return false;
            }

            //按行读取内容
            std::string line;
            while(std::getline(file,line))
            {
                *content += line + "\n";
            }


            file.close();
            return true;
        }
        static bool Write(const std::string& file_path,const std::string& content)
        {
            std::ofstream file(file_path.c_str());
            if(!file.is_open())
            {
                return false;
            }
            file.write(content.c_str(),content.size());
            file.close();
            return true; 
        }
};



class StringUtil{
public:
    // 基于 boost 的字符串切分来实现
    // aaa\3bbb\3ccc  => 3
    // aaa\3\3ccc     => 3 or 2 off 的话就是压缩关闭，就是3。on 就是启动压缩，就是2.
    static void Split(const std::string& input,std::vector<std::string>* output,const std::string& split_char)
    {
        boost::split(*output,input,boost::is_any_of(split_char),boost::token_compress_off);
    }
};

