#include "Syntax_Analysis.h"
#include <fstream>
#include <sstream>

/*************语法分析相关变量声明**********/

ifstream in1;
ofstream out1;
string buf="";
int aline=0;//源码中的行数
string val="";//ID，NUM对应的字符串

/**********函数声明*************/
TreeNode *declaration_list();
TreeNode *declaration();
TreeNode *params();
TreeNode *params_list();
TreeNode *param();
TreeNode *compound_stmt();
TreeNode *local_declarations();
TreeNode *statement_list();
TreeNode *statement();
TreeNode *expression_stmt();
TreeNode *selection_stmt();
TreeNode *iteration_stmt();
TreeNode *return_stmt();
TreeNode *expression();
TreeNode *var();
TreeNode *simple_expression(TreeNode* tmp);
TreeNode *additive_expression(TreeNode* tmp);
TreeNode *term(TreeNode* tmp);
TreeNode *factor(TreeNode* tmp);
TreeNode *call();
TreeNode *args();
TreeNode *arg_list();

/*************分配新结点*************/
TreeNode * newNode(NodeKind kind){
    TreeNode * t = new TreeNode;
	int i;
	for (i = 0; i<5; i++)
	{
		t->child[i] = NULL;
	}
	t->sibling = NULL;
	t->nodekind = kind;
	t->lineno = aline;
    t->val="";
	t->is_deal=false;
	return t;
}

/***********文件打开关闭操作**************/
void open_file(){
    in1.open("../OutputFile/Tokens.txt");
    out1.open("../OutputFile/Tree.txt");

}
void close_file(){
    in1.close();
    out1.close();

}

/*********读取记号流**************/
TokenType find_token(string s){
    
    if(s== "IF")return IF;
    if(s=="ELSE")return ELSE;
    if(s=="INT")return INT;
    if(s=="RETURN")return RETURN;
    if(s=="WHILE")return WHILE;
    if(s=="VOID")return VOID;
    if(s=="PLUS")return PLUS;
    if(s=="MINUS")return MINUS;
    if(s=="TIMES")return TIMES;
    if(s=="OVER")return OVER;
    if(s=="LT")return LT;
    if(s=="LTE")return LTE;
    if(s=="HT")return HT;
    if(s=="HTE")return HTE;
    if(s=="EQ")return EQ;
    if(s=="NEQ")return NEQ;
    if(s=="ASSIGN")return ASSIGN;
    if(s=="SEMI")return SEMI;
    if(s=="COM")return COM;
    if(s=="LRAREN")return LPAREN;
    if(s=="RPAREN")return RPAREN;
    if(s=="MLPAREN")return MLPAREN;
    if(s=="MRPAREN")return MRPAREN;
    if(s=="LLPAREN")return LLPAREN;
    if(s=="LRPAREN")return LRPAREN;
    else return ERROR;
}

TokenType read_token(){
	
    getline(in1,buf);
    if(buf.find("EOF")!=string::npos)return ENDFILE;
    int pos=buf.find(":");
    string tmp1=buf.substr(0,pos-1);
    stringstream str;
    str<<tmp1;
    str>>aline;
    
    str.clear();
    tmp1=buf.substr(pos+2);
   
    
    if(tmp1.find('(')==string::npos){return find_token(tmp1);}
    else{
        int pos1=tmp1.find("(");
        int pos2=tmp1.find(")");
        val=tmp1.substr(pos1+1,pos2-pos1-1);
        if(tmp1.find("NUM")!=string::npos)return NUM;
        else return ID;
    }
}

/**************将记号转化为在源码中的字符*************/
string error_token(TokenType t1){
    switch(t1){
        case IF:return "if";
        case ELSE:return "else";
        case VOID:return "void";
        case RETURN:return "return";
        case WHILE:return "while";
        case INT:return "int";
        case ASSIGN: return "="; 
        case LTE: return "<=";
        case LT: return "<";
        case HT:return ">";
        case HTE: return ">=";
        case EQ: return "=="; 
        case NEQ: return "!=";
        case LPAREN: return "("; 
        case RPAREN: return ")"; 
        case MLPAREN: return "[";
        case MRPAREN: return "]"; 
        case LLPAREN: return "{"; 
        case LRPAREN: return "}"; 
        case SEMI: return ";"; 
        case PLUS: return "+"; 
        case MINUS: return "-"; 
        case TIMES: return "*"; 
        case OVER: return "/"; 
        case COM: return ",";
        case NUM:
        case ID:
            return val;
        default:return "";
    }
}

