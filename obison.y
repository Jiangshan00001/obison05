/* https://pubs.opengroup.org/onlinepubs/9699919799/utilities/yacc.html */
/* Grammar for the input to yacc. */
/* Basic entries. */
/* The following are recognized by the lexical analyzer. */

%{
#include "otoken.h"
#define YYSTYPE OToken
#include <stdio.h>
#include <iostream>
#include "lex_header.h"
#include "trim.h"
int yylex();
int yyerror(const char*ss);

#include <vector>
#include <string>


std::string get_one_action();


extern int yyunput(char ss);
int yyinput();
int yypeek(int i);
int yyprint();
extern char* yytext;
void process_spec(OToken &tk);

std::vector<std::string> s_actions;

#define TK_PUSH1(a) {tk.m_children.push_back(a);}
#define TK_PUSH12(a,b) {tk.m_children.push_back(a);tk.m_children.push_back(b);}
#define TK_PUSH123(a,b,c) {tk.m_children.push_back(a);tk.m_children.push_back(b);tk.m_children.push_back(c);}
#define TK_PUSH1234(a,b,c,d) {tk.m_children.push_back(a);tk.m_children.push_back(b);tk.m_children.push_back(c);tk.m_children.push_back(d);}

#define DTK_PUSH1(nn,a)  OToken tk(nn, ""); tk.m_children.push_back(a);
#define DTK_PUSH12(nn,a,b)  OToken tk(nn, ""); tk.m_children.push_back(a);tk.m_children.push_back(b);
#define DTK_PUSH123(nn,a,b,c) OToken tk(nn, ""); tk.m_children.push_back(a);tk.m_children.push_back(b);tk.m_children.push_back(c);
#define DTK_PUSH1234(nn,a,b,c,d) OToken tk(nn, ""); tk.m_children.push_back(a);tk.m_children.push_back(b);tk.m_children.push_back(c);tk.m_children.push_back(d);

%}


%token    IDENTIFIER      /* Includes identifiers and literals */
%token    C_IDENTIFIER    /* identifier (but not literal)
                             followed by a :. */
%token    NUMBER          /* [0-9][0-9]* */


/* Reserved words : %type=>TYPE %left=>LEFT, and so on */


%token    LEFT RIGHT NONASSOC TOKEN PREC TYPE START UNION


%token    MARK            /* The %% mark. */
%token    LCURL           /* The %{ mark. */
%token    RCURL           /* The %} mark. */


/* 8-bit character literals stand for themselves; */
/* tokens have to be defined for multi-byte characters. */


%start    spec


%%


spec  : defs MARK rules tail {DTK_PUSH1234("spec", $1, $2,$3,$4); $$=tk;process_spec($$);}
      ;
tail  : MARK
      {
        std::string last_code;
        int one_char =yyinput();
        while(-1!=one_char)
        {
            last_code.insert(last_code.end(), (char)one_char);
            one_char=yyinput();
        }
        OToken tk("tail", last_code);
        $$=tk;
        /* In this action, set up the rest of the file. */
      }
      | /* Empty; the second MARK is optional. */
      ;
defs  : /* Empty. */
        |    defs def  {DTK_PUSH12("defs", $1, $2); $$=tk; }
      ;
def   : START IDENTIFIER {DTK_PUSH12("def1", $1, $2); $$=tk; }
      |    UNION
      {
        /* Copy union definition to output. */
		printf("here should start union definition\n");
      }
      |    LCURL
      {
		printf("here should finish include definition\n");
        //m_file.m_include_code="";
        std::string include_code;
        int one_char = yypeek(0);
        int one_char2 = yypeek(1);

        while((0!=one_char)&&(0!=one_char2)&&(('%'!=one_char)||('}'!=one_char2) ))
        {
            one_char = yyinput();
            include_code.insert(include_code.end(), char(one_char));
            one_char = yypeek(0);
            one_char2 = yypeek(1);
        }

        //skipRCURL
        one_char = yyinput();
        one_char = yyinput();
        //$$=$1;
        yychar=RCURL;//add RCURL to stack???
        //yyunput('%');
        //yyunput('}');
        //yyunput(RCURL);
        /* Copy C code to output file. */
        OToken tk("def_inc_code", include_code);
        $$= tk;
      }
        RCURL  {OToken tk("def3", ""); TK_PUSH123($1,$2,$3);$$=tk;}
      |    rword tag nlist  {OToken tk("def4", ""); TK_PUSH123($1,$2,$3);$$=tk;}
      ;
rword : TOKEN  {OToken tk("rword1", "token"); TK_PUSH1($1);$$=tk;}
      | LEFT  {OToken tk( "rword2", "left"); TK_PUSH1($1);$$=tk;}
      | RIGHT  {OToken tk("rword3", "right"); TK_PUSH1($1);$$=tk;}
      | NONASSOC  {OToken tk("rword4", "nonassoc"); TK_PUSH1($1);$$=tk;}
      | TYPE {OToken tk("rword5", "type"); TK_PUSH1($1);$$=tk;}
      ;
