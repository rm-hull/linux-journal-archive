#if 0
	shc Version 3.7, Generic Script Compiler
	Copyright (c) 1994-2003 Francisco Rosales <frosal@fi.upm.es>

	shc -f happy-friendly.sh 
#endif

static  long date = 0;
static  char mail[] = "Please contact your provider";
static  int  relax = 0;
typedef char pswd_t[336];
static  char pswd[] = 
	"\021\113\324\024\346\053\364\215\113\343\244\264\342\332\343\244"
	"\116\043\272\340\166\203\274\343\167\273\274\172\052\206\313\073"
	"\322\237\117\270\312\104\106\025\047\352\312\011\304\255\255\023"
	"\320\147\363\106\352\257\051\141\152\346\334\225\154\247\320\076"
	"\106\040\367\020\144\075\046\213\047\360\224\353\235\102\376\155"
	"\251\362\264\224\241\335\365\014\303\321\241\060\170\161\156\276"
	"\221\145\317\365\242\365\200\311\345\025\265\202\127\263\357\000"
	"\245\243\224\107\201\212\123\104\133\364\164\324\145\343\222\367"
	"\110\141\354\353\126\155\264\073\202\151\275\331\035\255\331\302"
	"\120\156\011\321\370\134\026\123\120\212\047\266\155\272\255\266"
	"\033\231\241\162\006\125\255\210\277\153\141\334\030\073\236\150"
	"\251\250\072\241\004\120\364\125\332\034\013\110\326\270\376\361"
	"\121\237\143\130\364\021\340\263\174\102\217\224\175\056\374\046"
	"\326\066\307\332\206\273\057\141\327\072\251\255\362\247\237\104"
	"\106\002\234\072\023\174\356\217\276\175\043\073\253\040\141\201"
	"\126\050\134\335\344\213\076\273\306\347\151\270\216\010\374\324"
	"\012\230\016\036\025\374\255\323\172\321\017\045\361\160\247\107"
	"\231\003\044\175\216\142\070\124\111\241\015\327\251\011\253\264"
	"\242\272\322\267\266\177\212\060\120\231\126\101\012\375\211\243"
	"\000\255\040\216\020\130\343\131\372\360\061\243\371\334\127\233"
	"\226\051\122\115\251\335\175\371\166\323\073\200\320\304\043\320"
	"\103";
typedef char shll_t[8];
static  char shll[] = 
	"\372\150\020\004\360\113\223\255\201\234\102\333\226\062\014\071"
	"\053\350\221\307\177\272\031";
typedef char inlo_t[3];
static  char inlo[] = 
	"\163\362\171\143\366\111\135\155\035\230\355\355\134\021\276";
typedef char xecc_t[15];
static  char xecc[] = 
	"\112\073\077\116\165\244\242\175\013\353\153\015\355\040\366\124"
	"\035\117\360\137\052\206\221\066\300\274\036\121";
typedef char lsto_t[1];
static  char lsto[] = 
	"\257\235\013\235";
#define TEXT_chk1	"odSDEebVF"
typedef char chk1_t[10];
static  char chk1[] = 
	"\200\125\117\172\234\004\173\320\076\141\036\323\072\266\260\176";
typedef char opts_t[1];
static  char opts[] = 
	"\373\266\110\066\363\323\173\116\256\104\106\225\251\250\353";
typedef char text_t[93];
static  char text[] = 
	"\050\030\230\016\234\317\052\334\360\147\124\225\317\204\105\027"
	"\224\346\055\146\057\321\243\251\047\150\135\167\132\275\053\321"
	"\270\042\366\340\210\040\367\327\150\276\067\165\273\140\155\050"
	"\116\117\322\222\333\317\032\043\114\022\220\266\351\361\363\152"
	"\120\124\172\023\167\243\145\271\361\251\166\027\312\122\233\224"
	"\217\257\370\117\134\041\174\107\365\116\344\177\053\275\015";
#define TEXT_chk2	"EjcjyTgCmzK7"
typedef char chk2_t[13];
static  char chk2[] = 
	"\246\223\110\314\075\271\347\055\276\025\005\165\240\361\131\054"
	"\005\303";
