//数据处理模块
//把boost文档中涉及到的html进行处理
//1.去标签
//2.把文件进行合并
//	把文档中涉及到的N个HTML的内容合并成一个 '行文本' 文件 
//	生成的结果是一个大文件，里面包含很多行，每一行对应boost文档中的一个html ，
//	这么做的目的是为了后面的索引模块处理起来比较方便
//3.对文档的结构进行分析，提取出文档的标题，正文，目标url

#include<string>
#include<vector>
#include<iostream>
#include<fstream>
//为了目录遍历和枚举所有所用到的头文件
#include<boost/filesystem/path.hpp>
#include<boost/filesystem/operations.hpp>

// g_ 前缀的指的是全局变量
const std::string g_input_path = "/home/zy/project/search_engine/doc_searcher/data/input/";  
const std::string g_output_path = "/home/zy/project/search_engine/doc_searcher/data/tmp/raw_input";

#include"../common/util.hpp"

//doc 指的是文档，也就是待搜索的 html
struct DocInfo{

	std::string title;  // 标题
	std::string content; // 正文
	std::string url;    // 目标 url 
};


//将输入路径中所有的html文档路径存到vector中
bool EnumFile(const std::string& input_path,std::vector<std::string>* file_list) 
{
	namespace fs = boost::filesystem; //定义boost库中filesystem这个命名空间
	//input_path 是一个字符串，根据这个字符串构造出一个path对象
	fs::path root_path(input_path);
	if(!fs::exists(root_path))
	{
		std::cout<<"input_path not exist! input_path = "<<input_path<<std::endl;
		return false;
	}
	//boost 递归遍历目录，借助一个特殊的迭代器即可
	//下面是构造一个未初始化的迭代器作为遍历结束标记
	fs::recursive_directory_iterator end_iter;
	for(fs::recursive_directory_iterator iter(root_path);iter != end_iter;++iter)
	{
		//1.此处我们需要剔除目录
		if(!fs::is_regular_file(*iter))
			continue;
		//2.根据扩展名，只保留html
		if(iter->path().extension() != ".html")
			continue;
		file_list->push_back(iter->path().string());
	}
	return true;
}

// 从 html 中的title 标签中提取标题
// 第一次有问题的地方，可能会没有 title,比如<title></title> 因此应该是beg > end 
bool ParseTitle(const std::string& html,std::string* title)
{
    //1、先查找 <title> 标签
    size_t beg = html.find("<title>");
    if(beg == std::string::npos)
    {
        std::cout << "<title> not found!" << std::endl;
        return false;
    }
    //2、再查找 </title> 标签
    size_t end = html.find("</title>");
    if(end == std::string::npos)
    {
        std::cout << "</title> not found!" << std::endl;
        return false;
    }
    //3、通过字符串取子串的方式获取到 title 标签中的内容了
    beg += std::string("<title>").size();
    if(beg > end)
    {
        std::cout << "beg end error!" << std::endl;
        return false;
    }
    *title = html.substr(beg,end-beg);
    return true;
}

// 除了标签之外的东西，都认为是正文
// '<' &lt 
// '>' &gt 
bool ParseContent(const std::string& html,std::string* content)
{
    // 一个一个字符读取
    // 如果当前字符是正文内容，就写入结果
    // 如果当前字符是 < 认为标签开始，接下来的字符就舍弃
    // 一直遇到 > 认为标签结束，接下来的字符就恢复
    
    // 这个变量为true 意味着当前在处理正文，为false 意味着当前在处理标签
    bool is_content = true;
    for(auto c : html)
    {
        if(is_content)    // 当前为正文状态
        {
            if(c == '<')  // 进入标签状态 (正常的正文中万一有 '<' 或 '>' ,就会出现问题，但是正文中'<' '>' 会被转义)
                is_content = false;
            else  // 当前字符就是普通的正文字符，需要加入到结果中
            {
                if(c == '\n')
                    c = ' '; // 此处把换行替换为空格，为了最终的行文本文件
                content->push_back(c);
            }
        }
        else   // 当前是标签状态
        {
            if(c == '>')
                is_content = true;
        }
    }
    return true;
}

// Boost 文档 URL 有一个统一的前缀 
// https://www.boost.org/doc/libs/1_53_0/doc/
// URL 的后半部分可以通过该文档的路径中解析出来
// 文档的路径形如
// /home/zy/project/search_engine/doc_searcher/data/input/html/thread.html  
// 需要的后缀的形式
// /html/thread.html 
bool ParseUrl(const std::string& file_path,std::string* url)
{
    std::string prefix = "https://www.boost.org/doc/libs/1_53_0/doc/";
    std::string tail = file_path.substr(g_input_path.size());
    *url = prefix + tail;
    return true;
}

bool ParseFile(const std::string& file_path,DocInfo* doc_info)
{
    // 1、打开文件，读取文件内容
    std::string html;
    bool ret = FileUtil::Read(file_path,&html);
    if(!ret)
    {
        std::cout << "Read file failed! file_path=" << file_path << std::endl;
        return false;
    }
    // 2、解析标题
    ret = ParseTitle(html,&doc_info->title);
    if(!ret)
    {
        std::cout << "ParseTitle failed! file_path=" << file_path << std::endl;
        return false;
    }
    // 3、解析正文，并且去除 html 标签
    ret = ParseContent(html,&doc_info->content);
    if(!ret)
    {
        std::cout << "ParseContent failed! file_path=" << file_path << std::endl;
        return false;
    }
    // 4、解析出 url 
    ret = ParseUrl(file_path,&doc_info->url);
    if(!ret)
    {
        std::cout << "ParseUrl failed! file_path=" << file_path << std::endl;
        return false;
    }
    return true;
}

// c++ 中的 iostream 和 fstream 等这些对象都是禁止拷贝
// 最终的输出结果是一个行文本文件，每一行对应到一个 html 文档
// 也就是每一行对应一个 doc_info 
void WriteOutput(const DocInfo& doc_info, std::ofstream& file)
{
    std::string line = doc_info.title + "\3" + doc_info.url +"\3" + doc_info.content + "\n";
    file.write(line.c_str(),line.size());
}

int main()
{
	//1.枚举出输入路径中所有的html 文档的路径
	std::vector<std::string> file_list;  //vector中的每个元素就是一个文件的路径
	bool ret = EnumFile(g_input_path,&file_list); //将输入路径中所有的html文档路径存到vector中
	if(ret == false)
	{
		std::cout << "EnumFile faild!" <<std::endl;
		return 1;
	}
    
	std::ofstream output_file(g_output_path.c_str());
	if(!output_file.is_open())
	{
		std::cout<<"open output_file failed! g_output_path="<<g_output_path<<std::endl;
		return 1;
	}	

	//2.依次处理每个枚举出的路径，对该文件进行分析，分析出文件的标题/正文/url，并且进行去标签
    // auto 类型的自动推导
	for(const auto& file_path : file_list)
	{
		DocInfo info;
		//输入的是当前要解析的文件路径
		//输出的是解析之后得到的 DocInfo 结构
		ret = ParseFile(file_path,&info);
		if(ret == false)
		{
			std::cout<<"ParseFile failed! file_path = "<<file_path <<std::endl;
			//return 1;
			continue;
		}
		//3、把分析结果按照一行的形式写入到输出文件中
		WriteOutput(info,output_file);
	}
	output_file.close();
	return 0;
}


