#include "Lexical_Analysis.h"
#include <fstream>
#include <sstream>

/**********词法分析变量声明***********/
int line=0; //行数 
int linepos=0;//读取到bufline中的位置 
string bufline;//从文件中读出的一行源码内容  
int bufsize=0;//bufline中字符个数  
ifstream in;
ofstream out;
int EOF_flag = 0;
string tokenString;

/**************保留字的查找表 ****************/
map<string,TokenType> m={
    pair<string,TokenType>("if",IF),pair<string,TokenType>("else",ELSE),
    pair<string,TokenType>("while",WHILE),pair<string,TokenType>("void",VOID),
    pair<string,TokenType>("return",RETURN),pair<string,TokenType>("int",INT)
};

/*******DFA中的状态 ******/
typedef enum{
    START,INCOMF,INCOMMENT,INCOMR,INASSIGN,INNUM,INID,INLT,INHT,INNEQ,DONE
}States;

/***********查找表，根据字符串判断是ID还是保留字************/
TokenType look_up(string s){
    if(m.find(s)!=m.end()){
		return m[s];
	}
	else return ID;
}

/***从bufline中读取下个字符，若linebuf耗尽，读取新的一行到bufline中***/
char getnextchar(){
	if(linepos>=bufsize){
		line++; 
		if(getline(in,bufline)){
			bufline+=' ';
			bufsize=bufline.length();
			linepos=0;
			return bufline[linepos++];
		}
		else{
			EOF_flag=1;
			return EOF; 
		}
	} 
	else return bufline[linepos++];
}

/***********回退一个字符**********/
void back(){
	if(!EOF_flag)linepos--;
}

/**********将字符转化为字符串************/
string char_to_string(char c){
	string s;
	stringstream str;
	str<<c;
	s=str.str();
	return s;
}

/***************打印记号流，保存到文件中**************/
void printToken( TokenType token, string tokenString )
{ 

    switch (token)
    { 
        case IF:out<<"IF"<<endl;break;
        case ELSE:out<<"ELSE"<<endl;break;
        case VOID:out<<"VOID"<<endl;break;
        case RETURN:out<<"RETURN"<<endl;break;
        case WHILE:out<<"WHILE"<<endl;break;
        case INT:out<<"INT"<<endl;break;
        case ASSIGN: out<<"ASSIGN\n"; break;
        case LTE: out<<"LTE\n";break;
        case LT: out<<"LT\n"; break;
        case HT: out<<"HT\n";break;
        case HTE: out<<"HTE\n";break;
        case EQ: out<<"EQ\n"; break;
        case NEQ: out<<"NEQ\n";break;
        case LPAREN: out<<"LRAREN\n"; break;
        case RPAREN: out<<"RPAREN\n"; break;
        case MLPAREN: out<<"MLPAREN\n"; break;
        case MRPAREN: out<<"MRPAREN\n"; break;
        case LLPAREN: out<<"LLPAREN\n"; break;
        case LRPAREN: out<<"LRPAREN\n"; break;
        case SEMI: out<<"SEMI\n"; break;
        case PLUS: out<<"PLUS\n"; break;
        case MINUS: out<<"MINUS\n"; break;
        case TIMES: out<<"TIMES\n"; break;
        case OVER: out<<"OVER\n"; break;
        case COM: out<<"COM\n";break;
        case ENDFILE: out<<"EOF\n"; break;
        case NUM:
            out<<"NUM("<<tokenString<<")"<<endl;
        break;
        case ID:
            out<<"ID("<<tokenString<<")"<<endl;
        break;
        case ERROR:
            out<<  "ERROR,unexpected token: "<<tokenString<<endl;
        break;
    }
}

