/* cmd.c */

#include <conf.h>
#include <kernel.h>
#include <shell.h>
#include <cmd.h>

/*  Initialize shell command structure */

extern	int
    x_arp(),
    x_blast(),
    x_bpool(),
	x_butler(),
	x_cat(),
	x_chdsk(),
	x_chwin(),
	x_clear(),
	x_close(),
	x_color(),
    x_conf(),
    x_cp(),
	x_date(),
    x_debug(),
    x_devs(),
	x_dgstat(),
    x_di(),
    x_dir(),
    x_dos(),
    x_dskst(),
    x_dump(),
    x_echo(),
    x_edit(),
    x_exit(),
    x_finger(),
    x_format(),
	x_goto(),
    x_help(),
    x_ifstat(),
    x_int(),
    x_kill(),
	x_ls(),
    x_memstat(),
    x_more(),
    x_mount(),
	x_mv(),
    x_netstat(),
    x_ns(),
    x_pipstat(),
    x_ping(),
    x_pmem(),
    x_ps(),
    x_redraw(),
    x_rf(),
    x_rlogin(),
    x_rls(),
    x_rm(),
    x_rmount(),  
    x_route(),
    x_routes(),
    x_shell(),
    shell(),
    x_sleep(),
    x_snmp(),
    x_status(),
    x_tcl(),
    x_tcp(),
    x_tcpc(),
    x_test(),
    x_tftp(),
    x_timerq(),
    x_ttyst(),
    x_udp(),
    x_unmou(),
    x_wc(),
    x_window(),
    x_nice();

struct cmdent cmds[] = {
/* name     Builtin ?   Stack       Priority        Procedure   */

"arp",      FALSE,      SHCMDSTK,   SHCMDPRI,       x_arp,
"blast",    FALSE,      SHCMDSTK,   SHCMDPRI,       x_blast,
"bpool",    FALSE,      SHCMDSTK,   SHCMDPRI,       x_bpool,
"butler",   FALSE,      SHCMDSTK,   SHCMDPRI,       x_butler,
"cat",      FALSE,      SHCMDSTK,   SHCMDPRI,       x_cat,
"chwin",    TRUE,       SHCMDSTK,   SHCMDPRI,       x_chwin,
"clear",    FALSE,      SHCMDSTK,   SHCMDPRI,       x_clear,
"close",    FALSE,      SHCMDSTK,   SHCMDPRI,       x_close,
"color",    FALSE,      SHCMDSTK,   SHCMDPRI,       x_color,
"conf",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_conf,
"cp",       FALSE,      SHCMDSTK,   SHCMDPRI,       x_cp,
"date",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_date,
"devs",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_devs,
"dgstat",   FALSE,      SHCMDSTK,   SHCMDPRI,       x_dgstat,
"dos",      TRUE,       SHCMDSTK,   SHCMDPRI,       x_dos,
"echo",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_echo,
"edit",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_edit,
"exit",     TRUE,       SHCMDSTK,   SHCMDPRI,       x_exit,
"finger",   FALSE,      SHCMDSTK,   SHCMDPRI,       x_finger,
"goto",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_goto,
"help",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_help,
"home",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_goto,
"ifstat",   FALSE,      SHCMDSTK,   SHCMDPRI,       x_ifstat,
"kill",     TRUE,       SHCMDSTK,   SHCMDPRI,       x_kill,
"man",      FALSE,      SHCMDSTK,   SHCMDPRI,       x_help,
"memstat",  FALSE,      SHCMDSTK,   SHCMDPRI,       x_memstat,
"more",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_more,
"mount",    FALSE,      SHCMDSTK,   SHCMDPRI,       x_mount,
"mv",       FALSE,      SHCMDSTK,   SHCMDPRI,       x_mv,
"ns",       FALSE,      SHCMDSTK,   SHCMDPRI,       x_ns,
"netstat",  FALSE,      SHCMDSTK,   SHCMDPRI,       x_netstat,
"ping",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_ping,
"pipstat",  FALSE,      SHCMDSTK,   SHCMDPRI,       x_pipstat,
"pmem",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_pmem,
"ps",       FALSE,      SHCMDSTK,   SHCMDPRI,       x_ps,
"redraw",   TRUE,       SHCMDSTK,   SHCMDPRI,       x_redraw,
"rlogin",   FALSE,      SHCMDSTK,   SHCMDPRI,       x_rlogin,
"rm",       FALSE,      SHCMDSTK,   SHCMDPRI,       x_rm,
"route",    FALSE,      SHCMDSTK,   SHCMDPRI,       x_route,
"routes",   FALSE,      SHCMDSTK,   SHCMDPRI,       x_routes,
"shell",    TRUE,       SHCMDSTK,   SHCMDPRI,       x_shell,
"sleep",    FALSE,      SHCMDSTK,   SHCMDPRI,       x_sleep,
"snmp",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_snmp,
"status",   FALSE,      SHCMDSTK,   SHCMDPRI,       x_status,
"tcl",      FALSE,      8192,       SHCMDPRI,       x_tcl,
"tcp",      FALSE,      SHCMDSTK,   SHCMDPRI,       x_tcp,
"tcpc",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_tcpc,
"test",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_test,
"time",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_date,
"tftp",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_tftp,
"timerq",   FALSE,      SHCMDSTK,   SHCMDPRI,       x_timerq,
"ttystat",  FALSE,      SHCMDSTK,   SHCMDPRI,       x_ttyst,
"udp",      FALSE,      SHCMDSTK,   SHCMDPRI,       x_udp,
"unmount",  FALSE,      SHCMDSTK,   SHCMDPRI,       x_unmou,
"wc",       FALSE,      SHCMDSTK,   SHCMDPRI,       x_wc,
"window",   FALSE,      SHCMDSTK,   SHCMDPRI,       x_window,
"ls",       FALSE,      SHCMDSTK,   SHCMDPRI,       x_ls,
"?",        FALSE,      SHCMDSTK,   SHCMDPRI,       x_help,
"nice",     FALSE,      SHCMDSTK,   SHCMDPRI,       x_nice,
"sh",       FALSE,      SHCMDSTK,   SHCMDPRI,       shell

};
int ncmds = sizeof(cmds)/sizeof(struct cmdent);