typedef char hide_t[4096];

#define DEBUGEXEC	0	/* Define as 1 to debug execvp calls */
#define TRACEABLE	0	/* Define as 1 to enable ptrace the executable */

/* rtc.c */

#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/**
 * 'Alleged RC4' Source Code picked up from the news.
 * From: allen@gateway.grumman.com (John L. Allen)
 * Newsgroups: comp.lang.c
 * Subject: Shrink this C code for fame and fun
 * Date: 21 May 1996 10:49:37 -0400
 */

static unsigned char state[256], indx, jndx;

/*
 * Reset rc4 state. 
 */
void state_0(void)
{
	indx = jndx = 0;
	do {
		state[indx] = indx;
	} while (++indx);
}

/*
 * Set key. Can be used more than once. 
 */
void key(char * str, int len)
{
	unsigned char tmp, * ptr = (unsigned char *)str;
	while (len > 0) {
		do {
			tmp = state[indx];
			jndx += tmp;
			jndx += ptr[(int)indx % len];
			state[indx] = state[jndx];
			state[jndx] = tmp;
		} while (++indx);
		ptr += 256;
		len -= 256;
	}
}

/*
 * Crypt data. 
 */
void rc4(char * str, int len)
{
	unsigned char tmp, * ptr = (unsigned char *)str;
	jndx = 0;
	while (len > 0) {
		indx++;
		tmp = state[indx];
		jndx += tmp;
		state[indx] = state[jndx];
		state[jndx] = tmp;
		tmp += state[indx];
		*ptr ^= state[tmp];
		ptr++;
		len--;
	}
}

/*
 * Key with file invariants. 
 */
int key_with_file(char * file)
{
	struct stat statf[1];
	struct stat control[1];

	if (stat(file, statf) < 0)
		return -1;

	/* Turn on stable fields */
	memset(control, 0, sizeof(control));
	control->st_ino = statf->st_ino;
	control->st_dev = statf->st_dev;
	control->st_rdev = statf->st_rdev;
	control->st_uid = statf->st_uid;
	control->st_gid = statf->st_gid;
	control->st_size = statf->st_size;
	control->st_mtime = statf->st_mtime;
	control->st_ctime = statf->st_ctime;
	key((char *)control, sizeof(control));
	return 0;
}

#if DEBUGEXEC
void debugexec(char * shll, int argc, char ** argv)
{
	int i;
	fprintf(stderr, "shll=%s\n", shll ? shll : "<null>");
	fprintf(stderr, "argc=%d\n", argc);
	if (!argv) {
		fprintf(stderr, "argv=<null>\n");
	} else { 
		for (i = 0; i <= argc ; i++)
			fprintf(stderr, "argv[%d]=%.60s\n", i, argv[i] ? argv[i] : "<null>");
	}
}
#endif /* DEBUGEXEC */

void rmarg(char ** argv, char * arg)
{
	for (; argv && *argv && *argv != arg; argv++);
	for (; argv && *argv; argv++)
		*argv = argv[1];
}

int chkenv(int argc)
{
	char buff[512];
	unsigned mask, m;
	int l, a, c;
	char * string;
	extern char ** environ;

	mask  = (unsigned)chkenv;
	mask ^= (unsigned)getpid() * ~mask;
	sprintf(buff, "x%x", mask);
	string = getenv(buff);
#if DEBUGEXEC
	fprintf(stderr, "getenv(%s)=%s\n", buff, string ? string : "<null>");
#endif
	l = strlen(buff);
	if (!string) {
		/* 1st */
		sprintf(&buff[l], "=%u %d", mask, argc);
		putenv(strdup(buff));
		return 0;
	}
	c = sscanf(string, "%u %d%c", &m, &a, buff);
	if (c == 2 && m == mask) {
		/* 3rd */
		rmarg(environ, &string[-l - 1]);
		return 1 + (argc - a);
	}
	return -1;
}

#if !TRACEABLE

