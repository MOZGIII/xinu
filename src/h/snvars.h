/* variables for the Xinu Shell SNMP Command */

LOCAL   echoch(), erase1(), eraseall(), eraseword();
struct snbentry *parseoid();
struct snbentry *getnewbl();

#define EXIT		'\004'		/* \004 is Cntl-d		*/
#define COMPLETE	'\011'		/* \011 is TAB			*/
#define KILLWORD	'\027'		/* \027 is Cntl-w		*/
#define NEXT        '\016'      /* \016 is Cntl-n       */

#define M_COMPL     1           /* completion mode      */
#define M_CHARIN    2           /* char-in mode         */

#define PROMPT		"snmp > "
#define EXITSTR		"exit"
#define AGENTSTR    "agent"

#define BUFSIZE		256
struct  snvars {
        char    buf[BUFSIZE];           /* buffer to hold the input line*/
        char    str2compl[BUFSIZE];     /* string to complete       */
        int     s2clen;                 /* length of str2compl      */
        int     completions[BUFSIZE];   /* array of indices into the mib*/
                                        /* array to signal completions  */
        int     num_completions;        /* # of indices in completions[]*/
        int     next_completion;        /* next completion to try   */
        char    *nextchar;              /* points to start of next word */
        struct  oid lastobjid;          /* holds last returned objid */
        int     lastoidset;             /* flags if there is a last objid   */

};

