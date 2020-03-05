#include "Lexical_Analysis.h"
#include <fstream>
#include <sstream>

/**********�ʷ�������������***********/
int line=0; //���� 
int linepos=0;//��ȡ��bufline�е�λ�� 
string bufline;//���ļ��ж�����һ��Դ������  
int bufsize=0;//bufline���ַ�����  
ifstream in;
ofstream out;
int EOF_flag = 0;
string tokenString;

/**************�����ֵĲ��ұ� ****************/
map<string,TokenType> m={
    pair<string,TokenType>("if",IF),pair<string,TokenType>("else",ELSE),
    pair<string,TokenType>("while",WHILE),pair<string,TokenType>("void",VOID),
    pair<string,TokenType>("return",RETURN),pair<string,TokenType>("int",INT)
};

/*******DFA�е�״̬ ******/
typedef enum{
    START,INCOMF,INCOMMENT,INCOMR,INASSIGN,INNUM,INID,INLT,INHT,INNEQ,DONE
}States;

/***********���ұ������ַ����ж���ID���Ǳ�����************/
TokenType look_up(string s){
    if(m.find(s)!=m.end()){
		return m[s];
	}
	else return ID;
}

/***��bufline�ж�ȡ�¸��ַ�����linebuf�ľ�����ȡ�µ�һ�е�bufline��***/
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

/***********����һ���ַ�**********/
void back(){
	if(!EOF_flag)linepos--;
}

/**********���ַ�ת��Ϊ�ַ���************/
string char_to_string(char c){
	string s;
	stringstream str;
	str<<c;
	s=str.str();
	return s;
}

/***************��ӡ�Ǻ��������浽�ļ���**************/
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

/************�ʷ�����������������Ӧ��DFA���з���**********/
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
				 	
				 }//��Linux����һ�е�ĩβ������һ��ascii��Ϊ13����CR�س����ַ�
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

/*******************���ôʷ�����****************/
int Lexical(string path3){
    int error1=0;
    in.open(path3);
	out.open("../OutputFile/Tokens.txt");
	if(!in){
		cout<<"�ļ���ʧ��,�˳�"<<endl;
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