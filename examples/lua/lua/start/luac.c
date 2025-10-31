/*
** $Id: luac.c $
** Lua compiler (saves bytecodes to files; also lists bytecodes)
** See Copyright Notice in lua.h
*/

#define luac_c
#define LUA_CORE

#include "lprefix.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"

#include "ldebug.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lopnames.h"
#include "lstate.h"
#include "lundump.h"

static void PrintFunction(const Proto *f, int full);
#define luaU_print PrintFunction

#define PROGNAME   "luac"          /* default program name */
#define OUTPUT     PROGNAME ".out" /* default output file */

static int listing = 0;                 /* list bytecodes? */
static int dumping = 1;                 /* dump bytecodes? */
static int stripping = 0;               /* strip debug information? */
static char Output[] = { OUTPUT };      /* default output file name */
static const char *output = Output;     /* actual output file name */
static const char *progname = PROGNAME; /* actual program name */
static TString **tmname;

static void fatal(const char *message)
{
    luaport_fprintf(luaport_stderr, "%s: %s\n", progname, message);
    luaport_exit(EXIT_FAILURE);
}

static void cannot(const char *what)
{
    luaport_fprintf(luaport_stderr, "%s: cannot %s %s: %s\n", progname, what, output, luaport_strerror(luaport_errno));
    luaport_exit(EXIT_FAILURE);
}

static void usage(const char *message)
{
    if (*message == '-')
        luaport_fprintf(luaport_stderr, "%s: unrecognized option '%s'\n", progname, message);
    else
        luaport_fprintf(luaport_stderr, "%s: %s\n", progname, message);
    luaport_fprintf(luaport_stderr,
                    "usage: %s [options] [filenames]\n"
                    "Available options are:\n"
                    "  -l       list (use -l -l for full listing)\n"
                    "  -o name  output to file 'name' (default is \"%s\")\n"
                    "  -p       parse only\n"
                    "  -s       strip debug information\n"
                    "  -v       show version information\n"
                    "  --       stop handling options\n"
                    "  -        stop handling options and process stdin\n",
                    progname, Output);
    luaport_exit(EXIT_FAILURE);
}

#define IS(s) (luaport_strcmp(argv[i], s) == 0)

static int doargs(int argc, char *argv[])
{
    int i;
    int version = 0;
    if (argv[0] != NULL && *argv[0] != 0)
        progname = argv[0];
    for (i = 1; i < argc; i++) {
        if (*argv[i] != '-') /* end of options; keep it */
            break;
        else if (IS("--")) /* end of options; skip it */
        {
            ++i;
            if (version)
                ++version;
            break;
        } else if (IS("-")) /* end of options; use stdin */
            break;
        else if (IS("-l")) /* list */
            ++listing;
        else if (IS("-o")) /* output file */
        {
            output = argv[++i];
            if (output == NULL || *output == 0 || (*output == '-' && output[1] != 0))
                usage("'-o' needs argument");
            if (IS("-"))
                output = NULL;
        } else if (IS("-p")) /* parse only */
            dumping = 0;
        else if (IS("-s")) /* strip debug information */
            stripping = 1;
        else if (IS("-v")) /* show version */
            ++version;
        else /* unknown option */
            usage(argv[i]);
    }
    if (i == argc && (listing || !dumping)) {
        dumping = 0;
        argv[--i] = Output;
    }
    if (version) {
        luaport_printf("%s\n", LUA_COPYRIGHT);
        if (version == argc - 1)
            luaport_exit(EXIT_SUCCESS);
    }
    return i;
}

#define FUNCTION "(function()end)();"

static const char *reader(lua_State *L, void *ud, size_t *size)
{
    UNUSED(L);
    if ((*(int *)ud)--) {
        *size = sizeof(FUNCTION) - 1;
        return FUNCTION;
    } else {
        *size = 0;
        return NULL;
    }
}

#define toproto(L, i) getproto(s2v(L->top + (i)))