/************词法分析函数，根据相应的DFA进行分析**********/
TokenType getToken(){

	States state=START;
	TokenType currentToken;
	int save;
	char c;
	while(state!=DONE){
		
		c=getnextchar();
		save=1;
		//cout<<line<<":"<<(int)c<<endl;
		switch(state){
			
			case START:
				 if(c==' '||c=='\t'||c=='\n'||c==13){
				 	save=0;
				 	
				 }//在Linux下在一行的末尾处会有一个ascii码为13，即CR回车的字符
				 else if(c=='/') state=INCOMF;
				 else if(isdigit(c))state=INNUM;
				 else if(isalpha(c))state=INID;
				 else if(c=='=')state=INASSIGN;
				 else if(c=='>')state=INHT;
				 else if(c=='!')state=INNEQ;
				 else if(c=='<')state=INLT;
				 else{
				 	state=DONE;
				 	switch(c){
					 	
				 		case EOF:
				 			state=DONE;
				 			currentToken=ENDFILE;
				 			save=0;
				 			break;
				 			
				 		case '+':
               				currentToken = PLUS;
               				break;
             			case '-':
               				currentToken = MINUS;
               				break;
             			case '*':
               				currentToken = TIMES;
               				break;
               			case ';':
               				currentToken = SEMI;
              				break;
              			case ',':
              				currentToken = COM;
              				break;
               			case '(':
               				currentToken = LPAREN;
               				break;
             			case ')':
               				currentToken = RPAREN;
               				break;
               			case '[':
               				currentToken = MLPAREN;
               				break;
             			case ']':
               				currentToken = MRPAREN;
               				break;
               			case '{':
               				currentToken = LLPAREN;
               				break;
             			case '}':
               				currentToken = LRPAREN;
               				break;
               			default:
						    currentToken=ERROR;
               				
							break;	
             		}
				 }
				 break;
			case INCOMF:
				if(c=='*'){
                    save=0;
					state=INCOMMENT;
					tokenString.clear();
				
				}
				
				else {
					back();
					state=DONE;
					currentToken=OVER;
					save=0;
				}
				break;
			case INNEQ:
				if(c=='='){
					state=DONE;
					currentToken=NEQ;
					
				}
				else{
					back();
					save=0;
					state=DONE;
					currentToken=ERROR;
				
				}
				break;
			case INLT:
				if(c=='='){
					state=DONE;
					currentToken=LTE;
					
				}
				else{
					back();
					save=0;
					state=DONE;
					currentToken=LT;
				
				}
				break;
			case INHT:
				if(c=='='){
					state=DONE;
					currentToken=HTE;
					
				}
				else{
					back();
					save=0;
					state=DONE;
					currentToken=HT;
				
				}
				break;
			case INASSIGN:
				if(c=='='){
					state=DONE;
					currentToken=EQ;
					
				}
				else{
					back();
					save=0;
					state=DONE;
					currentToken=ASSIGN;
				
				}
				break;
			case INID:
				if(!isalpha(c)){
					back();
					save=0;
					state=DONE;
					currentToken=ID;
				}
				break;
			case INNUM:
				if(!isdigit(c)){
					back();
					save=0;
					state=DONE;
					currentToken=NUM;
				}
				break;
			case INCOMMENT:
				save=0;
				if(c=='*'){
					
					state=INCOMR;
					
				}
				else if(c==EOF){
					back();
					state=DONE;
					currentToken=ENDFILE;
				}
				break;
			case INCOMR:
				save=0;
				if(c=='/'){
					state=START;
					
				} 
				else if(c=='*') state=INCOMR;
				else if(c==EOF){
					back();
					state=DONE;
					currentToken=ENDFILE;
				}
				else{
					state=INCOMMENT;
				}
				break;
				case DONE:break;
		}
		
		if(save)tokenString+=char_to_string(c);
		if(state==DONE&&currentToken==ID){
		
			currentToken = look_up(tokenString);
			
		}
	}
	
	out<<line<<" : ";
	printToken(currentToken,tokenString);
	tokenString.clear();
	return currentToken;
}

/*******************调用词法分析****************/
int Lexical(string path3){
    int error1=0;
    in.open(path3);
	out.open("../OutputFile/Tokens.txt");
	if(!in){
		cout<<"文件打开失败,退出"<<endl;
		error1=0;
		exit(0);
	}
    TokenType tokens;
	while((tokens=getToken())!=ENDFILE){
        if(tokens==ERROR){
            errors1=1;
            break;
        }
    }
	in.close();
	out.close();
    return error1;
}