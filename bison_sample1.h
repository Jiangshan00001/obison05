#ifndef BISON_SAMPLE1_H
#define BISON_SAMPLE1_H
#include <string>
#include <map>
#include <vector>
#include "lalr1.h"


class bison_sample1
{
public:
    bison_sample1();
    std::string render_def_header(const std::map<int,std::string> &aterm_nval,std::string namespace_to_add, std::string enum_name);
    std::string render_parser(std::string &class_name, std::vector<std::vector<std::string> > &mrules,
                              std::map<std::string, int> &materm_val,
                              std::string &def_code, std::string &last_code,
                              const std::vector<std::string>  &all_actions,
                              std::vector<Closure> &mclosures,
                              std::vector< std::vector<int> > &maction_id,
                              std::vector< std::vector<int> > &maction_type,
                              std::vector<std::string> &maction_table_x_str,
                              std::map<int, int> &mmiddle_action_len, std::map<int, std::set<int> > &mmiddle_action_state,
                              std::map<int, int> &mbefore_action, std::map<int, int> &mafter_action,
                              std::map<int, int> &mcompaction);
private:
    std::string generate_actions_code(const std::vector<std::string> &mactions,
                                      const std::map<int, int> &mbefore_action, const std::map<int, int> &mafter_action,
                                      const std::vector<std::vector<std::string> > &mrules);

    std::string generate_rules(const std::vector<std::vector<std::string> > &mrules,
                               std::map<std::string, int> &materm_val, std::vector<std::string> &maction_table_x_str);
    std::string generate_action_table(int state_cnt, int terms_cnt,
                                      std::vector<std::vector<int> > &action_type,
                                      std::vector<std::vector<int> > &action_id);


    std::string generate_middle_action(std::map<int, int> &mmiddle_action_len,
                                       const  std::map<int, std::set<int> > &mmiddle_action_state,
                                       const std::vector<std::string > &mactions);

};

#endif // BISON_SAMPLE1_H