static const Proto *combine(lua_State *L, int n)
{
    if (n == 1)
        return toproto(L, -1);
    else {
        Proto *f;
        int i = n;
        if (lua_load(L, reader, &i, "=(" PROGNAME ")", NULL) != LUA_OK)
            fatal(lua_tostring(L, -1));
        f = toproto(L, -1);
        for (i = 0; i < n; i++) {
            f->p[i] = toproto(L, i - n - 1);
            if (f->p[i]->sizeupvalues > 0)
                f->p[i]->upvalues[0].instack = 0;
        }
        luaM_freearray(L, f->lineinfo, f->sizelineinfo);
        f->sizelineinfo = 0;
        return f;
    }
}

static int writer(lua_State *L, const void *p, size_t size, void *u)
{
    UNUSED(L);
    return (luaport_fwrite(p, size, 1, (FILE *)u) != 1) && (size != 0);
}

static int pmain(lua_State *L)
{
    int argc = (int)lua_tointeger(L, 1);
    char **argv = (char **)lua_touserdata(L, 2);
    const Proto *f;
    int i;
    tmname = G(L)->tmname;
    if (!lua_checkstack(L, argc))
        fatal("too many input files");
    for (i = 0; i < argc; i++) {
        const char *filename = IS("-") ? NULL : argv[i];
        if (luaL_loadfile(L, filename) != LUA_OK)
            fatal(lua_tostring(L, -1));
    }
    f = combine(L, argc);
    if (listing)
        luaU_print(f, listing > 1);
    if (dumping) {
        FILE *D = (output == NULL) ? luaport_stdout : luaport_fopen(output, "wb");
        if (D == NULL)
            cannot("open");
        lua_lock(L);
        luaU_dump(L, f, writer, D, stripping);
        lua_unlock(L);
        if (luaport_ferror(D))
            cannot("write");
        if (luaport_fclose(D))
            cannot("close");
    }
    return 0;
}

int main(int argc, char *argv[])
{
    lua_State *L;
    int i = doargs(argc, argv);
    argc -= i;
    argv += i;
    if (argc <= 0)
        usage("no input files given");
    L = luaL_newstate();
    if (L == NULL)
        fatal("cannot create state: not enough memory");
    lua_pushcfunction(L, &pmain);
    lua_pushinteger(L, argc);
    lua_pushlightuserdata(L, argv);
    if (lua_pcall(L, 2, 0, 0) != LUA_OK)
        fatal(lua_tostring(L, -1));
    lua_close(L);
    return EXIT_SUCCESS;
}

/*
** print bytecodes
*/

#define UPVALNAME(x) ((f->upvalues[x].name) ? getstr(f->upvalues[x].name) : "-")
#define VOID(p)      ((const void *)(p))
#define eventname(i) (getstr(tmname[i]))

static void PrintString(const TString *ts)
{
    const char *s = getstr(ts);
    size_t i, n = tsslen(ts);
    luaport_printf("\"");
    for (i = 0; i < n; i++) {
        int c = (int)(unsigned char)s[i];
        switch (c) {
            case '"':
                luaport_printf("\\\"");
                break;
            case '\\':
                luaport_printf("\\\\");
                break;
            case '\a':
                luaport_printf("\\a");
                break;
            case '\b':
                luaport_printf("\\b");
                break;
            case '\f':
                luaport_printf("\\f");
                break;
            case '\n':
                luaport_printf("\\n");
                break;
            case '\r':
                luaport_printf("\\r");
                break;
            case '\t':
                luaport_printf("\\t");
                break;
            case '\v':
                luaport_printf("\\v");
                break;
            default:
                if (isprint(c))
                    luaport_printf("%c", c);
                else
                    luaport_printf("\\%03d", c);
                break;
        }
    }
    luaport_printf("\"");
}

static void PrintType(const Proto *f, int i)
{
    const TValue *o = &f->k[i];
    switch (ttypetag(o)) {
        case LUA_VNIL:
            luaport_printf("N");
            break;
        case LUA_VFALSE:
        case LUA_VTRUE:
            luaport_printf("B");
            break;
        case LUA_VNUMFLT:
            luaport_printf("F");
            break;
        case LUA_VNUMINT:
            luaport_printf("I");
            break;
        case LUA_VSHRSTR:
        case LUA_VLNGSTR:
            luaport_printf("S");
            break;
        default: /* cannot happen */
            luaport_printf("?%d", ttypetag(o));
            break;
    }
    luaport_printf("\t");
}

