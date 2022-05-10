#include <string>
#include <iostream>
#include <fstream>
#include <assert.h>
//#include "bison_file_io.h"
#include "obisonfile.h"
#include "lalr1.h"
#include "argv.h"
#include "bison_sample1.h"
#include "lex_header.h"
#include "trim.h"

extern FILE* yyin;
extern int yydebug;

extern int yyparse();

OBisonFile m_file;
//bison_file_io m_file;
oflex_sample g_oflex;
extern OToken yylval;
char * yytext;


int yylex()
{
    yylval = g_oflex.yylex();
    std::cerr<<"yylex_out:L:"<<yylval.m_line<<".C:"<< yylval.m_column<<". \""<< yylval.m_yytext<<"\":"<< yylval.m_ret<<"\n";
    yytext = &yylval.m_yytext[0];
    return yylval.m_ret;
}
int yyunput()
{
    return g_oflex.unput();
}
int yyinput()
{
    return g_oflex.input();
}
int yyprint()
{
    std::cerr<<g_oflex.m_line<<":"<<g_oflex.m_column<<"\n";
    return 0;
}
int yypeek(int i)
{
    return g_oflex.peek(i);
}

int before_process_children(OToken&tk)
{
    if(tk.m_typestr=="tail")//
    {
        m_file.m_last_code=
        tk.m_yytext;
    }
    else if(tk.m_typestr=="def_inc_code")//
    {
        m_file.m_def_includes=tk.m_yytext;
    }
    else if(tk.m_typestr=="def1")//
    {
        m_file.m_start = tk.m_children[1].m_yytext;
    }
    else if(tk.m_typestr=="def4")//
    {///rword tag nlist
        //m_file.m_defs_token.push_back()
        m_file.m_curr_rword.clear();
    }
    else if(tk.m_typestr=="rules2")//
    {
        m_file.m_curr_rule.clear();
        m_file.m_curr_rule_is_action.clear();
    }
    else if(tk.m_typestr=="rules1")//
    {
        m_file.m_curr_rule_left = tk.m_children[0].m_yytext;
        m_file.m_curr_rule_left = trim(m_file.m_curr_rule_left);
        m_file.m_curr_rule_left = trim1(m_file.m_curr_rule_left,':');
        m_file.m_curr_rule_left = trim(m_file.m_curr_rule_left);

        m_file.m_curr_rule.push_back(m_file.m_curr_rule_left);
        m_file.m_curr_rule_is_action.push_back(0);
    }
    else if(tk.m_typestr=="rule1")//
    {
        m_file.m_curr_rule_left = tk.m_children[0].m_yytext;
        m_file.m_curr_rule_left = trim(m_file.m_curr_rule_left);
        m_file.m_curr_rule_left = trim1(m_file.m_curr_rule_left,':');
        m_file.m_curr_rule_left = trim(m_file.m_curr_rule_left);

        m_file.m_curr_rule.push_back(m_file.m_curr_rule_left);
        m_file.m_curr_rule_is_action.push_back(0);
    }
    else if(tk.m_typestr=="rule2")//
    {
        m_file.m_curr_rule.push_back(m_file.m_curr_rule_left);
        m_file.m_curr_rule_is_action.push_back(0);
    }
    else if(tk.m_typestr=="rbody2")//
    {

    }
    else if(tk.m_typestr=="rbody3")//
    {

    }
    else if(tk.m_typestr=="nmno1")
    {
        m_file.m_curr_rword.push_back(tk.m_children[0].m_yytext);
    }
    else if(tk.m_typestr=="acts1")
    {
        tk.m_yytext = tk.m_children[0].m_yytext;
        //tk.m_children[0].m_yytext="";
    }
    else if(tk.m_typestr=="acts2")
    {
    }

    return 0;
}
int unpack_acts(std::vector<std::string> &rule, OToken &tk)
{
    if(tk.m_typestr=="act")
    {
        rule.push_back(tk.m_yytext);
    }
    else if(tk.m_typestr=="acts1")
    {
        rule.push_back(tk.m_yytext);
    }
    else if(tk.m_typestr=="acts2")
    {
        unpack_acts(rule, tk.m_children[0]);
        unpack_acts(rule, tk.m_children[1]);
    }
    return 0;
}