#define _LINUX_SOURCE_COMPAT
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void untraceable(char * argv0)
{
	char proc[80];
	int pid, mine;

	switch(pid = vfork()) {
	case  0:
		pid = getppid();
		/* For problematic SunOS ptrace */
		sprintf(proc, "/proc/%d/as", (int)pid);
		close(0);
		mine = !open(proc, O_RDWR|O_EXCL);
		if (!mine && errno != EBUSY)
			mine = !ptrace(PTRACE_ATTACH, pid, 0, 0);
		if (mine) {
			kill(pid, SIGCONT);
		} else {
			fprintf(stderr, "%s is being traced!\n", argv0);
			kill(pid, SIGKILL);
		}
		_exit(mine);
	case -1:
		break;
	default:
		if (pid == waitpid(pid, 0, 0))
			return;
	}
	perror(argv0);
	_exit(1);
}
#endif /* !TRACEABLE */

char * xsh(int argc, char ** argv)
{
	char buff[512];
	char * scrpt;
	int ret, i, j;
	char ** varg;

	state_0();
	key(pswd, sizeof(pswd_t));
	rc4(shll, sizeof(shll_t));
	rc4(inlo, sizeof(inlo_t));
	rc4(xecc, sizeof(xecc_t));
	rc4(lsto, sizeof(lsto_t));
	rc4(chk1, sizeof(chk1_t));
	if (strcmp(TEXT_chk1, chk1))
		return "location has changed!";
	ret = chkenv(argc);
	if (ret < 0)
		return "abnormal behavior!";
	varg = (char **)calloc(argc + 10, sizeof(char *));
	if (!varg)
		return 0;
	if (ret) {
		if (!relax && key_with_file(shll))
			return shll;
		rc4(opts, sizeof(opts_t));
		rc4(text, sizeof(text_t));
		fprintf(stderr,"TEXT=%s\n", text);
		rc4(chk2, sizeof(chk2_t));
		if (strcmp(TEXT_chk2, chk2))
			return "shell has changed!";
		if (sizeof(text_t) < sizeof(hide_t)) {
			/* Prepend spaces til a sizeof(hide_t) script size. */
			scrpt = malloc(sizeof(hide_t));
			if (!scrpt)
				return 0;
			memset(scrpt, (int) ' ', sizeof(hide_t));
			memcpy(&scrpt[sizeof(hide_t) - sizeof(text_t)], text, sizeof(text_t));
		} else {
			scrpt = text;	/* Script text */
		}
	} else {			/* Reexecute */
		if (*xecc) {
			sprintf(buff, xecc, argv[0]);
			scrpt = buff;
		} else {
			scrpt = argv[0];
		}
	}
	j = 0;
	varg[j++] = argv[0];		/* My own name at execution */
	if (ret && *opts)
		varg[j++] = opts;	/* Options on 1st line of code */
	if (*inlo)
		varg[j++] = inlo;	/* Option introducing inline code */
	varg[j++] = scrpt;		/* The script itself */
	if (*lsto)
		varg[j++] = lsto;	/* Option meaning last option */
	i = (ret > 1) ? ret : 0;	/* Args numbering correction */
	while (i < argc)
		varg[j++] = argv[i++];	/* Main run-time arguments */
	varg[j] = 0;			/* NULL terminated array */
#if DEBUGEXEC
	debugexec(shll, j, varg);
#endif
	execvp(shll, varg);
	return shll;
}

int main(int argc, char ** argv)
{
#if DEBUGEXEC
	debugexec("main", argc, argv);
#endif
#if !TRACEABLE
	untraceable(argv[0]);
#endif
	if (date && (date < (long)time(NULL))) {
		fprintf(stderr, "%s has expired!\n", argv[0]);
		fprintf(stderr, "%s\n", mail);
	} else {
		argv[1] = xsh(argc, argv);
		fprintf(stderr, "%s%s%s: %s\n", argv[0],
			errno ? ": " : "",
			errno ? strerror(errno) : "",
			argv[1] ? argv[1] : "<null>"
		);
	}
	return 1;
}