static void PrintConstant(const Proto *f, int i)
{
    const TValue *o = &f->k[i];
    switch (ttypetag(o)) {
        case LUA_VNIL:
            luaport_printf("nil");
            break;
        case LUA_VFALSE:
            luaport_printf("false");
            break;
        case LUA_VTRUE:
            luaport_printf("true");
            break;
        case LUA_VNUMFLT: {
            char buff[100];
            luaport_snprintf(buff, sizeof(buf), LUA_NUMBER_FMT, fltvalue(o));
            luaport_printf("%s", buff);
            if (buff[luaport_strspn(buff, "-0123456789")] == '\0')
                luaport_printf(".0");
            break;
        }
        case LUA_VNUMINT:
            luaport_printf(LUA_INTEGER_FMT, ivalue(o));
            break;
        case LUA_VSHRSTR:
        case LUA_VLNGSTR:
            PrintString(tsvalue(o));
            break;
        default: /* cannot happen */
            luaport_printf("?%d", ttypetag(o));
            break;
    }
}

#define COMMENT   "\t; "
#define EXTRAARG  GETARG_Ax(code[pc + 1])
#define EXTRAARGC (EXTRAARG * (MAXARG_C + 1))
#define ISK       (isk ? "k" : "")

static void PrintCode(const Proto *f)
{
    const Instruction *code = f->code;
    int pc, n = f->sizecode;
    for (pc = 0; pc < n; pc++) {
        Instruction i = code[pc];
        OpCode o = GET_OPCODE(i);
        int a = GETARG_A(i);
        int b = GETARG_B(i);
        int c = GETARG_C(i);
        int ax = GETARG_Ax(i);
        int bx = GETARG_Bx(i);
        int sb = GETARG_sB(i);
        int sc = GETARG_sC(i);
        int sbx = GETARG_sBx(i);
        int isk = GETARG_k(i);
        int line = luaG_getfuncline(f, pc);
        luaport_printf("\t%d\t", pc + 1);
        if (line > 0)
            luaport_printf("[%d]\t", line);
        else
            luaport_printf("[-]\t");
        luaport_printf("%-9s\t", opnames[o]);
        switch (o) {
            case OP_MOVE:
                luaport_printf("%d %d", a, b);
                break;
            case OP_LOADI:
                luaport_printf("%d %d", a, sbx);
                break;
            case OP_LOADF:
                luaport_printf("%d %d", a, sbx);
                break;
            case OP_LOADK:
                luaport_printf("%d %d", a, bx);
                luaport_printf(COMMENT);
                PrintConstant(f, bx);
                break;
            case OP_LOADKX:
                luaport_printf("%d", a);
                luaport_printf(COMMENT);
                PrintConstant(f, EXTRAARG);
                break;
            case OP_LOADFALSE:
                luaport_printf("%d", a);
                break;
            case OP_LFALSESKIP:
                luaport_printf("%d", a);
                break;
            case OP_LOADTRUE:
                luaport_printf("%d", a);
                break;
            case OP_LOADNIL:
                luaport_printf("%d %d", a, b);
                luaport_printf(COMMENT "%d out", b + 1);
                break;
            case OP_GETUPVAL:
                luaport_printf("%d %d", a, b);
                luaport_printf(COMMENT "%s", UPVALNAME(b));
                break;
            case OP_SETUPVAL:
                luaport_printf("%d %d", a, b);
                luaport_printf(COMMENT "%s", UPVALNAME(b));
                break;
            case OP_GETTABUP:
                luaport_printf("%d %d %d", a, b, c);
                luaport_printf(COMMENT "%s", UPVALNAME(b));
                luaport_printf(" ");
                PrintConstant(f, c);
                break;
            case OP_GETTABLE:
                luaport_printf("%d %d %d", a, b, c);
                break;
            case OP_GETI:
                luaport_printf("%d %d %d", a, b, c);
                break;
            case OP_GETFIELD:
                luaport_printf("%d %d %d", a, b, c);
                luaport_printf(COMMENT);
                PrintConstant(f, c);
                break;
            case OP_SETTABUP:
                luaport_printf("%d %d %d%s", a, b, c, ISK);
                luaport_printf(COMMENT "%s", UPVALNAME(a));
                luaport_printf(" ");
                PrintConstant(f, b);
                if (isk) {
                    luaport_printf(" ");
                    PrintConstant(f, c);
                }
                break;
            case OP_SETTABLE:
                luaport_printf("%d %d %d%s", a, b, c, ISK);
                if (isk) {
                    luaport_printf(COMMENT);
                    PrintConstant(f, c);
                }
                break;
            case OP_SETI:
                luaport_printf("%d %d %d%s", a, b, c, ISK);
                if (isk) {
                    luaport_printf(COMMENT);
                    PrintConstant(f, c);
                }
                break;
            case OP_SETFIELD:
                luaport_printf("%d %d %d%s", a, b, c, ISK);
                luaport_printf(COMMENT);
                PrintConstant(f, b);
                if (isk) {
                    luaport_printf(" ");
                    PrintConstant(f, c);
                }
                break;
            case OP_NEWTABLE:
                luaport_printf("%d %d %d", a, b, c);
                luaport_printf(COMMENT "%d", c + EXTRAARGC);
                break;
            case OP_SELF:
                luaport_printf("%d %d %d%s", a, b, c, ISK);
                if (isk) {
                    luaport_printf(COMMENT);
                    PrintConstant(f, c);
                }
                break;
            case OP_ADDI:
                luaport_printf("%d %d %d", a, b, sc);
                break;
            case OP_ADDK:
                luaport_printf("%d %d %d", a, b, c);
                luaport_printf(COMMENT);
                PrintConstant(f, c);
                break;
            case OP_SUBK:
                luaport_printf("%d %d %d", a, b, c);
                luaport_printf(COMMENT);
                PrintConstant(f, c);
                break;
            case OP_MULK:
                luaport_printf("%d %d %d", a, b, c);
                luaport_printf(COMMENT);
                PrintConstant(f, c);
                break;
            case OP_MODK:
                luaport_printf("%d %d %d", a, b, c);
                luaport_printf(COMMENT);
                PrintConstant(f, c);
                break;
            case OP_POWK:
                luaport_printf("%d %d %d", a, b, c);
                luaport_printf(COMMENT);
                PrintConstant(f, c);
                break;
            case OP_DIVK:
                luaport_printf("%d %d %d", a, b, c);
                luaport_printf(COMMENT);
                PrintConstant(f, c);
                break;
            case OP_IDIVK:
                luaport_printf("%d %d %d", a, b, c);
                luaport_printf(COMMENT);
                PrintConstant(f, c);
                break;
            case OP_BANDK:
                luaport_printf("%d %d %d", a, b, c);
                luaport_printf(COMMENT);
                PrintConstant(f, c);
                break;
            case OP_BORK:
                luaport_printf("%d %d %d", a, b, c);
                luaport_printf(COMMENT);
                PrintConstant(f, c);
                break;
            case OP_BXORK:
                luaport_printf("%d %d %d", a, b, c);
                luaport_printf(COMMENT);
                PrintConstant(f, c);
                break;
            case OP_SHRI:
                luaport_printf("%d %d %d", a, b, sc);
                break;
            case OP_SHLI:
                luaport_printf("%d %d %d", a, b, sc);
                break;
            case OP_ADD:
                luaport_printf("%d %d %d", a, b, c);
                break;
            case OP_SUB:
                luaport_printf("%d %d %d", a, b, c);
                break;
            case OP_MUL:
                luaport_printf("%d %d %d", a, b, c);
                break;
            case OP_MOD:
                luaport_printf("%d %d %d", a, b, c);
                break;
            case OP_POW:
                luaport_printf("%d %d %d", a, b, c);
                break;
            case OP_DIV:
                luaport_printf("%d %d %d", a, b, c);
                break;
            case OP_IDIV:
                luaport_printf("%d %d %d", a, b, c);
                break;
            case OP_BAND:
                luaport_printf("%d %d %d", a, b, c);
                break;
            case OP_BOR:
                luaport_printf("%d %d %d", a, b, c);
                break;
            case OP_BXOR:
                luaport_printf("%d %d %d", a, b, c);
                break;
            case OP_SHL:
                luaport_printf("%d %d %d", a, b, c);
                break;
            case OP_SHR:
                luaport_printf("%d %d %d", a, b, c);
                break;
            case OP_MMBIN:
                luaport_printf("%d %d %d", a, b, c);
                luaport_printf(COMMENT "%s", eventname(c));
                break;
            case OP_MMBINI:
                luaport_printf("%d %d %d %d", a, sb, c, isk);
                luaport_printf(COMMENT "%s", eventname(c));
                if (isk)
                    luaport_printf(" flip");
                break;
            case OP_MMBINK:
                luaport_printf("%d %d %d %d", a, b, c, isk);
                luaport_printf(COMMENT "%s ", eventname(c));
                PrintConstant(f, b);
                if (isk)
                    luaport_printf(" flip");
                break;
            case OP_UNM:
                luaport_printf("%d %d", a, b);
                break;
            case OP_BNOT:
                luaport_printf("%d %d", a, b);
                break;
            case OP_NOT:
                luaport_printf("%d %d", a, b);
                break;
            case OP_LEN:
                luaport_printf("%d %d", a, b);
                break;
            case OP_CONCAT:
                luaport_printf("%d %d", a, b);
                break;
            case OP_CLOSE:
                luaport_printf("%d", a);
                break;
            case OP_TBC:
                luaport_printf("%d", a);
                break;
            case OP_JMP:
                luaport_printf("%d", GETARG_sJ(i));
                luaport_printf(COMMENT "to %d", GETARG_sJ(i) + pc + 2);
                break;
            case OP_EQ:
                luaport_printf("%d %d %d", a, b, isk);
                break;
            case OP_LT:
                luaport_printf("%d %d %d", a, b, isk);
                break;
            case OP_LE:
                luaport_printf("%d %d %d", a, b, isk);
                break;
            case OP_EQK:
                luaport_printf("%d %d %d", a, b, isk);
                luaport_printf(COMMENT);
                PrintConstant(f, b);
                break;
            case OP_EQI:
                luaport_printf("%d %d %d", a, sb, isk);
                break;
            case OP_LTI:
                luaport_printf("%d %d %d", a, sb, isk);
                break;
            case OP_LEI:
                luaport_printf("%d %d %d", a, sb, isk);
                break;
            case OP_GTI:
                luaport_printf("%d %d %d", a, sb, isk);
                break;
            case OP_GEI:
                luaport_printf("%d %d %d", a, sb, isk);
                break;
            case OP_TEST:
                luaport_printf("%d %d", a, isk);
                break;
            case OP_TESTSET:
                luaport_printf("%d %d %d", a, b, isk);
                break;
            case OP_CALL:
                luaport_printf("%d %d %d", a, b, c);
                luaport_printf(COMMENT);
                if (b == 0)
                    luaport_printf("all in ");
                else
                    luaport_printf("%d in ", b - 1);
                if (c == 0)
                    luaport_printf("all out");
                else
                    luaport_printf("%d out", c - 1);
                break;
            case OP_TAILCALL:
                luaport_printf("%d %d %d%s", a, b, c, ISK);
                luaport_printf(COMMENT "%d in", b - 1);
                break;
            case OP_RETURN:
                luaport_printf("%d %d %d%s", a, b, c, ISK);
                luaport_printf(COMMENT);
                if (b == 0)
                    luaport_printf("all out");
                else
                    luaport_printf("%d out", b - 1);
                break;
            case OP_RETURN0:
                break;
            case OP_RETURN1:
                luaport_printf("%d", a);
                break;
            case OP_FORLOOP:
                luaport_printf("%d %d", a, bx);
                luaport_printf(COMMENT "to %d", pc - bx + 2);
                break;
            case OP_FORPREP:
                luaport_printf("%d %d", a, bx);
                luaport_printf(COMMENT "exit to %d", pc + bx + 3);
                break;
            case OP_TFORPREP:
                luaport_printf("%d %d", a, bx);
                luaport_printf(COMMENT "to %d", pc + bx + 2);
                break;
            case OP_TFORCALL:
                luaport_printf("%d %d", a, c);
                break;
            case OP_TFORLOOP:
                luaport_printf("%d %d", a, bx);
                luaport_printf(COMMENT "to %d", pc - bx + 2);
                break;
            case OP_SETLIST:
                luaport_printf("%d %d %d", a, b, c);
                if (isk)
                    luaport_printf(COMMENT "%d", c + EXTRAARGC);
                break;
            case OP_CLOSURE:
                luaport_printf("%d %d", a, bx);
                luaport_printf(COMMENT "%p", VOID(f->p[bx]));
                break;
            case OP_VARARG:
                luaport_printf("%d %d", a, c);
                luaport_printf(COMMENT);
                if (c == 0)
                    luaport_printf("all out");
                else
                    luaport_printf("%d out", c - 1);
                break;
            case OP_VARARGPREP:
                luaport_printf("%d", a);
                break;
            case OP_EXTRAARG:
                luaport_printf("%d", ax);
                break;
#if 0
   default:
	luaport_printf("%d %d %d",a,b,c);
	luaport_printf(COMMENT "not handled");
	break;
#endif
        }
        luaport_printf("\n");
    }
}

