%{
    #include <stdio.h>
    #include <string.h>
    void yyerror(char const *);
    struct Node* createLeaf(char const);
    struct Node* createNodeOne(char *action, struct Node *left);
    struct Node* createNodeTwo(char *action, struct Node* left, struct Node *right);
    void print(struct Node* root, int *count);

    struct Node {
        struct Node* left;
        struct Node* right;
        char *content;
    };
%}

%union {
	char cType;
	struct Node* nType;
}

%token <cType> CHAR
%type  <nType> line
%type  <nType> alt
%type  <nType> concat
%type  <nType> single
%start input
%left '|'
%left '*'

%%

input : /* empty */
      | input line
;

line : '\n'
     | alt '\n'        { int count = 1; print($1, &count); printf("\n"); }
;

alt : concat          { $$ = $1; }
    | alt '|' concat  { $$ = createNodeTwo("Alt", $1, $3); }
;

concat : single
       | concat single  { $$ = createNodeTwo("Cat", $1, $2); }
;

single   : CHAR           { $$ = createLeaf($1); }
    | single '?'          { $$ = createNodeOne("Quest", $1); }
    | single '*'          { $$ = createNodeOne("Star", $1); }
    | single '+'          { $$ = createNodeOne("Plus", $1); }
    | single '*' '?'      { $$ = createNodeOne("NgStar", $1); }
    | single '+' '?'      { $$ = createNodeOne("NgPlus", $1); }
    | single '?' '?'      { $$ = createNodeOne("NgQuest", $1); }
    | '(' '?' ':' alt ')' { $$ = $4; }
    | '(' alt ')'         { $$ = createNodeOne("Paren", $2); }
;
%%

void print(struct Node* root, int *count) {
    printf("%s", root->content);
    if (root->left != NULL) {
        printf("(");
        if (strcmp(root->content, "Paren") == 0) {
            printf("%d, ", (*count)++);
        }
        print(root->left, count);
    }
    if (root->right != NULL) {
        printf(", ");
        print(root->right, count);
    }
    if (root->left != NULL || root->right != NULL) {
        printf(")");
    }
    free(root);
}

struct Node* createLeaf(char const s) {
    struct Node* leaf = (struct Node*)malloc(sizeof(struct Node));
    if (s == '.') {
        leaf->content = (char*)malloc(sizeof(char)*(strlen("Dot")+1));
        strcpy(leaf->content, "Dot");
        leaf->content[3] = '\0';
    } else {
        leaf->content = (char*)malloc(sizeof(char)*(strlen("Lit()")+2));
        strcpy(leaf->content, "Lit(");
        leaf->content[4] = s;
        leaf->content[5] = ')';
        leaf->content[6] = '\0';
    }
    leaf->left = NULL;
    leaf->right = NULL;
    return leaf;
}

struct Node* createNodeOne(char* action, struct Node* left) {
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    node->content = action;
    node->left = left;
    node->right = NULL;
    return node;
}

struct Node* createNodeTwo(char* action, struct Node* left, struct Node* right) {
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    node->content = action;
    node->left = left;
    node->right = right;
    return node;
}


int main() {
	  return yyparse();
}

void yyerror(char const *s) {
	  fprintf(stderr, "%s\n", s);
}