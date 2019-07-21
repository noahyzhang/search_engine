#pragma once 
// 构建索引模块和搜索模块

#include<string>
#include<vector>
#include<unordered_map>
#include"cppjieba/Jieba.hpp"


namespace  searcher{

    struct DocInfo{
        uint64_t doc_id;
        std::string title;
        std::string content;
        std::string url;
    };


    // Weight 表示的含义是某个词在某个文档中出现过，以及该词的权重是多少
    struct Weight{
       uint64_t doc_id;  // 无符号长整形 
       int weight;   // 权重，为了后边的排序做准备。当前我们采用词频进行计算权重。
       std::string key;
    };

    // 类型重命名，创建一个"倒排拉链" 类型　
    typedef std::vector<Weight> InvertedList;

    // 通过这个类来描述索引模块
    class Index{
    private:
        // 知道 id 获取到对应的文档内容
        // 使用 vector 下标来表示文档 id 
        std::vector<DocInfo> forward_index_;
        // 知道词，获取到对应的 id 列表 
        std::unordered_map<std::string, InvertedList> inverted_index_;  // 查找速度 O(1) 哈希map 
        cppjieba::Jieba jieba_;

    public:
        Index(); // 为了构造jieba 对象
        bool Build(const std::string& input_path); // 通过 raw_input 文件，在内存中构建索引 
        const DocInfo* GetDocInfo(uint64_t doc_id) const; // 查正排,给定 id 找到文档内容
        const InvertedList* GetInvertedList(const std::string& key) const; // 查倒排,给定词，找到这个词在那些文档中出现过
        void CutWord(const std::string& input,std::vector<std::string>* output);

    private:
        const DocInfo* BuildForward(const std::string& line);
        void BuildInverted(const DocInfo& doc_info);
    };


    // 搜索模块
    class Searcher{
        private:
            Index* index_; // 索引类型的指针
        public:
            Searcher()
                :index_(new Index())
            {}
            ~Searcher()
            {
                delete index_; 
            }
            bool Init(const std::string& input_path);  // 加载索引
            // 通过特定的格式再 result 字符串中表示搜索结果
            bool Search(const std::string& query,std::string* result);
        private:
            std::string GetDesc(const std::string& content,const std::string& key);
    };

}  // end searcher 