#define SS(x) ((x == 1) ? "" : "s")
#define S(x)  (int)(x), SS(x)

static void PrintHeader(const Proto *f)
{
    const char *s = f->source ? getstr(f->source) : "=?";
    if (*s == '@' || *s == '=')
        s++;
    else if (*s == LUA_SIGNATURE[0])
        s = "(bstring)";
    else
        s = "(string)";
    luaport_printf("\n%s <%s:%d,%d> (%d instruction%s at %p)\n",
                   (f->linedefined == 0) ? "main" : "function", s,
                   f->linedefined, f->lastlinedefined,
                   S(f->sizecode), VOID(f));
    luaport_printf("%d%s param%s, %d slot%s, %d upvalue%s, ",
                   (int)(f->numparams), f->is_vararg ? "+" : "", SS(f->numparams),
                   S(f->maxstacksize), S(f->sizeupvalues));
    luaport_printf("%d local%s, %d constant%s, %d function%s\n",
                   S(f->sizelocvars), S(f->sizek), S(f->sizep));
}

static void PrintDebug(const Proto *f)
{
    int i, n;
    n = f->sizek;
    luaport_printf("constants (%d) for %p:\n", n, VOID(f));
    for (i = 0; i < n; i++) {
        luaport_printf("\t%d\t", i);
        PrintType(f, i);
        PrintConstant(f, i);
        luaport_printf("\n");
    }
    n = f->sizelocvars;
    luaport_printf("locals (%d) for %p:\n", n, VOID(f));
    for (i = 0; i < n; i++) {
        luaport_printf("\t%d\t%s\t%d\t%d\n",
                       i, getstr(f->locvars[i].varname), f->locvars[i].startpc + 1, f->locvars[i].endpc + 1);
    }
    n = f->sizeupvalues;
    luaport_printf("upvalues (%d) for %p:\n", n, VOID(f));
    for (i = 0; i < n; i++) {
        luaport_printf("\t%d\t%s\t%d\t%d\n",
                       i, UPVALNAME(i), f->upvalues[i].instack, f->upvalues[i].idx);
    }
}

static void PrintFunction(const Proto *f, int full)
{
    int i, n = f->sizep;
    PrintHeader(f);
    PrintCode(f);
    if (full)
        PrintDebug(f);
    for (i = 0; i < n; i++)
        PrintFunction(f->p[i], full);
}
