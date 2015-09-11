#ifndef AIDL_AIDL_LANGUAGE_H_
#define AIDL_AIDL_LANGUAGE_H_

#include <string>

#include "macros.h"

typedef enum {
    NO_EXTRA_TEXT = 0,
    SHORT_COMMENT,
    LONG_COMMENT,
    COPY_TEXT,
    WHITESPACE
} which_extra_text;

typedef struct extra_text_type {
    unsigned lineno;
    which_extra_text which;
    char* data; 
    unsigned len;
    struct extra_text_type* next;
} extra_text_type;

typedef struct buffer_type {
    unsigned lineno;
    unsigned token;
    char *data;
    extra_text_type* extra;
} buffer_type;

typedef struct type_type {
    buffer_type type;
    buffer_type array_token;
    int dimension;
} type_type;

typedef struct arg_type {
    buffer_type comma_token; // empty in the first one in the list
    buffer_type direction;
    type_type type;
    buffer_type name;
    struct arg_type *next;
} arg_type;

enum {
    METHOD_TYPE
};

typedef struct interface_item_type {
    unsigned item_type;
    struct interface_item_type* next;
} interface_item_type;

typedef struct method_type {
    interface_item_type interface_item;
    type_type type;
    bool oneway;
    buffer_type oneway_token;
    buffer_type name;
    buffer_type open_paren_token;
    arg_type* args;
    buffer_type close_paren_token;
    bool hasId;
    buffer_type equals_token;
    buffer_type id;
    // XXX missing comments/copy text here
    buffer_type semicolon_token;
    buffer_type* comments_token; // points into this structure, DO NOT DELETE
    int assigned_id;
} method_type;

enum {
    USER_DATA_TYPE = 12,
    INTERFACE_TYPE_BINDER
};

typedef struct document_item_type {
    unsigned item_type;
    struct document_item_type* next;
} document_item_type;


typedef struct user_data_type {
    document_item_type document_item;
    buffer_type keyword_token; // only the first one
    char* package;
    buffer_type name;
    buffer_type semicolon_token;
    bool parcelable;
} user_data_type;

typedef struct interface_type {
    document_item_type document_item;
    buffer_type interface_token;
    bool oneway;
    buffer_type oneway_token;
    char* package;
    buffer_type name;
    buffer_type open_brace_token;
    interface_item_type* interface_items;
    buffer_type close_brace_token;
    buffer_type* comments_token; // points into this structure, DO NOT DELETE
} interface_type;


#if __cplusplus
extern "C" {
#endif

int parse_aidl(char const *);

// strips off the leading whitespace, the "import" text
// also returns whether it's a local or system import
// we rely on the input matching the import regex from below
char* parse_import_statement(const char* text);

// in, out or inout
enum {
    IN_PARAMETER = 1,
    OUT_PARAMETER = 2,
    INOUT_PARAMETER = 3
};
int convert_direction(const char* direction);

// callbacks from within the parser
// these functions all take ownership of the strings
typedef struct ParserCallbacks {
    void (*document)(document_item_type* items);
    void (*import)(buffer_type* statement);
} ParserCallbacks;

extern ParserCallbacks* g_callbacks;

// the package name for our current file
extern char const* g_currentPackage;

typedef enum {
    STATEMENT_INSIDE_INTERFACE
} error_type;

void init_buffer_type(buffer_type* buf, int lineno);


class ParseState {
 public:
  ParseState();
  ParseState(const std::string& filename);
  ~ParseState();

  bool OpenFileFromDisk();
  int RunParser();
  void ReportError(const std::string& err);

  bool FoundNoErrors();
  std::string FileName();
  std::string Package();
  void *Scanner();

  void ProcessDocument(const document_item_type& items);
  void ProcessImport(const buffer_type& statement);

 private:
  int error_ = 0;
  std::string filename_;
  std::string package_;
  void *scanner_ = nullptr;

  DISALLOW_COPY_AND_ASSIGN(ParseState);
};

#if __cplusplus
}
#endif


#endif // AIDL_AIDL_LANGUAGE_H_