tag   : {OToken tk("tag1", ""); $$=tk;} /* Empty: union tag ID optional. */
      | '<'    IDENTIFIER      '>'      {OToken tk("tag2", ""); TK_PUSH123($1,$2,$3);$$=tk;}
      ;
nlist : nmno {OToken tk("nlist1", ""); TK_PUSH1($1);$$=tk;}
      | nlist nmno  {OToken tk("nlist2", ""); TK_PUSH12($1,$2); $$=tk;}
      ;
nmno  : IDENTIFIER {OToken tk("nmno1", ""); TK_PUSH1($1);$$=tk;}        /* Note: literal invalid with % type. */
      | IDENTIFIER NUMBER {OToken tk("nmno2", ""); TK_PUSH12($1,$2);$$=tk;}  /* Note: invalid with % type. */
      ;


/* Rule section */


rules : C_IDENTIFIER rbody prec {DTK_PUSH123("rules1", $1,$2,$3);$$=tk;}
      | rules  rule {DTK_PUSH12("rules2", $1,$2);$$=tk;}
      ;
rule  : C_IDENTIFIER rbody prec {DTK_PUSH123("rule1", $1,$2,$3);$$=tk;}
      | '|' rbody prec {DTK_PUSH123("rule2", $1,$2,$3);$$=tk;}
      ;
rbody : /* empty */  {OToken tk("rbody1", ""); $$=tk;}
        | rbody IDENTIFIER {DTK_PUSH12("rbody2", $1,$2);$$=tk;}
        | rbody acts {DTK_PUSH12("rbody3", $1,$2);$$=tk;}
        | rbody '<' {DTK_PUSH12("rbody2", $1,$2);$$=tk;}
        | rbody '>' {DTK_PUSH12("rbody2", $1,$2);$$=tk;}
      ;


acts: act          { DTK_PUSH1("acts1", $1);$$=tk; }
        | acts act { DTK_PUSH12("acts2", $1,$2);$$=tk; }
     ;

act   : '{'
        {
		
          /* Copy action, translate $$, and so on. */
            std::string curr_act = get_one_action();
            std::cerr<<"action finish here\n";
          yychar='}';//add RCURL to stack???
          OToken tk("act", curr_act);
          $$=tk;
        }

          '}'  {$$=$2;}
      ;
prec  : /* Empty */  {OToken tk("prec1", "");$$=tk;}
          | PREC IDENTIFIER  {DTK_PUSH12("prec2", $1,$2);$$=tk;}
      | PREC IDENTIFIER act {DTK_PUSH12("prec3", $1,$2);$$=tk;}
          | prec ';' {DTK_PUSH12("prec4", $1,$2);$$=tk;}
      ;
	  
%% 

int yyerror(const char*ss)
{
    return 0;
}

std::string get_one_action()
{
    std::cerr<<"action should do here\n";
    yyprint();
    std::string curr_act="{";
    int one_char = 0;
    int depth = 1;
    do{
      one_char = yyinput();

      ///去除注释
      if(one_char=='/' && yypeek(0)=='/')
      {
          std::cerr<<"comment remove start\n";
          yyprint();
          while(one_char!='\n')
          {
              curr_act.insert(curr_act.end(), (char)one_char);// = curr_act + yytext;
              one_char=yyinput();
          }
          std::cerr<<"comment remove end\n";
          yyprint();
      }

      ///去除字符串
      if(one_char=='\'')
      {
          std::cerr<<"single remove start\n";
          yyprint();
          //skip one char
          do
          {
              curr_act.insert(curr_act.end(), (char)one_char);// = curr_act + yytext;
              one_char=yyinput();
              if(one_char=='\\')///去除转义字符
              {
                  curr_act.insert(curr_act.end(), (char)one_char);// = curr_act + yytext;
                  one_char=yyinput();
                  curr_act.insert(curr_act.end(), (char)one_char);// = curr_act + yytext;
                  one_char=yyinput();
              }
          }while(one_char!='\'');
          std::cerr<<"single remove finish\n";
          yyprint();
      }
      if(one_char=='\"')
      {
          std::cerr<<"dquote remove start\n";
          yyprint();
          //skip one char
          do
          {
              curr_act.insert(curr_act.end(), (char)one_char);// = curr_act + yytext;
              one_char=yyinput();
              if(one_char=='\\')///去除转义字符
              {
                  curr_act.insert(curr_act.end(), (char)one_char);// = curr_act + yytext;
                  one_char=yyinput();
                  curr_act.insert(curr_act.end(), (char)one_char);// = curr_act + yytext;
                  one_char=yyinput();
              }
          }while(one_char!='\"');
          std::cerr<<"dquote remove finish\n";
          yyprint();

      }

      curr_act.insert(curr_act.end(), (char)one_char);
      if((char)one_char=='{'){ depth++;std::cerr<<"action parser+:"<<depth<<"\n"; yyprint();}
      if(((char)one_char=='}')&&(depth>0)){depth--;std::cerr<<"action parser-:"<<depth<<"\n"; yyprint();};

    }while(!(('}'==one_char)&&(depth==0)));

    return curr_act;
}






