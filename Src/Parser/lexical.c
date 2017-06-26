#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "KNX_Hash.h"

#include "parser.h"
#include "debug.h"

#define isNum(x) (x>='0' && x<='9')
#define isAlpha(x) ((x>='a' && x<='z') || (x>='A' && x<='Z'))
#define isNOChar(x) (x=='.' || x=='_')
#define isText(x) (isNum(x) || isAlpha(x) || isNOChar(x))
#define isWhiteSpace(x) (x==' ' || x=='\t' || x==0)
#define isOCap(x) (x=='(' || x=='[' || x=='{')
#define isECap(x) (x==')' || x==']' || x=='}')
#define isQuote(x) (x=='\'' || x=='\"')

#define isMOCap(x) (x>= lx_ENC_OBRACK && x <= lx_ENC_OPARAN)

char getEscapeChar(char c){

    return c;
}

token * resolveSymbol(char * sym){

    lexeme lex = lx_NA;

    unsigned long long hash = FNV_1a_32(sym);

    switch(hash){

        case 2902189215581572LLU: lex = lx_KW_INT; break;//int
        case 339556138201278380LLU: lex = lx_KW_UINT; break;//uint
        case 313436435282222236LLU: lex = lx_KW_LINT; break;//lint
        case 333751759433611532LLU: lex = lx_KW_SINT; break;//sint
        case 2155467746076386996LLU: lex = lx_KW_USINT; break;//usint
        case 246621463160055696LLU: lex = lx_KW_CHAR; break;//char
        case 10407967113323926400LLU: lex = lx_KW_UCHAR; break;//uchar
        case 10901210006236253888LLU: lex = lx_KW_WCHAR; break;//wchar
        case 8203415641640515025LLU: lex = lx_KW_STRING; break;//string
        case 16975719106092910113LLU: lex = lx_KW_WSTRING; break;//wstring
        case 13602979746211663506LLU: lex = lx_KW_ARRAY; break;//arr
        case 327683545976869276LLU: lex = lx_KW_LIST; break;//list
        case 11263903348313536949LLU: lex = lx_KW_CLASS; break;//class
        case 300937673386998447LLU: lex = lx_KW_BYTE; break;//byte
        case 303925260367076040LLU: lex = lx_KW_FUNCTION; break;//proc
        case 297906538444820100LLU: lex = lx_KW_VOID; break;//void

        case 23199318181086LLU: lex = lx_CNT_IF; break;//if
        case 274441695294207238LLU: lex = lx_CNT_ELSE; break;//else
        case 253058162427981432LLU: lex = lx_CNT_ELIF; break;//elif
        case 24044111249349LLU: lex = lx_CNT_DO; break;//do
        case 12257646299530473087LLU: lex = lx_CNT_WHILE; break;//while
        case 2795849259767220LLU: lex = lx_CNT_FOR; break;//for

        case 9665038429183275952LLU: lex = lx_CNT_SWITCH; break;//switch
        case 241608324446159497LLU: lex = lx_CNT_CASE; break;//case
        case 6921684308587554939LLU: lex = lx_CNT_BREAK; break;//break
        case 13840090565086158820LLU: lex = lx_CNT_CONTINUE; break;//continue

        case 1698529464417901130LLU: lex = lx_KW_TYPEOF; break;//typeof
        case 10505907191231071951LLU: lex = lx_KW_DELETE; break;//delete
        case 277490748868856452LLU: lex = lx_KW_CAST; break;//cast
        case 17446148261328024908LLU: lex = lx_KW_IMPORT; break;//import
        case 1303734914311023094LLU: lex = lx_KW_RELEASE; break;//release
        case 348816759751995988LLU: lex = lx_KW_QUIT; break;//quit
        
        default:
        lex = lx_NA_SYM;
    }

    token * ret = createToken(lex == lx_NA_SYM ? sym : NULL, lex, NULL);

    return ret;
}

lexeme popOpStack(tBuffer * buf){
    return buf->oCount > 0 ? 
        buf->opStack[--buf->oCount] :
        lx_NA;
}