/***************语法分析递归下降方法****************/
TokenType token;//记号

void syntaxError(string message)
{ 	
	
	out1<<"\n>>> ";
  	out1<<"Syntax error at line "<<aline<<" : "<<message;
    
  
}
void match(TokenType expected){
	if(token==expected){
		token=read_token();
		
	}
	else{
		
		syntaxError("unexpected token -> ");
		out1<<error_token(token)<<"\n";
		token=ERROR;
	}
}



TreeNode *declaration_list(){
	
	TreeNode *t=declaration();
	TreeNode *p=t;
	TreeNode *q;
	if(token==ENDFILE) return t;
	while((q=declaration())!=NULL){
		p->sibling=q;
		p=p->sibling;
	}
	p=NULL;
	return t;
}
TreeNode *declaration(){
	
	TreeNode* t=NULL;
	if(token==ENDFILE) return NULL;
	
	else if(token==LLPAREN){
		t=compound_stmt();
		
	}
	else if(token==INT||token==VOID){
		TreeNode *p;
		if(token==INT)p=newNode(INTK);
		else p=newNode(VOIDK);
		TreeNode *q=NULL;
		token=read_token();
		if(token==ID){
			q=newNode(IDK);
			
			q->val=val;	
			
			token=read_token();
			
			if(token==SEMI){
				t=newNode(VAR_DECK);
				t->child[0]=p;
				if(p->nodekind==VOIDK) {
					syntaxError("var's type can not be void\n");
					return NULL;
				}
				t->child[1]=q;
				token=read_token();
			}
			else if(token==MLPAREN){
				t=newNode(VAR_DECK);
				t->child[0]=p;
				if(p->nodekind==VOIDK) {
					syntaxError("var's type can not be void\n");
					return NULL;
				}
				token=read_token();
				
				if(token==NUM){
					TreeNode* t1=newNode(NUMK);
					t1->val=val;
					match(NUM);
					TreeNode* s=newNode(ARRK);
					s->child[0]=q;
					s->child[1]=t1;
					t->child[1]=s;
					match(MRPAREN);
					match(SEMI);
				}
				else match(NUM);
				
			}
			else if(token==LPAREN){
				token=read_token();
				t=newNode(FUN_DECK);
				t->child[0]=p;
				t->child[1]=q;
				
				t->child[2]=params();
				match(RPAREN);
			}
			else{
				match(SEMI);
			}
		}
		else{
			match(ID);
		}
	}
	else{
		match(INT);
	}
	return t;
}
TreeNode* compound_stmt(){
	TreeNode* t=newNode(COM_SK);
	match(LLPAREN);
	
	t->child[0]=local_declarations();
	
	t->child[1]=statement_list();
	match(LRPAREN);
	
	return t;
}
TreeNode* params(){
	
	TreeNode* t=NULL;
	if(token==VOID){
		t=newNode(VOIDK);
		t->val="void";
		match(VOID);
	}
	else{
		t=params_list(); 
	}
	return t;
	
}
TreeNode* params_list(){
	
	TreeNode *t=NULL; 
	TreeNode *p=NULL;
	if(token==INT||token==VOID){
		
		t=param();
	}
	else match(INT);
	
	p=t;
	while(token==COM){
		match(COM);
		p->sibling=param();
		p=p->sibling;
	}
	return t;
}
TreeNode* param(){
	TreeNode *t=newNode(PARAMK);
	TreeNode *p=NULL;
	if(token==INT){
		p=newNode(INTK);
		p->val="int";
	}
	else{
		p=newNode(VOIDK);
		p->val="void";
	}
	t->child[0]=p;
	token=read_token();
	if(token==ID){
		p=newNode(IDK);
		p->val=val;
		match(ID);
		t->child[1]=p;
	}
	else match(ID);
	if(token==MLPAREN){
		match(MLPAREN);
		TreeNode *tmp=newNode(ARRK);
		tmp->child[0]=p;
		tmp->child[1]=newNode(NUMK);
		tmp->child[1]->val="unknown";
		t->child[1]=tmp; 
	
		match(MRPAREN);
	}
	return t;
}
TreeNode* local_declarations(){
	TreeNode *t=NULL;
	if(token==INT||token==VOID){
		
		t=declaration();	
	}
	else return t;
	TreeNode *p=t;
	TreeNode *q=NULL;
	while(token==INT||token==VOID){
		
		q=declaration();
		p->sibling=q;
		p=p->sibling;
	}
	return t;
}
TreeNode* statement_list(){
	
	TreeNode* t=NULL;
	int i=0;
	TreeNode* p=NULL;
	TreeNode* q=NULL;
	do{
		p=statement();
	
		if(i==0){
			t=p;
			q=t;
		}
		else {
			q->sibling=p;
			q=q->sibling;
		}
		i++;
		
	}while(p!=NULL);
	return t;
}
TreeNode* statement(){
	
	TreeNode *p=NULL;
	switch(token){
		case IF: p=selection_stmt();break;
		case WHILE:p=iteration_stmt();break;
		case LLPAREN:p=compound_stmt();break;
		case RETURN:p=return_stmt();break;
		case SEMI:
		case ID:
		case LPAREN:
		case NUM: p=expression_stmt();break;
        default:break;
	}
	return p;
}
TreeNode *selection_stmt(){
	TreeNode* t=newNode(IFK);
	t->val="if";
	match(IF);
	match(LPAREN);
	
	t->child[0]=expression();
	
	match(RPAREN);
	
	t->child[1]=statement();
	
	if(token==ELSE){
		match(ELSE);
		
		t->child[2]=statement();
	}
	return t;
} 
TreeNode *iteration_stmt(){
	TreeNode* t=newNode(WHILEK);
	match(WHILE);
	match(LPAREN);
	t->child[0]=expression();
	match(RPAREN);
	t->child[1]=statement();
	return t;
}
TreeNode *return_stmt(){
	TreeNode* t=newNode(RETURNK);
	match(RETURN);
	if(token==SEMI){
		match(SEMI);
	}
	else{
		t->child[0]=expression();
		match(SEMI);
	}
	return t;
}
TreeNode* expression_stmt(){
	TreeNode* t=NULL;
	
	if(token==SEMI){
		match(SEMI);
	}
	else{
		
		t=expression();
		
		match(SEMI);
		
	}
	return t;

}
TreeNode *expression(){
	TreeNode* t=NULL;
	
	if(token==ID){
		TreeNode* p=var();
		
		
		if(token==ASSIGN){
			t=newNode(ASSIGNK);
			t->val="=";
			match(ASSIGN);
			t->child[0]=p;
			t->child[1]=expression();
		}
		
		else {
			t=simple_expression(p);
		}
		
		
	}
	else {
		t=simple_expression(NULL);
	}
	
	return t;
}
TreeNode* var(){
	TreeNode* t=NULL;
	TreeNode* p=newNode(IDK);
	p->val=val;
	match(ID);
	if(token==MLPAREN){
		match(MLPAREN);
		t=newNode(ARR_ELEMK);
		t->child[0]=p;
		t->child[1]=expression();
		match(MRPAREN);
	}
	else t=p;
	return t;
}
TreeNode *simple_expression(TreeNode* tmp){
	TreeNode* t=NULL;
	TreeNode* p=additive_expression(tmp);
	if(token==LT||token==LTE||token==HT||token==HTE||token==EQ||token==NEQ){
		t=newNode(RELOPK);
        switch(token){
            case LT:t->val="<";break;
            case LTE:t->val="<=";break;
            case HT:t->val=">";break;
            case HTE:t->val=">=";break;
            case EQ:t->val="==";break;
            case NEQ:t->val="!=";break;
            default:break;
        }
		token=read_token();
        
		t->child[0]=p;
		t->child[1]=additive_expression(NULL);
	}	
	else t=p;
	return t;
	
}
TreeNode *additive_expression(TreeNode* tmp){

	TreeNode* t=NULL;
	TreeNode* q=term(tmp);
	TreeNode *p;
	
	while(token==PLUS||token==MINUS){
		p=newNode(ADDOPK);
        if(token==PLUS) p->val="+";
        else p->val="-";
		token=read_token();
        
		p->child[0]=q;
		p->child[1]=term(NULL);
		q=p;
	}
	t=q;
	
	return t;
}
TreeNode* term(TreeNode* tmp){
	TreeNode* t=NULL;
	TreeNode* q=factor(tmp);
	TreeNode *p;
	
	while(token==OVER||token==TIMES){
		p=newNode(MULOPK);
        if(token==OVER)p->val="/";
        else p->val="*";
		token=read_token();
		
		p->child[0]=q;
		p->child[1]=factor(NULL);
		q=p;
	}
	t=q;
	
	return t;
}	
TreeNode* factor(TreeNode* tmp){
	
	TreeNode* t=NULL;
	
	if(tmp!=NULL){
		if(token==LPAREN&&tmp->nodekind==IDK){
			TreeNode* q=newNode(CALLK);
			q->child[0]=tmp;
			match(LPAREN);
			
			q->child[1]=args();
			t=q;
			
			match(RPAREN);
			
			
		}
		else t=tmp;
		
		return t;
	}
	else{
		if(token==LPAREN){
			match(LPAREN);
			t=expression();
			match(RPAREN);
		}
		else if(token==NUM){
		
			TreeNode* p=newNode(NUMK);
			p->val=val;
			t=p;
			match(NUM);
		}
		else if(token==ID){
			
			TreeNode* p=newNode(IDK);
			p->val=val;
			token=read_token();
		
			if(token==LPAREN){
				TreeNode* q=newNode(CALLK);
				q->child[0]=p;
				match(LPAREN);
				q->child[1]=args();
			
				t=q;
			
				match(RPAREN);
			}
			else if(token==MLPAREN){
				match(MLPAREN);
				TreeNode* q=newNode(ARR_ELEMK);
				q->child[0]=p;
				q->child[1]=expression();
				t=q;
			
				match(MRPAREN);
			}
			else{
				t=p;
			}
		}
		else{
			match(LPAREN);	
		}
	}
	return t;
}
TreeNode* args(){
	TreeNode* t=NULL;
	
	if(token!=RPAREN)t=arg_list();
	
	return t;
}
TreeNode* arg_list(){
	TreeNode* t=newNode(ARGK);	
	t->child[0]=expression();
	TreeNode *p=t->child[0];
	while(token==COM){
		token=read_token(); 
		TreeNode* q=expression();
		p->sibling=q;
		p=p->sibling;
	}
	return t;
}

