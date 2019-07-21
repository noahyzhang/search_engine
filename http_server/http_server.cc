#include<iostream>
#include"httplib.h"
#include"/home/zy/project/search_engine/doc_searcher/searcher/searcher.h"

int main()
{
    // 1、创建 searcher 对象，并初始化
    searcher::Searcher s;
    bool ret = s.Init("/home/zy/project/search_engine/doc_searcher/data/tmp/raw_input");
    if(!ret)
    {
        std::cout<<"Searcher Init Faild" <<std::endl;
        return 1;
    }
    // 2、创建 http 服务器
#if 1
    using namespace httplib;
    Server server;
    // /search?query=filesystem
    server.Get("/search",[&s](const Request& req, Response& res){
            std::string query = req.get_param_value("query");
            std::string result;
            s.Search(query,&result);
            res.set_content(result,"text/plain");
            });
    server.listen("0.0.0.0",9090);
#endif
    return 0;
}