//returns first open on stack
char collapseEncap(tBuffer * buf)
{
    
    char c = 0;

    //collapse opStack to first open

    return isOCap(c) ? c : 0;
}

size_t pushOperator(tBuffer * buf, char * str, size_t max)
{
    //printf("{%s}", str);

    lexeme result = lx_NA_OPERATOR;
    size_t ret = 0;

    char c0 = str[0];
    char c1 = max >= 2 ? str[1] : 0;
    //char c2 = max >= 3 ? str[2] : 0;

    bool rToL = false;

    //printf("<<%d %c%c %d %d\r\n", c0, c1, getType(buf->head->type), !isKeyword(buf->head->type), !isOperator(buf->head->type));

    switch (c0){
        case '+':
            if (c1=='+') {
                if (buf->head && !isKeyword(buf->head->type) && !isOperator(buf->head->type)){
                    result=lx_SET_POST_INC | LEVEL_TWO; ret=1;
                    rToL = true;
                }else{
                    result=lx_SET_PRE_INC; ret=1;
                }
            }
            else if (c1=='='){result=lx_SET_ADD; ret=1;}
            else result = lx_ADD;
        break;
        case '-':
            if (c1=='-') {
                if (buf->head && !isKeyword(buf->head->type) && !isOperator(buf->head->type)){
                    result=lx_SET_POST_DEC | LEVEL_TWO; ret=1;
                    rToL = true;
                }else{
                    result=lx_SET_PRE_DEC; ret=1;
                }
            }
            else if (c1=='='){result=lx_SET_SUB; ret=1;}
            else result = lx_SUB;
        break;
        case '*':
            if (c1=='=') {result=lx_SET_MULT; ret=1;}
            else result = lx_MULT | LEVEL_TWO;
        break;
        case '/':
            if (c1=='=') {result=lx_SET_DIV; ret=1;}
            else result = lx_DIV | LEVEL_TWO;
        break;
        case '%':
            result = lx_MOD;
        break;
        case '^':
            if (c1=='&') {result=lx_BIT_AND; ret=1;}
            else if (c1=='|'){result=lx_BIT_OR; ret=1;}
            else if (c1=='!'){result=lx_BIT_NOT; ret=1;}
            else if (c1=='<'){result=lx_BIT_LEFT; ret=1;}
            else if (c1=='>'){result=lx_BIT_RIGHT; ret=1;}
            else result=lx_POW;
        break;
        case (char)251:
        case -30:
            result=lx_ROOT;
        break;

        case '&':
            if (c1=='=') {result=lx_SET_AND; ret=1;}
            else result = lx_LOG_AND;
        break;
        case '|':
            if (c1=='=') {result=lx_SET_OR; ret=1;}
            else if (c1=='|') {result=lx_LOG_XOR; ret=1;}
            else if (c1=='!') {result=lx_LOG_XNOR; ret=1;}
            else result = lx_LOG_OR;
        break;
        case '!':
            if (c1=='=') {result=lx_CMP_NEQU; ret=1;}
            else if (c1=='|') {result=lx_LOG_NOR; ret=1;}
            else if (c1=='&') {result=lx_LOG_NAND; ret=1;}
            else result = lx_LOG_NOT | LEVEL_TWO;
        break;
        case '?':
            result = lx_SET_TERN;
        break;

        case '=':
            if (c1=='=') {result=lx_CMP_EQU; ret=1;}
            else result = lx_SET;
        break;

        case '<':
            if (c1=='=') {result=lx_CMP_LSS_EQU; ret=1;}
            else result = lx_CMP_LSS;
        break;
        case '>':
            if (c1=='=') {result=lx_CMP_GTR_EQU; ret=1;}
            else result = lx_CMP_GTR;
        break;

        case ':':
            result = lx_GEN_INDEX;
        break;
        case '$':
            result = lx_GEN_LAMBDA;
        break;
        case '.':
            result = lx_GEN_MEMBERREF;
        break;
        case ',':
            result = lx_GEN_LISTITEM;
        break;

        //encapsulation
        case '(':
            result = lx_ENC_OPARAN;
        break;
        case '{':
            result = lx_ENC_OBRACE;
        case '[':
            result = lx_ENC_OBRACK;
        break;
        case ')':
            result = lx_ENC_CPARAN;
        break;
        case '}':
            result = lx_ENC_CBRACE;
        break;
        case ']':
            result = lx_ENC_CBRACK;
        break;

        //control operators
        case '\'':
        buf->qState = QBIT_S;
        return 0;
        break;
        case '\"':
        buf->qState = QBIT_D;
        return 0;
        break;
        case '\\':
        if (max == 1){
            buf->yieldLine = true;
            return 0;
        } else {
            //TODO throw warning/error
        }
        break;

        //comments
        case '#':
            if (c1=='*'){
                buf->commentMode = CB_COMMENT;
                return 1;
            }else{
                buf->commentMode = CS_COMMENT;
                return 0;
            }
        break;

        case 0:
        printf("Zero\r\n");
        break;
    }

/*//TODO REMOVE
    printf("\r\n%u %u %u\r\n", LEVEL_THREE, lx_ADD, LEVEL_THREE | lx_ADD);

    printf("||%u %u\r\n", (result & (1 << 31)), (result & (1 << 30)));

    printf("!!%u\r\n", result); fflush(stdout);
    printf("!!%hu\r\n", result >> 15);fflush(stdout);
    printf("!!%hu\r\n", result);fflush(stdout);
    */

    buf->opStack[buf->oCount++] = result;
    
    token * t = createToken(NULL, result, NULL);
    appendTBuffer(buf, t, rToL);

    printf("{%u}", result);

    return ret;
} 