/*****************打印生成的语法树***********/
int space_num=0;
int size1=0;
int param1=0;
int op1=0;
void printTree( TreeNode * tree )
{ 
	
	if(tree!=NULL){
		
		for(int i=0;i<space_num;i++)out1<<"    ";
		switch(tree->nodekind){
			case VAR_DECK:
				out1<<"var-declaration:"<<endl;
				break;
			case FUN_DECK:
				out1<<"fun-declaration:"<<endl;
				break;
			case INTK:
				
				if(param1==0)out1<<"int"<<endl;
				else{
					out1<<"int    ";
					param1=0;
				}
				break;
			case VOIDK:
				if(param1==0)out1<<"void"<<endl;
				else{
					out1<<"void    ";
					param1=0;
				}
				break;
			case IDK:
				if(size1==0)
					out1<<"ID:"<<tree->val<<endl;
				else out1<<"ID:"<<tree->val<<"    ";
				break;
			case NUMK:
				if(size1==1){
					out1<<"SIZE:"<<tree->val<<endl;
					size1=0;
				}
				else out1<<"val:"<<tree->val<<endl;
				break;
			case  PARAMK:
				out1<<"param:"<<endl;param1=1;
				break;
			case COM_SK:
				out1<<"compound-stmt"<<endl;
				break;
			case IFK:
				out1<<"if"<<endl;break;
			case WHILEK:
				out1<<"while"<<endl;break;
			case RETURNK:
				out1<<"return"<<endl;break;
			case RELOPK:
			case ADDOPK:
			case MULOPK:
				out1<<"op:"<<tree->val<<endl;op1=1;
				break;
			case ARRK:
				out1<<"array   ";size1=1;break;
			case ARR_ELEMK:
				out1<<"array-element"<<endl;size1=1;break;
			case CALLK:
				out1<<"call"<<endl;break;
			case ARGK:
				out1<<"arg-list"<<endl;break;
			case ASSIGNK:
				out1<<"assign to"<<tree->val<<endl;op1=1;
			
		}
		space_num++;
		for (int i=0;i<5;i++){
		
         	if (tree->child[i] != NULL)printTree(tree->child[i]);
        }
        space_num--;
        if (tree->sibling != NULL)printTree(tree->sibling);
	}
}

/***********语法分析调用函数*****************/
TreeNode * Syntax_parse(void)
{ 
    open_file();
	TreeNode *t;
  	token = read_token();
  	t = declaration_list();
  	if (token!=ENDFILE){
    	out1<<"\nCode ends before file\n";
		cout<<"Code ends before file"<<endl;
		errors1=1;
        return NULL;
    }
    else printTree(t);
    close_file();
    
  	return t;
}