int after_process_children(OToken &tk)
{
    if(tk.m_typestr=="def4")
    {
        std::vector<std::string> def_tks;
        def_tks.push_back(tk.m_children[0].m_yytext);
        for(unsigned i=0;i<m_file.m_curr_rword.size();++i)
        {
            def_tks.push_back(m_file.m_curr_rword[i]);
            if(tk.m_children[0].m_yytext!="type"){
                m_file.m_terms.push_back(m_file.m_curr_rword[i]);
            }
        }
        m_file.m_defs_token.push_back(def_tks);
        m_file.m_curr_rword.clear();
    }
    else if(tk.m_typestr=="rbody2")
    {
        m_file.m_curr_rule.push_back(tk.m_children[1].m_yytext);
        m_file.m_curr_rule_is_action.push_back(0);

    }
    else if(tk.m_typestr=="rbody3")
    {
        unsigned old_size = m_file.m_curr_rule.size();
        unpack_acts(m_file.m_curr_rule, tk.m_children[1]);
        while(m_file.m_curr_rule_is_action.size()<m_file.m_curr_rule.size())
        {
            m_file.m_curr_rule_is_action.push_back(1);
        }
    }
    else if(tk.m_typestr=="rules1")
    {
        m_file.m_rules.push_back(m_file.m_curr_rule);
        m_file.m_is_action.push_back(m_file.m_curr_rule_is_action);

        m_file.m_curr_rule.clear();
        m_file.m_curr_rule_is_action.clear();
    }
    else if(tk.m_typestr=="rule1")
    {
        m_file.m_rules.push_back(m_file.m_curr_rule);
        m_file.m_is_action.push_back(m_file.m_curr_rule_is_action);

        m_file.m_curr_rule.clear();
        m_file.m_curr_rule_is_action.clear();
    }
    else if(tk.m_typestr=="rule2")
    {
        m_file.m_rules.push_back(m_file.m_curr_rule);
        m_file.m_is_action.push_back(m_file.m_curr_rule_is_action);

        m_file.m_curr_rule.clear();
        m_file.m_curr_rule_is_action.clear();
    }
    return 0;
}

void print_one_token(OToken &tk, int depth=0)
{
    before_process_children(tk);
    for(unsigned i=0;i<depth;++i)
        std::cout<<" ";
    std::cout<<tk.m_typestr<<":"<<tk.m_yytext<<"\n";
    for(unsigned i=0;i<tk.m_children.size();++i)
    {
        print_one_token(tk.m_children[i], depth+1);
    }
    after_process_children(tk);
}


void process_spec(OToken &tk)
{
    print_one_token(tk, 0);

    m_file.print_all();
}

#ifndef QT_WID

int main(int argc, char *argv[])
#else
int main222(int argc, char *argv[])
#endif

{
    yydebug=1;


    ArgsParser parse(argc, argv);



    if(parse.HaveOption('i'))
    {
        int is_debug = 0;
        std::string file_name = parse.GetOption('i');
        std::string def_file="bison_header.h";
        std::string parser_file="bison_parser.h";
        std::string def_namespace="";
        std::string class_name = "obison_sample";

        if(parse.HaveOption('d'))
        {
            is_debug=1;
        }
        if(parse.HaveOption('e'))
        {
            def_file=parse.GetOption('e');
        }
        if(parse.HaveOption('p'))
        {
            parser_file=parse.GetOption('p');
        }
        if(parse.HaveOption('n'))
        {
            def_namespace = parse.GetOption('n');
        }
        if(parse.HaveOption('c'))
        {
            class_name = parse.GetOption('c');
        }

        g_oflex.set_file_name(file_name);
        yyparse();

        m_file.m_debug = is_debug;

        lalr1 m_lalr1;
        m_lalr1.generate_table(&m_file);

        bison_sample1 sample1;


        std::ofstream ofile;
        ofile.open(def_file);
        auto aterm = m_lalr1.get_def_file();
        ofile<<sample1.render_def_header(aterm,def_namespace, "e_bison_head");
        ofile.close();

        std::string ret = sample1.render_parser(class_name,
                                                m_lalr1.m_rules, m_lalr1.m_aterm_val,
                                                m_file.m_def_includes, m_file.m_last_code, m_lalr1.m_actions, m_lalr1.m_closures,
                                                m_lalr1.m_action_id, m_lalr1.m_action_type,
                                                m_lalr1.m_action_table_x_str, m_lalr1.m_middle_action_len,
                                                m_lalr1.m_middle_action_state, m_lalr1.m_before_action, m_lalr1.m_after_action,
                                                m_lalr1.m_comp_action);
        ofile.open(parser_file);
        ofile<<ret;
        ofile.close();

        ofile.open("parser.dbg.txt");
        ofile<<m_lalr1.print_debug_info();
        ofile.close();


        exit(0);

    }

    std::cout<<"usage: prog -i lex.yy -e def_file.h -p parser_file.h -n namespace -c class_name\n";

    return 0;
}
