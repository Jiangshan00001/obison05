
#ifndef OToken_h
#define OToken_h
#include <vector>
#include <string>
class OToken
{
public:
   OToken()
   {
      m_ret= is_eof=m_line=m_column=0;
   }
   OToken(int typ, std::string ytext)
   {
      m_yytext=ytext;
      m_ret= is_eof=m_line=m_column=0;
      m_ret=typ;
   }
   OToken(std::string typ_str, std::string ytext)
   {
      m_yytext=ytext;
      m_ret= is_eof=m_line=m_column=0;
      //m_ret=typ;
   }

   std::string m_yytext;
   int m_ret;
   int is_eof;
   int m_line;
   int m_column;

   int m_state_id;
   std::vector<OToken> m_children;
};
#endif

                   