void pushOperand(tBuffer * buf, char * str, size_t max, lexeme explicit)
{
    token * t = NULL;
    if (explicit == lx_NA)
        t = resolveSymbol(str);
    else
        t = createToken(str, explicit, NULL);

    //token * t = createToken(str, explicit, data);
    appendTBuffer(buf, t, false);

    printf("[%s, %d]", str, explicit);
}

//TODO implement modified shunting yard as in docs
int tokenize(node * node, char * raw)
{
    printf("Input: %s from node %d\r\n", raw, node->id_index);
    fflush(stdout);

    size_t len = strlen(raw);
    char * buffer = node->buffer.buffer;
    size_t index = node->buffer.index;

    for (size_t x=0; x <= len; ++x)
    {
        char c = raw[x];

        //literal read mode
        if (node->buffer.qState){
            if ((c=='\'' && node->buffer.qState == QBIT_S) ||
                (c=='\"' && node->buffer.qState == QBIT_D)){
                    buffer[index]=0;
                    pushOperand(
                        &node->buffer, 
                        buffer, index, 
                        node->buffer.qState == QBIT_S && index == 1 ? lx_CHAR : lx_STRING);
                    node->buffer.qState = 0;
                    index = 0;
                    continue;
            }

            if (c=='\\'){
                c = getEscapeChar(raw[x+1]);
                ++index;
            }

            buffer[index++] = c;
            
            continue;
        }

        if (node->buffer.commentMode){
            if (node->buffer.commentMode == CS_COMMENT){
                if (c=='#'){
                    node->buffer.commentMode = CNO_COMMENT;
                }
            } else if (x < len && c=='*' && raw[x+1] == '#') {
                node->buffer.commentMode = CNO_COMMENT;
                ++x;
            }

            continue;
        }

        if (isText(c)){
            buffer[index++] = c;
        } else {
            if (index > 0){
                buffer[index] = 0;
                pushOperand(&node->buffer, buffer, index, lx_NA);
                index = 0;
            }

            if (!isWhiteSpace(c)){
                x += pushOperator(&node->buffer, raw + x, len - x);
            }
        }
    }

    node->buffer.index = index;

    //TODO add quotes and encapsulates

    printBufferStream(&node->buffer);

    printf("\r\n");

    return  node->buffer.oCount == 0 && 
            node->buffer.qState == 0 &&
            !node->buffer.yieldLine ?
        TK_COMPLETE :
        TK_PENDING;
}